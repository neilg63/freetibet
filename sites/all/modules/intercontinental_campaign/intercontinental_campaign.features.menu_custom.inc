<?php
/**
 * @file
 * intercontinental_campaign.features.menu_custom.inc
 */

/**
 * Implements hook_menu_default_menu_custom().
 */
function intercontinental_campaign_menu_default_menu_custom() {
  $menus = array();

  // Exported menu: menu-intercontinental.
  $menus['menu-intercontinental'] = array(
    'menu_name' => 'menu-intercontinental',
    'title' => 'Intercontinental top menu',
    'description' => 'Top menu in the Intercontinental section',
  );
  // Exported menu: menu-intercontinental-sidebar-me.
  $menus['menu-intercontinental-sidebar-me'] = array(
    'menu_name' => 'menu-intercontinental-sidebar-me',
    'title' => 'Intercontinental sidebar menu',
    'description' => 'Sidebar menu in the Intercontinental section',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('Intercontinental sidebar menu');
  t('Intercontinental top menu');
  t('Sidebar menu in the Intercontinental section');
  t('Top menu in the Intercontinental section');


  return $menus;
}
