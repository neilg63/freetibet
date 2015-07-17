<?php

function  _ftoverrides_form_payment_donation_form_alter(&$form,&$form_state,$form_id) {
  $pmids = $form['payment_method']['#pmids'];
  $context = request_path();
  $parts = explode('/',$context);
  if (count($parts)>1) {
    $context = array_shift($parts);
  }
  if (is_array($pmids) && count($pmids) == 1) {
    switch ($context) {
      case 'join':
        $submitLabel = t("Join");
        break;
      default:
        $submitLabel = t("Donate");
        break;
    }
    
    $form['context'] = array(
      '#type' => 'hidden',
      '#value' => $context,
      '#attributes' => array('id' => 'edit-context')
    );
    
    drupal_add_js(drupal_get_path('module','ftoverrides') . '/js/donate.js');
    $pmid = array_shift($pmids) ;
    $query = db_select('payment_method','pm')->fields('pm');
    $query->condition('pm.pmid',$pmid);
    $result = $query->execute();
    if ($result) {
      $pm = $result->fetch();
      $method_label = t("Pay by");
      
      $presets = ftoverrides_donation_presets($context);
      if (!empty($presets)) {
        $form['presets'] = array(
          '#type' => 'markup',
          '#markup' => theme_ftoverrides_donation_presets(array('items' => $presets))
        );
      }
      
      $form['payment_method']['#title'] = $method_label;
      $min_amount = variable_get('ftoverrides_min_donation_amount', 5);
      $params = array('@amount'=> $min_amount,'@currency' => '£');
      $form['payment_donation_amount']['#description'] = t("Minimum amount : @currency @amount",$params);
      $pm_class = str_replace('_','-',$pm->name);
      
      $form['actions']['#weight'] = 200;
      $form['actions']['#suffix'] = '</div><!-- suufix -->';
      $form['actions']['payment_donation_pay']['#value'] = $submitLabel;
      $form['info'] = array(
        '#type' => 'markup',
        '#markup' => '<div class="payment-info '.$pm_class.'" title="'.$method_label . ": " . $pm->title_generic.'"></div>',
        '#prefix' => '<div class="donation-controls">'
      );
      
      $default_currency_code = 'GBP';
      
      $options = array(
        'GBP' => '£',
        'EUR' => '€',
        'USD' => '$',
      );

      $form['payment_donation_amount']['currency_code'] = array(
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => $default_currency_code,
        '#weight' => -10
      );
      
      $options = array(
        'single' => 'Just once',
        'month' => 'Monthly',
        'year' => 'Annually'
      );
      $form['cycle'] = array(
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => 'single',
        '#weight' => 100
      );
              
      $form['payment_method']['#access'] = false;
      $form['min_amount'] = array(
        '#type' => 'hidden',
        '#value' => $min_amount,
        '#attributes' => array('id' => 'edit-min-donation-amount')
      );

      $default_amount = variable_get('ftoverrides_default_donation_amount', 10);
      $form['default_amount'] = array(
        '#type' => 'hidden',
        '#value' => $default_amount,
        '#attributes' => array('id' => 'edit-default-donation-amount')
      );
      $form['error_msg'] = array(
        '#type' => 'hidden',
        '#value' => t("Please enter a valid amount of at least @currency @amount",$params),
        '#attributes' => array('id' => 'edit-donation-error-msg')
      );
      $form['comma_error_msg'] = array(
        '#type' => 'hidden',
        '#value' => t("Please use only points for decimals"),
        '#attributes' => array('id' => 'edit-comma-error-msg')
      );
      
      if (is_array($form['#validate']) && !in_array('ftoverrides_donation_form_validate', $form['#validate'])) {
        $form['#validate'][] = 'ftoverrides_donation_form_validate';
      }
      $form['#after_build'][] = 'ftoverrides_donation_form_after_build';
    }
  }
}

