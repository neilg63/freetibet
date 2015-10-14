<?php
/**
 * @file
 * Contains the theme's functions to manipulate Drupal's default markup.
 *
 * A QUICK OVERVIEW OF DRUPAL THEMING
 *
 *   The default HTML for all of Drupal's markup is specified by its modules.
 *   For example, the comment.module provides the default HTML markup and CSS
 *   styling that is wrapped around each comment. Fortunately, each piece of
 *   markup can optionally be overridden by the theme.
 *
 *   Drupal deals with each chunk of content using a "theme hook". The raw
 *   content is placed in PHP variables and passed through the theme hook, which
 *   can either be a template file (which you should already be familiary with)
 *   or a theme function. For example, the "comment" theme hook is implemented
 *   with a comment.tpl.php template file, but the "breadcrumb" theme hooks is
 *   implemented with a theme_breadcrumb() theme function. Regardless if the
 *   theme hook uses a template file or theme function, the template or function
 *   does the same kind of work; it takes the PHP variables passed to it and
 *   wraps the raw content with the desired HTML markup.
 *
 *   Most theme hooks are implemented with template files. Theme hooks that use
 *   theme functions do so for performance reasons - theme_field() is faster
 *   than a field.tpl.php - or for legacy reasons - theme_breadcrumb() has "been
 *   that way forever."
 *
 *   The variables used by theme functions or template files come from a handful
 *   of sources:
 *   - the contents of other theme hooks that have already been rendered into
 *     HTML. For example, the HTML from theme_breadcrumb() is put into the
 *     $breadcrumb variable of the page.tpl.php template file.
 *   - raw data provided directly by a module (often pulled from a database)
 *   - a "render element" provided directly by a module. A render element is a
 *     nested PHP array which contains both content and meta data with hints on
 *     how the content should be rendered. If a variable in a template file is a
 *     render element, it needs to be rendered with the render() function and
 *     then printed using:
 *       <?php print render($variable); ?>
 *
 * ABOUT THE TEMPLATE.PHP FILE
 *
 *   The template.php file is one of the most useful files when creating or
 *   modifying Drupal themes. With this file you can do three things:
 *   - Modify any theme hooks variables or add your own variables, using
 *     preprocess or process functions.
 *   - Override any theme function. That is, replace a module's default theme
 *     function with one you write.
 *   - Call hook_*_alter() functions which allow you to alter various parts of
 *     Drupal's internals, including the render elements in forms. The most
 *     useful of which include hook_form_alter(), hook_form_FORM_ID_alter(),
 *     and hook_page_alter(). See api.drupal.org for more information about
 *     _alter functions.
 *
 * OVERRIDING THEME FUNCTIONS
 *
 *   If a theme hook uses a theme function, Drupal will use the default theme
 *   function unless your theme overrides it. To override a theme function, you
 *   have to first find the theme function that generates the output. (The
 *   api.drupal.org website is a good place to find which file contains which
 *   function.) Then you can copy the original function in its entirety and
 *   paste it in this template.php file, changing the prefix from theme_ to
 *   freetibet_. For example:
 *
 *     original, found in modules/field/field.module: theme_field()
 *     theme override, found in template.php: freetibet_field()
 *
 *   where freetibet is the name of your sub-theme. For example, the
 *   zen_classic theme would define a zen_classic_field() function.
 *
 *   Note that base themes can also override theme functions. And those
 *   overrides will be used by sub-themes unless the sub-theme chooses to
 *   override again.
 *
 *   Zen core only overrides one theme function. If you wish to override it, you
 *   should first look at how Zen core implements this function:
 *     theme_breadcrumbs()      in zen/template.php
 *
 *   For more information, please visit the Theme Developer's Guide on
 *   Drupal.org: http://drupal.org/node/173880
 *
 * CREATE OR MODIFY VARIABLES FOR YOUR THEME
 *
 *   Each tpl.php template file has several variables which hold various pieces
 *   of content. You can modify those variables (or add new ones) before they
 *   are used in the template files by using preprocess functions.
 *
 *   This makes THEME_preprocess_HOOK() functions the most powerful functions
 *   available to themers.
 *
 *   It works by having one preprocess function for each template file or its
 *   derivatives (called theme hook suggestions). For example:
 *     THEME_preprocess_page    alters the variables for page.tpl.php
 *     THEME_preprocess_node    alters the variables for node.tpl.php or
 *                              for node--forum.tpl.php
 *     THEME_preprocess_comment alters the variables for comment.tpl.php
 *     THEME_preprocess_block   alters the variables for block.tpl.php
 *
 *   For more information on preprocess functions and theme hook suggestions,
 *   please visit the Theme Developer's Guide on Drupal.org:
 *   http://drupal.org/node/223440 and http://drupal.org/node/1089656
 */


