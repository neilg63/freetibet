<?php
  /**
   * @file
   * Processing of Postcode and IP-based geolocation
   */

//Load includes
  module_load_include('inc', 'campaigner', 'includes/campaigner_polygons');
  /**
   * Class LocationServices This class is designed to eventually replace the Location Services individual functions. At
   * present it provides redundant versions of some of the functions. The main advantage is that it stores potentially
   * useful information for retrieval by callers.
   */
  class LocationServices {

    protected $identifier = array();

    public function getIdentifier(){
      return $this->identifier;
    }

    public function ip_to_region($region_class){
      $users_ip = $_SERVER['REMOTE_ADDR'];
      if ($users_ip === "127.0.0.1") {
        //Dev environment does not have geolocatable IP, so using this for testing
        $users_ip = "87.114.72.49";
      }

      $long_lat   = ip_to_long_lat($users_ip);
      if (empty($long_lat['long']) || empty($long_lat['lat'])){
         return false;
      }
      $region_nid = long_lat_to_region_nid($long_lat, $region_class);
      $this->identifier = array("type" => "ip", "value" => $users_ip, "details"=>$long_lat);
      $this->identifier['details']["region"] = $region_nid;
      $this->identifier['details']['region_class'] = $region_class;
      return ($region_nid != FALSE ? node_load($region_nid) : FALSE);
    }

    public function postcode_to_region($postcode, $region_class) {

      if (!$long_lat = postcode_to_long_lat($postcode)) {
        return 'Invalid postcode';
      }

      $region_nid = long_lat_to_region_nid($long_lat, $region_class);
      return ($region_nid != FALSE ? node_load($region_nid) : 'No region found');
    }

  }

  function postcode_to_region($postcode, $region_class) {

    if (!$long_lat = postcode_to_long_lat($postcode)) {
      return 'Invalid postcode';
    }

    $region_nid = long_lat_to_region_nid($long_lat, $region_class);
    return ($region_nid != FALSE ? node_load($region_nid) : 'No region found');
  }


  function ip_to_region($region_class) {

    $users_ip = $_SERVER['REMOTE_ADDR'];
    if ($users_ip === "127.0.0.1") {
      //Dev environment does not have geolocatable IP, so using this for testing
      $users_ip = "87.114.72.49";
    }

    $long_lat   = ip_to_long_lat($users_ip);
    $region_nid = long_lat_to_region_nid($long_lat, $region_class);

    return ($region_nid != FALSE ? node_load($region_nid) : FALSE);
  }

  function ip_to_long_lat($ip) {

    $uri      = "http://freegeoip.net/xml/" . $ip;
    $response = simplexml_load_string(file_get_contents($uri));

    return array('long' => strip_tags($response->Longitude[0]), 'lat' => strip_tags($response->Latitude[0]));
  }

  function postcode_to_long_lat($postcode) {
    $postcode = strtoupper(str_replace(' ', '', $postcode));
    // Stripped of spaces, any valid postcode should be 7 characters or less
    if (strlen($postcode) > 7) {
      drupal_set_message(t("Invalid postcode"), 'error');
      return FALSE;
    }
    $lookup_result = db_query('SELECT lat, lng FROM postcode_lookup WHERE postcode = :postcode', array(':postcode' => $postcode));
    $lookup_result = $lookup_result->fetchObject();

    if ($lookup_result != FALSE) {
      return array('lat' => $lookup_result->lat, 'long' => $lookup_result->lng);
    } else {
      return FALSE;
    }

  }


  /**
   * Takes a longitude/latitude and returns an entity id for the region that contains that long/lat.
   *
   * @param array $long_lat
   * @param int $region_class
   *
   * @return mixed
   */
  function long_lat_to_region_nid($long_lat, $region_class) {
    if (function_exists("geophp_load")){
      $geophp = geophp_load();
    }
    if (variable_get("campaigner_use_geos", 0) === 1 && geoPHP::geosInstalled()){
      return long_lat_to_region_nid_geos($long_lat, $region_class);
    }
    return long_lat_to_region_nid_non_geos($long_lat, $region_class);


  }

  /**
   * Takes a longitude/latitude and returns an entity id for the region that contains that long/lat.
   *
   * This version is designed to make use of the
   *
   * @param array $long_lat
   * @param int   $region_class
   *
   * @return mixed
   */
