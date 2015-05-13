<?php
/**
 * @file
 * stop_torture.features.inc
 */

/**
 * Implements hook_views_api().
 */
function stop_torture_views_api() {
  return array("version" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function stop_torture_node_info() {
  $items = array(
    'q_a_page' => array(
      'name' => t('Q&A page'),
      'base' => 'node_content',
      'description' => t('Question and Answers page'),
      'has_title' => '1',
      'title_label' => t('Question'),
      'help' => '',
    ),
    'survivor' => array(
      'name' => t('Survivor'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Name'),
      'help' => '',
    ),
    'video' => array(
      'name' => t('Video'),
      'base' => 'node_content',
      'description' => t('Video testimony'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
