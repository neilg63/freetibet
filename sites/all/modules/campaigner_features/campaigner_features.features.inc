<?php
/**
 * @file
 * campaigner_features.features.inc
 */

/**
 * Implements hook_node_info().
 */
function campaigner_features_node_info() {
  $items = array(
    'campaigner_email_target' => array(
      'name' => t('Campaigner Email Target'),
      'base' => 'node_content',
      'description' => t('A campaign action where supporters send email to one or more targets'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'campaigner_petition' => array(
      'name' => t('Campaigner Petition'),
      'base' => 'node_content',
      'description' => t('A campaign action where supporters sign a petition'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'campaigner_region' => array(
      'name' => t('Campaigner Region'),
      'base' => 'node_content',
      'description' => t('This is a special content type used by the Campaigner module to represent geographical regions.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
