<?php

/*
* Called by svgimg_field_formatter_settings_summary()
* @param array &$field
* @param array &$instance
* @param string $view_mode
* @param array $display
*/
function _svgimg_field_formatter_settings_summary(&$field, &$instance, $view_mode,$display) {
  $text = "";
  $settings = $display['settings'];
  $items = array();
  if (isset($settings['wrapper_tag'])) {
    $items[] = t("Wrapper tag") . ": " . $settings['wrapper_tag'];
  }
  if (is_string($settings['classes'])) {
    $settings['classes'] = trim($settings['classes']);
    if (!empty($settings['classes'])) {
      $items[] = t("Extra class names") . ": " . $settings['classes'];
    }
  }
  if (isset($settings['add_script'])) {
    $settings['add_script'] = (bool) $settings['add_script'];
    if ($settings['add_script']) {
      $items[] = t("PNG replacement script added");
    }
  }
  if (!empty($items)) {
    $vars = array(
      'type' => 'ul',
      'title' => t("Inline SVG"),
      'items' => $items
    );
    $text = theme('item_list',$vars);
  }
  return $text;
}

function _svgimg_field_formatter_settings_form(&$field, &$display,&$form, &$form_state) {
  $element = array();
  $settings = $display['settings'];
  $tags = array(
    'figure',
    'div',
    'section'
  );
  $element['wrapper_tag'] = array(
    '#type' => 'select',
    '#title' =>  t("Wrapper tag"),
    '#options' => drupal_map_assoc($tags),
    '#default_value' => $settings['wrapper_tag']
  );
  
  $element['classes'] = array(
    '#type' => 'textfield',
    '#title' =>  t("Extra wrapper tag class names"),
    '#size' => 32,
    '#default_value' => trim($settings['classes'])
  );
  
  $element['add_script'] = array(
    '#type' => 'checkbox',
    '#title' =>  t("Add PNG replacement script"),
  );
  if (isset($settings['add_script']) && $settings['add_script'] == 1) {
    $element['add_script']['#attributes'] = array('checked' => true);
  }
  
  return $element;
}

function svgimg_admin($form_state) {
  $form = array();
  module_load_include('inc', 'svgimg');
  $imagick_data = svgimg_imagick_data();
  
  if ($imagick_data->installed) {
    $install_text = svgimg_get_imagickmagic_info($imagick_data);
    }
    else {
      $install_text = t("Not installed or detected. Please follow instructions at") .": ". l("Imagick Setup",'http://php.net/manual/en/imagick.setup.php');
    }
  
  $form['info'] = array(
    '#type' => 'fieldset',
    '#title' => t("ImageMagick status"),
  );
  
  $form['info']['status'] = array(
    '#type' => 'markup',
    '#markup' => '<p>' . $install_text . '</p>',
  );
  
  if (!$imagick_data->use_php) {
    $form['info']['svg_imagemagick'] = array(
      '#type' => 'textfield',
      '#title' => t("Path ImageMagick convert command"),
      '#size' => 80,
      '#default_value' => $imagick_data->path
    );
    if (!$imagick_data->valid) {
      $form['info']['svg_imagemagick']['#description'] = t("A file exists at this location, but is not executable. Please check file permissions");
      $form['info']['svg_imagemagick']['#attributes']['class'][] = 'error';
    }
  }
  
  $form['preview'] =  array(
    '#type' => 'fieldset',
    '#title' => t("Preview"),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );
    
  $markup = '<div class="sample-svgimg-set">';
  $vars = array(
    'path' => drupal_get_path('module','svgimg') . '/samples/bicycle_diagram.svg',
    'attributes' => array(),
    'alt' => t("Sample Image"),
    'title' => t("Source SVG")
  );
  $markup .= svgimg_figure($vars);
  $vars = array(
    'path' => svgimg_uri('svgimg/samples/bicycle_diagram.svg'),
    'attributes' => array(),
    'alt' => t("Sample Image"),
    'title' => t("PNG Alternative")
  );
  if ($imagick_data->valid) {
    $markup .= svgimg_figure($vars,'rendered-png');
  }
  else {
    $markup .= svgimg_not_available(t("SVG image cannot be automatically converted to PNG. ImageMagick not available."));
  }
  $markup .= '</div>';
  
  $form['preview']['sample'] = array(
    '#type' => 'markup',
    '#markup' => $markup
  );
    
  
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t("Global options"),
  );
  
  $size = svgimg_default_size();
  
  $form['options']['svgimg_default_size'] = array(
    '#type' => 'fieldset',
    '#title' => t("Default size of PNG conversion"),
    '#description' => t("Only used if no other size can be deduced from the source SVG and/or image style"),
    '#tree' => TRUE,
  );
  
  $form['options']['svgimg_default_size']['width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#size' => 5,
    '#default_value' => $size['width'],
    '#element_validate' => array('element_validate_integer_positive'), 
    '#required' => TRUE
  );
  
  $form['options']['svgimg_default_size']['height'] = array(
    '#type' => 'textfield',
    '#title' => t("Height"),
    '#size' => 5,
    '#default_value' => $size['height'],
    '#element_validate' => array('element_validate_integer_positive'),
    '#required' => TRUE
  );
  
  $form['actions'] = array(
    '#type' => 'fieldset'
  );
  
  $form['actions']['save'] = array(
     '#type' => "submit",
     '#value' => t("Save"), 
  );
  $form['#submit'] = array('svgimgs_admin_submit');
  
  $mod_path = drupal_get_path('module', 'svgimg');
  	$form['#attached'] = array(
  			'css' => array(
  					$mod_path . '/css/admin.css'
  				),
  	);
  
  return $form;
}

