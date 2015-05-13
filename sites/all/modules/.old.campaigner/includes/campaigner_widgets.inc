<?php
  /**
   * Takes a text string and adds any widgets it finds in that text string.
   *
   * @param string $text
   * @param array  $data The data should contain information needed for constructing a thermometer and information on the current action.
   * @param array $block This array can be used to block any of the widgets from showing by putting the appropriate string as an array entry
   *                     e.g. to block donation provide array("[donate]")
   *
   * @return string
   */
  function campaigner_widgets_add_widgets($text, $data, $block = array()) {

    $socialProofPosition = strpos($text, "[socialproof]");
    $thermometerPosition = strpos($text, "[thermometer]");
    $donatePosition      = strpos($text, "[donate]");
    $sharePosition      = strpos($text, "[share]");

    if ($socialProofPosition !== FALSE && !in_array("[socialproof]", $block)) {
      $text = campaigner_widgets_add_widget_social_proof($text, $data);
    }
    if ($thermometerPosition !== FALSE && !in_array("[thermometer]", $block)) {
      $text = campaigner_widgets_add_widget_thermometer($text, $data);
    }
    if ($donatePosition !== FALSE && !in_array("[donate]", $block)) {
      $text = campaigner_widgets_add_widget_donate($text, $data);
    }
    if ($sharePosition !== FALSE && !in_array("[share]", $block)){
      $text = campaigner_widgets_add_widget_share($text, $data);
    }
    if ($socialProofPosition !== FALSE || $thermometerPosition !== FALSE) {
      drupal_add_css(drupal_get_path("module", "campaigner") . '/css/widgets.css');
    }

    return $text;
  }

  /**
   * Adds the donation widget to the text and returns it.
   *
   * @param $text
   * @param array $data
   *
   * @return mixed The modified text
   */
  function campaigner_widgets_add_widget_donate($text, $data) {
    global $base_url;
    $donate = theme("campaigner_donate", array());
    $text   = str_replace("[donate]", $donate, $text);
    drupal_add_js("(function ($){
    var donate, hideCountry, showCountry, redirect = '" . $base_url . "/donate_process.php';

     showCountry = function (e) {
      $('#country').removeClass('hide');
      $('#helptext').removeClass('hide');
    };
     hideCountry = function (e) {
      $('#country').addClass('hide');
      $('#helptext').addClass('hide');
    };
    donate = function (context, e) {
        var form;
        form = $(context).closest('form');
        form.get(0).setAttribute('action', redirect);
    };

    $(document).ready(function (){
      $('#donate_button').click(function (e){
        donate(this, e);
      });

      $('#radio_regular').click(function (e){
        showCountry(e);
      });
  $('#radio_single').click(function (e){
    hideCountry(e);
    $('#country').change(function() {

    if ($('#country').val() == 'GB') {
      $('p.amountvalue').html('&pound; <input type=\"text\" name=\"amount\" class=\"text\" id=\"amount\" />');
    } else {
      $('p.amountvalue').html('&dollar;  <input type=\"text\" name=\"amount\"  class=\"text\" id=\"amount\" />');
    }

  });
  });
  });


  })(jQuery);", array("type" => "inline", "scope" => "footer", "weight" => 10));
    return $text;
  }

  /**
   * Adds the thermometer widget.
   *
   * @param $text
   * @param $data
   *
   * @return mixed
   */
  function campaigner_widgets_add_widget_thermometer($text, $data) {
    global $base_url;
    $modulePath          = drupal_get_path("module", "campaigner");
    $thermometer = theme("campaigner_thermometer", array("thermometer_count" => $data['sig_count'], "thermometer_target" => $data['sig_target']));
    $text        = str_replace("[thermometer]", $thermometer, $text);
    drupal_add_js($modulePath . '/js/thermometer.js', array("type" => "file", "scope" => "footer", "weight" => 1));
    drupal_add_js("(function ($){
    $(document).ready(function (){
      $('.thermometer').campaignerThermometer();
    });
  }
    )(jQuery);", array("type" => "inline", "scope" => "footer", "weight" => 10));
    return $text;
  }

  /**
   * Adds the share widget.
   *
   * @param $text
   * @param $data
   *
   * @return mixed
   */
  function campaigner_widgets_add_widget_share($text, $data){
    global $base_url;
    $share = theme("campaigner_share", array());
    $node = node_load($data['node_id']);
    $text = str_replace("[share]", $share, $text);
    $metaTag = (!empty($node->field_share_description)) ? "<meta property='og:description' content='".$node->field_share_description['und'][0]['safe_value']. "' />" : '';
    drupal_add_html_head(array(
      '#type' => "markup",
      '#markup' => '<meta property="og:title" content="' . $node->title . '" /><meta property="og:site_name" content="'. variable_get("site_name") .'"/>
      <meta property="og:url" content="'. $base_url . '/node/' . $node->nid .'"/>' . $metaTag
    ), "campaigner_share_meta_tags");
    return $text;
  }

  /**
   * Adds the social proof widget.
   *
   * @param $text
   * @param $data
   *
   * @return mixed
   */
  function campaigner_widgets_add_widget_social_proof($text, $data) {
    global $base_url;
    $modulePath          = drupal_get_path("module", "campaigner");
    if (!empty($data['social_proof_fields'])) {
      $fieldArrayJS = array();
      $fieldArray   = array();
      foreach ($data['social_proof_fields'] as $field) {
        $fieldArrayJS[] = "'" . $field['value'] . "'";
        $fieldArray[]   = $field['value'];
      }
      $fields = "[" . implode(",", $fieldArrayJS) . "]";
    } else {
      $fieldArray = array("name");
      $fields     = "['name']";
    }
    $proofText = !empty($data['social_proof_title']) ? $data['social_proof_title'] : 'Recent Signatures';
    $socialProof = theme("campaigner_social_proof", array("proofText"=>$proofText,"fields" => $fieldArray, "proof" => json_encode(campaigner_get_social_proof($data['node_id'], $fieldArray))));
    $text        = str_replace("[socialproof]", $socialProof, $text);
    $refreshRate = isset($data['social_proof_refresh_rate']) ? $data['social_proof_refresh_rate'] : 30000;
    drupal_add_js($modulePath . '/js/social_proof.js', array("type" => "file", "scope" => "footer", "weight" => 1));
    drupal_add_js("(function ($){
    $(document).ready(function(){
      $('.socialproof').campaignerSocialProof({
        url: '" . $base_url . "/campaigner/social_proof',
        action: " . $data["node_id"] . ",
        fields: " . $fields . ",
        'refresh-rate': " . $refreshRate . "
      });
    });
  })(jQuery);", array("type" => "inline", "scope" => "footer", "weight" => 10));
    return $text;
  }