<style>
  .tickets .ticket {
    background-color: #C0F0E2;
    border: 1px solid black;
    padding: 10px;
    width: 400px;
    margin-bottom: 5px;
  }

  .tickets .ticket .product {
    font-weight: bold;
  }
</style>

<h2>Raffle tickets</h2>
<?php
  if (!empty($tickets)):
?>
    <p>There are <em><?php echo count($tickets)?></em> tickets associated with this order</p>
<div class='tickets'>
<?php

    foreach ($tickets as $ticket):
      $productName = isset($order->products[$ticket->order_product_id]) ? $order->products[$ticket->order_product_id]->title : "";
      ?>
      <div class='ticket'>

        <p><span class='product'><?php echo $productName;?></span></p>
        <p><span class='code'><?php echo $ticket->code;?></span></p>
      </div>
  <?php
    endforeach;
  ?>
</div>
<?php
    if (user_access("email order raffle tickets")):
      $path = $_GET['q'];
      $query = drupal_http_build_query(drupal_get_query_parameters());
      if ($query != '') {
        $path .= '?' . $query;
      }
     ?>
      <p>
      <?php
        echo l(t("Email tickets to user"), "admin/store/raffle/email_order/" . $order->order_id,
          array("attributes"=>array("class"=>array("button")),"query"=>array("destination"=>$path)));
      ?>
      </p>
    <?php
      endif;
    ?>
  <?php
  else:
?>
<p>This order has no raffle tickets</p>
<?php
    endif;
?>