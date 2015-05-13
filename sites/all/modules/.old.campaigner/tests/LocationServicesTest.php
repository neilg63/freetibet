<?php
$_SERVER["REMOTE_ADDR"] = "127.0.0.1";

define('DRUPAL_ROOT', getcwd());
require_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

if (!function_exists("postcode_to_region")){

  require dirname(__FILE__) . '/../includes/campaigner_location_services.inc';
}

class LocationServicesTest extends PHPUnit_Framework_TestCase {

  public function testCanConvertPostcodeToRegion(){
    $pairs = array("BN1 2LJ" => 395, "RG24 7ER"=>413, "SO164GU"=>423, "BANANA" => false);
    foreach ($pairs as $postcode => $regionCode){
      $region = postcode_to_region($postcode, 9);
      $this->assertEquals($region->nid, $regionCode);
    }
  }

}