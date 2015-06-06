<?php
/**
 * @file
 * media_internet handler for slideshare.
 */

/**
 * Implementation of MediaInternetBaseHandler.
 *
 * @see hook_media_internet_providers()
 */
class MediaInternetSlideshareHandler extends MediaInternetBaseHandler {

  static $slideshare;

  public static function parse($embedCode) {
    $pattern = '@slideshare\.net/([^"\&\? ]+)@i';
    preg_match($pattern, $embedCode, $matches);
    if (isset($matches[1])) {
      return $matches[1];
    }
  }

  public function claim($embedCode) {
    $parse = self::parse($embedCode);
    if ($parse && self::getObjectFromUrl($embedCode)) {
      return TRUE;
    }
  }

  public function validate() {
    $slideshow = self::$slideshare;
    if ($slideshow) {
      $uri = file_stream_wrapper_uri_normalize('slideshare://s/' . $slideshow['slideshow_id']);
      $existing_files = file_load_multiple(array(), array('uri' => $uri));
      if (count($existing_files)) {
        throw new MediaInternetValidationException(t('You have entered a URL for a slideshare presentation that is already in your library.'));
      }
    }
    else {
      throw new MediaInternetValidationException(t('Cannot find presentation for given slideshare embed code.'));
    }
  }

  public function save() {
    $file = $this->getFileObject();
    db_insert('media_slideshare_url')
    ->fields(array(
      'id' => self::$slideshare['slideshow_id'],
      'url' => self::parse($this->embedCode),
    ))
    ->execute();

    file_save($file);
    return $file;
  }

  public function getFileObject() {
    $uri = file_stream_wrapper_uri_normalize('slideshare://s/' . self::$slideshare['slideshow_id']);
    return file_uri_to_object($uri);
  }

  public static function getObjectFromUrl($embedCode) {
    $url = 'http://www.slideshare.net/' . self::parse($embedCode);
    $response = drupal_http_request('http://www.slideshare.net/api/oembed/2?format=json&url=' . $url);
    if ($response->code == '200') {
      self::$slideshare = drupal_json_decode($response->data);
      return self::$slideshare;
    }
  }

}
