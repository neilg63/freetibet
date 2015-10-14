<?php

function _ftoverrides_form_node_form_alter(&$form,&$form_state,$form_id = NULL) {
	$node = $form['#node'];
  if (arg(1) == 'add') {
    $parent_nid = arg(3);
    if (is_numeric($parent_nid) && $parent_nid > 0) {
      $form['parent_entity_id'] = array(
        '#type' => 'hidden',
        '#value' => (int) $parent_nid,
      );
    }
  }
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
        $default = $form['node_class']['css_class']['#default_value'];
        if (!empty($default) && is_string($default)) {
          $default = explode(' ', $default);
        }
        else {
          $default = array();
        }
				$form['node_class']['css_class']['#type'] = 'select';
        $form['node_class']['css_class']['#multiple'] = TRUE;
        $form['node_class']['css_class']['#size'] = count($arrClasses);
        $form['node_class']['css_class']['#options'] = $arrClasses;
        $form['node_class']['css_class']['#default_value'] = $default;
				$form['node_class']['css_class']['#description'] .= " " . t("These will be added to wrapper tag of the node for additional styling");
				$items = array();
				foreach ($arrClasses as $key => $value) {
					$items[] = $key . ' : ' . $value;
				}
				
				/*$vars = array(
					'type' => 'ul',
					'title' => 'Available class names for this content type',
					'items' => $items
				);
				
				$form['node_class']['css_class']['#suffix'] = theme('item_list', $vars);*/
	  }
	}
			
	if (isset($form['field_block']) && is_array($form['field_block'])) {
    foreach ($form['field_block'] as $lang => &$blockControl) {
      if (is_array($blockControl) && isset($blockControl['#options'])) {
        $blockControl['#size'] = 8;
        break;
      }
    }
  }		
  if (isset($form['field_donation_settings'])) {
    $form['field_donation_settings']['#type'] = 'fieldset';
    $form['field_donation_settings']['#title'] = t("Donation settings");
    $form['field_donation_settings']['#collapsible'] = TRUE;
    $form['field_donation_settings']['#collapsed'] = TRUE;
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
      case 'slide_section':
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
      case 'page':
      case 'blog':
      case 'donate':
      case 'news_article':
        $form['#after_build'][] = 'ftoverrides_node_form_after_build';
        break;
		}
	}
  
}


function _ftoverrides_node_presave($node) {
  if (isset($node->node_class)) {
    if (isset($node->node_class['css_class']) && is_array($node->node_class['css_class'])) {
      if (!empty( $node->node_class['css_class'])) {
        $node->node_class['css_class'] = implode(' ', $node->node_class['css_class']);
      }
      else {
        $node->node_class['css_class'] = '';
      }
      
    }
  }
  if (isset($node->field_donation_settings)) {
    $valid = false;
    if (is_array($node->field_donation_settings)) {
      foreach ($node->field_donation_settings as $lang => $fcData) {
        if (is_string($lang) && is_array($fcData) && array_key_exists(0,$fcData)) {
          if (isset($fcData[0]['field_default'])) {
            if (isset($fcData[0]['field_default'][$lang][0]['value'])) {
              $value = $fcData[0]['field_default'][$lang][0]['value'];
              $valid = is_numeric($value);
            }
          }
        }
      }
    }
    if (!$valid) {
      $node->field_donation_settings = array();
    }
  }
}

function _ftoverrides_node_insert($node) {
  $parentNid = 0;
  if (isset($node->parent_entity_id) && $node->parent_entity_id > 0) {
    $parentNid = (int) $node->parent_entity_id;
  }
  if (arg(1) == 'add') {
    $parNid = arg(3);
    if (is_numeric($parNid) && $parNid > 1) {
      switch ($node->type) {
        case 'text':
        case 'media_section':
        case 'key_points':
        case 'slide_section':
        case 'box':
        case 'take_action':
          $parentNid = (int) $parNid;
          break;
      }
    }
  }
  if ($parentNid > 0) {
    $pn = node_load($parentNid);
    if (is_object($pn)) {
      
      if (empty($pn->language)) {
        $pn->language = LANGUAGE_NONE;
      }
      if (empty($pn->field_page_sections) || !isset($pn->field_page_sections[$pn->language])) {
        $pn->field_page_sections = array($pn->language => array());
      }
      $pn->field_page_sections[$pn->language][] = array(
        'target_id' => $node->nid
      );
      drupal_set_message("New page section: " . $node->nid . " added to " . $parentNid);
      node_save($pn);
    }
  }
}

