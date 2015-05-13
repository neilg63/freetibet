<?php

require_once drupal_get_path("module", "campaigner") . "/includes/libraries/SpamFilter.php";

class CampaignerReport {
  /**
   * @var array Lists all the reports available in the report class.
   */
  protected $reports = array("signatures", "spam", "queue");
  /**
   * Select the rows to be loaded for each report.
   *
   * @var array
   */
  protected $rows = array(
    "signatures" => array(
      "database"   => array(
        "signatures" => array(
          "id",
          "region",
          "action",
          "email",
          "name",
          "postcode",
          "comments",
          "spam",
          "created",
          "uid"
        )
      ),
      "calculated" => array(
        "admin"
      )
    ),
    "spam"       => array(
      "database"   => array(
        "signatures" => array("id", "created", "email", "name", "comments", "spam", "created", "uid"),
      ),
      "calculated" => array("spam rating", "admin")
    ),
    "queue"      => array(
      'database'   => array(
        'queued_mails' => array("id", "signature_id", "from_address", "to_address", "subject", "message")
      ),
      "calculated" => array(
        'send'
      )
    )
  );

  protected $sortables = array(
    "signatures" => array("id", "region", "action", "email", "name", "uid", "created"),
    "spam"       => array("spam rating", "id", "created", "name")
  );
  /**
   * @var array Stores the latest generated results for the current report.
   */
  protected $results = array();

  protected $query;

  protected $tables = array();

  protected $sort = FALSE;

  protected $limit = FALSE;