function _theme_ftoverrides_donation_presets(array &$vars) {
  $html = '<div class="donation-presets-container hidden"><dl id="donation-presets">';
  if (isset($vars['items']) && is_array($vars['items'])) {
    foreach ($vars['items'] as $item) {
      $details = '';
      $attrs = array();
      $attrs['class'] = array('donation-'.$item['key']);
      $attrs['data-cycle'] = $item['cycle'];
      $attrs['data-limit'] = $item['cycle_limit'];
      $options = _ftoverrides_currency_value_options($item['amount']);
      if (!empty($item['amount'])) {
        switch ($item['cycle']) {
          case 'single':
            $attrs['data-amount'] = json_encode($item['amount']);
              break;
            default:
              $details .= _ftoverrides_currency_value_selector($item['key'], $options);
              break;
        }
      }
      $addendum = implode('/', $opt_vals);
      $attrs['title'] = $addendum . " (" .$item['description'] .")";
      $opt_vals = array_values($options);
    
      $html .= '<dt'.drupal_attributes($attrs).'>'.$item['title'].'</dt>';
      $html .= '<dd class="donation-'.$item['key'].'">'.$details.'</dd>';
    }
  }

  return $html . "</dl>\n</div>";
}

function _ftoverrides_currency_value_options($values) {
  $options = array();
  foreach ($values as $key => $val) {
    switch ($key) {
      case 'GBP':
        $prefix = '£';
        break;
      case 'EUR':
        $prefix = '€';
        break;  
      case 'USD':
        $prefix = '$';
        break;          
    }
    $options[$key . ":" . trim($val) ] = $prefix . ' ' . $val;
  }
  return $options;
}

function _ftoverrides_currency_value_selector($key = NULL,  array $options = array()) {
  
  $element = array(
    '#options' => $options,
    '#name' => 'donation_preset_' . $key,
    '#id' => 'edit-donation-preset-' . $key
  );
  return theme_select(array('element' => $element));
}

function ftoverrides_donation_presets($context = 'donate') {
  $default = array();
  switch ($context) {
    case 'join':
      $default = ftoverrides_donation_presets_join();
      break;
    case 'donate':
      $default = ftoverrides_donation_presets_donate();
      $context = 'donate';
      break;
  }
  if (!empty($context)) {
    $return = variable_get('ftoverrides_donation_presets_'.$context,$default);
  }
  else {
    $return = $default;
  }
  return $return;
}

function ftoverrides_donation_presets_donate() {
  $default = array();
  $default[] = array(
    'key' => 'monthly',
    'title' => 'Give monthly',
    'description' => 'max. 52 instalments',
    'cycle' => 'month',
    'cycle_limit' => 52,
    'amount' => array('GBP' => 2, 'EUR' => 3, 'USD' => 3)
  );

  $GBP_def = variable_get('ftoverrides_default_donation_amount', 10);
  $EUR_def = floor($GBP_def * 1.5);
  $USD_def = floor($GBP_def * 1.5);
  
  $default[] = array(
    'key' => 'single',
    'title' => 'Single payment',
    'cycle' => 'single',
    'cycle_limit' => 1,
    'amount' => array('GBP' => $GBP_def, 'EUR' => $EUR_def, 'USD' => $USD_def ),
  );
  return $default;
}

function _ftoverrides_donation_form_validate(&$form,&$form_state) {
  $values = $form_state['values'];
  if (isset($values['payment_donation_amount']) && isset($values['payment_donation_amount']['currency_code'])) {
    $cc = $values['payment_donation_amount']['currency_code'];
    if (is_string($cc) && strlen($cc) === 3) {
      $form['payment_donation_amount']['#currency_code'] = $cc;
      $form['payment_donation_amount']['currency_code']['#default_value'] = $cc;
      if (isset($form_state['payment']) && is_object($form_state['payment'])) {
        $form_state['payment']->currency_code = $cc;
      }
    }
     $cycle = $values['cycle'];
     if ($cycle != 'single') {
       if (isset($values['limit'])) {
         $limit = $values['limit'];
       } else {
         switch ($cycle) {
           case 'month':
             $limit = 52;
             break;
           case 'year':
             $limit = 10;
             break;
         }
       }
       $form_state['payment']->context_data['cycle'] = $cycle;
       $form_state['payment']->context_data['cycle_limit'] = $limit;
     }
     $type = $values['context'];
     if (!empty($type)) {
       $form_state['payment']->context_data['type'] = $type;
     }
  }
}

