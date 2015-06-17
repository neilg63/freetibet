<?php

/*
* Override for media 2.0's theme_media_thumbnail required to replace thumbnail images with SVGs, which are naturally scalable. Should work with media 1.x series too
*/
function svgimg_theme_media_thumbnail($variables) {
  $label = '';
  $element = $variables['element'];

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