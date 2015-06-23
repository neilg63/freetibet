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
			$page = freetibet_add_page_section_classes($variables['classes_array']);
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
		$page = freetibet_add_page_section_classes($variables['classes_array']);

		// Assign section a variable in template.php
		$variables['section'] = $page->section;
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
	if ($path == '<front>') {
		$path = 'home';
	}
	if (!is_array($element['#attributes']['class'])) {
		$element['#attributes']['class']=array();
	}
	$element['#attributes']['class'][] = str_replace('/','-',$path);
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


function freetibet_references_dialog_links($links) {
  return theme('links', array('links' => $links, 'attributes' => array('class' => array('references-dialog-links'))));
}