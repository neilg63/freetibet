<?php
/**
 * @file
 * Theme functions for the Media: Thinglink module.
 */

function theme_media_thinglink_preview_style($variables) {
  $variables['uri'] = media_thinglink_variable_get('preview_uri');
  return theme('media_thinglink_embed', $variables);
}

function theme_media_thinglink_styles($variables) {
  $style = $variables['style'];
  $variables['file'] = $variables['object'];
  $variables['uri'] = $variables['object']->uri;
  $variables['style_name'] = $style['name'];
  return theme('media_thinglink_embed', $variables);
}

function theme_media_thinglink_embed($variables) {
  if(isset($variables['preset']) && isset($variables['object'])){
    //get preset information from the variables object
    $preset_name = $variables[(string)'preset'];
    $all_presets = $variables[(string)'presets'];

    $preset = array(
      'effects' => $all_presets[(string)'presets'][(string)$preset_name],
    );

    // Build the URL for display.
    $uri = $variables['uri'];
    $wrapper = file_stream_wrapper_get_instance_by_uri($uri);
    $parts = $wrapper->get_parameters();

    $width = $height = 0;

    foreach ($preset['effects'] as $effect) {
      switch ($effect['name']) {
        case 'resize':
          $width = isset($effect['settings']['width']) ? $effect['settings']['width'] : NULL;
          $height = isset($effect['settings']['height']) ? $effect['settings']['height'] : NULL;
          break;
      }
    }
    if (isset($variables['object']->override)) {
      $override = $variables['object']->override;
      if (isset($override['width'])) {
        $width = $override['width'];
      }
      if (isset($override['height'])) {
        $height = $override['height'];
      }
      if (isset($override['thumbnail'])) {
        unset($variables['photoset']);
        unset($variables['search']);
      }
    }

    $attributes = array();
    if ($width) {
      $attributes['width'] = $width;
    }
    if ($height) {
      $attributes['height'] = $height;
    }

    $output = theme('image', array(
      'path' => media_thinglink_image_url($parts),
      'alt' => $variables['alt'],
      'title' => $variables['title'],
      'attributes' => $attributes,
      'getsize' => FALSE,
    ));
    return $output;
  }
}

function theme_media_thinglink_field_formatter_styles($variables) {
  $element = $variables['element'];
  $style = $variables['style'];
  $variables['file'] = $element['#item'];
  $variables['uri'] = $variables['file']['uri'];
  $variables['style_name'] = $style['name'];
  return theme('media_thinglink_embed', $variables);
}
