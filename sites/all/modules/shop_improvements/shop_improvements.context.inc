<?php
/**
 * @file
 * shop_improvements.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function shop_improvements_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'shop_activemenu';
  $context->description = 'Shop active menu';
  $context->tag = 'shop';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'donation' => 'donation',
        'product' => 'product',
        'product_kit' => 'product_kit',
      ),
      'options' => array(
        'node_form' => '0',
      ),
    ),
    'path' => array(
      'values' => array(
        'shop/*' => 'shop/*',
        'cart' => 'cart',
        'cart/*' => 'cart/*',
      ),
    ),
  );
  $context->reactions = array(
    'menu' => 'node/1274',
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Shop active menu');
  t('shop');
  $export['shop_activemenu'] = $context;

  return $export;
}
