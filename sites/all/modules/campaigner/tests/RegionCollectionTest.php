<?php

require_once dirname(__FILE__) .'/../campaigner/includes/collections/RegionCollection.php';

class RegionCollectionTest extends PHPUnit_Framework_TestCase {

  public function testCanMakeResourceCollection(){
    $output = new RegionCollection();
    $this->assertInstanceOf("RegionCollection", $output);
  }

  public function testCanAddRegionsAtConstruct(){
    $test1 = new stdClass();
    $test1->entity_type = "node";
    $test1->bundle = "campaigner_region";
    $test1->deleted = 0;
    $test1->entity_id = 182;
    $test1->revision_id = 182;
    $test1->language = "und";
    $test1->delta = 0;
    $test1->field_campaigner_region_class_tid = 7;
    $test2 = new stdClass();
    $test2->entity_type = "node";
    $test2->bundle = "campaigner_region";
    $test2->deleted = 0;
    $test2->entity_id = 220;
    $test2->revision_id = 220;
    $test2->language = "und";
    $test2->delta = 0;
    $test2->field_campaigner_region_class_tid = 7;
    $output = new RegionCollection(array(
                                        $test1, $test2
                                   ));
    $this->assertInstanceOf("RegionCollection", $output);
    $this->assertCount(2, $output->getAll());
  }

}