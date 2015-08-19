<?php

/**
 * @file
 * Display Suite 1 column template for flexslider only.
 *
*/

freetibet_slide_fieldset_classes_alter($classes,$variables);
?>
<figure<?php print $layout_attributes; ?> class="<?php print $classes;?>">
<?php print $ds_content; ?>
</figure>
