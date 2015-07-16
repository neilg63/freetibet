<?php

/**
* @file
* Views field handler. Adds the add to cart radio field to the view.
* Implements the Views Form API.
*/

class ft_checkoutlink_handler_field extends views_handler_field {
  
  function construct() {
      parent::construct();
      $this->additional_fields['field_something'] = array(
        'table' => 'commerce_product',
        'field' => 'product_id',
      );
    }
  
  function render($values) {
    $label = variable_get('ft_checkoutlink_label',t("Pay") );
    $label_long = variable_get('ft_checkoutlink_title',t("Proceed to express checkout") );
    if (isset($this->view->result[$this->view->row_index]) && is_object($this->view->result[$this->view->row_index])) {
      $product_id = $this->view->result[$this->view->row_index]->product_id;
      $hash = drupal_hmac_base64($product_id, drupal_get_private_key().drupal_get_hash_salt());
      $opts = array(
        'attributes' => array('title' => $label_long, 'class' => array('checkout-button')),
      );
      return l($label, 'commerce-express-checkout/' . $product_id . '/' . $hash, $opts);
    }
  }

  function option_definition() {
    $options = parent::option_definition();

    return $options;
  }
}