function _ftoverrides_form_block_admin_configure_alter(&$form,&$form_state) {
  module_load_include('inc','ftoverrides','ftoverrides.donate');
  $min_amount = variable_get('ftoverrides_min_donation_amount', 5);
  $form['settings']['ftoverrides_min_donation_amount'] = array(
    '#type' => 'textfield',
    '#title' => t("Minimum amount"),
    '#default_value' => $min_amount,
    '#size' => 4
  );

  $default_amount = variable_get('ftoverrides_default_donation_amount', 10);
  $form['settings']['ftoverrides_default_donation_amount'] = array(
    '#type' => 'textfield',
    '#title' => t("Default amount"),
    '#default_value' => $default_amount,
    '#size' => 4
  );
  
  
  
  
  $presetDesc = t("Each suggested amount must be place on one line with a series of key / value pairs separated by commas. Each key is followed by a colon (:) and its value. Values may not comprise commas. The amount key may be followed a set of semi-colon-separated currency values, where the currency is denoted by its three letter abbreviation, e.g. GBP, EUR or USD");
  
  $default = ftoverrides_donation_presets('donate');
  
  $form['settings']['ftoverrides_donation_presets_donate'] = array(
    '#type' => 'textarea',
    '#title' => t("Default amounts for donations"),
    '#default_value' => _ftoverrides_expand_presets($default),
    '#size' => 4,
    '#description' => $presetDesc
  );
  
  $default = ftoverrides_donation_presets('join');
  
  $form['settings']['ftoverrides_donation_presets_join'] = array(
    '#type' => 'textarea',
    '#title' => t("Default amounts for joining"),
    '#default_value' => _ftoverrides_expand_presets($default),
    '#size' => 4,
    '#description' => $presetDesc
  );
  
  $keyNotes = '<p>Please use the following keys and values</p><dl>';
  
  $keyNotes .= '<dt>key:</dt><dd>Short machine name, use only letters, numbers and underscores (required)</dd>';
  $keyNotes .= '<dt>title:</dt><dd>Display name (required)</dd>';
  $keyNotes .= '<dt>description:</dt><dd>Extra information added to title attribute (optional)</dd>';
  $keyNotes .= '<dt>cycle:</dt><dd>Repeat period: month or year used with subscriptions / recurring payments only</dd>';
  $keyNotes .= '<dt>cycle_limit:</dt><dd>Max number of instalments before automatic expiry. The largest value is 52</dd>';
  $keyNotes .= '<dt>amount:</dt><dd>Set of currency amounts separated by semi-colons, e.g. GBP 5; EUR 8; USD 10</dd>';
  
  $keyNotes .= '</dl>';
  
  $form['settings']['ftoverrides_donation_presets_notes'] = array(
    '#type' => 'markup',
    '#markup' => $keyNotes
  );

  if (is_array($form['#submit'])) {
    array_unshift($form['#submit'],'ftoverrides_donation_block_admin_configure_submit');
  }
}

function _ftoverrides_expand_presets($default = array()) {
  $rows = array();
  if (is_array($default)) {
    foreach ($default as $def_map) {
      $items = array();
      if (is_array($def_map)) {
        foreach ($def_map as $key => $value) {
          switch ($key) {
            case 'amount':
              $vals = array();
              if (is_array($value)) {
                foreach ($value as $currency => $amount) {
                  $vals[] = $currency. " " . $amount;
                }
              }
              $items[] = $key.': ' . implode('; ', $vals);
              break;
            default:
              $items[] = $key.': ' . $value;
              break;
          }
        }
        $rows[] = implode(', ', $items);
      }
    }
  }
  return implode("\n", $rows);
}

