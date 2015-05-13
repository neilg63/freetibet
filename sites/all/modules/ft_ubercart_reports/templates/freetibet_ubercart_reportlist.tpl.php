<?php
/**
 * @file
 */
if (!empty($reports)):
  ?>
<ul>
  <?php
  foreach ($reports as $key => $value) :
    ?>

      <li>
        <?php echo l($value, $url . $key);?>
      </li>
    <?php
  endforeach;
  ?>
</ul>
<?php
endif;