function ftoverrides_donation_presets_join() {
  $default = array();
  $default[] = array(
    'key' => 'monthly',
    'title' => 'Monthly',
    'description' => 'max. 52 instalments',
    'cycle' => 'month',
    'cycle_limit' => 52,
    'amount' => array('GBP' => 2, 'EUR' => 3, 'USD' => 3)
  );
  $default[] = array(
    'key' => 'annual',
    'title' => 'Annual',
    'description' => '5 instalments',
    'cycle' => 'year',
    'cycle_limit' => 52,
    'amount' => array('GBP' => 24, 'EUR' => 36, 'USD' => 36)
  );
  
  return $default;
}

function _ftoverrides_form_paypal_payment_pps_form_redirect_alter(&$form,&$form_state,$form_id) {
  $payment = NULL;
  if (!empty($form_state['build_info']['args']) && is_array($form_state['build_info']['args'])) {
    foreach ($form_state['build_info']['args'] as $arg) {
      if (is_object($arg) && $arg instanceof Payment) {
        $payment = $arg;
        break;
      }
    }
  }
  if (is_object($payment)) {
    if (isset($payment->context_data)) {
      $form['amount_1']['#value'] = (float) $form['amount_1']['#value'];
      $form['amount_1']['#value'] = number_format($form['amount_1']['#value'],2,'.','');
      $type = 'standard';
      if (isset($payment->context_data['type'])) {
        $type = $payment->context_data['type'];
      }
      
      if (isset($payment->context_data['cycle'])) {
        switch ($type) {
          case 'join':
            $name = t("Free Tibet Membership");
            break;
          default:
            $name = t("Regular Donation to Free Tibet");
            break;
        }
          switch ($payment->context_data['cycle']) {
            case 'month':
              $unit = 'M';
              $name .= ": " . t("Monthly");
              break;
            case 'year':
              $unit = 'Y';
              $name .= ": " . t("Yearly");
              break;  
            default:
              $unit = NULL;
              break;
          }
          $form['item_name_1']['#value'] = $name;
          
          $form['custom'] = array(
            '#type' => 'hidden',
            '#value' => $type,
          );
          
          if (!empty($unit)) {
            $limit = $payment->context_data['cycle_limit'];
            $form['cmd']['#value'] = '_xclick-subscriptions';
            $form['item_name'] = array(
              '#type' => 'hidden',
              '#value' => $form['item_name_1']['#value'],
            );
            unset($form['item_name_1']);
            unset($form['quantity_1']);
            $form['a3'] = array(
              '#type' => 'hidden',
              '#value' => $form['amount_1']['#value'],
            );
            unset($form['amount_1']);
            switch ($unit) {
              case 'M':
                $limit = min(24,$limit);
                break;
              case 'Y':
                $limit = min(5,$limit);
                break; 
            }
            
            $form['p3'] = array(
              '#type' => 'hidden',
              '#value' => $limit,
            );
            $form['t3'] = array(
              '#type' => 'hidden',
              '#value' => $unit,
            );
            $form['src'] = array(
              '#type' => 'hidden',
              '#value' => 1,
            );
            $form['srt'] = array(
              '#type' => 'hidden',
              '#value' => $limit,
            );
            $weight = -200;
            foreach ($form as $key => $data) {
              if (strpos($key,'#') !== 0 && is_array($data) && isset($data['#type'])) {
                switch ($data['#type']) {
                  case 'hidden':
                    $form[$key]['#weight'] = $weight;
                    $weight++;
                    break;
                }
              }
            }
          }
          $form['message']['#weight'] = 1000;
      }
    }
  }
}