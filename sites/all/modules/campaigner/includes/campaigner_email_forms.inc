<?php
/**
* @file
* Form building, processing and validation for Campaigner module email target forms.
*/

//Load includes
module_load_include('inc', 'campaigner', 'includes/campaigner_location_services');
  module_load_include('inc', 'campaigner', 'includes/campaigner_signatures');
  module_load_include('inc', 'campaigner', 'includes/campaigner_widgets');

/**
* Three step form builder
*/
function campaigner_email_target_form($form, &$form_state, $email_target_nid) {
	$email_target_node = node_load($email_target_nid);

	// Default taxonomy for dev purposes

	if(!empty($email_target_node->field_campaigner_region_class)) {
		$form_state['region_class_tid'] = $email_target_node->field_campaigner_region_class["und"][0]['tid'];
	}

	// Display page 2 if $form_state['page_num'] == 2
	if (!empty($form_state['page_num']) && $form_state['page_num'] == 2) {
		return campaigner_email_target_step_2($form, $form_state, $email_target_node);
	}

	// Display page 3 if $form_state['page_num'] == 3
	if (!empty($form_state['page_num']) && $form_state['page_num'] == 3) {
		return campaigner_email_target_step_3($form, $form_state, $email_target_node);
	}

	// Display page 4 if email_target has been signed
	if (!empty($form_state['page_num']) && $form_state['page_num'] == 4) {
		return campaigner_email_target_step_4($form, $form_state, $email_target_node);
	}

	// If either region class or location method have not been set, assume global email_target and proceed to step 3
	if (empty($email_target_node->field_campaigner_region_class) || $email_target_node->field_campaigner_location_method['und'][0]['value'] == 'global') {
		return campaigner_email_target_step_3($form, $form_state, $email_target_node);
	}

	// If location is by IP, try to find the region. If successful, proceed to step 3, otherwise step 2
	/*
	TODO: As per above comment, write logic to deal with the IP use case, linking to the ip_to_region
	function in the campaigner_location_services include file. See step 2 below.
	*/

	if ($email_target_node->field_campaigner_location_method['und'][0]['value'] == "ip") {
    	$form_state['region_class_tid']  = isset($email_target_node->field_campaigner_region_class['und'][0]['tid']) ? $email_target_node->field_campaigner_region_class['und'][0]['tid'] : 8;
    	$locationServices = new LocationServices();
		$form_state['campaigner_region'] = $locationServices->ip_to_region($form_state['region_class_tid']);
    	$form_state['identifier']        = $locationServices->getIdentifier();
    if (empty($form_state['campaigner_region'])){
      return campaigner_email_target_step_2($form, $form_state, $email_target_node);
    }
		return campaigner_email_target_step_3($form, $form_state, $email_target_node);

	}

	// Final, default case is location by postcode, proceed to step 1
	return campaigner_email_target_step_1($form, $form_state, $email_target_node);

}


/**
* Step 1
*
* This step is for the location by postcode scenario.
*/
function campaigner_email_target_step_1($form, &$form_state, $email_target_node) {

	// Make location method and region class accessible to form callback functions
	$form_state['location_method'] = $email_target_node->field_campaigner_location_method['und'][0]['value'];
	$form_state['region_class_tid'] = $email_target_node->field_campaigner_region_class['und'][0]['tid'];

	$form['title'] = array(
		'#markup' => t('<h2>' . $email_target_node->field_campaigner_step_1_title['und'][0]['value'] . '</h2>'),
	);

	$form['text'] = array(
		'#markup' => t('<p>' . $email_target_node->field_campaigner_step_1_text['und'][0]['value'] . '</p>'),
	);

	$form['postcode'] = array(
		'#type' => 'textfield',
		'#title' => t('Postcode'),
		'#description' => "Please enter your postal/zip code.",
		'#size' => 30,
		'#maxlength' => 15,
		'#required' => TRUE,
		'#default_value' => !empty($form_state['values']['postcode']) ? $form_state['values']['postcode'] : '',
		'#rules' => array(
			array('rule' => "regexp[/^[0-9a-zA-Z ]+$/]", 'error' => 'Please enter a valid postcode.'),
		)
	);

	$form['next'] = array(
		'#type' => 'submit',
		'#value' => 'Next >>',
		'#submit' => array('campaigner_email_target_step_1_submit'),
	);

	return $form;
}

