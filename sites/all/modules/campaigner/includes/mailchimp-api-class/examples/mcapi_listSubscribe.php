<?php
/**
This Example shows how to Subscribe a New Member to a List using the MCAPI.php 
class and do some basic error checking.
**/
require_once 'inc/MCAPI.class.php';
require_once 'inc/config.inc.php'; //contains apikey

//campaigner_mc_api_subscribe('Test Name','8b3b75e260','christestname@turnfront.com')
/**
 * Subscribe to a Mailchimp list.
 *
 * @param mixed $name A string is interpreted as a full name, an array should contain keys first, last and full
 * @param $listId
 * @param $email
 */
function campaigner_mc_api_subscribe($name,$listId,$email) {
	$api = new MCAPI('6f3c11539ddae791a12360b6be5d7693-us4');

	$merge_vars = array('MMERGE7'=>'clicktivist');
  if (is_array($name)){
    $merge_vars['FNAME'] = $name['first'];
    $merge_vars['LNAME'] = $name['last'];
    $merge_vars['MMERGE2'] = $name['first'];
  }
  else {
    $merge_vars['MMERGE2'] = $name;
  }
	// By default this sends a confirmation email - you will not see new members
	// until the link contained in it is clicked!
	$retval = $api->listSubscribe( $listId, $email, $merge_vars );
	
	if ($api->errorCode){
		//echo "Unable to load listSubscribe()!\n";
		//echo "\tCode=".$api->errorCode."\n";
		//echo "\tMsg=".$api->errorMessage."\n";
	} else {
	    //echo "Subscribed - look for the confirmation email!\n";
	}

}

?>