/**
 * Override or insert variables into the maintenance page template.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("maintenance_page" in this case.)
 */
/* -- Delete this line if you want to use this function
function freetibet_preprocess_maintenance_page(&$variables, $hook) {
  // When a variable is manipulated or added in preprocess_html or
  // preprocess_page, that same work is probably needed for the maintenance page
  // as well, so we can just re-use those functions to do that work here.
  freetibet_preprocess_html($variables, $hook);
  freetibet_preprocess_page($variables, $hook);
}
// */

/**
 * Override or insert variables into the html templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("html" in this case.)
 */
function freetibet_preprocess_html(&$variables, $hook) {
	global $theme;
	if ($theme == 'freetibet') {
		$htmlClasses = array();
		if (isset($variables['classes_array']) && is_array($variables['classes_array'])) {
			$badIndex = array_search('page-node-',$variables['classes_array']);
			if (is_int($badIndex) && isset($variables['classes_array']) && is_array($variables['classes_array']) && array_key_exists($badIndex, $variables['classes_array'])) {
				unset($variables['classes_array'][$badIndex]);
			}
      _freetibet_add_aside($variables);
   
			$pageData = freetibet_add_page_section_classes($variables['classes_array']);
		}
	}
}

/**
 * Override or insert variables into the page templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("page" in this case.)
 */
function freetibet_preprocess_page(&$variables, $hook) {
	global $theme;
	if ($theme == 'freetibet') {
		if (!is_array($variables['classes_array'])) {
			$variables['classes_array'] = array();
		}
		$pageData = freetibet_add_page_section_classes($variables['classes_array']);
    _freetibet_add_aside($variables);
		// Assign section a variable in template.php
		$variables['section'] = $pageData->section;
		$variables['is_admin'] = user_is_logged_in() && user_access('administer nodes');
    
		if (isset($variables['page']['content']['system_main']['nodes'])) {
			$nData = $variables['page']['content']['system_main']['nodes'];
			if (count($nData)>1 && isset($nData['#sorted']) && $nData['#sorted'] == 1) {
				foreach ($nData as $nid => $cData) {
					if (is_numeric($nid) && is_array($cData) && isset($cData['#node'])) {
						$node = $cData['#node'];
						$variables['classes_array'][] = 'node-type-' . str_replace('_','-',$node->type);
						break;
					}
				}
			}
		}
		$variables['show_title'] = TRUE;
		if (function_exists('freetibet_inner_content_id')) {
			$variables['inner_content_id'] = freetibet_inner_content_id();
		} else {
			$variables['inner_content_id'] = 'inner-content';
		}
		
		$no_title_node_types = array('exhibition');
		// NG Some pages do not have a main node
		if (isset($variables['node']) && in_array($variables['node']->type, $no_title_node_types)) {
			$variables['show_title'] = FALSE;
		}
		//add script to scale viewport on ios devices including ipad
		//https://drupal.org/node/1888028
		//also add maximum scale
		//http://stackoverflow.com/questions/2581957/iphone-safari-does-not-auto-scale-back-down-on-portrait-landscape-portrait
		$metaItems = array();
		
		$metaItems[] = array(
				'#tag' => 'meta',
				'#attributes' => array(
						'name' => 'viewport',
						'content' => 'width=device-width, height=device-height, maximum-scale=1.0, initial-scale=1.0, user-scalable=yes',
				),
		);
		
		$metaItems[] = array(
				'#tag' => 'meta',
				'#attributes' => array(
						'name' => 'apple-mobile-web-app-capable',
						'content' => 'yes',
				),
		);
		
		$metaItems[] = array(
				'#tag' => 'meta',
				'#attributes' => array(
						'name' => 'apple-mobile-web-app-status-bar-style',
						'content' => 'black',
				),
		);

		// Loop around each meta item and ensure correct array keys exist
		
		foreach ($metaItems as $meta) {
			if (isset($meta['#attributes']) && is_array($meta['#attributes']) && isset($meta['#attributes']['name'])) {
				drupal_add_html_head($meta, $meta['#attributes']['name']);
			}
		}
		
	} // end of default theme overrides
	
}

