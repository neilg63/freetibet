<?php


class SpamFilter {

  protected $rawText = "";
  protected $tokens = array();
  protected $ratings = array();

  /**
   * Present a training example.
   *
   * @param      $text
   * @param bool $isSpam
   */
  public function train($text, $isSpam = TRUE){
    $tokens = $this->tokenise($text);
    $records = $this->getSpamTableRecords($tokens);
    $badTotal = variable_get("campaigner_spam_bad_total", 0);
    $goodTotal = variable_get("campaigner_spam_good_total", 0);
    if ($isSpam){
      $badTotal++;
      variable_set("campaigner_spam_bad_total", $badTotal);
    }
    else {
      $goodTotal++;
      variable_set("campaigner_spam_good_total", $goodTotal);
    }
    if (!empty($tokens)){
      foreach ($tokens as $token){
        if ($token === ""){
          continue;
        }
        if (!isset($records[$token])){
          // Create a new record for this token
          $tokenRecord = new stdClass();
          $tokenRecord->token = $token;
          $tokenRecord->spam_count = $isSpam ? 1 : 0;
          $tokenRecord->ham_count = $isSpam ? 0 : 1;
          // Default value for now will be 1 if this is a spam and 0 if not
          // 1/n / (1/n + 0/n1) = 1
          // 0/n / (0/n + 1/n1) = 0
          $tokenRecord->spam_rating = $isSpam ? 1  : 0;
          $keys = null;

        }
        else {
          $tokenRecord = $records[$token];
          // Update the existing record
          if ($isSpam)
            $tokenRecord->spam_count = $tokenRecord->spam_count + 1;
          else
            $tokenRecord->ham_count = $tokenRecord->ham_count + 1;
          $spamProbability = $tokenRecord->spam_count / $badTotal;
          $hamProbability = $tokenRecord->ham_count / $goodTotal;
          $tokenRecord->spam_rating = $spamProbability / ($hamProbability + $spamProbability);
          $keys = array("token");
        }
        drupal_write_record("spam", $tokenRecord, $keys);
        $records[$token] = $tokenRecord;
      }
    }

    // Calculate score
  }

  /**
   * Get all the records from the spam table for a list of tokens.
   *
   * @param $tokens
   *
   * @return mixed
   */
  protected function getSpamTableRecords($tokens){
    $query = db_select("spam", "s");
    $query->condition("s.token", $tokens, "IN");
    $query->fields("s", array("token", "spam_rating", "ham_count", "spam_count"));
    $result = $query->execute();
    $results = $result->fetchAllAssoc("token");
    return $results;
  }

  /**
   * Check if the provided text string is spam.
   *
   * @param $text
   * @return float|int
   */
  public function checkForSpam($text){
    $this->tokens = $this->tokenise($text);
    $this->ratings = $this->getSpamRatings($this->tokens);
    return $this->calculateMessageRating($this->ratings);
  }

  protected function calculateMessageRating($ratings){
    $spam = null;
    $notSpam = null;
    if (!empty($ratings)){
      foreach ($ratings as $rating){
        if (trim($rating['token']) === ""){
          // Ignore the "" token
          continue;
        }
        $spamRating = is_numeric($rating["rating"]) ? (float) $rating :  (float) $rating['rating']->spam_rating;
        // Make sure the rating has a value
       $ratingValue = (float) max((float)$spamRating, 0.01);
        // Calculate chance of spam
        $spam = is_null($spam) ? (float) $ratingValue : (float) $spam * (float)$ratingValue;
        // Calculate chance of not spam
        $notSpam = is_null($notSpam) ? 1 - (float) $ratingValue : (float) $notSpam * (1 - (float)$ratingValue);
      }
    }
    // Prevent divide by zero problem
    if ($spam + $notSpam == 0) {
      return 0;
    }
    return $spam / ($spam + $notSpam);
  }

  /**
   * For a list of tokens get all the spam ratings associated.
   *
   * @param       $tokens
   *
   * @param float $default
   *
   * @return array
   */
  protected function getSpamRatings($tokens, $default = 0.4){
    $outTokens = array();
    $query = db_select("spam", "s");
    $query->condition("s.token", $tokens, "IN");
    $query->fields("s", array("token", "spam_rating"));
    $result = $query->execute();
    $results = $result->fetchAllAssoc("token");
    if (!empty($tokens)){
      foreach ($tokens as $key => $token){
        if (array_key_exists($token, $results)){
          $score = $results[$token];
        }
        else {
          $score = $default;
        }
        $outTokens[$key] = array("token"=>$token, "rating"=>$score);
      }
    }
    return $outTokens;
  }

  /**
   * Take a chunk of text and splits it into tokens, returning the tokens as an array.
   *
   * @param string $text The text to tokenise
   *
   * @return array
   */
  protected function tokenise($text){
    $text = preg_replace('/\W+/',' ',$text);
    $text = preg_replace('/\s\s+/',' ',$text);
    $text = strtolower($text);
    $textArray = explode(' ', $text);
    return $textArray;
  }

  public function resetSpamTraining(){
    db_truncate("spam");
    variable_set("campaigner_spam_bad_total", 0);
    variable_set("campaigner_spam_good_total", 0);
  }

}