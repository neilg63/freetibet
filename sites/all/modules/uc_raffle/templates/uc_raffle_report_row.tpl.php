<tr>
  <?php
    if (!empty($row)):
      foreach ($row as $field):
        ?>
      <td><?php echo $field;?></td>
      <?php
      endforeach;
    endif;
  ?>
</tr>