function _freetibet_add_aside(&$variables) {
  $page = $variables['page'];
  $aside = NULL;
  $has_aside = false;
  $variables['aside'] = NULL;
  if (isset($page['aside'])) {
    $aside = render($page['aside']);
    $has_aside = (!empty($aside) && is_string($aside) && strlen($aside) > 10);
    if ($has_aside) {
      $variables['classes_array'][] = 'has-sidebar';
      $variables['aside'] = $aside;
      $ns = array_search('no-sidebars',$variables['classes_array']);
      if ($ns >- 0) {
        unset($variables['classes_array'][$ns]);
        $variables['classes_array'] = array_values($variables['classes_array']);
      }
    }
  }
  $variables['has_aside'] = $has_aside;
}

function freetibet_css_alter(&$css) {
	global $theme;
	if ($theme == 'freetibet') {
		$contribModuleCss = freetibet_allow_module_css_paths();
		$contribModuleCssRegex = '#/modules/contrib/('.implode('|',$contribModuleCss).')#';
		foreach ($css as $path => $data) {
			if (preg_match('#(/modules/contrib/|modules/system/|/zen/)#', $path) ) {
				if (!preg_match($contribModuleCssRegex, $path)) {
					unset($css[$path]);
				}
			}
		}
	}
}

function freetibet_allow_module_css_paths() {
	$paths = variable_get('freetibet_allow_module_css_paths',NULL);
	if (!is_array($paths)) {
		$paths = array('admin_menu','colorbox','jquery_update','apachesolr','eu_cookie_compliance','live_css');
	}
	return $paths; 
}

function freetibet_device_size_breakpoints() {
	$hash = array('width' => 640, 'height' => 0);
	$defaults = variable_get('freetibet_device_size_breakpoints', $hash);
	if (!is_array($defaults)) {
		$defaults = $hash;
	}
	return $defaults;
}

function freetibet_add_page_section_classes(&$classes_array) {
	$data = new StdClass;
	$data->path = request_path();
	if (empty($data->path)) {
		$data->path = 'home';
	}
	$parts = explode('/',$data->path);
	$data->section = array_shift($parts);
	$pathAlias = str_replace('/','-',$data->path);
//	$classes_array[] = 'section-' . $data->section;
	$classes_array[] = 'page-' . $pathAlias;
	return $data;
}


function freetibet_menu_tree__main_menu(array $variables) {
	return _freetibet_menu_tree($variables);
}

function freetibet_menu_tree__menu_about(array $variables) {
	return _freetibet_menu_tree($variables);
}

function _freetibet_menu_tree(array &$variables) {
	$num_items = 0;
	if (is_string($variables['tree']) && strlen($variables['tree']) > 10) {
		$string = preg_replace('#<ul.*?>(.|\s+)*?</ul>#','',$variables['tree']);
		$num_items = count(explode('<li',$string)) - 1;
	}
	return '<ul class="menu num-items-'.$num_items.'">' . $variables['tree'] . "</ul>\n";
}

