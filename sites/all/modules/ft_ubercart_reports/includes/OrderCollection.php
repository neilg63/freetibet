<?php
/**
 * @file
 */

if (!class_exists("OrderModel")) {
  require dirname(__FILE__) . '/OrderModel.php';
}
/**
 * An OrderCollection contains a set of OrderModel objects and allows operations to be performed on them as a group.
 */
class OrderCollection {

  protected $collected = array();

  public function __construct($orders = array()) {
    $this->phpVersion = floatval(phpversion());
    if (!empty($orders))
      $this->collected = $orders;
  }

  /**
   * @param $records
   */
  public function addSeveral($records) {
    if (!empty($records)) {
      foreach ($records as $key => $record) {
        $model                              = new OrderModel($record);
        $this->collected[$record->order_id] = $model;
      }
    }
  }

  /**
   * Simple wrapper around array_filter.
   *
   * It is the caller's responsibility to ensure that the Callable item is executable.
   *
   * @param Callable $callback
   *
   * @return OrderCollection
   */
  public function filter($callback) {
    $this->collected = array_filter($this->collected, $callback);
    return $this;
  }

  /**
   * Takes a Callable item and runs array_map with that item.
   *
   * It is the caller's responsibility to ensure that the Callable item is executable.
   *
   * Unlikely most other methods this does not return the collection but the contents of the collection, post-map.
   *
   * @param Callable $closure
   *
   * @return array
   */
  public function map($closure) {
    return array_map($closure, $this->collected);
  }

  /**
   * Runs array_walk on the collections contents, mimicking each in a functional language.
   *
   * @param Callback $closure
   *
   * @return OrderCollection
   */
  public function each($closure){
    array_walk($this->collected, $closure);
    return $this;
  }

  /**
   * Get all the orders in a given time range.
   *
   * @param int  $start_date The UNIX timestamp to start with, defaults to 0
   * @param null $end_date The UNIX timestamp for the latest date to be included, defaults to the current time
   *
   * @return OrderCollection
   */
  public static function getOrders($start_date = 0, $end_date = NULL) {
    if ($end_date === NULL)
      $end_date = time();
    // ASSUMPTION: We are interested in creation rather than modified date
    $orderQuery = db_select("uc_orders", "o")->fields("o")->condition("created", array($start_date, $end_date), "between")->condition('order_status', 'abandoned', '<>')->execute();
    $collection = new OrderCollection;
    $collection->addSeveral($orderQuery);
    return $collection;
  }

}
