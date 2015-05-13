<?php
/**
 * @file
 */

if (!interface_exists("FreeTibetReportInterface")){
  require dirname(__FILE__) . '/FreeTibetReportInterface.php';
}
/**
 * A basic class that all reports should extend, this allows us to get basic information that is set on the descendant
 * class and return it to the relevant controller.
 */
abstract class FreeTibetReport implements FreeTibetReportInterface{

  protected $name = NULL;
  /**
   * @var null|string This should be set to a value in descendant classes in order to define the unique name of this report.
   */
  protected $computerName = NULL;

  protected $reportStart = 0;
  /**
   * @var array The outputFields array is used to encode the output that the CSV will have. The keys of the array are the
   *            formatting headings for the file. The value is either simply the property of the OrderModel that should
   *            be displayed on each row or an array with the name of a function to format the value and the property of
   *            the OrderModel to pass in to that function.
   */
  protected $outputFields = array();

  protected $year = 0;

  protected $month = 0;

  protected $reportEnd = 0;

  protected $anonymous = FALSE;

  public function getName(){
    return $this->name;
  }

  /**
   * Can't be abstract in PHP 5.2 so this is not abstract but now does nothing, you should still override this for a successful report.
   * @return mixed|void
   */
  public function outputCSV(){}

  public function setDates($month, $year){
    $this->month = $month;
    $this->year = $year;
    $this->reportStart = $this->dayMonthYearToTimestamp(1, $month, $year);
    $this->reportEnd = $this->dayMonthYearToTimestamp(cal_days_in_month(CAL_GREGORIAN, $month, $year), $month, $year);
  }
  /**
   * Provides caller with the computer name of the report (should be unique - behaviour undefined otherwise).
   *
   * Uses the computerName property of the class to do this.
   * @return mixed
   */
  public function getComputerName(){
    return $this->computerName;
  }

  /**
   * Takes a model and uses currently defined rules for generating a CSV row, produces an output row.
   * @param OrderModel $model The order to generate a row for
   *
   * @return array The output row
   */
  public function mapRow($model){
    $data = array();
    if (!empty($this->outputFields)){
      foreach ($this->outputFields as $field => $value){
        $dataItem = $model->$value['data'];
        if (is_array($value) && isset($value['callback']) && is_callable($value['callback'])){
          $dataItem = $value['callback']($dataItem);
        }
        if ($this->anonymous && isset($value['anonymise']) && $value['anonymise'] === TRUE){
          $dataItem = substr($dataItem, 0, 1) . "xxx";
        }
        $data[] = $dataItem;

      }
    }
    return $data;
  }

  /**
   * Takes a collection of OrderModel objects and outputs them as a CSV file.
   *
   * @param OrderCollection $collection
   */
  protected function collectionToCSVFile(OrderCollection $collection){
    $filename = $this->getFilename();
    $rows = $collection->map(array($this, "mapRow"));
    $header = array_keys($this->outputFields);
    drupal_add_http_header("Content-Type", "text/csv");
    drupal_add_http_header("Content-Disposition", "attachment;filename=" . $filename);
    $stdOut = fopen('php://output', "w");
    fputcsv($stdOut, $header);
    if (!empty($rows)){
      foreach ($rows as $row){
        fputcsv($stdOut, $row);
      }
    }
    fclose($stdOut);
    drupal_exit();
  }

  /**
   * Get all orders.
   *
   * Currently basically a passthrough for getAllOrdersCollection but maybe we'll do something different in the future.
   *
   * @return OrderCollection
   */
  protected function getOrders(){
    $collection = $this->getAllOrdersCollection();
    return $collection;
  }

  /**
   * Finds all orders that have at least one piece of merchandise.
   *
   * @return OrderCollection
   */
  protected function getOrdersIncludingMerchandise(){
    $collection = $this->getAllOrdersCollection();
    $collection->filter("freetibet_ubercart_reports_order_contains_merchandise");
    return $collection;
  }

  /**
   * Finds all orders that have at least one donation.
   *
   * @return OrderCollection
   */
  protected function getOrdersIncludingADonation(){
    $collection = $this->getAllOrdersCollection();
    $collection->filter("freetibet_ubercart_reports_order_contains_donation");
    return $collection;
  }

  /**
   * Finds all orders that have at least one donation and no merchandise.
   *
   * @return OrderCollection
   */
  protected function getOrdersIncludingADonationAndNoMerchandise(){
    $collection = $this->getAllOrdersCollection();
    $collection->filter("freetibet_ubercart_reports_order_contains_donation")->filter("freetibet_ubercart_reports_order_contains_no_merchandise");
    return $collection;
  }

  /**
   * @return OrderCollection|void
   */
  protected function getOrdersIncludingMerchandiseAndADonation(){
    $collection = $this->getAllOrdersCollection();
    $collection->filter("freetibet_ubercart_reports_order_contains_donation")->filter("freetibet_ubercart_reports_order_contains_merchandise");
    return $collection;
  }

  protected function getAllOrdersCollection(){
    if (!class_exists("OrderCollection")){
      require dirname(__FILE__) . '/OrderCollection.php';
    }
    $collection = OrderCollection::getOrders($this->reportStart, $this->reportEnd);
    return $collection;
  }

  /**
   * @param $day
   * @param $month
   * @param $year
   *
   * @return int
   */
  protected function dayMonthYearToTimestamp($day, $month, $year){
    return mktime(0, 0, 0, $month, $day, $year);
  }

  /**
   * Takes the year and month
   *
   * @return string
   */
  public function getFilename(){
    $dateString = strtolower(date("FY", $this->reportStart));
    return $this->computerName . "_" . $dateString;
  }

}
