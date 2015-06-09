<?php

/**
 *  @file
 *  Create a Thinglink Stream Wrapper class for the Media/Resource module.
 */

/**
 *  Create an instance like this:
 *  $thinglink = new MediaThinglinkStreamWrapper('thinglink://u/[server]/p/[folder]/s/[image]');
 */
class MediaThinglinkStreamWrapper extends MediaReadOnlyStreamWrapper {
  protected $base_url = '.thingpic.com/';
  protected $parameters = array('u' => NULL, 'p' => NULL, 's' => NULL);

  function interpolateUrl() {
    if (isset($this->parameters['u']) && isset($this->parameters['p'])) {
      return 'http://' . check_plain($this->parameters['u']) . $this->base_url . 'images/' . check_plain($this->parameters['p']) . '/' . check_plain($this->parameters['s']);
    }
  }

  function getTarget($f) {
    return FALSE;
  }

  /**
   * Get the url of the original image.
   */
  function getExternalUrl() {
    // Return the direct image.
    $parts = $this->parameters;
    return 'http://'. $parts['u'] . $this->base_url .'images/' . $parts['p'] .'/'. $parts['s'];
  }
  function getOriginalThumbnailPath() {
    return $this->getExternalUrl();
  }

  function getLocalThumbnailPath() {
    $parts = $this->get_parameters();
    $filename = explode('#',$parts['s']);
    $local_path = ('public://media-thinglink/'. check_plain($filename[0]));
    if (!file_exists($local_path)) {
      $dirname = drupal_dirname($local_path);
      file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
      @copy($this->getOriginalThumbnailPath(), $local_path);
    }
    return $local_path;
  }
  static function getMimeType($uri, $mapping = NULL) {
    return 'image/thinglink';
  }
}
