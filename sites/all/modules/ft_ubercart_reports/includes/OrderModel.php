<?php
/**
 * @file
 */

/**
 * An OrderModel contains information about a single specific ubercart order.
 */
class OrderModel {

  protected $hasDonation = NULL;

  protected $hasMerchandise = NULL;

  protected $shipmentInformation = array();

  protected $shipmentCost = 0.00;

  public static $schema = array();

  /**
   * Standard constructor that allows a database row to be passed in.
   * @param stdClass $record
   */
  public function __construct($record){
    $this->load($record);
    $this->loadOrder();
  }

  /**
   * Takes a database record and unpacks it into the OrderModel.
   * @param stdClass $record
   */
  protected function load($record){
    if (!empty($record)){
      foreach ($record as $key => $value){
        if (array_key_exists($key, OrderModel::$schema)){
          $this->$key = $value;
        }
      }
    }
  }

  /**
   * Checks whether the current order includes an item that could be considered a donation.
   * @return bool|null
   */
  public function includesDonation(){
    if ($this->hasDonation === NULL){
      $donate = $this->hasProductType("donation");
      $this->hasDonation = $donate;
    }
    return $this->hasDonation;
  }

  /**
   * Does the current order have a product of the type provided by the user.
   * @param string $type The type
   *
   * @return bool Whether the standard product type
   */
  protected function hasProductType($type){
    if (!empty($this->products)){
      foreach ($this->products as $product){
        if (isset($product->data['type']) && $product->data['type'] === $type){
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  /**
   * Does the current order have an item that counts as merchandise in it.
   * @return bool|null
   */
  public function includesMerchandise(){
    if ($this->hasMerchandise === NULL){
      $merch = $this->hasProductType("product");
      $this->hasMerchandise = $merch;
    }
    return $this->hasMerchandise;
  }

  /**
   * Find the shipping information for the current order.
   */
  public function fetchShipmentInformation(){
    if (isset($this->order_id) && empty($this->shipmentInformation)){
      $query = db_query("SELECT sid, order_id, cost FROM {uc_shipments} WHERE order_id = :order", array(":order"=>$this->order_id))->fetchAll();
      $this->shipmentCost = 0.00;
      if (!empty($query)){
        foreach ($query as $result){
          $this->shipmentCost += (float) $result->cost;
          $this->shipmentInformation[] = $result;
        }
      }
    }
  }

  /**
   * We use the uc_order_load to load all of the details about a given order (so that we can find products, line_items etc.).
   */
  protected function loadOrder(){
    if (isset($this->order_id)){
      $order = uc_order_load($this->order_id);
      // @todo check we have data to add before adding
      $this->data = $order->data;
      $this->products = $order->products;
      $this->line_items = $order->line_items;
    }
  }

  /**
   * Uses information from the uc_order module's schema hook to work out what fields we should allow on this model.
   */
  public static function populateSchema(){
    if (!function_exists("uc_order_schema")){
      module_load_include("install", "uc_order");
    }
    $schema = uc_order_schema();
    $ucOrdersDefinition = $schema["uc_orders"];
    OrderModel::$schema = $ucOrdersDefinition["fields"];
    // Add on empty data for the included shipment information
    // We don't need any of the schema information as these will never touch the database.
    OrderModel::$schema["shipmentCost"] = array();
    OrderModel::$schema["shipmentInformation"] = array();
  }

  public function __get($key){
    if (array_key_exists($key, OrderModel::$schema)){
      return $this->$key;
    }
    return NULL;
  }

}

if (empty(OrderModel::$schema)){
  OrderModel::populateSchema();
}