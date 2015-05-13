<p style="font-family: verdana, arial, helvetica; font-size: small;">Congratulations! You have now been entered into the 2014 Free Tibet Raffle.</p>

<p style="font-family: verdana, arial, helvetica; font-size: small;">Our annual raffle raises vital funds for our campaigning work, so thank you for your support. 
The closing date for entry into the draw is 17th November 2014 and the lucky winners will be drawn on 21st November 2014.</p>

<p style="font-family: verdana, arial, helvetica; font-size: small;">All winners will be notified within two working days from the draw date and a list of winners will be available <a href="http://freetibet.org/raffle">on our website</a> once the draw has been made.</p> 

<p style="font-family: verdana, arial, helvetica; font-size: small;">Please retain this email as your receipt and please find below your unique raffle ticket numbers:</p>

<?php
if (!empty($products)):
  foreach ($products as $productID=>$product):
    ?>
    <p style="font-family: verdana, arial, helvetica; font-size: small;"><strong><?php echo $product['title'];?></strong></p>
    <ul>
      <?php
      foreach ($product['tickets'] as $ticket):
?>
        <li><p style="font-family: verdana, arial, helvetica; font-size: small;"><?php echo $ticket['code'];?></p></li>
        <?php
      endforeach;
      ?>
    </ul>
<?php
  endforeach;
endif;
?>
<p style="font-family: verdana, arial, helvetica; font-size: small;">If you have any questions, please email <a href="mailto:raffle@freetibet.org">raffle@freetibet.org</a>.</p>

<p style="font-family: verdana, arial, helvetica; font-size: small;">With best wishes,</p>

<p style="font-family: verdana, arial, helvetica; font-size: small;">The Free Tibet team</p>

<p style="font-family: verdana, arial, helvetica; font-size: small;">Promoter: Free Tibet, 28 Charles Square, London, N1 6HT</p>

<p style="font-family: verdana, arial, helvetica; font-size: small;">Contact: Eleanor Byrne-Rosengren, <a href="mailto:raffle@freetibet.org">raffle@freetibet.org</a></p>



