<div class='thermometer' data-count='<?php echo $thermometer_count;?>' data-target='<?php echo $thermometer_target;?>'>
  <div class='thermometer-holder'>
    <div class='counter' style='width: <?php echo ($thermometer_count * 100) / $thermometer_target;?>%; display: block;'></div>
    </div>
  <?php
    if ($showCount):
?>
     <p><strong><?php echo $thermometer_count;?></strong>&nbsp;<?php echo t("have signed to date");?></p>
    <?php
    endif;
  ?>
</div>
