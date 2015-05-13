<?php
/**
 * @file
 */

if (!class_exists("FreeTibetReport")){
  require dirname(__FILE__) . '/../FreeTibetReport.php';
}
	class DonationsOrdersReport extends FreeTibetReport{
	
	  protected $computerName = "donation_orders";
	
	  protected $data = array();
	
	  public function __construct(){
	    $this->name = t("Monthly breakdown of personal details associated with orders that include donations");
	    $this->outputFields = array(
	      t("Order ID") => array("data"=>"order_id"),
	      t("Email address") => array("data"=>'primary_email', 'anonymise'=>TRUE),
	      t("First name - Delivery") => array("data"=>'delivery_first_name', 'anonymise'=>TRUE),
	      t("Last name - Delivery") => array("data"=>'delivery_last_name', 'anonymise'=>TRUE),
	      t("Phone - Delivery") => array("data"=>'delivery_phone', 'anonymise'=>TRUE),
	      t("Company - Delivery") => array("data"=>'delivery_company', 'anonymise'=>TRUE),
	      t("Street 1 - Delivery") => array("data"=>'delivery_street1', 'anonymise'=>TRUE),
	      t('Street 2  - Delivery') => array("data"=>'delivery_street2', 'anonymise'=>TRUE),
	      t("City - Delivery") => array("data"=>'delivery_city', 'anonymise'=>TRUE),
	      t("Zone - Delivery") => array("data"=>'delivery_zone', "callback"=>"freetibet_ubercart_reports_format_zone", 'anonymise'=>TRUE),
	      t("Postal Code - Delivery") => array("data"=>'delivery_postal_code', 'anonymise'=>TRUE),
	      t("Country - Delivery") => array("data"=>'delivery_country', "callback"=>"freetibet_ubercart_reports_format_country", 'anonymise'=>TRUE),
	      t("First name - Billing") => array("data"=>'billing_first_name', 'anonymise'=>TRUE),
	      t("Last name - Billing") => array("data"=>'billing_last_name', 'anonymise'=>TRUE),
	      t("Phone - Billing") => array("data"=>'billing_phone', 'anonymise'=>TRUE),
	      t("Company - Billing") => array("data"=>'billing_company', 'anonymise'=>TRUE),
	      t("Street 1 - Billing") => array("data"=>'billing_street1', 'anonymise'=>TRUE),
	      t('Street 2  - Billing') => array("data"=>'billing_street2', 'anonymise'=>TRUE),
	      t("City - Billing") => array("data"=>'billing_city', 'anonymise'=>TRUE),
	      t("Zone - Billing") => array("data"=>'billing_zone', "callback"=>"freetibet_ubercart_reports_format_zone", 'anonymise'=>TRUE),
	      t("Postal Code - Billing") => array("data"=>'billing_postal_code', 'anonymise'=>TRUE),
	      t("Country - Billing") => array("data"=>'billing_country',"callback"=>"freetibet_ubercart_reports_format_country", 'anonymise'=>TRUE),
	      t("Date created") => array("data"=>'created', "callback" => "freetibet_ubercart_reports_format_date")
	    );
	  }
	
	  public function outputCSV() {
	    $orderCollection = $this->getOrdersIncludingADonation();
	    $this->collectionToCSVFile($orderCollection);
	  }
	
	
}

