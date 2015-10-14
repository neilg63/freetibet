<?php

/*
* Render an SVG image inline
* @param array &$items
* @param array $display
*/
function svgimg_build_element(&$items,$display) {
  $element = array();
  if (!empty($items) && is_array($items)) {
    foreach ($items as $index => $item) {
      $path = drupal_realpath($item['uri']);
      if (file_exists($path)) {
        $xml = file_get_contents($path);
        $settings = $display['settings'];
        $wrapper = !empty($settings['wrapper_tag'])? $settings['wrapper_tag'] : 'figure';
        $classes = array('svg-object');
        if (is_string($settings['classes'])) {
          if (!empty($settings['classes'])) {
            $classes = array_merge($classes, explode(' ', $settings['classes']));
          }
        }
        $attrs = array(
          'class' => $classes
        );
        $attrs['data-src'] = url(svgimg_uri($item['uri'],NULL));
        $render_map = array(
          '#markup' => '<' . $wrapper . drupal_attributes($attrs).'>' . $xml . '</'. $wrapper .'>',
        );
        if (isset($settings['add_script'])) {
          $settings['add_script'] = (bool) $settings['add_script'];
        }
        else {
          $settings['add_script'] = false;
        }
        if ($settings['add_script']) {
           $render_map['#attached'] = array(
            'js' => array(drupal_get_path('module', 'svgimg') . '/js/svgimg.js' )
          );
        }
        $element[] = $render_map;
      }
    }
  }
  return $element;
}

/*
* Look around image field items to override display for SVG images
* called by svgimg_field_attach_view_alter
* @param array &$field_data
* @param init $count
*/
function svgimg_image_svg_loop_items(array &$field_data,$count = 0) {
  $matched = false;
  for ($i=0; $i < $count; $i++) {
    $item_matched = false;
    if (isset($field_data[$i]['#item'])) {
      $item_matched = _svgimg_apply_field_item_override($field_data[$i]['#item'], $field_data, $i, false);
    }
    else if (isset($field_data[$i]['file'])) {
      $item_matched = _svgimg_apply_field_item_override($field_data[$i]['file']['#item'], $field_data, $i, true);
    }
    if ($item_matched) {
      $matchhed = true;
    }
  }
  if ($matched) {
    if (!isset($field_data['#attached']['js'])) {
      $field_data['#attached']['js'] = array();
    }
    $field_data['#attached']['js'][] = drupal_get_path('module', 'svgimg') . '/js/svgimg.js';
  }
}

/*
* @param array &$item
* @param array &$field_data
* @param int $delta
* @param bool $use_file_entity
* @return bool
*/
function _svgimg_apply_field_item_override(&$item, &$field_data, $delta = 0,$use_file_entity = false) {
  $matched = false;
  if (svgimg_image_is_svg($item)) {
    $matched = true;
    if ($use_file_entity) {
      $style_name = $field_data[$delta]['file']['#image_style'];
      if ($field_data[$delta]['file']['#theme'] != 'image_formatter') {
        $field_data[$delta]['file']['#theme'] = 'image_formatter';
        $field_data[$delta]['#image_style'] = NULL;
      } 
    } else {
      $style_name = $field_data[$delta]['#image_style'];
    }
    $data = svgimg_fetch_style_size($style_name);
    $size = svgimg_image_svg_fetch_size($item);
    svgimg_image_svg_map_style($item,$data,$size, $style_name);
    if ($use_file_entity) {
      $field_data[$delta]['file']['#image_style'] = NULL;
    } else {
      $field_data[$delta]['#image_style'] = NULL;
    }
  }
  return $matched;
}

/*
* Calculate display according to designated image style 
* @param string $style_name
*/
function svgimg_fetch_style_size($style_name = NULL) {
  $style = image_style_load($style_name);
  $data = array();
  if (is_array($style['effects'])) {
    foreach ($style['effects'] as $style_index => $effect) {
      if (isset($effect['name'])) {
        switch($effect['name']) {
          case 'image_scale':
          case 'image_scale_crop':
            $data = $effect['data'];
            $data['effect'] = $effect['name'];
            break;
        }
      }
    }
  }
  return $data;
}

/*
* Determine if an image file is SVG
* @param array &$item
*/
function svgimg_image_is_svg(&$item) {
  $mime_key = 'jpg';
  if (is_array($item) && isset($item['filemime'])) {
    $mime_parts = explode('/',$item['filemime']);
    if ($mime_parts[0] == 'image') {
      $image_mime_parts = explode('+',$mime_parts[1]);
      $mime_key = array_shift($image_mime_parts);
    }
  }
  return $mime_key == 'svg';
}

/*
* Calculate image size from source XML
* @param array &$item
*/
function svgimg_image_svg_fetch_size(array &$item) {
  $size = array('width' => 0,'height' => 0);
  if (isset($item['uri'])) {
    $path  = drupal_realpath($item['uri']);
    if ($path) {
      $svg_data = svgimg_fetch_data($path);
      $size = $svg_data->size();
    }
  }
  if ($size['height'] < 1) {
    if ($size['width'] > 0) {
      $size['height'] = $size['width'];
    }
    else {
      $size = svgimg_default_size();
    }
  }
  return $size;
}

