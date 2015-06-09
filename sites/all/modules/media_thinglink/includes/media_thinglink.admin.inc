<?php

/**
 *  @file
 *  Administrative page callbacks for Media: Thinglink.
 */

/**
 * Callback for /admin/configure/media/media_thinglink.
 */
function media_thinglink_settings_form() {
  $form = array();
  $form['thinglink_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add thinglink tagging tool to all pages.'),
    '#default_value' => variable_get('thinglink_enabled', 0),
    '#description' => t('Enables the Thinglink "personal tagging tool" on all pages.')
  );
 
  $form['thinglink_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Thinglink ID'),
    '#default_value' => variable_get('thinglink_id', ''),
    '#size' => 20,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#description' => t('Your Thinglink ID. Get it from <a href="@url">Thinglink website</a>.', array('@url' => 'http://www.thinglink.com/action/massembed')),
  );
 
  $form['thinglink_endpoint'] = array(
    '#type' => 'textfield',
    '#title' => t('Thinglink Endpoint'),
    '#default_value' => variable_get('thinglink_endpoint', 'http://www.thinglink.com/jse/embed.js'),
    '#size' => 45,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#description' => t('The URL for the Thinglink Javascript code. Likely no reason to change this, but see <a href="@url">Thinglink website</a> if needed.', array('@url' => 'http://www.thinglink.com/action/massembed')),
  );
 
  return system_settings_form($form);
}
 
/**
 *  Callback for /media/add/media_thinglink and
 *  /admin/content/media/add/media_thinglink.
 */
function media_thinglink_add($form, &$form_state = array(), $redirect = NULL) {
  global $user;
  $form = array();
  $form['thinglink'] = array(
    '#type' => 'vertical_tabs',
  );
  $form['thinglink']['all'] = array(
    '#type' => 'fieldset',
    '#title' => t('My Thinglink photos'),
  );

  // Get all thinglink files for this user
  $results = db_query("SELECT fid FROM {file_managed} WHERE uid = :uid AND uri LIKE :uri", array(
    ':uid' => $user->uid,
    ':uri' => 'thinglink%%'
  ))->fetchAll();

  module_load_include('inc', 'media_thinglink', 'includes/media_thinglink.utilities');

  foreach ($results as $result) {
    $file = file_load($result->fid);
    $url =  _media_thinglink_photo_url(basename($file->uri));
    $output = theme('image', array(
      'path' => $url,
      'title' => 'title',
      'alt' => 'alt',
      'attributes' => array('width' => 200, 'hspace' => 10, 'vspace' => 10),
      'getsize' => FALSE,
    ));

    $form['thinglink']['all'][$file->fid] = array(
      '#markup' => l($output, $url, array('html' => TRUE, 'attributes' => array('target' => '_blank'))),
    );
  }

  $form['thinglink']['add_from_url'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add from URL'),
  );

  $form['thinglink']['add_from_url'] ['url'] = array(
    '#type' => 'textarea',
    '#title' => 'URL',
    '#description' => 'Input the URL of the desired Thinglink photo page.',
  );

  $form['thinglink']['add_from_url'] ['redirect'] = array(
    '#type' => 'value',
    '#value' => $redirect,
  );

  $form['thinglink']['add_from_url'] ['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;
}

/**
 *  Validation for media_thinglink_add().
 */
function media_thinglink_add_validate($form, &$form_state) {
  $pattern = '(([0-9A-Za-z]{2})\.thingpic.com\/images\/([\\w]+)\/([^\\s"]*))';
  if (!preg_match($pattern, $form_state['values']['url'], $matches)) {
    form_set_error('url', t('Please submit a valid Thinglink image URL.'));
  }
}

/**
 *  Submission for media_thinglink_add().
 *
 *  This will create a file object for the Thinglink video.
 */
function media_thinglink_add_submit($form, &$form_state) {
  $uri = media_thinglink_media_parse($form_state['values']['url']);

  $defaults = array (
    'display' => TRUE,
  );

  // @TODO: This won't work for Thinglink and many other streams.
//   copy($url, $destination);

  $file = file_uri_to_object($uri);

  file_save($file);

  if ($file) {
    $form_state['redirect'] = 'media/' . $file->fid . '/edit';
    field_attach_submit('media', $file, $form, $form_state);
    // Make a copy of the file object to use as the media object
    // (file is a field of media and the base table). This is weird.
    $media = clone($file);
    $file->file = array();
    $file->file[LANGUAGE_NONE] = array();
    $file->file[LANGUAGE_NONE][0] = (array)$file + $defaults;
    $file->is_new = TRUE;
    field_attach_insert('media', $file);
  }
  else {
    drupal_set_message(t('An error occurred and no file was saved.'), 'error');
  }

  $form_state['redirect'] = !empty($form_state['values']['redirect']) ? $form_state['values']['redirect'] : 'media/' . $file->fid . '/edit';
}
