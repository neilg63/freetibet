<?php

/**
 * @file
 * Display Suite 1 column template.
 */
$classes .= ' ';
$classes = preg_replace('#\bnode(-by-viewer)?\s+#','',$classes);
$classes = trim(preg_replace('#\s\s+#',' ',$classes));
$has_wrapper = $ds_content_wrapper != 'span';
if (function_exists('node_class') && is_object($node)) {
	$extra_class = node_class($node);
	if (!empty($extra_class)) {
		$classes .= ' ' . trim($extra_class);
	}
}

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
