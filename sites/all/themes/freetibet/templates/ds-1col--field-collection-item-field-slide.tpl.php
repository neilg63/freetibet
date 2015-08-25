<?php

/**
 * @file
 * Display Suite 1 column template for flexslider only.
 *
*/
if (function_exists('freetibet_slide_fieldset_classes_alter')) {
  freetibet_slide_fieldset_classes_alter($classes,$variables);
  if ($variables['override_block_title']) {
    freetibet_replace_block_title($variables, $ds_content);
  }
}
?>
<figure<?php print $layout_attributes; ?> class="<?php print $classes;?>">
<?php print $ds_content; ?>
</figure>
