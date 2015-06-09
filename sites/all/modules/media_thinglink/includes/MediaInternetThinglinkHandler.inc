<?php

/**
 * Implementation of MediaInternetBaseHandler.
 *
 * @see hook_media_internet_providers().
 */
class MediaInternetThinglinkHandler extends MediaInternetBaseHandler {
  public function parse($embedCode) {
    $pattern = '(([0-9A-Za-z]{2})\.thingpic.com\/images\/([\\w]+)\/([^\\s"]*))';
    preg_match($pattern, $embedCode, $matches);
    if (isset($matches[1]) && isset($matches[2])) {
      return file_stream_wrapper_uri_normalize('thinglink://u/' . $matches[1] . "/p/" . $matches[2] . "/s/" . $matches[3]);
    }
  }

  public function claim($embedCode) {
    if ($this->parse($embedCode)) {
      return TRUE;
    }
  }

  public function getFileObject() {
    $uri = $this->parse($this->embedCode);
    return file_uri_to_object($uri, TRUE);
  }
}
