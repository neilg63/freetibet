<?php
/**
 * @file
 * image_styles.default_picture_mapping.inc
 */

/**
 * Implements hook_default_picture_mapping().
 */
function image_styles_default_picture_mapping() {
  $export = array();

  $picture_mapping = new PictureMapping();
  $picture_mapping->disabled = FALSE; /* Edit this to true to make a default picture_mapping disabled initially */
  $picture_mapping->api_version = 2;
  $picture_mapping->label = 'Full width';
  $picture_mapping->machine_name = 'full_width';
  $picture_mapping->breakpoint_group = 'regular';
  $picture_mapping->mapping = array(
    'custom.user.large' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'full_width_largedesktop',
      ),
    ),
    'custom.user.medium' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'full_width_desktop',
      ),
    ),
    'custom.user.tablet' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'full_width_tablet',
      ),
    ),
    'custom.user.phone' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'full_width_phone',
      ),
    ),
  );
  $export['full_width'] = $picture_mapping;

  $picture_mapping = new PictureMapping();
  $picture_mapping->disabled = FALSE; /* Edit this to true to make a default picture_mapping disabled initially */
  $picture_mapping->api_version = 2;
  $picture_mapping->label = 'Half width';
  $picture_mapping->machine_name = 'half_width';
  $picture_mapping->breakpoint_group = 'regular';
  $picture_mapping->mapping = array(
    'custom.user.large' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'half_width_largedesktop',
      ),
    ),
    'custom.user.medium' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'half_width_desktop',
      ),
    ),
    'custom.user.tablet' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'half_width_tablet',
      ),
    ),
    'custom.user.phone' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'half_width_tablet',
      ),
    ),
  );
  $export['half_width'] = $picture_mapping;

  $picture_mapping = new PictureMapping();
  $picture_mapping->disabled = FALSE; /* Edit this to true to make a default picture_mapping disabled initially */
  $picture_mapping->api_version = 2;
  $picture_mapping->label = 'Large';
  $picture_mapping->machine_name = 'large';
  $picture_mapping->breakpoint_group = 'responsive';
  $picture_mapping->mapping = array(
    'custom.user.large' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'large_largedesktop',
      ),
    ),
    'custom.user.medium' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'large_desktop',
      ),
    ),
    'custom.user.tablet' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'large_tablet',
      ),
    ),
    'custom.user.phone' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'large_phone',
      ),
    ),
  );
  $export['large'] = $picture_mapping;

  $picture_mapping = new PictureMapping();
  $picture_mapping->disabled = FALSE; /* Edit this to true to make a default picture_mapping disabled initially */
  $picture_mapping->api_version = 2;
  $picture_mapping->label = 'Third width';
  $picture_mapping->machine_name = 'regular';
  $picture_mapping->breakpoint_group = 'regular';
  $picture_mapping->mapping = array(
    'custom.user.large' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'regular_largedesktop',
      ),
    ),
    'custom.user.medium' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'regular_desktop',
      ),
    ),
    'custom.user.tablet' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'regular_tablet',
      ),
    ),
    'custom.user.phone' => array(
      '1x' => array(
        'mapping_type' => 'image_style',
        'image_style' => 'regular_desktop',
      ),
    ),
  );
  $export['regular'] = $picture_mapping;

  return $export;
}
