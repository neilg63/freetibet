<div data-proof='<?php echo $proof; ?>' class='socialproof<?php echo in_array("comments", $fields) ? " fields-comment" : "";?>'>
  <h2><?php echo $proofText;?></h2>
  <?php
  $proof = json_decode($proof);
  if (!empty($proof)):
    ?>
    <div id='signature_block'>
    <?php
    foreach ($proof as $signature):
?>
      <div data-id='<?php echo isset($signature->id) ? $signature->id : '';?>' class='individual_signature'>
      <?php
      if (!empty($fields)):
        foreach ($fields as $fieldName):
          ?>
          <div class='sig_field <?php echo $fieldName;?>'><?php echo ($signature->$fieldName !== null) ? $signature->$fieldName : ''?></div>
        <?php
        endforeach;
      endif;
        ?>
      </div>
        <?php
    endforeach;
      ?>
    </div>
      <?php
  endif;
  ?>
</div>
