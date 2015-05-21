<?php
/**
 * @file
 * image_styles.default_breakpoint_group.inc
 */

/**
 * Implements hook_default_breakpoint_group().
 */
function image_styles_default_breakpoint_group() {
  $export = array();

  $breakpoint_group = new stdClass();
  $breakpoint_group->disabled = FALSE; /* Edit this to true to make a default breakpoint_group disabled initially */
  $breakpoint_group->api_version = 1;
  $breakpoint_group->machine_name = 'regular';
  $breakpoint_group->name = 'regular';
  $breakpoint_group->breakpoints = array(
    0 => 'custom.user.large',
    1 => 'custom.user.medium',
    2 => 'custom.user.tablet',
    3 => 'custom.user.phone',
  );
  $breakpoint_group->type = 'custom';
  $breakpoint_group->overridden = 0;
  $export['regular'] = $breakpoint_group;

  $breakpoint_group = new stdClass();
  $breakpoint_group->disabled = FALSE; /* Edit this to true to make a default breakpoint_group disabled initially */
  $breakpoint_group->api_version = 1;
  $breakpoint_group->machine_name = 'responsive';
  $breakpoint_group->name = 'responsive';
  $breakpoint_group->breakpoints = array(
    0 => 'custom.user.large',
    1 => 'custom.user.medium',
    2 => 'custom.user.tablet',
    3 => 'custom.user.phone',
  );
  $breakpoint_group->type = 'custom';
  $breakpoint_group->overridden = 0;
  $export['responsive'] = $breakpoint_group;

  return $export;
}