// This is the memory hungry original version
  function long_lat_to_region_nid_geos($long_lat, $region_class) {
    static $regions;
    // Load geoPHP for point-in-polygon
    $geophp = geophp_load();
    if (!$geophp) {
      drupal_set_message(t("Unable to load geoPHP library."), 'error');
      return;
    }
    if (empty($regions) || empty($regions[$region_class])){
      if (!class_exists("RegionCollection")) {
        require dirname(__FILE__) . '/collections/RegionCollection.php';
      }
      $regions[$region_class] = RegionCollection::getByRegionClass($region_class);
    }
    $regionId = $regions[$region_class]->sortByDistance($long_lat)->findContainingRegion($long_lat);
    return $regionId;
  }

  /**
   * Takes a longitude/latitude and returns an entity id for the region that contains that long/lat.
   *
   * @param $long_lat
   * @param $region_class
   *
   * @return mixed
   */
function long_lat_to_region_nid_non_geos($long_lat, $region_class){
  $query = new EntityFieldQuery();
  $query->entityCondition("entity_type", "node");
  $query->entityCondition("bundle", "campaigner_region");
  $result = $query->execute();
  $regions  = $nodes = node_load_multiple(array_keys($result['node']));
 // Sort the regions according to how close the desired point is to the centre of each region
  usort($regions, function ($a, $b) use ($long_lat) {
    $entities = array($a, $b);
    foreach ($entities as $entity) {
      $entity->centreCoords = array(
        $entity->field_campaigner_area_polygon[$entity->language][0]['lon'],
        $entity->field_campaigner_area_polygon[$entity->language][0]['lat']
      );
    }
    $aX    = $a->centreCoords[0] - $long_lat["long"] * cos($long_lat["lat"] - $a->centreCoords[1]) / 2;
    $aY    = $a->centreCoords[1] - $long_lat["lat"];
    $aCalc = sqrt($aX * $aX + $aY * $aY);

    $bX    = $b->centreCoords[0] - $long_lat["long"] * cos($long_lat["lat"] - $b->centreCoords[1]) / 2;
    $bY    = $b->centreCoords[1] - $long_lat["lat"];
    $bCalc = sqrt($bX * $bX + $bY * $bY);

    return $aCalc > $bCalc ? 1 : -1;
  });
  include_once(dirname(__FILE__) . '/libraries/gisconverter.php');
  $decoder = new gisconverter\WKT();
  $point   = $decoder->geomFromText("POINT (" . $long_lat['long'] . " " . $long_lat['lat'] . ")");
 foreach ($regions as $region) {
    if (empty($region->field_campaigner_region_class) || $region->field_campaigner_region_class['und'][0]['tid'] === $region_class) {
      $polygon = $region->field_campaigner_area_polygon['und'][0]['wkt'];
      if (is_null($polygon)) {
        // There's no polygon so we should just continue here
        continue;
      }
      try {
        $decoded       = $decoder->geomFromText($polygon);
        $isPointInside = inGeom($point, $decoded);
        if ($isPointInside) {
          return $region->nid;
        }
      } catch (Exception $e) {
        // we shouldn't be here
      }
    }
  }
  return null;
}

function inGeom($point, $geom) {
  if (!empty($geom->components)) {
    foreach ($geom->components as $component) {
      if ($component instanceof gisconverter\Polygon) {
        $success = inGeom($point, $component);
        if ($success) {
          return TRUE;
        }
      }
      else {
        if ($component instanceof gisconverter\LinearRing) {
          return $component->contains($point);
        }
        else {
          $success = inGeom($point, $component);
          if ($success) {
            return TRUE;
          }
        }
      }
    }
  }
  return FALSE;
}