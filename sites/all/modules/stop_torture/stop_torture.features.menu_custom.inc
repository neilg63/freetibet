<?php
/**
 * @file
 * stop_torture.features.menu_custom.inc
 */

/**
 * Implements hook_menu_default_menu_custom().
 */
function stop_torture_menu_default_menu_custom() {
  $menus = array();

  // Exported menu: menu-stop-torture.
  $menus['menu-stop-torture'] = array(
    'menu_name' => 'menu-stop-torture',
    'title' => 'Stop torture',
    'description' => 'Stop torture section navigation',
  );
  // Exported menu: menu-stop-torture-sub-menu.
  $menus['menu-stop-torture-sub-menu'] = array(
    'menu_name' => 'menu-stop-torture-sub-menu',
    'title' => 'Stop torture sub-menu',
    'description' => '',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('Stop torture');
  t('Stop torture section navigation');
  t('Stop torture sub-menu');


  return $menus;
}
