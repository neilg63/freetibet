<?php

function theme_txorg_table($vars) {
	$defaultTextfieldLength = variable_get('txorg_default_textfield_length',60);
	$variables = $vars['fields'];
	if (!isset($variables['#caption'])) {
		$variables['#caption'] = t("Entities referencing this term");
	}
	$table = array(
			'caption' => $variables['#caption'],
			'colgroups' => array(),
			'sticky' => FALSE,
			'empty' => FALSE
	);
	$table['header'] = array('title','options');
	$table['rows'] = array();
	if (is_array($variables)) {
		foreach($variables as $key => $element) {
			if (empty($key) && is_array($element)) {
				$variables = $element;
				break;
			}
		}
	}
	$attrs = isset($variables['#attributes'])? $variables['#attributes'] : array();
	$table['attributes'] = $attrs;
	foreach ($variables as $key => $var) {
		if (is_numeric($key) && is_array($var) && isset($var['#title'])) {
			$default_value = isset($var['#default_value'])? $var['#default_value'] : NULL; 
      $mutliple = isset($var['#multiple'])? !empty($var['#multiple']) : false;
			$cells = array();
			$cells[] = $var['#title'];
			$name = $variables['#name'].'['.$key.']';
			$id = 'edit-'.$variables['#name'].'-'.$key;
			if (isset($var['#size'])) {
				$size = $var['#size'];
			} else if (isset($var['#cols'])) {
				$size = $var['#cols'];
			} else {
				$size = $defaultTextfieldLength;
			}
			if (isset($var['#rows'])) {
				$rows = $var['#rows'];
			} else {
				$rows = 4;
			}
			if (isset($var['#maxlength'])) {
				$maxlength = $var['#maxlength'];
			} else {
				$maxlength = 255;
			}
			switch ($var['#type']) {
				case 'textfield':
				case 'textarea':
					$optVars['element'] = array(
							'#value' => $default_value,
							'#attributes' => array('name' => $name,'id' => $id),
					);
					switch($var['#type']) {
						case 'textfield':
							$optVars['element']['#attributes'] += array('size' => $size,'maxlength' => $maxlength);
							break;
						case 'textarea':
							$optVars['element']['#attributes'] += array('rows' => $rows,'cols' => $size);
							break;
					}
					$themeFunction = 'theme_'.$var['#type'];
					$cells[] = $themeFunction($optVars);
					break;
				default:
					if ($mutliple) {
						$name .= '[]';
					}
					$optVars['element'] = array(
							'#options' => $var['#options'],
							'#value' => $default_value,
							'#attributes' => array('name' => $name,'id' => $id),
					);
					if ($mutliple) {
						$optVars['element']['#attributes']['multiple'] = 'multiple';
					}
					if ($var['#type'] == 'checkboxes'  || $var['#type'] == 'radios') {
						$checkCells = array();
						$headerIndex = 0;
						foreach ($var['#options'] as $optVal => $optTitle) {
							$headerIndex++;
							$table['header'][$headerIndex] = $optTitle;
							$cVars['element'] = array(
									'#value' => $optVal,
									'#attributes' => array('id' => $id, 'name' => $name,'title' => $optTitle,'value' => $optVal)
							);
							if (in_array($optVal,$var['#default_value'] )) {
								$cVars['element']['#attributes']['checked'] = true;
							}
							if ($var['#type'] == 'checkboxes') {
								$checkCells[] = theme_checkbox($cVars);
							} else {
								$checkCells[] = theme_radio($cVars);
							}
						}
						if (empty($checkCells)) {
							$cells[] = '';
						} else {
							foreach ($checkCells as $cCell) {
								$cells[] = $cCell;
							}
						}
					} else {
						$cells[] = theme_select($optVars);
					}
					
					break;
				
			}
			$table['rows'][] = $cells;
      $table['sticky'] = TRUE;
		}
	}
	
	return theme('table', $table);
}

function _txorg_form_taxonomy_overview_vocabularies_alter(&$form) {
	foreach ($form as $key => $data) {
		if (is_numeric($key) && is_array($data) && isset($data['#vocabulary'])) {
			$vocab = $data['#vocabulary'];
			if (is_object($vocab)) {
				$form[$key]['txorg'] = array(
						'#type' => "link",
						'#title' => t("organise terms"),
						'#href' => 'admin/structure/taxonomy/'.$vocab->machine_name.'/taxonomy-organiser/list',
				);
			}
		}
	}
	taxonomy_overview_vocabularies();
	$form['#theme'] = 'txorg_taxonomy_overview_vocabularies';
}

function theme_txorg_taxonomy_overview_vocabularies($variables) {
	$form = $variables['form'];
	$rows = array();
	foreach (element_children($form) as $key) {
		if (isset($form[$key]['name'])) {
			$vocabulary = &$form[$key];

			$row = array();
			$row[] = drupal_render($vocabulary['name']);
			if (isset($vocabulary['weight'])) {
				$vocabulary['weight']['#attributes']['class'] = array('vocabulary-weight');
				$row[] = drupal_render($vocabulary['weight']);
			}
			$row[] = drupal_render($vocabulary['edit']);
			$row[] = drupal_render($vocabulary['list']);
			$row[] = drupal_render($vocabulary['txorg']);
			$row[] = drupal_render($vocabulary['add']);
			$rows[] = array('data' => $row, 'class' => array('draggable'));
		}
	}

	$header = array(t('Vocabulary name'));
	if (isset($form['actions'])) {
		$header[] = t('Weight');
		drupal_add_tabledrag('taxonomy', 'order', 'sibling', 'vocabulary-weight');
	}
	$header[] = array('data' => t('Operations'), 'colspan' => '4');
	return theme('table', array('header' => $header, 'rows' => $rows, 'empty' => t('No vocabularies available. <a href="@link">Add vocabulary</a>.', array('@link' => url('admin/structure/taxonomy/add'))), 'attributes' => array('id' => 'taxonomy'))) . drupal_render_children($form);
}