function campaigner_email_target_step_1_submit($form, &$form_state) {
	// Values are saved for each page.
	// to carry forward to subsequent pages in the form.
	// and we tell FAPI to rebuild the form.
	$form_state['page_values'][1] = $form_state['values'];

	// Retrieve and store user's region based on postcode
	$form_state['campaigner_region'] = postcode_to_region($form_state['values']['postcode'], $form_state['region_class_tid']);
	$form_state['campaigner_postcode'] = strtoupper($form_state['values']['postcode']);

	// If region has been successfully determined, proceed to step 3, otherwise step 2

	if ($form_state['campaigner_region']) {
		if (!empty($form_state['page_values'][3])) {
			$form_state['values'] = $form_state['page_values'][3];
		}
		// When form rebuilds, it will look at this to figure which page to build.
		$form_state['page_num'] = 3;
	}
	else {
		if (!empty($form_state['page_values'][2])) {
			$form_state['values'] = $form_state['page_values'][2];
		}
		// When form rebuilds, it will look at this to figure which page to build.
		$form_state['page_num'] = 2;
	}

	$form_state['rebuild'] = TRUE;
}


/**
* Step 2
*/
function campaigner_email_target_step_2($form, &$form_state, $email_target_node) {
	/*
	TODO: Step 2 catches the case where the geolocation has failed. This should build a
	form that allows the user to select the region manually from a list. The list should be
	populated with all the regions that match the email_target's region class taxonomy tag.
	*/
  $text = t('<p>(Step 2) We have failed to find a region</p>');
  if (!empty($email_target_node->field_campaigner_step_2_text) && !empty($email_target_node->field_campaigner_step_2_text[$email_target_node->language])){
    $text = $email_target_node->field_campaigner_step_2_text[$email_target_node->language][0]['safe_value'];
  }
  $title = '';
  if (!empty($email_target_node->field_campaigner_step_2_title) && !empty($email_target_node->field_campaigner_step_2_title[$email_target_node->language])){
    $title = "<h2>" . $email_target_node->field_campaigner_step_2_title[$email_target_node->language][0]['safe_value'] . "</h2>";
  }
  $form['title'] = array(
    '#markup' => $title
  );
	$form['text'] = array(
		'#markup' => $text,
	);
  if (!class_exists("RegionCollection")){
    require_once dirname(__FILE__) . "/collections/RegionCollection.php";
  }
  if (!empty($email_target_node->field_campaigner_region_class) && !empty($email_target_node->field_campaigner_region_class[$email_target_node->language])){
    $regionCollection = new RegionCollection;
    $regionClass = $email_target_node->field_campaigner_region_class[$email_target_node->language][0]['tid'];
    $regionClass = $regionCollection->getByRegionClass($regionClass);
    if ($form_state['identifier']['type'] === "ip" && !empty($form_state['identifier']['details'])
      && !empty($form_state['identifier']['details']['long']) && !empty($form_state['identifier']['details']['lat'])){
      $regionClass->sortByDistance($form_state['identifier']['details']);
    }
    $regions = array();
    foreach ($regionClass->getAll() as $region){
      $regions[$region->entity_id] = $region->title;
    }
    $form['region'] = array(
      '#type' => "select",
      "#options" => $regions,
      '#title' => t("Select your region")
    );
    $form['submit'] = array(
      '#type' => "submit",
      '#value' => t("Select"),
      '#submit' => array('campaigner_email_target_step_2_submit')
    );
  }
  $form_state['page_num'] = 3;

  $form_state['rebuild'] = TRUE;
	return $form;
}

function campaigner_email_target_step_2_submit($form, &$form_state){
  if (!empty($form_state['values']['region'])){
    $region = node_load($form_state['values']['region']);
    if (!is_null($region)){
      $form_state['campaigner_region'] = $region;
    }
  }
  $form_state['page_num'] = 3;

  $form_state['rebuild'] = TRUE;
}



