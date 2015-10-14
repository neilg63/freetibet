<?php
define('FTOVERRIDES_LIVE_URL','http://www.freetibet.org');

function  _ftoverrides_form_payment_donation_form_alter(&$form,&$form_state,$form_id) {
  $pmids = $form['payment_method']['#pmids'];
  $data = _ftoverrides_donation_context();
  
  if (is_array($pmids) && count($pmids) == 1) {
    switch ($data->context) {
      case 'join':
        $submitLabel = t("Join");
        break;
      case 'renewal':
      case 'renew':
          $submitLabel = t("Renew");
          break;  
      default:
        $submitLabel = t("Donate");
        break;
    }
    
    $form['context'] = array(
      '#type' => 'hidden',
      '#value' => $data->context,
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
        
      if (!empty($data->presets)) {
        $form['presets'] = array(
          '#type' => 'markup',
          '#markup' => theme_ftoverrides_donation_presets(array('items' => $data->presets))
        );
      }   
      $form['payment_method']['#title'] = $method_label;
      if ($data->minimum > 0) {
        $params = array('@amount'=> $data->minimum,'@currency' => '£');
        $form['payment_donation_amount']['#description'] = t("Minimum amount : @currency @amount",$params);
        
      }
      $pm_class = str_replace('_','-',$pm->name);
      $form['actions']['#weight'] = 200;
      $form['actions']['#suffix'] = '</div>';
      $form['actions']['payment_donation_pay']['#value'] = $submitLabel;
      $form['info'] = array(
        '#type' => 'markup',
        '#markup' => '<div class="payment-info '.$pm_class.'" title="'.$method_label . ": " . $pm->title_generic.'"></div>',
        '#prefix' => '<div class="donation-controls">',
        '#weight' => 2000
      );
      
      $default_currency_code = 'GBP';
      
      if ($data->add_currency_selector) {
        $form['#attributes']['class'][] = 'has-currency-selector';
        $options = array(
          'GBP' => '£',
          'EUR' => '€',
          'USD' => '$',
        );
        $element = array(
          '#type' => 'select',
          '#options' => $options,
          '#default_value' => $default_currency_code,
          '#weight' => -10
        );
        if (!empty($data->by_currency)) {
          $element['#attributes'] = array('data-currency' => json_encode($data->by_currency) );
        }
        $form['payment_donation_amount']['currency_code'] = $element;
      }
      else {
        $form['#attributes']['class'][] = 'show-currency';
      }
      
      $form['item_name'] = array(
        '#type' => 'hidden',
        '#value' => $data->item_name
      );
      
      $form['custom'] = array(
        '#type' => 'hidden',
        '#value' => $data->custom
      );
      
      if ($data->add_frequency_selector) {
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
      }
              
      $form['payment_method']['#access'] = false;
      $form['min_amount'] = array(
        '#type' => 'hidden',
        '#value' => $data->minimum,
        '#attributes' => array('id' => 'edit-min-donation-amount')
      );

      
      $form['default_amount'] = array(
        '#type' => 'hidden',
        '#value' => $data->default,
        '#attributes' => array('id' => 'edit-default-donation-amount','data-fixed' => (int) $data->fixed )
      );
      $form['error_msg'] = array(
        '#type' => 'hidden',
        '#value' => t("Your donation is below the minimum amount",$params),
        '#attributes' => array('id' => 'edit-donation-error-msg','data-min' => t("@currency @amount") )
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
      $form['#attributes']['data-suggested'] = json_encode($data->suggested);
    }
  }
}

function _ftoverrides_donation_context() {
  $context = request_path();
  $parts = explode('/',$context);
  $pn = NULL;
  if (arg(0) == 'node') {
    $nid = arg(1);
    if (is_numeric($nid) && $nid > 0) {
      $pn = node_load($nid);
    }
  }
  $data = _ftoverrides_page_node_donation_settings($pn);
  if (count($parts)>1) {
    $context = array_shift($parts);
  }
  
  if ($data->type == 'donate') {
    $data->add_currency_selector = true;
    $data->add_frequency_selector = true;
    $data->presets = ftoverrides_donation_presets($context);
    if ($context == 'donate' && empty($data->suggested)) {
      $data->suggested = array(5,10,20,50);
    }
  } else {
    $data->presets = array();
    if (!empty($data->type)) {
      $context = $data->type;
    }
  }
  if (!$data->overridden) {
    $data->default = variable_get('ftoverrides_default_donation_amount', 10);
    $data->minimum = variable_get('ftoverrides_min_donation_amount', 1);
  }
  
  switch ($context) {
    case 'join':
    case 'renew':
    case 'renewal':
      if (!empty($data->presets)) {
        foreach ($data->presets as $preset) {
          if ($preset['key'] == 'month' && isset($preset['amount']['GBP'])) {
            $data->minimum = $preset['amount']['GBP'];
          }
        }
      }
      break;
  }
  
  $data->context = $context;
  if (empty($data->item_name)) {
    switch ($context) {
        case 'join':
          $data->item_name = t('Free Tibet supporter');
          break;
        case 'renewal':
            $data->item_name = t('Free Tibet renewal');
            break;  
        default:
          $data->item_name = t('Free Tibet donation');
          break;
    }
  }
  return $data;
}

function _ftoverrides_page_node_donation_settings($node) {
  $data = (object) array(
    'minimum' => 0,
    'default' => 10,
    'add_currency_selector' => false,
    'add_frequency_selector' => false,
    'by_currency' => array(),
    'overridden' => false,
    'type' => NULL,
    'fixed' => false,
    'item_name' => NULL,
    'custom' => 'donate'
  );
  if (is_object($node) && isset($node->type)) {
    $data->type = $node->type;
  }
  $data->suggested = array();
  if (!empty($data->type) && isset($node->field_donation_settings)) {
    $items = field_get_items('node',$node,'field_donation_settings');
    if (!empty($items) && isset($items[0]) && isset($items[0]['value'])) {
      $fc = field_collection_item_load($items[0]['value']);
      if (is_object($fc)) {
        $items = field_get_items('field_collection_item',$fc,'field_min');
        if (is_array($items) && isset($items[0]['value'])) {
          $data->minimum = (int) $items[0]['value'];
          $data->overridden = true;
        }
        $items = field_get_items('field_collection_item',$fc,'field_default');
        if (is_array($items) && isset($items[0]['value'])) {
          $data->default = (int) $items[0]['value'];
        }
        if ($data->minimum < 0 && $data->default > 0) {
          $data->fixed = true;
          $data->minimum = $data->default;
        }
        $items = field_get_items('field_collection_item',$fc,'field_currency_selector');
        if (is_array($items) && isset($items[0]['value'])) {
          $data->add_currency_selector = (bool) $items[0]['value'];
        }
        $items = field_get_items('field_collection_item',$fc,'field_default_by_currency');
        if (is_array($items) && isset($items[0]['value'])) {
          $currency_str = trim($items[0]['value']);
          $currency_data = array();
          if (!empty($currency_str)) {
            $parts = explode(',',$currency_str);
            foreach ($parts as $part) {
              $ps = explode(':', $part);
              if (count($ps)>1) {
                $cc = array_shift($ps);
                $cc = strtoupper(trim($cc));
                $value = array_shift($ps);
                if (is_numeric($value) && $value > 0) {
                  $currency_data[$cc] = (int) $value;
                }
              }
            }
          }
        }
        $items = field_get_items('field_collection_item',$fc,'field_suggested');
        if (is_array($items) && isset($items[0]['value'])) {
          foreach ($items as $item) {
            if (isset($item['value']) && is_numeric($item['value']) && $item['value'] > 0) {
              $data->suggested[] = (int) $item['value'];
            }
          }
        }
        
        
        $data->by_currency = $currency_data;
        
        $items = field_get_items('field_collection_item',$fc,'field_subtitle');          
        if (is_array($items) && isset($items[0]['value'])) {
          $items[0]['value'] = trim($items[0]['value']);
          if (strlen($items[0]['value']) > 2) {
            $data->item_name = $items[0]['value'];
          }
          module_load_include('inc','ctools','includes/cleanstring');
          $data->custom = ctools_cleanstring($data->item_name,array('lower case' => true));
        }
      }
    }
  }
  return $data;
}

function _theme_ftoverrides_donation_presets(array &$vars) {
  $html = '<div class="donation-presets-container hidden"><dl id="donation-presets">';
  if (isset($vars['items']) && is_array($vars['items'])) {
    foreach ($vars['items'] as $item) {
      $details = '';
      $attrs = array();
      $attrs['class'] = array('donation-'.$item['key']);
      $attrs['data-cycle'] = $item['cycle'];
      if (isset($item['cycle_limit'])) {
        $attrs['data-limit'] = $item['cycle_limit'];
      }
      $options = _ftoverrides_currency_value_options($item['amount']);
      $opt_vals = array();
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
      if (isset($item['description'])) {
        $addendum .= " (" .$item['description'] .")";
      }
      if (is_string($addendum)) {
        $attrs['title'] = $addendum;
      }
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
    case 'renewal':
    case 'renew':
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
    'description' => 'until you cancel your subscription',
    'cycle' => 'month',
    'cycle_limit' => 0,
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
  if (isset($values['payment_donation_amount'])) {
    if (isset($values['payment_donation_amount']['currency_code'])) {
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
               $limit = 0;
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
    if (isset($values['item_name'])) {
      $form_state['payment']->context_data['item_name'] = $values['item_name'];
    }
    if (isset($values['custom'])) {
      $form_state['payment']->context_data['custom'] = $values['custom'];
    }
  }
}

function ftoverrides_donation_presets_join() {
  $default = array();
  $default[] = array(
    'key' => 'monthly',
    'title' => 'Monthly',
    'description' => 'until subscription is cancelled',
    'cycle' => 'month',
    'cycle_limit' => 0,
    'amount' => array('GBP' => 2, 'EUR' => 3, 'USD' => 3)
  );
  $default[] = array(
    'key' => 'annual',
    'title' => 'Annual',
    'description' => 'until subscription is cancelled',
    'cycle' => 'year',
    'cycle_limit' => 0,
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
      if (isset($form['no_shipping'])) {
        $form['no_shipping']['#value'] = 0;
      }
      $form['amount_1']['#value'] = (float) $form['amount_1']['#value'];
      $form['amount_1']['#value'] = number_format($form['amount_1']['#value'],2,'.','');
      $type = 'standard';
      if (isset($payment->context_data['type'])) {
        $type = $payment->context_data['type'];
      }
      
      if (isset($payment->context_data['item_name'])) {
        $name = $payment->context_data['item_name'];
      }
      
      if (isset($payment->context_data['custom'])) {
        $custom = $payment->context_data['custom'];
      }
     
      if ($type != 'page') {
        $custom = $type;
      } else if (empty($custom)) {
        $custom = 'donate';
      }
      if (empty($name)) {
        $name = t('Free Tibet regular donation');
      }
      $form['item_name_1']['#value'] = $name;
      if (isset($payment->context_data['cycle'])) {
          switch ($type) {
            case 'join':
            case 'renew':
            case 'renewal':
              $altName = $name;
              break;
            default:
              $altName = t('Free Tibet regular donation');
              break;
          }
          
          switch ($payment->context_data['cycle']) {
            case 'month':
              $unit = 'M';
              $altName .= ": " . t("Monthly");
              break;
            case 'year':
              $unit = 'Y';
              $altName .= ": " . t("Yearly");
              break;  
            default:
              $unit = NULL;
              break;
          }
          if (!empty($unit)) {
            $name = $altName;
          }
          $form['item_name_1']['#value'] = $name;
          
          $form['custom'] = array(
            '#type' => 'hidden',
            '#value' => $custom,
          );
          
          if (!empty($unit)) {
            $cmdType = 'subscription';
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
              '#value' => 1, // multiples of t3 unit
            );
            $form['t3'] = array(
              '#type' => 'hidden',
              '#value' => $unit,
            );
            $form['src'] = array(
              '#type' => 'hidden',
              '#value' => 1,
            );
            // never set limit
            /*if ($limit > 1) {
              $form['srt'] = array(
                '#type' => 'hidden',
                '#value' => $limit,
              );
            }*/ 
        }
    }
  }
  $weight = -200;
  unset($form['upload']);
  switch ($cmdType) {
    case 'subscription':
      $form['cmd']['#value'] = '_xclick-subscriptions';
      break;
    default:
      $form['cmd']['#value'] = '_xclick';
      if (isset($form['no_note'])) {
        $form['no_note']['#value'] = 0;
      }
      break;
  }
  if (isset($form['amount_1'])) {
    $form['amount'] = $form['amount_1'];
    $form['item_name'] = $form['item_name_1'];
    unset($form['amount_1']);
    unset($form['item_name_1']);
    if (isset($form['quantity_1'])) {
      unset($form['quantity_1']);
    }
  }
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
  if (isset($form['return'])) {
    $regex = '#^https?://[^/]*?/#i';
    $form['return']['#value'] = preg_replace($regex,FTOVERRIDES_LIVE_URL . '/', $form['return']['#value']);
    $form['cancel_return']['#value'] = preg_replace($regex,FTOVERRIDES_LIVE_URL . '/', $form['cancel_return']['#value']);
    $form['notify_url']['#value'] = preg_replace($regex,FTOVERRIDES_LIVE_URL . '/', $form['notify_url']['#value']);
  }
}