function svgimg_imagick_data() {
  $data = new StdClass;
  $data->installed =  class_exists('Imagick');
  $data->valid = $data->installed;
  $data->use_php = $data->installed;
  $data->path = NULL;
  if (!$data->installed) {
    $data->use_php = false;
    $current_path = variable_get('svgimg_imagemagick',NULL);
    $im_paths = array(
      '/usr/bin/convert',
      '/opt/ImageMagick/bin/convert'
    );
    if (!empty($current_path) && !in_array($current_path,$im_paths)) {
      array_unshift($im_paths,$current_path);
    }
    foreach ($im_paths as $path)
    if (file_exists($path)) {
      $data->installed = true;
      $data->valid = is_executable($path);
      $data->use_php = false;
      if ($data->valid) {
        variable_set('svgimg_imagemagick',$path);
      }
      $data->path = $path;
      break;
    }
  } else {
    variable_set('svgimg_imagemagick','php');
  }
  return $data;
}

function svgimg_get_imagickmagic_info($imagick_data) {
  $text = '';
  $versionString = '';
  $versionNum = '';
  if ($imagick_data->use_php) {
    $im = new Imagick();
    $api = $im->getVersion();
    if (isset($api['versionString'])) { 
      $versionString = $api['versionString'];
      if (isset($api['versionNumber'])) {
        $versionNum = $api['versionNumber'];
      }
    }
  }
  else {
    $out = exec(variable_get('svgimg_imagemagick_path') ." -version", $return);
    $api = array();
    if (is_array($return)) {
      foreach ($return as $line) {
        $parts = explode(':',$line);
        $key = array_shift($parts);
        $string = implode(':', $parts);
        $key = strtolower(trim($key));
        switch ($key) {
          case 'version':
            $versionString = $string;
            break;
            
        }
      }
    }
  }
  
  if (!empty($versionString)) {
    $filter = filter_format_load('full_html');
    $filter->settings = array('filter_url_length' => 255);
    $api_text = _filter_url($versionString,$filter);
    if (!empty($versionNum)) {
      $api_text = ' (' . t('Version number') . $versionNum . ')';
    }
    $text = t("Successfully installed") . ": " . $api_text;
    if (strlen($imagick_data->path) > 3 && !$imagick_data->use_php) {
      $text .= '. Uses command.';
    }
  }
  return $text;
}

/*
* Form submit handler, validated by default integer validator
* @params array &$form
* @params array &$form_state
*/
function svgimgs_admin_submit(&$form, &$form_state) {
  $values = $form_state['values'];
  if (isset($values['svgimg_default_size'])) {
    $vars = $values['svgimg_default_size'];
    $dimensions = array('width','height');
    $valid_dims = 0;
    foreach ($dimensions as $dim) {
      if (isset($vars[$dim]) && is_numeric($vars[$dim]) && $vars[$dim] > 0) {
        $vars[$dim] = (int) $vars[$dim];
        $valid_dims++;
       }
    }
    if (count($dimensions) == $valid_dims) {
      variable_set('svgimg_default_size', $vars);
    }
  }
  if (isset($values['svgimg_imagemagick'])) {
    $path = trim($values['svgimg_imagemagick']);
    if (strlen($path) > 3) {
      $current_path = variable_get('svgimg_imagemagick', NULL);
      if (file_exists($path) && $current_path != $path) {
        variable_set('svgimg_imagemagick', $path);
        drupal_set_message(t("Saved new imageMagick convert path: @path",array('@path' => $path)));
      }
    }
  }
}

/*
* Wrapper for Drupal's theme_image with figure tags + caption
* @params array $vars
* @params string $type_class
*/
function svgimg_figure($vars, $type_class = 'source-svg') {
  return _svgimg_figure_wrapper(theme_image($vars), $vars['title'], $type_class);
}

/*
* Wrapper for Drupal's theme_image with figure tags + caption
* @params array $vars
* @params string $type_class
*/
function svgimg_not_available($caption) {
  return _svgimg_figure_wrapper('', $caption, 'not-available');
}

/*
* Abtract figure element with optional constructed image plus caption
* @params string $str_image
* @params string $caption
* @params string $type_class
*/
function _svgimg_figure_wrapper($str_image = '', $str_caption = '', $type_class = 'source-svg') {
  $classes = array();
  if (!empty($str_image)) {
    $classes[] = 'sample-image';
  }
  if (!empty($type_class)) {
    $classes[] = $type_class;
  }
  return '<figure class="'.implode(' ', $classes).'" >' . $str_image . '<figcaption>'. $str_caption .'</figcaption></figure>';
}
