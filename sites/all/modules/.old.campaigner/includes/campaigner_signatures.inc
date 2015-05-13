<?php

  function get_signature_count($actionID) {
    if (!is_numeric($actionID)){
      return 0;
    }	
    return db_query('SELECT count(*) FROM `signatures` WHERE spam != 1 AND action = ' . $actionID)->fetchField();

  }

  function get_all_signature_count() {

    return db_query('SELECT count(*) FROM `signatures` WHERE spam != 1')->fetchField();

  }

  function get_signature_count_by_region($actionID, $regionID) {

    return db_query('SELECT count(*) FROM `signatures` WHERE spam != 1 AND action = ' . $actionID . ' AND region = ' . $regionID)->fetchField();

  }

  function get_signature_locations($actionID) {

    return db_query('SELECT lng as LONGITUDE, lat as LATITUDE FROM `signatures` WHERE spam != 1 AND action = ' . $actionID)->fetchAll();

  }

  /**
   * @param      $actionID
   * @param null $limit   Can be an array with keys "start" and "length"
   * @param null $orderBy Can be an array with keys "field" and "direction" passed through to $query->orderBy in that order
   * @param bool $eager If true we will eager load the region title
   *
   * @return mixed
   */
  function get_signatures_action($actionID, $limit = NULL, $orderBy = NULL, $eager = false) {
    $query = db_select("signatures", "s");
    $query->condition("s.action", $actionID, "=");
    $query->condition("s.spam", 1, "!=");
    $query->fields("s");
    if ($limit !== NULL) {
      $query->range($limit['start'], $limit['length']);
    }
    if ($orderBy !== NULL) {
      $query->orderBy($orderBy['field'], $orderBy['direction']);
    }
    if ($eager){
      $query->leftJoin("node", "r", "r.nid = s.region");
      $query->addField("r", "title", "region_title");
    }
    $result = $query->execute();
    return $result->fetchAll();

  }


  function get_signature_locations_in_box($actionID, $longLat1, $longLat2) { // $longLatX should be an array with 0 => longitude, 1 => latitude

    $boundLong1 = ($longLat1[0] >= $longLat2[0]) ? $longLat2[0] : $longLat1[0];
    $boundLong2 = ($longLat1[0] >= $longLat2[0]) ? $longLat1[0] : $longLat2[0];
    $boundLat1  = ($longLat1[1] >= $longLat2[1]) ? $longLat2[1] : $longLat1[1];
    $boundLat2  = ($longLat1[1] >= $longLat2[1]) ? $longLat1[1] : $longLat2[1];

    return db_query('SELECT lng as LONGITUDE, lat as LATITUDE FROM `signatures` WHERE spam != 1 AND LONGITUDE BETWEEN ' . $boundLong1 . ' AND ' . $boundLong2 . ' AND LATITUDE BETWEEN ' . $boundLat1 . ' AND ' . $boundLat2 . ' AND action = ' . $actionID)->fetchAll();

  }

  function get_target($actionID) {

    return node_load($actionID)->field_campaigner_target['und'][0]['value'];

  }

  function get_target_percentage($actionID) {

    return get_signature_count($actionID) / node_load($actionID)->field_campaigner_target['und'][0]['value']; // Range is 0.0 - 1.0, multiply by 100 for human-readable percentage

  }
