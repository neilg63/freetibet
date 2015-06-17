<?php

function _ftoverrides_form_node_form_alter(&$form,&$form_state,$form_id = NULL) {
	$node = $form['#node'];
	if (isset($form['node_class']) && isset($form['node_class']['css_class'])) {
		$arrClasses = array();
			switch ($node->type) {
				case 'media_section':
				$arrClasses = array(
					'white-text' => t("White text"),
					'large-text' => t("Large text"),
					'translucent-black-text-box' => t("Translucent dark text box"),
					'translucent-white-text-box' => t("Translucent light text box"),
					'text-box-top-left' => t("Text box top left"),
					'text-box-top-right' => t("Text box top right"),
					'text-box-bottom-left' => t("Text box bottom left"),
					'text-box-bottom-right' => t("Text box bottom right"),
				);
				break;
			}
			
			if (count($arrClasses) > 0) {
				$form['node_class']['css_class']['#size'] = 120;
				$form['node_class']['css_class']['#description'] .= " " . t("These will be added to wrapper tag of the node for additional styling");
				$items = array();
				foreach ($arrClasses as $key => $value) {
					$items[] = $key . ' : ' . $value;
				}
				
				$vars = array(
					'type' => 'ul',
					'title' => 'Available class names for this content type',
					'items' => $items
				);
				
				$form['node_class']['css_class']['#suffix'] = theme('item_list', $vars);
	  }
			}
			
			
	
	if (is_object($node) && isset($node->type)) {
		if (isset($form['field_layout_mode'])) {
			$view_modes = ftoverrides_get_available_view_modes($node->type);
			$lang = (isset($node->language))? $node->language : LANGUAGE_NONE;
			$opts = $form['field_layout_mode'][$lang]['#options'];
			switch ($node->type) {
				case 'text':
					$full_name = t("Full width Text");
					break;
				default:
					$full_name = t("Text with floated media items");
					break;
			}
			$options = array('full' => $full_name);
			foreach ($opts as $key => $name) {
				if (in_array($key,$view_modes)) {
					$options[$key] = $name;
				}
			}
			$form['field_layout_mode'][$lang]['#options'] = $options;
		}
		switch ($node->type) {
			case 'text':
			case 'take_action';
			case 'media_section':
			case 'box':
			case 'key_points':
				$form['path']['#access'] = FALSE;
				$form['author']['#access'] = FALSE;
				$form['metatags']['#access'] = FALSE;
				$form['xmlsitemap']['#access'] = FALSE;
				$form['revision_information']['#access'] = FALSE;
				$form['redirect']['#access'] = FALSE;
				$form['options']['status']['#default_value'] = 1;
				$form['options']['promote']['#access'] = FALSE;
				$form['options']['sticky']['#access'] = FALSE;
				break;
		}
	}
}
