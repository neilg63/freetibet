<?php

/**
 * @file
 * Display Suite 1 column template for flexslider only.
 *
*/
$classes = str_replace('entity-field-collection-item','flex-slide', trim($classes));

if (isset($variables['elements']['#entity'])) {
  $items = field_get_items('field_collection_item',$variables['elements']['#entity'], 'field_media');
  if (!empty($items) && is_array($items)) {
    if (isset($items[0]['filemime']) && is_string($items[0]['filemime'])) {
      $parts  = explode('/', $items[0]['filemime']);
      $classes .=  ' ' . array_shift($parts);
    }
  }
}
?>
<figure<?php print $layout_attributes; ?> class="<?php print $classes;?>">
<?php print $ds_content; ?>
</figure>