/**
* Step 3
*/
function campaigner_email_target_step_3($form, &$form_state, $email_target_node) {
	/*
	TODO: Step 3 is the final page that prints out the form with location-specific text etc.
	This will also need to provide for the storage of all the submitted email_target data into the database:
		Email address
		Postcode
		First name
		Surname
		Comment
		etc..
	This page could reuse the step 2 functionality by giving the user an option to manually
	change their region if it doesn't seem right (by diverting them back to page 2).
	*/

  $sigCount = get_signature_count($email_target_node->nid);
  $sigTarget      = (!empty($email_target_node->field_target)) ? $email_target_node->field_target['und'][0]["value"] : $sigCount + 1000;

	$form_state['list_id'] = !empty($email_target_node->field_campaigner_mclid) ? $email_target_node->field_campaigner_mclid['und'][0]['value'] : null;
 $replacements = array();
  $defaults = array(
    "email" => !empty($email_target_node->field_default_email_address) && !empty($email_target_node->field_default_email_address['und'])
      ? $email_target_node->field_default_email_address['und'][0]['safe_value'] : "",
    "[salut]" => !empty($email_target_node->field_default_salutation) && !empty($email_target_node->field_default_salutation['und'])
      ? $email_target_node->field_default_salutation['und'][0]['safe_value'] : "",
    "[target_name]" => !empty($email_target_node->field_default_target_name) && !empty($email_target_node->field_default_target_name['und'])
      ? $email_target_node->field_default_target_name['und'][0]['safe_value'] : "",
    "[target_surname]" => !empty($email_target_node->field_default_surname) && !empty($email_target_node->field_default_surname['und'])
      ? $email_target_node->field_default_surname['und'][0]['safe_value'] : "",
    "[location]" => !empty($email_target_node->field_default_location) && !empty($email_target_node->field_default_location['und']) 
      ? $email_target_node->field_default_location['und'][0]['safe_value'] : ""
  );
	if ($email_target_node->field_campaigner_location_method['und'][0]['value'] == "global" || empty($form_state['campaigner_region'])) {
		$form_state['target_email'] = $defaults['email'];
    		$replacements = $defaults;
	} else {
    		$region = $form_state['campaigner_region'];
    		$form_state['target_email'] = $form_state['campaigner_region']->field_campaigner_target_email['und'][0]['value'];
		    $replacements["[salut]"] = !empty($region->field_campaigner_target_salut) && !empty($region->field_campaigner_target_salut['und'])
		      ? $region->field_campaigner_target_salut['und'][0]['safe_value'] : $defaults['[salut]'];
		    $replacements['[target_name]'] = !empty($region->field_campaigner_target_name) && !empty($region->field_campaigner_target_name['und'])
		      ? $region->field_campaigner_target_name['und'][0]['safe_value'] : $defaults['[target_name]'];
		    $replacements['[target_surname]'] = !empty($region->field_campaigner_target_surname) && !empty($region->field_campaigner_target_surname['und'])
		      ? $region->field_campaigner_target_surname['und'][0]['safe_value'] : $defaults['[target_surname]'];
		    $replacements['[location]'] = !empty($region->title) ? $region->title : $defaults['[location'];
	}

  $stepTitle = !empty($email_target_node->field_campaigner_step_3_title) ? $email_target_node->field_campaigner_step_3_title['und'][0]['value'] : '';
	$form['title'] = array(
		'#markup' => t('<h2>' . ($email_target_node->field_campaigner_location_method['und'][0]['value'] != "global"
					? str_replace("[location]",$form_state['campaigner_region']->title,$stepTitle)
					: $stepTitle) . '</h2>'),
	);

  $stepText = (!empty($email_target_node->field_campaigner_step_3_text))
    ? $email_target_node->field_campaigner_step_3_text['und'][0]['safe_value'] : '';

  $stepText = campaigner_widgets_add_widgets($stepText, array(
    'node_id' => $email_target_node->nid,
    "sig_count"                 => $sigCount,
    "sig_target"                => $sigTarget,
    "social_proof_fields"       => !empty($email_target_node->field_social_proof_fields) ? $email_target_node->field_social_proof_fields['und'] : array(),
    "social_proof_title"        => !empty($email_target_node->field_social_proof_title) ? $email_target_node->field_social_proof_title['und'][0]['value'] : t("Recent Signatures"),
    "social_proof_refresh_rate" => !empty($email_target_node->field_social_proof_refresh_rate) ? $email_target_node->field_social_proof_refresh_rate['und'][0]['value'] * 1000 : 30000,
    "node"                  => $email_target_node
  ));

  $stepText = str_replace(array("[sigcount]", "[sigtarget]", "[sigtargethalf]"),
    array($sigCount, $sigTarget, $sigTarget / 2),
    $stepText);

	$form['text'] = array(
		'#markup' => str_replace("[location]",$replacements['[location]'],$stepText)
	);

	$form['subject'] = array(
		'#type' => 'textfield',
		'#title' => t('Email subject'),
		'#description' => "Please enter a title for your email.",
		'#size' => 30,
		'#maxlength' => 150,
		'#required' => TRUE,
		'#default_value' => $email_target_node->field_campaigner_email_subject["und"][0]["value"],
	);
$message = !empty($email_target_node->field_campaigner_email_text) && !empty($email_target_node->field_campaigner_email_text['und'])
    ? $email_target_node->field_campaigner_email_text["und"][0]["value"] : '';
  $message = str_replace(array_keys($replacements), array_values($replacements), $message);
	$form['message'] = array(
		'#type' => 'textarea',
		'#title' => t('Email message'),
		'#description' => "Please enter your message. The name you fill in below will be added to the end of the message.",
		'#size' => 30,
		'#maxlength' => 10000,
		'#required' => TRUE,
		'#default_value' => $message,
	);

  $form['f_name'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Your first name'),
    '#description'   => "Please enter your first name.",
    '#size'          => 30,
    '#maxlength'     => 100,
    '#required'      => TRUE,
    '#default_value' => '',
  );

  $form['l_name'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Your last name'),
    '#description'   => "Please enter your last name.",
    '#size'          => 30,
    '#maxlength'     => 100,
    '#required'      => TRUE,
    '#default_value' => '',
  );
  if (!empty($email_target_node->field_use_honeypot) && $email_target_node->field_use_honeypot['und'][0]['value']){
    /**
     * This is a honeypot
     */
    $form['namepot'] = array(
      '#type' => "textfield",
      '#title' => t("Do not fill in this field"),
      '#weight' => 0,
      '#required' => FALSE,
      '#default_value' => ''
    );
  }
	$form['email'] = array(
		'#type' => 'textfield',
		'#title' => t('Your email address'),
		'#description' => "Please enter your email address.",
		'#size' => 30,
		'#maxlength' => 150,
		'#required' => TRUE,
		'#default_value' => !empty($form_state['values']['email']) ? $form_state['values']['email'] : '',
		'#rules' => array(
			array('rule' => 'email', 'error' => 'Please enter a valid email address.'),
		)
	);

		$form['email_signup'] = array(
		'#type' => 'checkbox',
		'#title' => t('Send me email updates'),
		'#description' => "We will not share your address with any third parties",
		'#size' => 30,
		'#maxlength' => 150,
	);

	$form['next'] = array(
		'#type' => 'submit',
		'#value' => 'Send email',
		'#submit' => array('campaigner_email_target_sign'),
	);

	return $form;
}



