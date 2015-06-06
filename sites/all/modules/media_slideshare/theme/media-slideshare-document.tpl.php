<?php
/**
 * @file
 * Template file for theme('media_slideshare_document').
 *
 * Variables available:
 *  $uri - The embed uri for the slideshare document.
 *  $document_id - The unique identifier of the slideshare document.
 *  $width - The width to render.
 *  $height - The height to render.
 */
?>
<div class="media-slideshare-wrapper" id="media-slideshare-<?php print $document_id; ?>" style="width:<?php print $width; ?>">
  <iframe src="<?php print $url; ?>?rel=0" width="<?php print $width; ?>" height="<?php print $height; ?>" frameborder="0" marginwidth="0" marginheight="0" scrolling="no"></iframe>
</div>