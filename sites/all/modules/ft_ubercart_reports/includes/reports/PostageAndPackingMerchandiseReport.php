<?php
/**
 * @file
 */

if (!class_exists("FreeTibetReport")){
  require dirname(__FILE__) . '/../FreeTibetReport.php';
}

class PostageAndPackingMerchandiseReport extends FreeTibetReport{

  protected $computerName = "merchandise_pandp";

  protected $data = array();

  public function __construct(){
    $this->name = t("Monthly breakdown of postage and packing associated with orders that include merchandise");
    $this->outputFields = array(
      t("Order ID") => array("data"=>"order_id"),
      t("Subtotal") => array("data" => "line_items", "callback"=>"freetibet_ubercart_reports_get_subtotal"),
      t("Shipping paid") => array("data"=>"line_items", "callback"=>"freetibet_ubercart_reports_get_shipping_costs"),
      t("Shipment cost") => array("data"=>"shipmentCost", "callback"=>"freetibet_ubercart_reports_format_currency"),
      t("Number of shipments") => array("data"=>"shipmentInformation", "callback"=>"count"),
    );
  }

  public function outputCSV() {
    $orderCollection = $this->getOrdersIncludingMerchandiseAndADonation();
    $orderCollection->each("freetibet_ubercart_reports_get_order_shipment_information");
    $this->collectionToCSVFile($orderCollection);
  }

}