  /**
   * Pass a parameter to select the report to use.
   *
   * @param $report
   *
   * @return bool
   */
  public function chooseReport($report) {
    if (in_array($report, $this->reports)) {
      $this->currentReport = $report;
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Selects the appropriate delegate function for filtering.
   *
   * @param $filters
   */
  public function filter($filters) {
    if (!empty($filters)) {
      foreach ($filters as $filter => $value) {
        $function = "filter" . ucfirst($filter);
        if (is_callable(array($this, $function))) {
          $this->filters[] = array("function" => $function, "value" => $value);
        }
      }
    }
  }

  /**
   * Implement filtering by action id.
   *
   * @param $value
   *
   */
  public function filterAction($value) {
    if ($value > 0) {
      $this->query->condition("s.action", $value, "=");
    }
  }

  /**
   * @param $value
   */
  public function filterType($value) {
    if (!empty($value)) {
      $this->query->join("node", "n", "n.nid = s.action");
      $this->tables[] = "node";
      $this->query->fields("n", array("type", "title"));
      $this->query->condition("n.type", $value, "=");
    }
  }

  /**
   * @param $value
   */
  public function filterHasComment($value) {
    if (!empty($value)) {
      $this->query->isNotNull("s.comments");
    }
  }

  /**
   * @param $value
   */
  public function filterNotSpam($value) {
    if (!empty($value)) {
      $this->query->condition("s.spam", 0);
    }
  }


  /**
   * Runs the currently selected report (if it is valid).
   *
   * This function passes through to a delegate function to calculate the appropriate output.
   */
  public function run() {
    $this->tables = array();
    $function     = "run" . ucfirst($this->currentReport);
    if (is_callable(array($this, $function))) {
      $this->results = $this->$function();
    }
  }

  public function runQueue() {
    $this->query = db_select("queued_mails", "qm");
    $this->query->fields("qm", $this->rows['queue']['database']['queued_mails']);
    $this->query->isNull("sent");
    $this->countQuery = clone $this->query;
    $this->tables     = "queued_mails";
    $results          = $this->query->execute();
    return $results;
  }

  /**
   * Delegate function for the spam report.
   *
   * @return DatabaseStatementInterface|null
   */
  public function runSpam() {
    $this->query = db_select("signatures", "s");
    $this->query->fields("s", $this->rows['spam']["database"]['signatures']);
    $this->query->isNotNull("s.comments");
    if ($this->sort && $this->sort['column'] !== "spam rating") {
      $this->query->orderBy($this->sort['column'], $this->sort['direction']);
    }
    $this->countQuery = clone $this->query;
    $this->tables[]   = "signatures";
    $results          = $this->query->execute();
    $outResults       = array();
    $spamRatingCache  = cache_get("campaigner_reports_spam_ratings");
    $spamRatings      = $spamRatingCache ? $spamRatingCache->data : array();
    if (!empty($results)) {
      // Do this here as we need to filter small values of spam
      $filter = new SpamFilter();
      foreach ($results as $result) {
        if (isset($spamRatings[$result->id])) {
          $result->SpamRating = $spamRatings[$result->id];
        }
        else {
          $result->SpamRating = (int) (100 * $filter->checkForSpam($result->comments));

          $spamRatings[$result->id] = $result->SpamRating;
        }
        if ($result->SpamRating <> 0) {
          $outResults[] = $result;
        }
      }
    }
    if (!$this->sort || $this->sort['column'] === "spam rating") {
      usort($outResults, "campaigner_report_sort_by_spam_rating");
    }
    $this->count = count($outResults);
    // set the cache for the next hour
    cache_set("campaigner_reports_spam_ratings", $spamRatings, "cache", time() + 3600);
    return $outResults;
  }

  /**
   * Delegate function for the signatures report.
   *
   * @return DatabaseStatementInterface|null
   */
  public function runSignatures() {
    $this->query = db_select("signatures", "s");
    $this->query->fields("s", $this->rows["signatures"]['database']['signatures']);
    $this->tables[] = "signatures";
    if (!empty($this->filters)) {
      foreach ($this->filters as $filter) {
        $this->$filter['function']($filter['value']);
      }
    }
    // set up the count query before we do stuff like limits etc.
    $this->countQuery = clone $this->query;

    if ($this->sort) {
      $this->query->orderBy($this->sort['column'], $this->sort['direction']);
    }

    if ($this->limit) {
      $first = ($this->limit['page']) * $this->limit['pageSize'];
      $this->query->range($first, $this->limit['pageSize']);
    }
    $results = $this->query->execute();

    return $results;
  }

  /**
   * Returns the count of the current query.
   *
   * @return mixed
   */
  public function getCount() {
    if (!empty($this->count)) {
      return $this->count;
    }
    return $this->countQuery->countQuery()->execute()->fetchField();
  }

  /**
   * Helper function to generate an appropriate array of available actions to filter on.
   *
   * @return mixed
   */
  public function getActionFilterOptions() {
    $query = db_select("signatures", "s");
    $query->fields("s", array("action"));
    $query->join("node", "n", "n.nid = s.action");
    if (!empty($_SESSION['filters']) && !empty($_SESSION['filters']['type'])) {
      $query->condition("n.type", $_SESSION['filters']['type'], "=");
    }
    $query->fields("n", array("title"));
    $results    = $query->execute();
    $results    = $results->fetchAllKeyed(0, 1);
    $results[0] = t("All actions");
    ksort($results);
    return $results;
  }

  /**
   * Allows a sort column and direction to be set for the report.
   *
   * Should be called before the report is executed.
   *
   * @param        $column
   * @param string $direction
   */
  public function setSortColumn($column, $direction = "ASC") {
    if (!is_array($this->sortables[$this->currentReport])) {
      return;
    }
    if (in_array($column, $this->sortables[$this->currentReport])) {
      $this->sort = array("column" => $column, "direction" => $direction);
    }
    return;
  }

  public function outputCSV() {
    $filename = "campaigner_report_export.csv";
    $fields   = $this->getFieldsFromDatabaseArray($this->rows[$this->currentReport]['database']);
    $header   = $fields;
    drupal_add_http_header("Content-Type", "text/csv");
    drupal_add_http_header("Content-Disposition", "attachment;filename=" . $filename);
    $stdOut = fopen('php://output', "w");
    fputcsv($stdOut, $header);
    if (!empty($this->results)) {

      foreach ($this->results as $row) {
        $result = array();
        foreach ($fields as $field) {
          $function = "csvField" . ucfirst($field);
          if (is_callable(array($this, $function)) && isset($row->$field)) {
            $result[$field] = $this->$function($row->$field, $row);
          }
          else if (!is_callable(array($this, $function)) && isset($row->$field)){
            $result[$field] = $row->$field;
          }
          else {
            $result[$field] = "";
          }
        }
        fputcsv($stdOut, $result);
      }
    }
    fclose($stdOut);
  }

  /**
   * Render the body of the table including the tbody tag.
   *
   * @return string
   */
  public function renderBody() {
    $output = '<tbody>';
    if (!empty($this->results)) {
      $fields = $this->getFieldsFromDatabaseArray($this->rows[$this->currentReport]['database']);
      foreach ($this->results as $row) {
        $output .= "<tr>";
        if (!empty($row)) {
          foreach ($row as $key => $field) {
            if (!in_array($key, $fields)) {
              continue;
            }
            $function = "displayField" . ucfirst($key);
            if (is_callable(array($this, $function))) {
              $output .= "<td class='" . $key . "'>" . $this->$function($field, $row) . "</td>";
            }
            else {
              $output .= "<td class='" . $key . "'>" . $field . "</td>";
            }
          }
        }
        if (!empty($this->rows[$this->currentReport]['calculated'])) {
          foreach ($this->rows[$this->currentReport]['calculated'] as $calculatedField) {
            $fieldName = $this->makeFunctionName($calculatedField);
            if (isset($row->$fieldName)) {
              $output .= "<td>" . $row->$fieldName . "</td>";
            }
            else {
              $function = "calculate" . $fieldName;
              if (is_callable(array($this, $function))) {
                $response = $this->$function($row);
                if ($response['value'] !== NULL) {
                  $row->$response['valueName'] = $response['value'];
                }
                $output .= "<td>" . $response['markup'] . "</td>";
              }
            }
          }
        }
        $output .= "</tr>";
      }
    }
    $output .= "</tbody>";
    return $output;
  }

  /**
   * @param $row
   *
   * @return string
   */
  protected function calculateAdmin($row) {
    if (!$row->spam) {
      $markup = l(t("Mark as spam"), "campaigner/signature/spam/" . $row->id, array("attributes" => array("class" => "spamlink")));
    }
    else {
      $markup = l(t("Mark as not spam"), "campaigner/signature/unspam/" . $row->id, array("attributes" => array("class" => "spamlink")));
    }
    return array(
      "valueName" => "admin",
      "value"     => NULL,
      "markup"    => $markup
    );
  }

  protected function calculateSend($row) {
    return array(
      "valueName" => "send",
      "value"     => NULL,
      "markup"    => l(t("Send mail"), "campaigner/send_queued_mail/" . $row->signature_id, array("attributes" => array("class" => "sendlink")))
    );
  }

  /**
   * Get the fields from the report array as a flat list.
   *
   * @param $reportArray
   *
   * @return array
   */
  protected function getFieldsFromReportArray($reportArray) {
    $outputArray = $this->getFieldsFromDatabaseArray($reportArray['database']);
    if (!empty($reportArray['calculated'])) {
      $outputArray = array_merge($outputArray, $reportArray['calculated']);
    }
    return $outputArray;
  }

  /**
   * @param $array
   *
   * @return array
   */
  protected function getFieldsFromDatabaseArray($array) {
    $outputArray = array();
    if (!empty($array)) {
      foreach ($array as $table) {
        $outputArray = array_merge($table, $outputArray);
      }
    }
    return $outputArray;
  }

  /**
   * Renders the action field in table rows.
   *
   * a displayField function overrides the default table field display style for its column.
   *
   * @param $action
   * @param $row
   *
   * @return string
   */
  public function displayFieldAction($action, $row) {
    if (empty($this->actions)) {
      $this->actions = array();
    }
    if (!array_key_exists($action, $this->actions)) {
      $node                   = node_load($action);
      $this->actions[$action] = $node;
    }
    if (!empty($this->actions[$action])) {
      return l($this->actions[$action]->title, "node/" . $action, array("attributes" => array("title" => "Action: " . $action)));
    }
    else {
      return $action;
    }

  }

  /**
   * @param $created
   * @param $row
   *
   * @return bool|null|string
   */
  public function displayFieldCreated($created, $row) {
    if ($created == 0) {
      return t("No date set");
    }
    return date("H:i:s j F Y", $created);
  }

  public function csvFieldCreated($created, $row) {
    if ($created == 0) {
      return "";
    }
    return date("H:i:s j F Y", $created);
  }

  /**
   * @param $isSpam
   * @param $row
   *
   * @return string
   */
  public function displayFieldSpam($isSpam, $row) {
    $spamDisplay = $isSpam ? "Yes" : "No";
    return $spamDisplay;
  }

  /**
   * Display the spam property for a CSV file.
   *
   * @param $isSpam
   * @param $row
   *
   * @return mixed
   */
  public function csvFieldSpam($isSpam, $row) {
    return $this->displayFieldSpam($isSpam, $row);
  }

  /**
   * Display the UID of the user.
   *
   * @param $uid
   * @param $row
   *
   * @return mixed
   */
  public function csvFieldUid($uid, $row) {
    if (is_null($uid)) {
      if (!function_exists("campaigner_identify_user")) {
        include_once drupal_get_path("module", "campaigner") . "/includes/campaigner_signatures.inc";
      }
      $user = campaigner_identify_user($row->email, FALSE);
      if ($user) {
        return $user->uid;
      }
    }
    return $uid;
  }

  public function displayFieldUid($uid, $row) {
    if (!is_null($uid)) {
      $user = user_load($uid);
      if ($user){
      return l($user->name, "user/" . $user->uid);
      }
    }
    return $uid;
  }

  /**
   * Format the region field.
   *
   * @param $region
   * @param $row
   *
   * @return string
   */
  public function displayFieldRegion($region, $row) {
    if (empty($region)) {
      return "No region set";
    }
    if (empty($this->regions)) {
      $this->regions = array();
    }
    if (!array_key_exists($region, $this->regions)) {
      $node                   = node_load($region);
      $this->regions[$region] = $node;
    }
    if (!empty($this->regions[$region])) {
      return l($this->regions[$region]->title, "node/" . $region, array("attributes" => array("title" => "Region: " . $region)));
    }
    else {
      return $region;
    }
  }

  public function csvFieldRegion($region, $row) {
    if (empty($region)) {
      return "";
    }
    if (empty($this->regions)) {
      $this->regions = array();
    }
    if (!array_key_exists($region, $this->regions)) {
      $node                   = node_load($region);
      $this->regions[$region] = $node;
    }
    if (!empty($this->regions[$region])) {
      return $this->regions[$region]->title;
    }
    else {
      return $region;
    }
  }

  /**
   * Displays a default message or the comment left by the signee.
   *
   * @param $comment
   * @param $row
   *
   * @return null|string
   */
  function displayFieldComments($comment, $row) {
    return $comment === NULL ? t("No comment recorded") : nl2br($comment);
  }

  /**
   * Takes the currently set header row and returns a marked-up table row/thead element.
   *
   * @return string
   */
  public function renderHeader() {
    $headerRow = $this->getFieldsFromReportArray($this->rows[$this->currentReport]);
    $output    = '<thead><tr>';
    if (!empty($headerRow)) {
      foreach ($headerRow as $headerField) {
        $headerField = str_replace("_", " ", $headerField);
        $direction   = ($this->sort && $this->sort['column'] === $headerField && $this->sort['direction'] === "ASC") ? 1 : 0;
        if (in_array($headerField, $this->sortables[$this->currentReport])) {
          $output .= "<th>" . l($headerField, current_path(), array(
              "query" => array(
                "sort"      => $headerField,
                "direction" => $direction
              )
            )) . "</th>";
        }
        else {
          $output .= "<th>$headerField</th>";
        }
      }
    }
    // Add a column to allow deleting of signatures
    $output .= "</tr></thead>";
    return $output;
  }

  /**
   * A setter to specify the current page size.
   *
   * @param     $page
   * @param int $pageSize
   */
  public function setPage($page, $pageSize = 100) {
    $this->limit = array("page" => $page, "pageSize" => $pageSize);
  }

  /**
   * @param $fieldName
   *
   * @return string
   */
  protected function makeFunctionName($fieldName) {
    $methodName = "";
    $array      = explode(" ", $fieldName);
    if (!empty($array)) {
      foreach ($array as $fieldNameComponent) {
        $methodName .= ucfirst($fieldNameComponent);
      }
    }
    return $methodName;
  }

}

/**
 * Callback to allow sorting by spam rating.
 *
 * The sort is not completely trivial, marked as spam items go to the bottom. Afterwards higher spam ratings are at the top.
 *
 * @param $a
 * @param $b
 *
 * @return int
 */
function campaigner_report_sort_by_spam_rating($a, $b) {
  if ($a->spam && !$b->spam) {
    return 1;
  }
  else {
    if ($b->spam && !$a->spam) {
      return -1;
    }
  }
  return $a->SpamRating > $b->SpamRating ? -1 : 1;
}