<?php

/**
 * @file
 * Display Suite 1 column template.
 */
if (function_exists('freetibet_add_node_classes')) {
  freetibet_add_node_classes($classes,$content,$node);
}
if (!isset($node->add_page_section)) {
  $node->add_page_section = '';
}
?>
<<?php print $ds_content_wrapper; print $layout_attributes; ?> class="ds-1col <?php print $classes;?> clearfix">

  <?php if (isset($title_suffix['contextual_links'])): ?>
  <?php print render($title_suffix['contextual_links']); ?>
  <?php endif; ?>

  <?php print $ds_content; ?>
  <?php print $node->add_page_section; ?>
</<?php print $ds_content_wrapper ?>>

<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