/*
* Map SVG size to target size based on image style resizing criteria
* @param array &$item
* @param array &$data
* @param array &$size
* @param string $style_name
*/
function svgimg_image_svg_map_style(array &$item,array &$data,array &$size, $style_name = NULL) {
  if (!empty($style_name) && isset($data['height'])) {
    svgimg_map_target_size($data, $size);
  }
  $dataSrc = svgimg_uri($item['uri'],$style_name);
  if (isset($data['width']) && $data['width'] > 0) {
    $item['width'] = (int) $data['width'];
  } else {
    $item['width'] = (int) $size['width'];
  }
  
  if (isset($data['height']) && $data['height'] > 0) {
    $item['height'] = (int) $data['height'];
  } else {
    $item['height'] = (int) $size['height'];
  }
  $classes = array('svg-img','size-' . str_replace('_','-',$style_name) );
  $item['attributes'] = array('class' => $classes,'data-src' => url($dataSrc));
}

/*
* Set image size attributes based on calculated image style attributes
* @param array &$data
* @param array &$size
* @param bool $setSize
*/
function svgimg_map_target_size(array &$data,array &$size, $setSize = false) {
  $imgRatio = $size['width'] / $size['height'];
  if (isset($data['height']) && isset($data['width']) && $data['height'] > 0) {
    $targetRatio = $data['width'] / $data['height'];
    if ($data['height'] > 1 && $data['width'] > 1) {
      if ($imgRatio < $targetRatio) {
        $data['width'] *= ($imgRatio * $targetRatio);
      } else {
        $data['height'] /= ($imgRatio * $targetRatio);
      }
    }
    else if ($data['width'] > 1) {
      $data['height'] = $data['width'] / $imgRatio;
    } else if ($data['height'] > 1) {
      $data['width'] = $data['height'] * $imgRatio;
    }
    if ($setSize) {
      $size['width'] = $data['width'];
      $size['height'] = $data['height'];
    }
  }
}

/*
* Fetch URI for PNG alternative
* @param string &$uri
* @param string $style_name
*/
function svgimg_uri($uri, $style_name = NULL) {
  $path = svgimg_path();
  if (!empty($style_name)) {
    $path .= '/' . $style_name . '/img-styles';
  }
  if (strpos($uri,'svgimg/samples/') === 0) {
    $file_ref = $uri;
  }
  else {
    $parts = explode('//', $uri);
    $file_ref = array_pop($parts);
  }
  $file_ref .= '.png';
  return $path . '/' . $file_ref;
}

/*
* Fetch assoc array of default PNG alternative width x height if none available
* @return array
*/
function svgimg_default_size() {
  $parts = explode('x', SVGIMG_DEFAULT_SIZE);
  $svgimg_default_size = array(
    'width' => (int) $parts[0],
    'height' => (int) $parts[1]
  );
  return variable_get('svgimg_default_size', $svgimg_default_size);
}

/*
* @param string $svg_path
*/
function svgimg_fetch_source_size($svg_path = NULL) {
  return svgimg_fetch_data($svg_path)->size();
}

/*
* @param string $svg_path
*/
function svgimg_fetch_data($svg_path = NULL) {
  $svg = file_get_contents($svg_path);
  module_load_include('php','svgimg','includes/SvgimgData');
  return new SvgimgData($svg);
}

/*
* @param string $uri
* @param string $directory_path
*/
function svgimg_root_path($uri = NULL, $directory_path = NULL) {
  if (strpos($uri,'://') === false || strpos($uri,'://') < 1) {
    $uri = 'public://' . $uri;
  }
  if (empty($directory_path)) {
    $directory_path = variable_get('file_public_path', conf_path() . '/files');
    // enable path translation for other file schemas, e.g. not on same server
    return drupal_realpath($uri);
  }
  else {
    return DRUPAL_ROOT . DIRECTORY_SEPARATOR .$directory_path . DIRECTORY_SEPARATOR . file_uri_target($uri);
  }
}

/*
* @param string $uri
*/
function svgimg_fetch_size($uri) {
  $svg_path = svgimg_root_path($uri);
  return svgimg_fetch_source_size($svg_path);
}

/*
* @param object $entity
* @param string $field_name
*/
function _svgimg_loop_entity_field_items($entity, $field_name) {
  if (isset($entity->{$field_name}) && !empty($entity->{$field_name})) {
    foreach ($entity->{$field_name} as $langcode => $field_items) {
      if (!empty($field_items)) {
        $changed = false;
        foreach ($field_items as &$item) {
          if ($item['fid'] > 0) {
            $file = file_load($item['fid']);
            if (is_object($file)) {
              if (!empty($file->uri) && $file->filemime == 'image/svg+xml' && empty($item['width'])) {
                
                $size = svgimg_fetch_size($file->uri);
                $item['width'] = $size['width'];
                $item['height'] = $size['height'];
                $changed = true;
              }    
            }
          }
        }
        if ($changed) {
          $entity->{$field_name} = array($langcode => $field_items);
        }
      }
    }
  }
}