function campaigner_email_target_sign($form, &$form_state) {
  $nodeID = $form_state["build_info"]["args"][0];
  $node = node_load($nodeID);
	// Values are saved for each page.
	// to carry forward to subsequent pages in the form.
	// and we tell FAPI to rebuild the form.
	$form_state['page_values'][4] = $form_state['values'];
	$form_state["page_num"] = 4;

  if (!empty($node) && !empty($node->field_use_honeypot) && $node->field_use_honeypot['und'][0]['value'] && !empty($form_state['values']['namepot'])){
    // this person is a spammer because they have filled in the honeypot
    watchdog("campaigner", "Likely spam detected in action @action. Honeypot field filled in, first name: @first_name, last name: @last_name, email: @email, comment: @comment",
      array("@action" => $form_state["build_info"]["args"][0],"@first_name" => $form_state['values']['f_name'],
           "@last_name" => $form_state['values']['l_name'], "@email" => $form_state['values']['email'],
           "@comment" => !empty($form_state['values']['comment']) ? $form_state['values']['comment'] : NULL),
      WATCHDOG_INFO);
    // Rebuild the page and move on to the next stage as if we have accepted the entry
    $form_state['rebuild'] = TRUE;
    return;
  }

	// Check for existing email address & action id in signatures table

  $user = campaigner_identify_user($form_state['values']['email']);
	$signature_duplicate = db_query('SELECT * FROM `signatures` WHERE email = \'' . $form_state['values']['email'] . '\' AND action = ' . $form_state["build_info"]["args"][0]);
  $name = $form_state['values']['f_name'] . " " . $form_state['values']['l_name'];
  if ($signature_duplicate->rowCount() == 0) {

    $id = db_insert('signatures')
      ->fields(array(
        'action'     => $nodeID, // @TODO : Not keen on this way of getting petition nid
        'email'      => $form_state['values']['email'],
        'postcode'   => !empty($form_state['campaigner_postcode']) ? $form_state['campaigner_postcode'] : '',
        'region'     => !empty($form_state['campaigner_region']) ? $form_state['campaigner_region']->nid : 0,
        'name'       => $name,
        'first_name' => $form_state['values']['f_name'],
        'last_name' => $form_state['values']['l_name'],
        'created'    => time(),
        'identifier' => !empty($form_state['identifier']) ? json_encode($form_state['identifier']) : '',
        'comments' => !empty($form_state['values']['comment']) ? $form_state['values']['comment'] : NULL,
        "uid" => $user->uid
      ))
      ->execute();

    $to      = campaigner_email_target_clean($form_state['target_email']);
		$subject = campaigner_email_target_clean($form_state['values']['subject']);
		$message = campaigner_email_target_clean($form_state['values']['message']) . PHP_EOL . PHP_EOL . $name;
		$headers = 'From: ' . $form_state['values']['email'] . ' '  . "\r\n" .
		    'Reply-To: ' . $form_state['values']['email']  . "\r\n" .
		    'X-Mailer: PHP/' . phpversion();

		mail($to, $subject, $message, $headers);


	} else {

		if (!empty($form_state['campaigner_postcode'])) {

			$id = db_update('signatures')
					->fields(array(
						'postcode' => $form_state['campaigner_postcode'],
						'region' => $form_state['campaigner_region']->nid,
            			'name'       => $name,
                  'first_name' => $form_state['values']['f_name'],
                  'last_name' => $form_state['values']['l_name'],
            			'created'    => time(),
            			'comments' => !empty($form_state['values']['comment']) ? $form_state['values']['comment'] : NULL,
                  "uid" => $user->uid
					))
					->condition('email', $form_state['values']['email'], '=')
					->condition('action', $form_state["build_info"]["args"][0], '=')
					->execute();
		} else if (!empty($form_state['campaigner_region']->nid) && $form_state['campaigner_region']->nid != NULL) {

			$id = db_update('signatures')
					->fields(array(
						'region' => $form_state['campaigner_region']->nid,
            			'name'       => $name,
                  'first_name' => $form_state['values']['f_name'],
                  'last_name' => $form_state['values']['l_name'],
            			'created'    => time(),
            			'comments' => !empty($form_state['values']['comment']) ? $form_state['values']['comment'] : NULL,
                  "uid" => $user->uid
					))
					->condition('email', $form_state['values']['email'], '=')
					->condition('action', $form_state["build_info"]["args"][0], '=')
					->execute();

		}

	}

	if ($form_state['values']['email_signup'] == TRUE && !empty($form_state['list_id'])) {

		include 'mailchimp-api-class/examples/mcapi_listSubscribe.php';
		campaigner_mc_api_subscribe(
      array("full" => $name, "first" => $form_state['values']['f_name'], "last" => $form_state['values']['l_name']),
      $form_state['list_id'],$form_state['values']['email']
    );

	}

	$form_state['rebuild'] = TRUE;

}


