<?php
/**
 * @file
 * campaigner_country_regions.features.inc
 */

/**
 * Implements hook_node_info().
 */
function campaigner_country_regions_node_info() {
  $items = array(
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
