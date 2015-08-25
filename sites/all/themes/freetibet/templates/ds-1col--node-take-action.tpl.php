<?php

/**
 * @file
 * Display Suite 1 column template.
 */


$has_wrapper = freetibet_page_section_classes($classes,$ds_content_wrapper);
freetibet_take_action_node_classes_alter($classes,$variables);
if ($has_wrapper):
?>
<<?php print $ds_content_wrapper; print $layout_attributes; ?> class="<?php print $classes;?>">
<?php endif; ?>
<?php if (isset($title_suffix['contextual_links'])): ?>
  <?php print render($title_suffix['contextual_links']); ?>
<?php endif; ?>
  <?php print $ds_content; ?>
<?php if ($has_wrapper): ?>
</<?php print $ds_content_wrapper ?>>
<?php endif; ?>
