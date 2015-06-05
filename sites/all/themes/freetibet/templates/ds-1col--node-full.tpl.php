<?php

/**
 * @file
 * Display Suite 1 column template.
 */
$content_fields = array_keys($content);

$classes = trim($classes);

if (in_array('field_image',$content_fields)) {
	$suppress = false;
	if (isset($field_hide_default_image) && is_array($field_hide_default_image)) {
		foreach ($field_hide_default_image as $lang => $vals) {
			if (!empty($vals) && isset($vals[0]) && isset($vals[0]['value'])) {
				$suppress = (bool) $vals[0]['value'];
			}
		}
	}
	if (!$suppress) {
		$classes .= ' has-default-image';
	}
}

if (in_array('field_key_points',$content_fields)) {
	$num_items = count($content['field_key_points']['#items']);
	$hasKeyPoints = false;
	if ($num_items > 0) {
		foreach ($content['field_key_points']['#items'] as $index => $data) {
			if (isset($content['field_key_points'][$index]['entity']['field_collection_item'])) {
				foreach ($content['field_key_points'][$index]['entity']['field_collection_item'] as $id => $fd) {
					if (isset($fd['field_text']['#items'][0]['value'])) {
						if (strlen($fd['field_text']['#items'][0]['value']) > 2) {
							$hasKeyPoints = true;
							break;
						}
					}
				}
			}
			break;
		}
	}
	if ($hasKeyPoints) {
		$classes .= ' has-key-points';
	}
}

if (function_exists('node_class') && is_object($node)) {
	$extra_class = node_class($node);
	if (!empty($extra_class)) {
		$classes .= ' ' . trim($extra_class);
	}
}


?>
<<?php print $ds_content_wrapper; print $layout_attributes; ?> class="ds-1col <?php print $classes;?> clearfix">

  <?php if (isset($title_suffix['contextual_links'])): ?>
  <?php print render($title_suffix['contextual_links']); ?>
  <?php endif; ?>

  <?php print $ds_content; ?>
</<?php print $ds_content_wrapper ?>>

<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
