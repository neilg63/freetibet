<?php

define('INLINE_FILES_PER_PAGE', 25);

function inline_files_node_list() {
	$page = array_key_exists('page',$_GET)? (int) $_GET['page'] : 0;
	$pager = '';
	$bundle = arg(4);
	if (!is_string($bundle) || strlen($bundle) < 3) {
		$bundle = NULL;
	}
	$title = "Images and Files embedded in node body text";
	$type_names = node_type_get_names();
	if (!empty($bundle)) {
		if (array_key_exists($bundle, $type_names)) {
			$type_name = $type_names[$bundle];
		} 
		else {
			$type_name = $bundle . ' (unmatched)';
		}
		$title .= ' filtered by ' . $type_name;
	}
	drupal_set_title($title);
	$form = drupal_get_form('inline_files_node_table',$page,$bundle,$type_names);
	drupal_add_css( drupal_get_path('module','inline_files') . '/css/admin.css');
	$admin_path = 'admin/content/file/inline-files';
	$items = array('-' => l('All',$admin_path));
	
	foreach ($type_names as $t => $tn) {
		$items[] = l($tn, $admin_path . '/' . $t);
	}
	
	$vars = array(
		'type' => 'ul',
		'title' => NULL,
		'attributes' => array('class' => array('pager')),
		'items' => $items
	);
	
	$type_selector = theme('item_list', $vars);
	
	return $type_selector . render($form);
}

function inline_files_node_table($form,&$form_state,$page = 0,$bundle = NULL,$type_names = array()) {
	$total = inline_files_nodes_get(0,true, $bundle);
	$data = array();
	$domain_ref = str_replace('.','\.', variable_get('inline_files_domain_ref','freetibet.org'));
	if ($total > 0) {
		$data = inline_files_nodes_get($page,false,$bundle);
	}
	$rows = array();
	if (!empty($data)) {
		$public_path = variable_get('file_public_path','sites/default/files');
		$file_dir = base_path() . $public_path;
		$rgx = '(src|href)=["\']*([^"\'> ]*)(' . $file_dir . ')?/(([^"\'>\/]+?/)*[^"\'>\/]+?\.(\w+))[^\w.]';
	
		$field_info = field_info_fields();
		$fileFields = array();
		$imageFields = array();
		foreach ($field_info as $fName => $field_data) {
			switch ($field_data['type']) {
				case 'image':
				case 'file':
					$fileFields[] = $fName;
					break;
			}
		}
		$opts = array('query' => array('destination' => $_GET['q']));
		$imgOpts = array('html' => TRUE,'attributes' => array('target' => '_blank'));
	
		pager_default_initialize($total, INLINE_FILES_PER_PAGE, 0);
		$pager = theme('pager');
		$form['top_pager'] = array(
			'#type' => 'markup',
			'#markup' => $pager
		);
		foreach ($data as $item) {
			$imgs = array();
			if (preg_match_all('#'.$rgx.'#' , $item->body_value,$matches)) {
				$validFields = array();
				if (array_key_exists($item->type,$type_names)) {
					$typeName = $type_names[$item->type];
					$validFields = inline_files_match_file_fields($item->type,$fileFields,$field_info);
				}	
				if (isset($matches[4])) {
					foreach ($matches[4] as $index => $sub_path) {
						$fd = '';
						if (!empty($matches[2][$index]) == $file_dir) {
							$src = NULL;
							$extension = strtolower($matches[6][$index]);
						
							$is_domain = (bool) (
								preg_match('#^(org|uk|com|net|info|edu)$#',$extension)
									|| preg_match('#^www\.#',$matches[3][$index])
									|| (
										preg_match('#^https?\b#',$matches[2][$index])
											&& !preg_match('#'.$domain_ref.'\b.*?\.(jpe?g|png|gif|pdf|mp3)$#',$matches[0][$index])
										)
							);
							if (!$is_domain) {
									$is_image = (bool) preg_match('#^(jpe?g|png|gif|bmp)$#',$extension);
									$file_type = $is_image? 'image' : 'file';
									$file = inline_files_match_entity($sub_path,$validFields,$file_type,$field_info,$index);
									$sub_path = urldecode($sub_path);
									$longSrc =  $file_dir . '/' . $sub_path;
									if ($file->fid > 0) {
										if ($is_image) {
											$src = image_style_url('media_thumbnail',$sub_path);
										}
										$fd = l('[file]','file/'.$file->fid.'/edit', $opts);
										switch ($file->related_entity_type) {
											case 'node':
												$fd .= ' ' . l('[entity]', 'node/'.$file->related_entity_id.'/edit', $opts);
												break;
										}
									}
									else {
										$src = $matches[2][$index] . $matches[3][$index] . '/' . $sub_path;
									}
									if (empty($src)) {
										$src = $longSrc;
									}
									if ($file->related_entity_id > 0 && $file->related_entity_id == $item->nid) {
										$fd .= ' (matched on <em>'.$file->related_entity_field.'<em>)';
									}
									if ($is_image) {
										$img = l('<img src="'.$src.'" title="'.$src.'" class="media-thumbnail">',$longSrc,$imgOpts);
									}
									else {
										$img = l('[File]' , $src);
									}
									/*if (preg_match('#www\.freetibet#',$src)) {
										var_dump($matches,$index);exit;
									}*/
									$cells = array();
									$cells['title'] = l($item->title,'node/'.$item->nid.'/edit',$opts) . ' ('.$type_names[$item->type].')';
									$cells['images'] = $img;
									$cells['status'] = $fd;
									$cells['default'] =  $file->default_field;
									$cells['published'] =  $item->status>0? 'yes' : 'no';
									$cells['created'] =  format_date($item->created,'short');
										$fref = $file->fid > 0? $file->fid : $sub_path;
										if (isset($field_info[$file->default_field]['cardinality'])) {
											$intPlural = $field_info[$file->default_field]['cardinality'] == 1? 0 : 1;
										}
										else {
											$intPlural = 0;
										}
										$key = $item->nid . '-' . $file->default_field. '-' . $intPlural . '-' .$fref;
									$rows[$key] = $cells;
								}		
						}
					}
				}
			}
		}
	}
	
	$header = array(
    'title' => t('Title'),
    'images' => t('Imges'),
    'status' => t('Edit file / related entity'),
		'default' => t('Default file field'),
		'published' => t('Published'),
		'created' => t('Created')
  );
	
	$form['table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t('No content available.'),
  );
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Update'
	);
	if (!empty($pager)) {
		$form['bottom_pager'] = array(
			'#type' => 'markup',
			'#markup' => $pager
		);
	}
	$form['#submit'] = array('inline_files_node_table_submit');
	return $form;
}


