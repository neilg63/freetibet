<?php

/*
* Override for media 2.0's theme_media_thumbnail required to replace thumbnail images with SVGs, which are naturally scalable. Should work with media 1.x series too
*/
function svgimg_theme_media_thumbnail($variables) {
  $label = '';
  $element = $variables['element'];
  if (!isset($element['#name'])) {
    $element['#name'] = $element['#file']->filename;
  }
 // Wrappers to go around the thumbnail.
  $attributes = array(
    'title' => $element['#name'],
    'class' => 'media-item',
    'data-fid' => $element['#file']->fid,
  );
  $prefix = '<div ' . drupal_attributes($attributes) . '><div class="media-thumbnail">';
  $suffix = '</div></div>';

  // Arguments for the thumbnail link.
  $thumb = $element['#children'];
  if (isset($element['#file']) && is_object($element['#file'])) {
    $file = $element['#file'];
    if (isset($file->filemime)) {
      if (strpos($file->filemime,'image/svg') === 0) {
        $element['#style_name'] = NULL;
        $thumb = preg_replace('#styles/[a-z0-9_-]+/(public|private)/#i','',$thumb);
      }
    }
  }
  $may_edit = true;
  if (function_exists('file_entity_access')) {
    $may_edit = file_entity_access('update', $element['#file']);
  }
  if ($may_edit) {
    $target = 'file/' . $element['#file']->fid . '/edit';
    $title = t('Click to edit details');
  }
  else {
    $target = 'file/' . $element['#file']->fid;
    $title = t('Click to view details');
  }
  $options = array(
    'query' => drupal_get_destination(),
    'html' => TRUE,
    'attributes' => array('title' => $title),
  );

  // Element should be a field renderable array. This should be improved.
  if (!empty($element['#show_names']) && $element['#name']) {
    $label = '<div class="label-wrapper"><label class="media-filename">' . $element['#name'] . '</label></div>';
  }

  $output = $prefix;
  if (!empty($element['#add_link'])) {
    $output .= l($thumb, $target, $options);
  }
  else {
    $output .= $thumb;
  }
  $output .= $label . $suffix;
  return $output;
}

/*
* Override for Drupal core theme_image_widget to render SVG Images without styles
*/
function svgimg_theme_image_widget($variables) {
  $element = $variables['element'];
  $output = '';
  $output .= '<div class="image-widget form-managed-file clearfix">';

  if (isset($element['preview'])) {
    $output .= '<div class="image-preview">';
    $vars = array('height' => 100,'width' => 100,'attributes' => array('style' => 'height:auto'));
    $vars['path'] = $element['#file']->uri;
    $vars['alt'] = $element['#file']->alt;
    $vars['title'] = $element['#file']->title;
    $element['preview']['#markup'] = theme('image', $vars);
    $output .= drupal_render($element['preview']);
    $output .= '</div>';
  }

  $output .= '<div class="image-widget-data">';
  if ($element['fid']['#value'] != 0) {
    $element['filename']['#markup'] .= ' <span class="file-size">(' . format_size($element['#file']->filesize) . ')</span> ';
  }
  $output .= drupal_render_children($element);
  $output .= '</div>';
  $output .= '</div>';

  return $output;
}