function _ftoverrides_parse_presets($strValue = '') {
  $default = array();
  if (!empty($strValue)) {
    $lines = explode("\n", $strValue);
    if (is_array($lines) && !empty($lines)) {
      foreach ($lines as $line) {
        $map = array();
        $items = explode(',', $line);
        if (is_array($items) && !empty($items)) {
          foreach ($items as $item) {
            $parts = explode(':', $item);
            if (count($parts)> 1) {
              $key = array_shift($parts);
              $key = trim($key);
              $value = join(':', $parts);
              $value = trim($value);
              switch ($key) {
                case 'amount':
                  $amounts = explode(';', $value);
                  $amData = array();
                  if (!empty($amounts)) {
                    foreach ($amounts as $amStr) {
                      $amStr = trim($amStr);
                      $amParts = preg_split('#[\s=:]#', $amStr);
                      if (count($amParts)>1) {
                        $currency = strtoupper(trim($amParts[0]));
                        $val = trim($amParts[1]);
                        if (strlen($currency) == 3 && is_numeric($val)) {
                          $amData[$currency] = (float) $val;
                        }
                      }
                    }
                  }
                  $map[$key] = $amData;
                  break;
                default:
                  $map[$key] = $value;
                  break;
              }
            }
          }
        }
        if (!empty($map) && isset($map['key']) && isset($map['title']) && isset($map['amount'])) {
          $default[] = $map;
        }
      }
    }
  }
  return $default;
}

function _ftoverrides_donation_block_admin_configure_submit(&$settings) {
  module_load_include('inc','ftoverrides','ftoverrides.donate');
  $var_name = 'ftoverrides_min_donation_amount';
  $value = (float) $settings[$var_name];
  if (is_numeric($value)) {
   variable_set($var_name, $value); 
  }
  $var_name = 'ftoverrides_default_donation_amount';
  if (isset($settings[$var_name])) {
    $value = (float) $settings[$var_name];
    if (is_numeric($value)) {
     variable_set($var_name, $value);
    }
  }
  
  $var_name = 'ftoverrides_donation_presets_donate';
  if (isset($settings[$var_name])) {
    $value = $settings[$var_name];
    if (is_string($value)) {
      $data = _ftoverrides_parse_presets($value);
      if (!empty($data)) {
        variable_set($var_name, $data);
        drupal_set_message("Membership fees saved");
      }
    }
  }
  
  $var_name = 'ftoverrides_donation_presets_join';
  if (isset($settings[$var_name])) {
    $value = $settings[$var_name];
    if (is_string($value)) {
      $data = _ftoverrides_parse_presets($value);
      if (!empty($data)) {
        variable_set($var_name, $data);
        drupal_set_message("Suggested donation amounts saved");
      }
    }
  }
  
}

function _ftoverrides_node_edit_view($node) {
  
  $info = field_info_field('field_page_sections');
  if (is_array($info) && isset($info['settings']['handler_settings']['target_bundles'])) {
    $bundles = $info['settings']['handler_settings']['target_bundles'];
    if (is_array($bundles)) {
      $links = array('<span class="label">'.t("Add page section:").'</span>');
      $types = node_type_get_names();
      $opts = array('query' => array('destination' => 'node/' . $node->nid));
      $bKeys = array('media_section','text','slide_section','key_points');
      $bKeys += $bundles;
      $bKeys = array_unique($bKeys);
      $lastIndex = count($bKeys) - 1;
      foreach ($bKeys as $index => $type_name) {
        $link = 'node/add/'. str_replace('_','-',$type_name) . '/' . $node->nid;
        if ($index == $lastIndex) {
          $opts['attributes']['class'] = array('last');
        }
        $links[] = l($types[$type_name],$link,$opts);
      }
      $node->add_page_section = '<div class="add-page-section">' . implode('',$links) . '</div>';
      
    }
  }
  
}