function inline_files_node_table_submit(&$form, &$form_state) {
	if (isset($form_state['values']['table']) && is_array($form_state['values']['table'])) {
		foreach ($form_state['values']['table'] as $key => $value) {
			if (is_string($value) && preg_match('#^\d+-\w+-#', $value)) {
				$parts = explode('-', $key);
				$nid = (int) array_shift($parts);
				if (!empty($parts)) {
					$field_ref = array_shift($parts);
					if (!empty($parts) && count($parts) > 1) {
						$plural = array_shift($parts);/181
						$plural = (bool) $plural;
						$fileRef = implode('-', $parts);
						$fid = 0;
						if (is_numeric($fileRef)) {
							$fid = (int) $fileRef;
						}
						if ($fid > 0) {
							$node = node_load($nid);
							if (is_object($node)) {
								$field = inline_files_build_file_field($node,$field_ref,$fid,$plural);
								node_save($node);
							}
						}
					}
				}
			}
		}
	}
}
function inline_files_build_file_field($node,$field_ref = NULL,$fid = 0,$plural = false) {
	if (is_string($field_ref) && !empty($field_ref) && isset($node->{$field_ref})) {
		$lang = (!empty($node->language))? $node->language : LANGUAGE_NONE;
		$items = field_get_items('node',$node,$field_ref);
		if (empty($items)) {
			$items = array();
		}
		$item = array(
			'fid' => $fid,
			'title' => $node->title
		);
		if ($plural || empty($items)) {
			$items[] = $item;
		}
		$node->{$field_ref} = array(
			$lang => $items
		);
	}
}

function inline_files_nodes_get($page=0,$count=false, $bundle = NULL) {
	$query = db_select("field_data_body","b")->fields('b',array('body_value'));
	$public_path = variable_get('file_public_path','sites/default/files');
	$rgx = '(src|href)=["\']*[^"\'> ]*' . base_path() . $public_path;
	if (!$count) {
		$query->join("node",'n',"b.entity_id=n.nid AND b.revision_id=n.vid");
		$query->fields('n', array('nid','vid', 'type','title', 'status','created','changed') );
	}	
	$query->condition('b.body_value',$rgx,'REGEXP');
	if (!empty($bundle)) {
		$query->condition('b.bundle',$bundle);
	}
	if (!$count) {
		$start = $page * INLINE_FILES_PER_PAGE;	
		$query->range($start,INLINE_FILES_PER_PAGE);
		$query->orderBy('n.created','DESC');
	}
	$result = $query->execute();
	$data = array();
	if (!$count) {
		if ($result) {
			$data = $result->fetchAll();
		}
	}
	else {
		$total = 0;
		if ($result) {
			$total = $result->rowCount();
		}
		return $total;
	}
	return $data;
}

