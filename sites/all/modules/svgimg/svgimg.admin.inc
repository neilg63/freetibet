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

  $imagick_installed =  class_exists('Imagick');
  
  if ($imagick_installed) {
      $im = new Imagick();
      $api = $im->getVersion();
      if (isset($api['versionString'])) {
        $filter = filter_format_load('full_html');
        $filter->settings = array('filter_url_length' => 255);
        $api_text = _filter_url($api['versionString'],$filter). ' (' . t('Version number') . $api['versionNumber'] . ')';
        $install_text = t("Successfully installed") . ". " . t("API") . ": " . $api_text;
      }
      $vars = array(
        'path' => drupal_get_path('module','svgimg') . '/samples/human_skeleton.svg',
        'attributes' => array(),
        'alt' => t("Source SVG")
      );
      $install_text .= '<div class="sample-svgimg-set">';
      $install_text .= svgimg_figure($vars);
      $vars = array(
        'path' => svgimg_uri('svgimg/samples/human_skeleton.svg'),
        'attributes' => array(),
        'alt' => t("PNG Alternative")
      );
      $install_text .= svgimg_figure($vars,'rendered-png');
      $install_text .= '</div>';
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
  return $form;
}

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
}


function svgimg_figure($vars, $type_class = 'source-svg') {
  return '<figure class="sample-image '.$type_class.'" >' . theme_image($vars) . '<caption>'.$vars['title'].'</caption></figure>';
}