function freetibet_menu_link($variables) {
	$element = $variables['element'];
	if (!is_array($element['#attributes']['class'])) {
		$element['#attributes']['class']=array();
	}
	$sub_menu = '';
	if ($element['#below']) {
		$sub_menu = render($element['#below']);
	}
	$alias = drupal_lookup_path('alias',$element['#href']);
	if (is_string($alias) && !empty($alias)) {
		$path = $alias;
	} else {
		$path = $element['#href'];
	}
  $parts = explode('/',$path);
  $path = array_pop($parts);
  $path = strtolower($path);
  $numParts = count($parts);
  $maxParts = $numParts < 4? $numParts : 4;
  if ($numParts> 0) {
    for ($i =0; $i < $numParts; $i++) {
      $cn = strtolower(trim($parts[$i]));
      if (strpos($cn,'.') > 0) {
        $ps = explode('.', $cn);
        $cNameParts = array();
        foreach ($ps as $index => $p) {
          if (strlen($p)>1 && !preg_match('#(www|freetibet|org|uk|com|info)#',$p)) {
            $cNameParts[] = $p;
          }
        }
        if (!empty($cNameParts)) {
          $domainClass = implode('-', $cNameParts);
          $element['#attributes']['class'][] = $domainClass;
          if (preg_match('#(twitter|facebook|youtube|intagram)#',$domainClass)) {
            $element['#attributes']['class'][] = 'follow-us';
            $element['#attributes']['title'] = $element['#title'];
          }
        }
      }
    }
  }
	if ($path == '<front>') {
		$path = 'home';
	}
	if (strlen($path) > 1) {
	  $element['#attributes']['class'][] = str_replace('/','-',$path);
	}
	if (current_path() == $element['#href']) {
		$element['#attributes']['class'][] = 'active';
	}
	if (isset($element['#original_link'])) {
		$link = $element['#original_link'];
		if (isset($link['has_children'])) {
			if ($link['has_children'] > 0) {
				$element['#attributes']['class'][] = 'has-children';
			}
		}
	}
	$output = l($element['#title'], $element['#href'], $element['#localized_options']);
	return '<li' . drupal_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

function freetibet_html_head_alter(&$head_elements) {
  if(isset($head_elements['metatag_og:image_0'])){
    $head_elements['metatag_og:image_0']['#value'] = str_replace(' ', '%20', $head_elements['metatag_og:image_0']['#value']);
  }
  if(isset($head_elements['metatag_image_src'])){
    if (arg(0) == 'node') {
      $arg1 = arg(1);
      $arg2 = arg(2);
      if (is_numeric($arg1) && empty($arg2)) {
        $arg1 = (int) $arg1;
        $n = node_load($arg1);
        if (is_object($n)) {
          $items = field_get_items('node', $n, 'field_image');
          if (!empty($items) && isset($items[0]['uri'])) {
            $image_url = file_create_url($items[0]['uri']);
            $image_url = str_replace(' ', '%20', $image_url);
            $head_elements['metatag_image_src']['#value'] = $image_url;
            $head_elements['og:image'] = $head_elements['system_meta_content_type'];
            $head_elements['og:image']['#attributes'] = array(
              'property' => 'og:image',
              'content' => $image_url
            );
            $head_elements['og:image']['#weight'] = 20;
          }
        }
      }
    }
    
  }
}


function freetibet_references_dialog_links($variables) {
  if (isset($variables['links']) && is_array($variables['links'])) {
      $links = array();
      $precedence = array('media-section','text','slide-section','key-points');
      foreach ($variables['links'] as $item) {
        $item['title'] = preg_replace('#^Create\s#','',$item['title']);
        $parts = explode('/',$item['href']);
        $type = array_pop($parts);
        $links[$type] = $item;
      }
      $sortedLinks = array();
      foreach ($precedence as $key) {
        if (isset($links[$key])) {
          $sortedLinks[$key] = $links[$key];
          unset($links[$key]);
        }
      }
      if (!empty($links)) {
        $sortedLinks += $links;
      }
    return theme('links', array('links' => $sortedLinks, 'attributes' => array('class' => array('references-dialog-links'))));  
  }
}

/*
* Aux function used in page section templates
* @return boolean
*/
function freetibet_page_section_classes(&$classes,&$ds_content_wrapper,$node) {
  $classes .= ' ';
  $classes = preg_replace('#\bnode(-by-viewer)?\s+#','',$classes);
  $classes = 'page-section ' . trim(preg_replace('#\s\s+#',' ',$classes));
  $has_wrapper = $ds_content_wrapper != 'span';
  if (function_exists('node_class') && is_object($node)) {
  	$extra_class = node_class($node);
  	if (!empty($extra_class)) {
  		$classes .= ' ' . trim($extra_class);
  	}
  }
  return $has_wrapper;
}

function freetibet_take_action_node_classes_alter(&$classes,&$variables) {
  $arrClasses = explode(' ', $classes);
  $has_body = false;
  $has_image = false;
  if (isset($variables['elements']['#node'])) {
    $n =& $variables['elements']['#node'];
    $items = field_get_items('node',$n, 'field_body');
    if (!empty($items) && is_array($items)) {
      if (isset($items[0]['value'])) {
        if (strlen($items[0]['value']) > 2) {
          $has_body = true;
        }
      }
    }
    $items = field_get_items('node',$n, 'field_image');
    if (!empty($items)) {
      if (isset($items[0]['fid']) && is_numeric($items[0]['fid'])) {
        if ($items[0]['fid'] > 0) {
          $has_image = true;
        }
      }
    }
  }
  if ($has_body) {
    $classes .= ' has-body';
  }
  else {
    $classes .= ' no-body';
  }
  if ($has_image) {
    $classes .= ' has-image';
  }
  else {
    $classes .= ' no-image';
  }
}

function freetibet_slide_fieldset_classes_alter(&$classes,&$variables) {
  $arrClasses = explode(' ', $classes);
  $filteredClasses = array();
  $modeClass = 'none';
  if (is_array($arrClasses) && !empty($arrClasses)) {
    foreach ($arrClasses as $cName) {
      $cName = trim($cName);
      if (strpos($cName, 'view-mode-') === 0) {
        $parts = explode('-mode-',$cName);
        $modeClass = array_pop($parts);
        break;
      }
    }
  }

  switch ($modeClass) {
    case 'slider':
    case 'full':
      $classes = str_replace('entity-field-collection-item','flex-slide', trim($classes));
      break;
    default:
    
      break;
  }

  $has_block = false;
  $has_link = false;
  $has_subtitle = false;
  $variables['override_block_title'] = false;
  if (isset($variables['elements']['#entity'])) {
    $fc =& $variables['elements']['#entity'];
    
    $items = field_get_items('field_collection_item',$fc, 'field_strapline');
    if (!empty($items) && isset($items[0]['value'])) {
      $val = $items[0]['value'];
      if ($val == '<none>') {
        unset($variables['field_strapline']);
        unset($variables['elements']['field_strapline']);
        unset($variables['content']['field_strapline']);
        unset($fc->field_strapline);
        $classes .= 'no-strapline';
      }
    }
    
    $items = field_get_items('field_collection_item',$fc, 'field_media');
    $hasMedia = false;
    if (!empty($items) && is_array($items)) {
      if (isset($items[0]['filemime']) && is_string($items[0]['filemime'])) {
        $parts  = explode('/', $items[0]['filemime']);
        $classes .=  ' ' . array_shift($parts);
        $hasMedia = true;
      }
    }
    if (!$hasMedia) {
      $classes .=  ' no-media';
    }
    $items = field_get_items('field_collection_item',$fc, 'field_highlighted');
    if (!empty($items)) {
      $classes .= ' has-block';
      $has_block = true;
    }
    
    $items = field_get_items('field_collection_item',$fc, 'field_subtitle');
    if (!empty($items) && !empty($items[0]['value'])) {
      $classes .= ' has-subitle';
      $has_subtitle = true;
      $subtitle = $items[0]['value'];
    }
    
    $items = field_get_items('field_collection_item',$fc, 'field_link');
    if (!empty($items)) {
      $classes .= ' has-link';
      $has_link = true;
    }
  }
  if ($has_block && $has_link) {
    $classes .= ' has-link-and-block';
  }
  if ($has_subtitle && $has_link) {
    $classes .= ' has-subtitle-and-link';
  }
  if ($has_subtitle && $has_block) {
    $classes .= ' has-subtitle-and-block';
    if (strlen($subtitle) > 1) {
      $variables['block_title'] = $subtitle;
      $fc->field_subtitle = array();
      $variables['override_block_title'] = true;
    }
  }
}

function freetibet_replace_block_title(&$variables, &$ds_content) {
  if (isset($variables['block_title']) && is_string($variables['block_title'])) {
    $block_title = trim($variables['block_title']);
    if (strlen($block_title) > 2) {
      $ds_content = preg_replace('#(<h3\b[^>]*?block-title\b[^>]*?>)[^<]*?(</h3>)#i',"$1".$block_title."$2",$ds_content);
      $ds_content = preg_replace('#(<p\b[^>]*?field-name-field-subtitle\b[^>]*?>)[^<]*?(</p>)#i','',$ds_content);
      $ds_content = preg_replace('#<div\b[^>]*?group-details\b[^>]*?>\s*</div>#i','',$ds_content);
    }
  }
}

function freetibet_add_node_classes(&$classes, &$content,$node) {
  $content_fields = array_keys($content);
  if (isset($node->field_hide_default_image)) {
    $hide_defimg = $node->field_hide_default_image;
  }
  else {
    $hide_defimg = false;
  }
  $classes = trim($classes);
  $has_image = false;
  if (in_array('field_image',$content_fields)) {
  	$suppress = false;
  	if (isset($hide_defimg) && is_array($hide_defimg)) {
  		foreach ($hide_defimg as $lang => $vals) {
  			if (!empty($vals) && isset($vals[0]) && isset($vals[0]['value'])) {
  				$suppress = (bool) $vals[0]['value'];
  			}
  		}
  	}
  	if (!$suppress) {
  		$classes .= ' has-default-image';
      $has_image = true;
  	}
  }
  if (!$has_image) {
    $classes .= ' no-default-image';
  }
  if (in_array('field_key_points',$content_fields)) {
  	$num_items = count($content['field_key_points']['#items']);
  	$hasKeyPoints = false;
  	if ($num_items > 0) {
  		foreach ($content['field_key_points']['#items'] as $index => $data) {
  			if (isset($content['field_key_points'][$index]['entity']['field_collection_item'])) {
  				foreach ($content['field_key_points'][$index]['entity']['field_collection_item'] as $id => $fd) {
  					if (isset($fd['field_text']['#items'][0]['value'])) {
  						if (strlen($fd['field_text']['#items'][0]['value']) > 2) {
  							$hasKeyPoints = true;
  							break;
  						}
  					}
  				}
  			}
  			break;
  		}
  	}
  	if ($hasKeyPoints) {
  		$classes .= ' has-key-points';
  	}
  }

  if (function_exists('node_class') && is_object($node)) {
  	$extra_class = node_class($node);
  	if (!empty($extra_class)) {
  		$classes .= ' ' . trim($extra_class);
  	}
  }

  if (isset($node->node_extra_class) && is_string($node->node_extra_class)) {
    $extra_class = $node->node_extra_class;
  	if (!empty($extra_class)) {
  		$classes .= ' ' . trim($extra_class);
  	}
  }
}