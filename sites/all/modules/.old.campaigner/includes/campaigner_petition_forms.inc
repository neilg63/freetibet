<?php
  /**
   * @file
   * Form building, processing and validation for Campaigner module petition forms.
   */

//Load includes
  module_load_include('inc', 'campaigner', 'includes/campaigner_location_services');
  module_load_include('inc', 'campaigner', 'includes/campaigner_one_time_links');
  module_load_include('inc', 'campaigner', 'includes/campaigner_signatures');
  module_load_include('inc', 'campaigner', 'includes/campaigner_widgets');

  /**
   * Three step form builder
   */
  function campaigner_petition_form($form, &$form_state, $petition_nid) {
    $petition_node = node_load($petition_nid);

    // Display page 2 if $form_state['page_num'] == 2
    if (!empty($form_state['page_num']) && $form_state['page_num'] == 2) {
      return campaigner_petition_step_2($form, $form_state, $petition_node);
    }

    // Display page 3 if $form_state['page_num'] == 3
    if (!empty($form_state['page_num']) && $form_state['page_num'] == 3) {
      return campaigner_petition_step_3($form, $form_state, $petition_node);
    }

    // Display page 4 if petition has been signed
    if (!empty($form_state['page_num']) && $form_state['page_num'] == 4) {
      return campaigner_petition_step_4($form, $form_state, $petition_node);
    }

    // If either region class or location method have not been set, assume global petition and proceed to step 3
    if (empty($petition_node->field_campaigner_region_class) || $petition_node->field_campaigner_location_method['und'][0]['value'] == 'global') {
      return campaigner_petition_step_3($form, $form_state, $petition_node);
    }

    // If location is by IP, try to find the region. If successful, proceed to step 3, otherwise step 2
    if ($petition_node->field_campaigner_location_method['und'][0]['value'] == "ip") {
      // Default taxonomy for dev purposes is 8
      $locationServices = new LocationServices();
      $form_state['region_class_tid']  = isset($petition_node->field_campaigner_region_class['und'][0]['tid']) ? $petition_node->field_campaigner_region_class['und'][0]['tid'] : 8;
      if (empty($form_state['campaigner_region'])){
        $form_state['campaigner_region'] = $locationServices->ip_to_region($form_state['region_class_tid']);
        $form_state['identifier']        = $locationServices->getIdentifier();
      }
      return campaigner_petition_step_3($form, $form_state, $petition_node);
    }

    // If postcode is passed in URL, try to find the region and store unique ID for one-time URL usage.
    if (arg(2)) {
      $url_pcode = urldecode(arg(2));
      $url_email = sanitise_email(urldecode(arg(3)));
      $url_uqid  = sanitise_uqid(urldecode(arg(4)));

      // If a safe unique ID is appended to the URL, and it doesn't exist in the DB, store it in the DB and continue
      // If no unique ID is appended to the URL, ignore and continue
      if (($url_uqid && !get_or_make_unique($url_uqid)) || $url_uqid === '') {
        $form_state['region_class_tid'] = $petition_node->field_campaigner_region_class['und'][0]['tid'];
        if (empty($form_state['campaigner_region'])){
          $form_state['campaigner_region'] = postcode_to_region($url_pcode, $form_state['region_class_tid']);
        }
        // If region has been successfully determined, proceed to step 3, otherwise step 1
        if (!is_string($form_state['campaigner_region'])) {
          $form_state['values']['email'] = $url_email;
          return campaigner_petition_step_3($form, $form_state, $petition_node);
        }
      }
    }
    // Final, default case is location by postcode, proceed to step 1
    return campaigner_petition_step_1($form, $form_state, $petition_node);
  }


  /**
   * Step 1
   *
   * This step is for the location by postcode scenario.
   */
  function campaigner_petition_step_1($form, &$form_state, $petition_node) {

    // Make location method and region class accessible to form callback functions
    $form_state['location_method']  = $petition_node->field_campaigner_location_method['und'][0]['value'];
    $form_state['region_class_tid'] = $petition_node->field_campaigner_region_class['und'][0]['tid'];
    if (isset($form_state['error_string'])) {
      $form['error'] = array(
        '#markup' => '<div class=" messages error">' . $form_state['error_string'] . '</div>',
      );
    }

    $form['title'] = array(
      '#markup' => t('<h2>' . (!empty($petition_node->field_campaigner_step_1_title) ? $petition_node->field_campaigner_step_1_title['und'][0]['value'] : "") . '</h2>'),
    );

    $form['text'] = array(
      '#markup' => t((!empty($petition_node->field_campaigner_step_1_text) ? $petition_node->field_campaigner_step_1_text['und'][0]['value'] : "")),
    );

    $form['postcode'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Postcode'),
      '#description'   => "Please enter your postal/zip code.",
      '#size'          => 30,
      '#maxlength'     => 15,
      '#required'      => TRUE,
      '#default_value' => !empty($form_state['values']['postcode']) ? $form_state['values']['postcode'] : '',
      '#rules'         => array(
        array('rule' => "regexp[/^[0-9a-zA-Z ]+$/]", 'error' => 'Please enter a valid postcode.'),
      )
    );

    $form['next'] = array(
      '#type'   => 'submit',
      '#value'  => 'Next >>',
      '#submit' => array('campaigner_petition_step_1_submit'),
    );

    return $form;
  }

  function campaigner_petition_step_1_submit($form, &$form_state) {
    // Values are saved for each page.
    // to carry forward to subsequent pages in the form.
    // and we tell FAPI to rebuild the form.
    $form_state['page_values'][1] = $form_state['values'];

    // Retrieve and store user's region based on postcode
    $form_state['campaigner_region']   = postcode_to_region($form_state['values']['postcode'], $form_state['region_class_tid']);
    $form_state['campaigner_postcode'] = strtoupper($form_state['values']['postcode']);

    // If region has been successfully determined, proceed to step 3, otherwise step 2
    if (!is_string($form_state['campaigner_region'])) {
      if (!empty($form_state['page_values'][3])) {
        $form_state['values'] = $form_state['page_values'][3];
      }
      // When form rebuilds, it will look at this to figure which page to build.
      $form_state['page_num'] = 3;
    } else if ($form_state['campaigner_region'] == "Invalid postcode") {
      $form_state['error_string'] = 'Invalid postcode';
      $form_state['page_num']     = 1;
    } else {
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
  function campaigner_petition_step_2($form, &$form_state, $petition_node) {
    /*
    TODO: Step 2 catches the case where the geolocation has failed. This should build a
    form that allows the user to select the region manually from a list. The list should be
    populated with all the regions that match the petition's region class taxonomy tag.
    */

    $form['text'] = array(
      '#markup' => t(($petition_node->field_campaigner_location_method['und'][0]['value'] != "global"
        ? $petition_node->field_campaigner_step_2['und'][0]['value']
        : 'We could not find a region for your postcode')),
    );

    return $form;
  }


  /**
   * Step 3
   */
  function campaigner_petition_step_3($form, &$form_state, $petition_node) {
    global $base_url;
    $form_state['list_id'] = (!empty($petition_node->field_campaigner_mclid) ? $petition_node->field_campaigner_mclid['und'][0]['value'] : NULL);
    $regionTitle = !empty($form_state['campaigner_region']) ? $form_state['campaigner_region']->title : '';
    if ($petition_node->field_campaigner_location_method['und'][0]["value"] !== "global") {
      $stepTitle = str_replace("[location]", $regionTitle, !empty($petition_node->field_campaigner_step_3_title) ? $petition_node->field_campaigner_step_3_title['und'][0]['value'] : '');
    } else {
      $stepTitle = !empty($petition_node->field_campaigner_step_3_title) ? $petition_node->field_campaigner_step_3_title['und'][0]['value'] : '';
    }
    $form['title'] = array(
      '#markup' => t('<h2>' . $stepTitle . '</h2>'),
    );


    // Just get all if we're not using Regions
    $sig_count = get_signature_count($petition_node->nid);

    $sig_target      = (!empty($petition_node->field_target)) ? $petition_node->field_target['und'][0]["value"] : $sig_count + 1000;
    $sig_target_half = $sig_target / 2;
    $textb_str_replace = !empty($petition_node->field_sign_petition_page_before) ? $petition_node->field_sign_petition_page_before['und'][0]['safe_value'] : '';
    $textb_str_replace = campaigner_widgets_add_widgets($textb_str_replace,
      array("node_id"               => $petition_node->nid,
            "sig_count"                 => $sig_count,
            "sig_target"                => $sig_target,
            "social_proof_fields"       => !empty($petition_node->field_social_proof_fields) ? $petition_node->field_social_proof_fields['und'] : array(),
            "social_proof_title"        => !empty($petition_node->field_social_proof_title) ? $petition_node->field_social_proof_title['und'][0]['value'] : t("Recent Signatures"),
            "social_proof_refresh_rate" => !empty($petition_node->field_social_proof_refresh_rate) ? $petition_node->field_social_proof_refresh_rate['und'][0]['value'] * 1000 : 30000,
            "node"                  => $petition_node
      ), array("[donate]"));
    $textb_str_replace = str_replace(array("[sigcount]", "[sigtarget]", "[sigtargethalf]"),
      array($sig_count, $sig_target, $sig_target_half),
      $textb_str_replace);

    if (($petition_node->field_campaigner_location_method['und'][0]['value'] != "global")){
      $textb = str_replace("[location]", $regionTitle, $textb_str_replace);
    }
    else {
      $textb = $textb_str_replace;
    }

    $form['textb'] = array(
      '#markup' => t($textb)
    );

    $step3text     = (!empty($petition_node->field_campaigner_step_3_text)) ?
      '<div class="step_3_text">' . $petition_node->field_campaigner_step_3_text['und'][0]['safe_value'] . "</div>" :
      '<div class="step_3_text"></div>';
    $step3text     = campaigner_widgets_add_widgets($step3text, array(
      "node_id"               => $petition_node->nid,
      "sig_count"                 => $sig_count,
      "sig_target"                => $sig_target,
      "social_proof_fields"       => !empty($petition_node->field_social_proof_fields) ? $petition_node->field_social_proof_fields['und'] : array(),
      "social_proof_title"        => !empty($petition_node->field_social_proof_title) ? $petition_node->field_social_proof_title['und'][0]['value'] : t("Recent Signatures"),
      "social_proof_refresh_rate" => !empty($petition_node->field_social_proof_refresh_rate) ? $petition_node->field_social_proof_refresh_rate['und'][0]['value'] * 1000 : 30000,
      "node"                  => $petition_node
    ), array("[donate]"));
    $step3text = str_replace(array("[sigcount]", "[sigtarget]", "[sigtargethalf]"),
      array($sig_count, $sig_target, $sig_target_half),
      $step3text);
    $form['text']  = array(
      '#markup' => t(($petition_node->field_campaigner_location_method['und'][0]['value'] != "global"
        ? str_replace("[location]", $regionTitle, $step3text)
        : $step3text)),
    );

    $form['petitiontext'] = array(
      '#markup' => t('<div id="petition_text">' . (!empty($petition_node->field_campaigner_petition_text) ? $petition_node->field_campaigner_petition_text['und'][0]['safe_value'] : "") . '</div>'),
    );
    if (!empty($petition_node->field_use_honeypot) && $petition_node->field_use_honeypot['und'][0]['value']){
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

    $form['email'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Email address'),
      '#description'   => "Please enter your email address.",
      '#size'          => 30,
      '#maxlength'     => 150,
      '#required'      => TRUE,
      '#default_value' => !empty($form_state['values']['email']) ? $form_state['values']['email'] : '',
      '#rules'         => array(
        array('rule' => 'email', 'error' => 'Please enter a valid email address.'),
      )
    );

    if (!empty($petition_node->field_petition_comments) && $petition_node->field_petition_comments['und'][0]['value'] == 1){
      $form['comment'] = array(
        '#type' => "textarea",
        '#title' => t("Comment"),
        '#description' => t("You may enter a comment of up to 1000 characters"),
        '#default_value' => '',
        '#attributes' => array(
          "maxlength" => 1000
        ),
      );
      if (!empty($petition_node->field_spam_type) && (int) $petition_node->field_spam_type["und"][0]['value'] === 1){
        $form['comment']['#element_validate'] = array("campaigner_validate_comment");
      }

    }


    if ($form_state['list_id'] != NULL) {
      $form['email_signup'] = array(
        '#type'        => 'checkbox',
        '#title'       => t('Send me email updates'),
        '#description' => "We will not share your address with any third parties",
        '#size'        => 30,
        '#maxlength'   => 150,
      );
    }
    /*
     Image upload for photo petitions

    $form['image_example_image_fid'] = array(
      '#title' => t('Image'),
      '#type' => 'managed_file',
      '#description' => t('The uploaded image will be displayed on this page using the image style choosen below.'),
      '#default_value' => variable_get('image_example_image_fid', ''),
      '#upload_location' => 'public://campaigner_avatars/',
    ); */

    $form['next'] = array(
      '#type'   => 'submit',
      '#value'  => 'Sign petition',
      '#submit' => array('campaigner_petition_sign'),
    );


    return $form;
  }


  function campaigner_petition_sign($form, &$form_state) {
    $nodeID = $form_state["build_info"]["args"][0];
    $node = node_load($nodeID);
    // Values are saved for each page.
    // to carry forward to subsequent pages in the form.
    // and we tell FAPI to rebuild the form.
    $form_state['page_values'][4] = $form_state['values'];
    $form_state["page_num"]       = 4;

    if (!empty($node) && !empty($node->field_use_honeypot) && $node->field_use_honeypot['und'][0]['value'] && !empty($form_state['values']['namepot'])){
      // this person is a spammer because they have filled in the honeypot
      watchdog("campaigner", "Likely spam detected honeypot field filled in, first name: @first_name, last name: @last_name, email: @email, comment: @comment",
        array("@first_name" => $form_state['values']['f_name'], "@last_name" => $form_state['values']['l_name'],
             "@email" => $form_state['values']['email'], "@comment" => !empty($form_state['values']['comment']) ? $form_state['values']['comment'] : NULL),
        WATCHDOG_INFO);
      // Rebuild the page and move on to the next stage as if we have accepted the entry
      $form_state['rebuild'] = TRUE;
      return;
    }

    $isSpam = false;
    if (!empty($node) && !empty($node->field_spam_type) && (int) $node->field_spam_type['und'][0]['value'] === 2 && !empty($form_state['values']['comment'])){
      require_once dirname(__FILE__) . '/libraries/SpamFilter.php';
      $filter = new SpamFilter();
      $cutoff = (!empty($node) && !empty($node->field_spam_cutoff)) ? $node->field_spam_cutoff['und'][0]['value'] : 90;
      $signatureRating = (int) (100 * $filter->checkForSpam($form_state['values']['comment']));
      $isSpam = $signatureRating >= $cutoff;
    }

    // Check for existing email address & action id in signatures table
    $signature_duplicate = db_query('SELECT * FROM `signatures` WHERE email = \'' . $form_state['values']['email'] . '\' AND action = ' . $form_state["build_info"]["args"][0]);
    $name = $form_state['values']['f_name'] . " " . $form_state['values']['l_name'];
    if ($signature_duplicate->rowCount() == 0) {

      $id = db_insert('signatures')
        ->fields(array(
          'action'     => $form_state["build_info"]["args"][0], // @TODO : Not keen on this way of getting petition nid
          'email'      => $form_state['values']['email'],
          'postcode'   => !empty($form_state['campaigner_postcode']) ? $form_state['campaigner_postcode'] : '',
          'region'     => !empty($form_state['campaigner_region']) ? $form_state['campaigner_region']->nid : 0,
          'name'       => $name,
          'first_name' => $form_state['values']['f_name'],
          'last_name' => $form_state['values']['l_name'],
          'created'    => time(),
          'identifier' => !empty($form_state['identifier']) ? json_encode($form_state['identifier']) : '',
          'comments' => !empty($form_state['values']['comment']) ? $form_state['values']['comment'] : NULL,
          "spam" => $isSpam ? 1 : 0
        ))
        ->execute();
    } else if (!$isSpam) {
      // Do nothing if there's already a submission and this current one is considered spam (don't want an attack vector via
      // submitting spam messages with existing submission email addresses).
      if (!empty($form_state['campaigner_postcode'])) {

        $id = db_update('signatures')
          ->fields(array(
            'postcode' => $form_state['campaigner_postcode'],
            'region'   => !empty($form_state['campaigner_region']) ? $form_state['campaigner_region']->nid : 0,
          ))
          ->condition('email', $form_state['values']['email'], '=')
          ->condition('action', $form_state["build_info"]["args"][0], '=')
          ->execute();
      } else {

        $id = db_update('signatures')
          ->fields(array(
            'region' => !empty($form_state['campaigner_region']) ? $form_state['campaigner_region']->nid : 0,
          ))
          ->condition('email', $form_state['values']['email'], '=')
          ->condition('action', $form_state["build_info"]["args"][0], '=')
          ->execute();
      }

    }

    if (!empty($form_state['values']['email_signup']) && $form_state['values']['email_signup'] == TRUE) {

      include 'mailchimp-api-class/examples/mcapi_listSubscribe.php';
      campaigner_mc_api_subscribe(
        array("full" => $name, "first" => $form_state['values']['f_name'], "last" => $form_state['values']['l_name']),
        $form_state['list_id'], $form_state['values']['email']
      );

    }

    $form_state['rebuild'] = TRUE;
  }


// Petition has been signed, show thank text

  function campaigner_petition_step_4($form, &$form_state, $petition_node) {
    $regionTitle = !empty($form_state['campaigner_region']) ? $form_state['campaigner_region']->title : '';
    $form['title'] = array(
      '#markup' => t('<h2>' . ($petition_node->field_campaigner_location_method['und'][0]['value'] != "global"
        ? str_replace("[location]", $regionTitle, $petition_node->field_campaigner_step_4_title['und'][0]['value'])
        : $petition_node->field_campaigner_step_4_title['und'][0]['value']) . '</h2>'),
    );

    $stepText      = $petition_node->field_campaigner_step_4_text['und'][0]['value'];
    if (!empty($form_state['campaigner_region'])) {
      $fieldCount = !empty($form_state['campaigner_region']->field_count) ? $form_state['campaigner_region']->field_count['und'][0]['value'] : 0;
      $sig_count  = get_signature_count_by_region($petition_node->nid, $form_state['campaigner_region']->nid) + $fieldCount;
    } else {
      // Just get all if we're not using Regions
      $sig_count = get_signature_count($petition_node->nid);
    }
    // @TODO override this when we get a field added to store the target
    $sig_target   = $sig_count + 10;
    $stepText     = campaigner_widgets_add_widgets($stepText,
      array(
        "node_id"               => $petition_node->nid,
        "sig_count"                 => $sig_count,
        "sig_target"                => $sig_target,
        "social_proof_fields"       => !empty($petition_node->field_social_proof_fields) ? $petition_node->field_social_proof_fields['und'] : array(),
        "social_proof_title"        => !empty($petition_node->field_social_proof_title) ? $petition_node->field_social_proof_title['und'][0]['value'] : t("Recent Signatures"),
        "social_proof_refresh_rate" => !empty($petition_node->field_social_proof_refresh_rate) ? $petition_node->field_social_proof_refresh_rate['und'][0]['value'] * 1000 : 30000,
        "node"                  => $petition_node
      )
    );
    $form['text'] = array(
      '#markup' => t(($petition_node->field_campaigner_location_method['und'][0]['value'] != "global"
        ? str_replace("[location]", $regionTitle, $stepText)
        : $stepText)),
    );

    return $form;
  }
