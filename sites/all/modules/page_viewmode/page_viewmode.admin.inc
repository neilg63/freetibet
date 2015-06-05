<?php


function page_viewmode_settings($form, &$form_state) {
	drupal_set_title("Custom Page Node View Modes");
	$form['bundles'] = array(
			'#type' => 'fieldset',
			'#title' => 'Extra page view mode options by content type',
			'#tree' => TRUE
	);
	$prefix = 'field_bundle_settings_node__';
	$default = array();
	$excludeViewModes = array('search_result','teaser');
	$typeNames = node_type_get_names();
	$pageViewData = variable_get('page_viewmode_options',$default);
	foreach ($typeNames as $bundle => $typeName) {
		$data = variable_get($prefix . $bundle, $default);
		if (is_array($data) && isset($data['view_modes']) && is_array($data['view_modes']) && !empty($data['view_modes'])) {
			$view_modes = array();
			foreach ($data['view_modes'] as $name => $data) {
				if ($data['custom_settings'] && !in_array($name,$excludeViewModes)) {
					$view_modes[$name] = $name;
				}
			}
			if (!empty($view_modes)) {
				$form['bundles'][$bundle] = array(
						'#type' => 'checkboxes',
						'#title' => $typeName,
						'#options' => $view_modes,
						'#default_value' => isset($pageViewData[$bundle])? $pageViewData[$bundle] : $default 
				);
			}
		}
	}
	$form['submit'] = array(
			'#type' => 'submit',
			'#value' => 'Save'
	);
	$form['#submit'] = array('page_viewmode_settings_submit');
	return $form;
}

function page_viewmode_settings_submit(&$form,$form_state) {
	if (isset($form_state['values']['bundles'])) {
		$bundles = $form_state['values']['bundles'];
		$data = array();
		$msg = '';
		if (!empty($bundles)) {
			foreach ($bundles as $bundle => $options) {
				if (!empty($options)){
					foreach ($options as $key => $value) {
						if ($key == $value && is_string($value) && strlen($value) > 2) {
							if (!isset($data[$bundle])) {
								$data[$bundle] = array();
								if (empty($msg)) {
									$msg = "<p>Nodes with content types may have alternative view modes<p>";
								}
							}
							$data[$bundle][] = $value;
						}
					}
				}
			}
		}
		if (empty($msg)) {
			$msg = t("No content types may use custom fill page view modes");
		} else {
			$typeNames = node_type_get_names();
			$items = array();
			foreach ($data as $bundle => $keys) {
				$items[] = $typeNames[$bundle] . ': ' . str_replace('_',' ',implode(", ", $keys));
			}
			$msg .= "<ul><li>". implode('</li><li>',$items) . "</li></ul>";
		}
		drupal_set_message($msg);
		variable_set('page_viewmode_options',$data);
	}
}