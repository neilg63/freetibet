<?php

/**
 * @file
 */

/**
 * Implements hook_file_formatter_info().
 */
function media_thinglink_file_formatter_info() {
  $formatters['media_thinglink_image'] = array(
    'label' => t('Thinglink Preview Image'),
    'file types' => array('image'),
    'default settings' => array(
      'image_style' => '',
      'link_to_thinglink' => FALSE,
    ),
    'view callback' => 'media_thinglink_file_formatter_image_view',
    'settings callback' => 'media_thinglink_file_formatter_image_settings',
  );
  return $formatters;
}

/**
 * Implements hook_file_formatter_FORMATTER_view().
 */
function media_thinglink_file_formatter_image_view($file, $display, $langcode) {
  $scheme = file_uri_scheme($file->uri);
  if ($scheme == 'thinglink') {
    $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri);
    $image_style = $display['settings']['image_style'];
    $valid_image_styles = image_style_options(FALSE);
    if (empty($image_style) || !isset($valid_image_styles[$image_style])) {
      $element = array(
        '#theme' => 'image',
        '#path' => $wrapper->getOriginalThumbnailPath(),
      );
    }
    else {
      $element = array(
        '#theme' => 'image_style',
        '#style_name' => $image_style,
        '#path' => $wrapper->getLocalThumbnailPath(),
      );
    }
    if ($display['settings']['link_to_thinglink']) {
      $element = array(
        '#theme' => 'link',
        '#text' => drupal_render($element),
        '#path' => media_thinglink_original_url($file->uri),
        '#options' => array(
          'attributes' => array(),
          'html' => TRUE,
        ),
      );
    }
    return $element;
  }
}

/**
 * Implements hook_file_formatter_FORMATTER_settings().
 */
function media_thinglink_file_formatter_image_settings($form, &$form_state, $settings) {
  $element = array();
  $element['image_style'] = array(
    '#title' => t('Image style'),
    '#type' => 'select',
    '#options' => image_style_options(FALSE),
    '#default_value' => $settings['image_style'],
    '#empty_option' => t('None (original image)'),
  );
  $element['link_to_thinglink'] = array(
    '#title' => t('Link to Thinglink'),
    '#type' => 'checkbox',
    '#default_value' => $settings['link_to_thinglink'],
  );
  return $element;
}

/**
 * Implements hook_file_default_displays().
 */
function media_thinglink_file_default_displays() {
  $default_displays = array();

  // Default settings for displaying a video preview image. We enable preview
  // images even for view modes that also play video, for use inside a running
  // WYSIWYG editor. The higher weight ensures that the video display is used
  // where possible.
  $default_image_styles = array(
    'media_preview' => 'square_thumbnail',
    'media_large' => 'large',
    'media_original' => ''
  );
  foreach ($default_image_styles as $view_mode => $image_style) {
    $display_name = 'image__' . $view_mode . '__media_thinglink_image';
    $default_displays[$display_name] = (object) array(
      'api_version' => 1,
      'name' => $display_name,
      'status' => 1,
      'weight' => 2,
      'settings' => array('image_style' => $image_style),
    );
  }

  return $default_displays;
}

