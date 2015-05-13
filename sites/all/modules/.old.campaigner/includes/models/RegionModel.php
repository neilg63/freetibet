<?php

/**
 * A RegionModel captures all of the information about a Region (a polygon with an associated title).
 */
class RegionModel {
  /** @var Polygon */
  public $polygon = NULL;
  /** @var string|bool A serialize()d version of the polygon for this region. */
  public $polygonobject = FALSE;
  /** @var bool|string The Well Known Text version of the polygon for this region. */
  public $field_campaigner_area_polygon_wkt = FALSE;

  public $latLongCalcs = array();

  /**
   * Just a constructor, does nothing.
   */
  public function __construct() {
    $this->generateCentreCoords();
  }

  public function generateCentreCoords(){
    if (!empty($this->centre)) {
      $this->centreCoords = explode(",", $this->centre);
    }
  }

  /**
   * This checks whether there's a serialize()d version of the region's polygon and unserialize()s it if so,
   */
  public function reconstitute() {
    if (!empty($this->polygonobject)) {
      $this->polygon = unserialize($this->polygonobject);
    }
  }

  /**
   * If we have a wkt (Well Known Text) string then we want to convert that into a Polygon object and stuff it into an instance property.
   */
  public function generatePolygon() {

    if (!empty($this->wkt)) {
      $this->polygon = geoPHP::load($this->wkt, 'wkt');
    }
  }

  /**
   * Writes the appropriate details from the current model into the GEOS cache.
   *
   * @param bool $safeSchema If we are running as part of hook_upgrade_NNNN then we can't rely on the schema of the db
   *  so we should use insert rather than drupal_write_record, setting this false moves us into upgrade mode.
   *
   * @return bool
   */
  public function updateCache($safeSchema = TRUE) {
    $this->reconstitute();
    empty($this->polygon) && $this->generatePolygon();
    if (empty($this->polygon)) {
      return FALSE;
    }
    $record       = new stdClass();
    $primary_keys = array();
    if (!empty($this->cid)) {
      $primary_keys["cid"] = $this->cid;
    }
    $record->polygonobject = (empty($this->polygon)) ? FALSE : serialize($this->polygon);
    $centroid              = (!empty($this->polygon) && is_callable(array($this->polygon, "centroid"))) ? $this->polygon->centroid() : NULL;
    $record->centre        = (!empty($centroid) && count($centroid->coords) > 0) ? $centroid->coords[0] . ',' . $centroid->coords[1] : NULL;
    $record->modified      = time();
    $record->entity_id     = $this->entity_id;
    return ($safeSchema && drupal_write_record("cache_geos", $record, $primary_keys)) || db_insert("cache_geos")->fields((array)$record)->execute();
  }

  /**
   * Simple getter for the modified value.
   * @return mixed
   */
  public function getModified() {
    return isset($this->modified) ? $this->modified : 0;
  }

  /**
   * Given a Point object, is it in the polygon for this region?
   *
   * @param Point $point
   *
   * @return bool
   */
  public function containsPoint(Point $point) {
    $this->reconstitute();
    empty($this->polygon) && $this->generatePolygon();
    return (!empty($this->polygon) && $point->within($this->polygon));
  }

  /**
   * Simple getter for the entity_id.
   *
   * @return mixed
   */
  public function getEntityId() {

    return $this->entity_id;
  }

  /**
   * Given two RegionModel objects and a target latitude/longitude pair (set globally) will work out which region is closer.
   *
   * This is very rough, speed is an issue so we are using an approximation based on pythagoras rather than something
   * like the haversine forumla.
   *
   * For efficiency reasons we also use another global to store the current latLong pair that we're using as an identifier
   * and then look that up from a list of precomputed values where possible. The use of globals in the running of this
   * function is due to not being able to rely on php > 5.3. If we had an appropriate version we would use an inline closure
   * with the uses command to include the scope of the calling function (on RegionCollection).
   *
   * @param RegionModel $a @see usort() documentation for info on params, you shouldn't be calling this
   * @param RegionModel $b @see usort() documentation for info on params, you shouldn't be calling this
   *
   * @return float -1 if A is smaller than B and 1 if A is larger than B (final intended order: -1 = AB, 1 = BA)
   */
  public static function getDistance($a, $b) {
    global $currentLongLat, $latLongSortIdentifier;
    $entities = array($a, $b);
    foreach ($entities as $entity){
      if (!isset($entity->centreCoords)){
        // hack to stop this happening due to not being able to store this in the db
        if (!empty($entity->centre)) {
          $entity->centreCoords = explode(",", $entity->centre);
        }
        if (!isset($entity->centreCoords)){
          return 0;
        }
      }
    }

    // Calculate A and B manually if we don't have a stored record for them, if not we use the stored version.
    if (!isset($a->latLongCalcs[$latLongSortIdentifier])) {
      $aX                                      = $a->centreCoords[0] - $currentLongLat["long"] * cos($currentLongLat["lat"] - $a->centreCoords[1]) / 2;
      $aY                                      = $a->centreCoords[1] - $currentLongLat["lat"];
      $a->latLongCalcs[$latLongSortIdentifier] = sqrt($aX * $aX + $aY * $aY);
    }
    if (!isset($b->latLongCalcs[$latLongSortIdentifier])) {
      $bX                                      = $b->centreCoords[0] - $currentLongLat["long"] * cos($currentLongLat["lat"] - $b->centreCoords[1]) / 2;
      $bY                                      = $b->centreCoords[1] - $currentLongLat["lat"];
      $b->latLongCalcs[$latLongSortIdentifier] = sqrt($bX * $bX + $bY * $bY);
    }
    return $a->latLongCalcs[$latLongSortIdentifier] > $b->latLongCalcs[$latLongSortIdentifier] ? 1 : -1;
  }



}