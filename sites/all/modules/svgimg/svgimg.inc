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
    $item = array();
    $use_file_entity = false;
    if (isset($field_data[$i]['#item'])) {
      $item =& $field_data[$i]['#item']; 
    }
    else if (isset($field_data[$i]['file'])) {
      $item =& $field_data[$i]['file']['#item'];
      $use_file_entity = true;
    }
    if (!empty($item)) {
      if (svgimg_image_is_svg($item)) {
        $matched = true;
        if ($use_file_entity) {
          $style_name = $field_data[$i]['file']['#image_style'];
          if ($field_data[$i]['file']['#theme'] != 'image_formatter') {
            $field_data[$i]['file']['#theme'] = 'image_formatter';
            $field_data[$i]['#image_style'] = NULL;
          } 
        } else {
          $style_name = $field_data[$i]['#image_style'];
        }
        $data = svgimg_fetch_style_size($style_name);
        $size = svgimg_image_svg_fetch_size($item);
        svgimg_image_svg_map_style($item,$data,$size, $style_name);
        if ($use_file_entity) {
          $field_data[$i]['file']['#image_style'] = NULL;
        } else {
          $field_data[$i]['#image_style'] = NULL;
        }
      }
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
function svgimg_image_is_svg(array &$item) {
  $mime_key = 'jpg';
  if (isset($item['filemime'])) {
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
      $xml = simplexml_load_file($path);
      $xml->registerXPathNamespace('svg', 'http://www.w3.org/2000/svg');
      $xml->registerXPathNamespace('xlink', 'http://www.w3.org/1999/xlink');
      $attrs = $xml->attributes();
      $size['height'] = (int) (string) $attrs['height'];
      $size['width'] = (int) (string) $attrs['width'];
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
