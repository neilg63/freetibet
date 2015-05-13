<?php
/**
* @file
* Form building, processing and validation for Campaigner module configuration settings.
*/

/**
* Page callback: Campaigner settings
*
* @see campaigner_menu()
*/
function campaigner_config_form($form, &$form_state) {
	$form['campaigner_petitions_path'] = array(
		'#type' => 'textfield',
		'#title' => t('Petitions URL'),
		'#default_value' => variable_get('campaigner_petitions_path', 'petitions'),
		'#size' => 20,
		'#maxlength' => 200,
		'#description' => t('The base URL through which the petitions will be accessible (do not include the Drupal base path, or leading or trailing slashes).'),
		'#required' => TRUE,
	);
	
	$form['campaigner_email_targets_path'] = array(
		'#type' => 'textfield',
		'#title' => t('Email targets URL'),
		'#default_value' => variable_get('campaigner_email_targets_path', 'email_targets'),
		'#size' => 20,
		'#maxlength' => 200,
		'#description' => t('The base URL through which the email actions will be accessible (do not include the Drupal base path, or leading or trailing slashes).'),
		'#required' => TRUE,
	);

  $form['geos'] = array(
    '#type' => "fieldset",
    '#description' => t("The Geos PHP library is used to work out whether given locations (e.g. from a postcode or an IP) belong in a defined region."),
    '#title' => t('Geos Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );
  $disabled = true;
  if (function_exists("geophp_load")){
    $geophp = geophp_load();
    $disabled = !geoPHP::geosInstalled();

  }
  if ($disabled){
    variable_set("campaigner_use_geos", 0);
  }
  $form['geos']['campaigner_use_geos'] = array(
    '#type' => "checkbox",
    '#title' => t("Use GEOS"),
    '#description' => t("Using GEOS might require a high amount of RAM (probably over 300M) particularly if you don't have the GEOS extension enabled. GEOS will
    be more accurate than other methods and will give better results."),
    '#default_value' => variable_get("campaigner_use_geos", 0),
    '#disabled' => $disabled
  );


  $form['geos']['campaigner_geos_cache_invalidation'] = array(
    '#type'=>'textfield',
    '#title' => t("Geos Cache invalidation time"),
    '#default_value' => variable_get("campaigner_geos_cache_invalidation", 86400),
    '#description' => t("The number of seconds before we should regenerate the cache for each region (ignored if a region is edited by a user).")
  );

  $form['geos']['campaigner_geos_cache_rebuild_link'] = array(
    '#markup' => "<div>
                      <p>" . l("Rebuild GEOS cache", 'admin/config/content/campaigner/geos/rebuildcache', array("attributes"=>array("class"=>"button btn-primary"))) . "</p>
                  </div>"
  );

  $form['spam'] = array(
    "#type" => "fieldset",
    '#title' => t("Spam filtering"),
    'train-spam' => array(
      '#markup' => "<div>
                      <p>" . l("Train spam filter", 'admin/config/content/campaigner/spam/train', array("attributes"=>array("class"=>"button btn-primary"))) . "</p>
                  </div>"
    )
  );

	/*
	IP lookup service does not require an API key
	Code below left in case we switch IP lookup service  

	$form['campaigner_geoip_api_key'] = array(
		'#type' => 'textfield',
		'#title' => t('IP geolocation API key'),
		'#default_value' => variable_get('campaigner_geoip_api_key', ''),
		'#size' => 50,
		'#maxlength' => 500,
		'#description' => t('The API key for the web service used to convert IP addresses to geographical locations.'),
		'#required' => FALSE,
	); */

	return system_settings_form($form);
}