<?php

/**
 *  @file
 *  Utility functions for Media: Thinglink.
 */

/**
 *  Based on the Photo ID of a Thinglink image, this will return the URL to the
 *  image itself.
 *  @param $photo_id
 *    The Thinglink photo code.
 *  @param $width
 *  @param $height
 *    We use this to guess the actual size provided by Thinglink.
 *  @param $cached
 *    If TRUE, then we'll cache the remote URL if the attempt to save the file
 *    locally fails.
 */
function _media_thinglink_photo_url($parts, $cached) {
  return 'http://'. $parts['u'] . '.thingpic.com/' . $parts['p'] .'/'. $parts['s'];
}

/**
 *  This returns a cached photo stream from the Thinglink farm servers.
 */
function _media_thinglink_photo_remote_url($parts) {
  return 'http://'. $parts['u'] . '.thingpic.com/' . $parts['p'] .'/'. $parts['s'];
}

/**
 *  This is a wrapper for emfield_request_xml that includes thinglink's api key.
 *  @param string $method
 *  @param array $args
 *  @param boolean $refresh
 *    Optional; if TRUE, then we'll force a new load of the XML. Otherwise,
 *    a cached version will be retrieved if possible.
 *  @return object
 *    A fully populated SimpleXML object.
 */
function _media_thinglink_sets_request($method, $args = array(), $refresh = FALSE) {
  // Display an error if we don't have an API key yet.
  if (!_media_thinglink_error_check()) {
    return array();
  }

  $args['api_key'] = trim(media_thinglink_variable_get('api_key'));
  if ($secret = trim(media_thinglink_variable_get('api_secret'))) {
    $args['secret'] = md5($secret);
  }
  $args['method'] = $method;

  $xml = media_retrieve_xml(MEDIA_THINGLINK_RESTPOINT . '?' . drupal_http_build_query($args), user_access('administer site configuration'), $refresh);
  return $xml;
}

/**
 *  This will log an error if we don't have a key yet.
 *  In addition, if the user is an admin, we'll display an error.
 */
function _media_thinglink_error_check() {
  static $checked;
  if (is_null($checked)) {
    /*if (media_thinglink_variable_get('api_key') == '') {
      $error = 'You do not yet have a Thinglink API key set. You will need to !apply and enter your key at the !settings before Thinglink images may be displayed.';
      $arguments = array('!apply' => l(t('apply for a Thinglink API key'), MEDIA_THINGLINK_API_APPLICATION_URL), '!settings' => l(t('Thinglink administration page'), 'admin/config/media/media_thinglink'));
      if (user_access('administer site configuration')) {
        drupal_set_message(t($error, $arguments), 'error');
      }
      watchdog('media_thinglink', $error, $arguments);
      $checked = FALSE;
    }
    else {*/
      $checked = TRUE;
    //}
  }
  return $checked;
}

function _media_thinglink_original_url($uri, $refresh = FALSE) {
  $wrapper = file_stream_wrapper_get_instance_by_uri($uri);
  return $wrapper->interpolateUrl();
}

function _media_thinglink_photo_exists($photo_id) {
  //$info = _media_thinglink_sets_request('thinglink.photos.getInfo', array('photo_id' => $photo_id));

  //if ($info && ($info['@attributes']['stat'] != 'fail')) {
    return TRUE;
  //}
}


function _media_thinglink_build_uri($parts) {
  if (isset($parts['u'])) {
    if (isset($parts['p'])) {
      $uri = file_stream_wrapper_uri_normalize('thinglink://u/' . $parts['u'] . "/p/" . $parts['p'] . "/s/" . $parts["s"]);
      if (_media_thinglink_photo_exists($parts['s'])) {
        return $uri;
      }
    }
  }
}

function _media_thinglink_media_parse($url, $options = array()) {
  // The following are single images.
  $pattern = '(([0-9A-Za-z]{2})\.thingpic.com\/images\/([\\w]+)\/([^\\s"]*))';
  preg_match($pattern, $url, $matches);
  if (isset($matches[1]) && isset($matches[2])) {
    return _media_thinglink_build_uri(array('u' => $matches[1], 'p' => $matches[2], 's' => $matches[3]));
  }
}
