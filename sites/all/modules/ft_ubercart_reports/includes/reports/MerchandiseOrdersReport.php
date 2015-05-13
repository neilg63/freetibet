<?php
/**
 * @file
 */

if (!class_exists("DonationsOrdersReport")){
  require dirname(__FILE__) . '/DonationsOrdersReport.php';
}

class MerchandiseOrdersReport extends DonationsOrdersReport{

  protected $computerName = "merchandise_orders";

  protected $data = array();

  public function __construct(){
    parent::__construct();
    $this->name = t("Monthly breakdown of personal details associated with orders that include merchandise");
  }

  public function outputCSV() {
    $orderCollection = $this->getOrdersIncludingMerchandise();
    $this->collectionToCSVFile($orderCollection);
  }


}
