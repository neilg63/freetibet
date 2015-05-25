<ul class="textsize-selector">
	<?php foreach ($sizes as $size): ?>
		<li<?php print drupal_attributes($size->attrs); ?>><?php print $size->label; ?></li>
	<?php endforeach; ?>
</ul>