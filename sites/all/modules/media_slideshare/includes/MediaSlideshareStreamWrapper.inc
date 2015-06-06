<?php

/**
 *  @file
 *  Create a Slideshare Stream Wrapper class for the Media/Resource module.
 */

/**
 *  Create an instance
 */
class MediaSlideshareStreamWrapper extends MediaReadOnlyStreamWrapper {

  protected $base_url = 'http://www.slideshare.net/slideshow/embed_code';

  function interpolateUrl() {
    if ($parameters = $this->get_parameters()) {
      return $this->base_url . '/' . $parameters['s'];
    }
  }

  function getTarget($f) {
    return FALSE;
  }

  static function getMimeType($uri, $mapping = NULL) {
    return 'document/slideshare';
  }

  function getOriginalThumbnailPath() {
    $parts = $this->get_parameters();
    $id = check_plain($parts['s']);

    $url = db_query("SELECT url FROM {media_slideshare_url} WHERE id=:id", array(':id' => $id))->fetchField();
    if ($url) {
      $slideshare = MediaInternetSlideshareHandler::getObjectFromUrl('http://www.slideshare.net/' . $url);
      if (!empty($slideshare['thumbnail'])) {
        return $slideshare['thumbnail'];
      }
    }
  }

  function getLocalThumbnailPath() {

    $parts = $this->get_parameters();
    $id = check_plain($parts['s']);

    $local_path = 'public://media-slideshare/' . $id . '.jpg';
    if (!file_exists($local_path)) {
      $dirname = drupal_dirname($local_path);
      file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
      @copy($this->getOriginalThumbnailPath(), $local_path);
    }
    return $local_path;
  }
}
