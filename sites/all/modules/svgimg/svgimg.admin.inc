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
