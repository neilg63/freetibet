<?php

/*
 * called via page_viewmode_node_save to implement hook_node_update and hook_node_insert
 * store custom full page view mode in separate table
 */
function _page_viewmode_node_save($node) {
	$value = page_viewmode_value($node,true);
	$fields = array(
			'view_mode' => $node->page_viewmode
	);
	$table = 'page_viewmode';
	if (empty($value)) {
		$fields['entity_id'] = $node->nid;
		$query = db_insert($table)->fields($fields);
	} else {
		if ($node->page_viewmode == 'full') {
			$query = db_delete($table)->condition('entity_id', $node->nid);
		} else {
			$query = db_update($table)->fields($fields)->condition('entity_id', $node->nid);
		}
	}
	$query->execute();
}

function _page_viewmode_form_node_form_alter(&$form, $form_state, $form_id) {
	$node = $form['#node'];
	if (is_object($node) && isset($node->type)) {
		$enabledTypes = page_viewmode_enabled_types();
		if (array_key_exists($node->type, $enabledTypes)) {
			$view_modes = array(
					'full' => 'Full (Default full page view)',
			);
			foreach ($enabledTypes[$node->type] as $m) {
				$view_modes[$m] = str_replace('_',' ',$m);
			}
			$page_viewmode = page_viewmode_value($node);
			$form['options']['page_viewmode'] = array(
					'#type' => 'select',
					'#title' => 'Full page view mode',
					'#options' => $view_modes,
					'#weight' => 10,
					'#default_value' => $page_viewmode
			);
		}
	}
}