function inline_files_match_file_fields($bundle,&$fileFields,&$field_info,$type = 'file') {
	static $inline_files_field_data = array();
	if (!isset($inline_files_field_data[$bundle])) {
		$validFields = array();
		foreach ($fileFields as $field) {
			if (isset($field_info[$field]) && isset($field_info[$field]['bundles']['node'])) {
				$bundles = $field_info[$field]['bundles']['node'];
				if (in_array($bundle, $bundles)) {
					$validFields[] = $field;
				}
			}
		}
		$inline_files_field_data[$bundle] = $validFields;
	}
	return $inline_files_field_data[$bundle];
}

/*
 * Wrapper for theme_table to make it easier to build tables programmatically
 * with miminal code
 */
function inline_files_table($header,$rows,$attrs = array(),$caption=NULL,$addPager = false) {
  if (!is_array($attrs)) {
    if (is_string($attrs)) {
      $attrs = array('class' => array($attrs));
    } else {
      $attrs = array();
    }
  }
  if (isset($attrs['class']) && !is_array($attrs['class'])) {
    $attrs['class'] = array($attrs['class']);
  }
  if (!is_array($header)) {
    $header = array();
  }
  if (!is_array($rows)) {
    $rows = array();
  }
  if (empty($caption) && isset($attrs['caption'])) {
    $caption = $attrs['caption'];
    unset($attrs['caption']);
  }
  $vars = array(
    'caption' => $caption,
    'attributes' => $attrs,
    'header' => $header,
    'rows' => $rows,
    'colgroups' => array(),
    'sticky' => FALSE,
    'empty' => FALSE
  );
	if ($addPager) {
		$vars['sticky'] = TRUE;
		$total = 0;
		if (isset($attrs['total'])) {
			$total = (int) $attrs['total'];
			unset($attrs['total']);
		}
		if (isset($attrs['per_page'])) {
			$perPage = (int) $attrs['per_page'];
			unset($attrs['per_page']);
		}
		pager_default_initialize($total, $perPage, 0);
		$pager = theme('pager');
		return $pager.theme_table($vars).$pager;
	} else {
		return theme_table($vars);
	}
}

function inline_files_match_entity($file_path = '',$file_fields = array(), $type = NULL,&$field_info, $delta = 0) {
	$query = db_select('file_managed','f')->fields('f',array('fid'));
	$default_field = NULL;
	$simpleType = $type == 'image'? $type : 'file';
	if (!empty($file_fields)) {
		foreach ($file_fields as $index => $field_name) {
			$cardinality = $field_info[$field_name]['cardinality'];
			$field_type = $field_info[$field_name]['type'];
			switch ($field_name) {
				case 'field_image':
					if (($delta === 0 || $cardinality < 0 ||  $cardinality > $delta) && $simpleType == 'image') {
						$default_field = $field_name;
					}
					break;
				default:
					if (empty($default_field) && $simpleType == $field_type) {
						$default_field = $field_name;
					}
				break;
			}
			$query->leftJoin('field_data_'.$field_name,'im'.$index,'f.fid = im'.$index.'.'.$field_name.'_fid');
			$query->addField('im' . $index,'entity_id','entity_id' . $index);
			$query->addField('im' . $index,'entity_type','entity_type' . $index);
		}
		
	}
	if (empty($default_field)) {
		$default_field = "none";
	}
	$rgx = '://(' . $file_path . '|'.urldecode($file_path).')$';
	$query->condition('f.uri',$rgx,'REGEXP');
	$result = $query->execute();
	$file = new StdClass;
	$file->fid = 0;
	if ($result) {
		$data = $result->fetch();
		
		if (is_object($data)) {
			$f = file_load($data->fid);
			if (is_object($f)) {
				$file = $f;
				$file->related_entity_id = 0;
				$file->related_entity_type = NULL;
				$file->related_entity_field = '';
				if (!empty($file_fields)) {
					foreach ($file_fields as $index => $field_name) {
						$entity_id_name = 'entity_id' . $index;
						
						if (isset($data->{$entity_id_name}) && $data->{$entity_id_name} > 0) {
							$file->related_entity_id = $data->{$entity_id_name};
							$entity_type_name = 'entity_type' . $index;
							$file->related_entity_type = $data->{$entity_type_name};
							$file->related_entity_field = $field_name;
							break;
						}
					}
				}
			}
		}
	}
	$file->default_field = $default_field;
	return $file;
}