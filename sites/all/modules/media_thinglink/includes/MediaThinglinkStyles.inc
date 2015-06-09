<?php

/**
 * @file
 * Definition for Styles.
 */
class MediaThinglinkStyles extends FileStyles {

  function getImageUri() {
    if ($image_uri = $this->get('imageUri')) {
      return $image_uri;
    }
    $object = $this->getObject();
    if ($object->uri) {
      $wrapper = file_stream_wrapper_get_instance_by_uri($object->uri);
      return $wrapper->getLocalThumbnailPath();
    }
  }
  function thinglink_image($effect) {
    $variables = array(
      'uri' => $this->getUri(),
      'width' => $this->getWidth(),
      'height' => $this->getHeight(),
    );
    $variables = array_merge($variables, $this->variables);
    $this->setOutput(theme('media_thinglink_embed', $variables));
  }
}
