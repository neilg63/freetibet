<?php
/**
 * @file
 * image_styles.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function image_styles_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "breakpoints" && $api == "default_breakpoint_group") {
    return array("version" => "1");
  }
  if ($module == "breakpoints" && $api == "default_breakpoints") {
    return array("version" => "1");
  }
  if ($module == "picture" && $api == "default_picture_mapping") {
    return array("version" => "2");
  }
}

/**
 * Implements hook_image_default_styles().
 */
function image_styles_image_default_styles() {
  $styles = array();

  // Exported image style: full_width_desktop.
  $styles['full_width_desktop'] = array(
    'label' => 'Full width desktop',
    'effects' => array(
      17 => array(
        'name' => 'imagefield_focus_scale_and_crop',
        'data' => array(
          'width' => 960,
          'height' => 593,
          'strength' => 'high',
        ),
        'weight' => 1,
      ),
    ),
  );

  // Exported image style: full_width_largedesktop.
  $styles['full_width_largedesktop'] = array(
    'label' => 'Full width largedesktop',
    'effects' => array(
      23 => array(
        'name' => 'imagefield_focus_scale_and_crop',
        'data' => array(
          'width' => 1360,
          'height' => 841,
          'strength' => 'high',
        ),
        'weight' => 1,
      ),
    ),
  );

  // Exported image style: full_width_phone.
  $styles['full_width_phone'] = array(
    'label' => 'Full width phone',
    'effects' => array(
      19 => array(
        'name' => 'imagefield_focus_scale_and_crop',
        'data' => array(
          'width' => 360,
          'height' => 223,
          'strength' => 'high',
        ),
        'weight' => 1,
      ),
    ),
  );

  // Exported image style: full_width_tablet.
  $styles['full_width_tablet'] = array(
    'label' => 'Full width tablet',
    'effects' => array(
      18 => array(
        'name' => 'imagefield_focus_scale_and_crop',
        'data' => array(
          'width' => 720,
          'height' => 445,
          'strength' => 'high',
        ),
        'weight' => 1,
      ),
    ),
  );

  // Exported image style: half_width_desktop.
  $styles['half_width_desktop'] = array(
    'label' => 'Half width desktop',
    'effects' => array(
      14 => array(
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => 465,
          'height' => 287,
        ),
        'weight' => 1,
      ),
    ),
  );

  // Exported image style: half_width_largedesktop.
  $styles['half_width_largedesktop'] = array(
    'label' => 'Half width largedesktop',
    'effects' => array(
      16 => array(
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => 701,
          'height' => 432,
        ),
        'weight' => 1,
      ),
    ),
  );

  // Exported image style: half_width_tablet.
  $styles['half_width_tablet'] = array(
    'label' => 'Half width tablet',
    'effects' => array(
      15 => array(
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => 351,
          'height' => 263,
        ),
        'weight' => 1,
      ),
    ),
  );

  // Exported image style: large_desktop.
  $styles['large_desktop'] = array(
    'label' => 'Large desktop',
    'effects' => array(
      1 => array(
        'name' => 'image_scale',
        'data' => array(
          'width' => 960,
          'height' => 960,
          'upscale' => 0,
        ),
        'weight' => 1,
      ),
    ),
  );

  // Exported image style: large_largedesktop.
  $styles['large_largedesktop'] = array(
    'label' => 'Large largedesktop',
    'effects' => array(
      11 => array(
        'name' => 'image_scale',
        'data' => array(
          'width' => 1360,
          'height' => 1360,
          'upscale' => 0,
        ),
        'weight' => 1,
      ),
    ),
  );

  // Exported image style: large_phone.
  $styles['large_phone'] = array(
    'label' => 'Large phone',
    'effects' => array(
      3 => array(
        'name' => 'image_scale',
        'data' => array(
          'width' => 640,
          'height' => 640,
          'upscale' => 0,
        ),
        'weight' => 1,
      ),
    ),
  );

  // Exported image style: large_tablet.
  $styles['large_tablet'] = array(
    'label' => 'Large tablet',
    'effects' => array(
      2 => array(
        'name' => 'image_scale',
        'data' => array(
          'width' => 720,
          'height' => 720,
          'upscale' => 0,
        ),
        'weight' => 1,
      ),
    ),
  );

  // Exported image style: regular_desktop.
  $styles['regular_desktop'] = array(
    'label' => 'Regular desktop',
    'effects' => array(
      20 => array(
        'name' => 'imagefield_focus_scale_and_crop',
        'data' => array(
          'width' => 300,
          'height' => 186,
          'strength' => 'high',
        ),
        'weight' => 2,
      ),
    ),
  );

  // Exported image style: regular_largedesktop.
  $styles['regular_largedesktop'] = array(
    'label' => 'Regular largedesktop',
    'effects' => array(
      21 => array(
        'name' => 'imagefield_focus_scale_and_crop',
        'data' => array(
          'width' => 465,
          'height' => 287,
          'strength' => 'high',
        ),
        'weight' => 2,
      ),
    ),
  );

  // Exported image style: regular_tablet.
  $styles['regular_tablet'] = array(
    'label' => 'Regular Tablet',
    'effects' => array(
      22 => array(
        'name' => 'imagefield_focus_scale_and_crop',
        'data' => array(
          'width' => 360,
          'height' => 223,
          'strength' => 'high',
        ),
        'weight' => 2,
      ),
    ),
  );

  return $styles;
}
