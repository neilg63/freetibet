<?php

/**
 * Implements hook_views_data_alter().
 */
function ft_checkoutlink_views_data_alter(&$data) {
  $data['commerce_product']['express_checkout_link'] = array(
    'title' => t('Express checkout link'),
    'help' => t('Add an express checkout link button.'),
    'real field' => 'product_id',
    'field' => array(
      'handler' => 'ft_checkoutlink_handler_field',
    ),
  );
}