// action has been signed, show thank text

function campaigner_email_target_step_4($form, &$form_state, $email_target_node) {

	$form['title'] = array(
		'#markup' => t('<h2>' . ($email_target_node->field_campaigner_location_method['und'][0]['value'] != "global"
					? str_replace("[location]",$form_state['campaigner_region']->title,$email_target_node->field_campaigner_step_4_title['und'][0]['value'])
					: $email_target_node->field_campaigner_step_4_title['und'][0]['value']) . '</h2>'),
	);
  $sigCount = get_signature_count($email_target_node->nid);
  $sigTarget      = (!empty($email_target_node->field_target)) ? $email_target_node->field_target['und'][0]["value"] : $sigCount + 1000;
  $stepText = !empty($email_target_node->field_campaigner_step_4_text) ? $email_target_node->field_campaigner_step_4_text['und'][0]['safe_value'] : '';
  $stepText = campaigner_widgets_add_widgets($stepText, array(
    'node_id' => $email_target_node->nid,
    "sig_count"                 => $sigCount,
    "sig_target"                => $sigTarget,
    "social_proof_fields"       => !empty($email_target_node->field_social_proof_fields) ? $email_target_node->field_social_proof_fields['und'] : array(),
    "social_proof_title"        => !empty($email_target_node->field_social_proof_title) ? $email_target_node->field_social_proof_title['und'][0]['value'] : t("Recent Signatures"),
    "social_proof_refresh_rate" => !empty($email_target_node->field_social_proof_refresh_rate) ? $email_target_node->field_social_proof_refresh_rate['und'][0]['value'] * 1000 : 30000,
    "node"                  => $email_target_node
  ));
	$form['text'] = array(
		'#markup' => t(($email_target_node->field_campaigner_location_method['und'][0]['value'] != "global"
					? str_replace("[location]",$form_state['campaigner_region']->title, $stepText)
					: $stepText)),
	);

	return $form;
}

  /**
   * This function strips header-like text strings from email data to keep header injection at bay.
   *
   * @param $string
   *
   * @return string
   */
function campaigner_email_target_clean($string){
  $headers = array(
    "/to\:/i",
    "/from\:/i",
    "/bcc\:/i",
    "/cc\:/i",
    "/Content\-Transfer\-Encoding\:/i",
    "/Content\-Type\:/i",
    "/Mime\-Version\:/i"
  );
  $string = preg_replace($headers, '', $string);
  return strip_tags($string);
}