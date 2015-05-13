<?php
  /**
   * COLLECTION_RESOUCE_MODE uses an open database handler to store records, it is most appropriate when we want to iterate through records rather than return them as a group
   */
  define("COLLECTION_RESOURCE_MODE", 0);
  /**
   * COLLECTION_RECORD_MODE uses a hash to store the members of the collection
   */
  define("COLLECTION_RECORD_MODE", 1);

  /**
   * A RegionCollection object stores database information into Region model objects.
   */
  class RegionCollection {
    /**
     * @var array The array that, when storageStyle is set to COLLECTION_RECORD_MODE, will store the items in the collection
     */
    protected $collected = array();
    /**
     * @var null|DatabaseStatementInterface A stored MySQL statement, used when storageStyle equals COLLECTION_RESOURCE_MODE
     */
    protected $resource = NULL;
    /**
     * @var int Determines whether we should use a resource handle or an array to store the data in the collection. Can take a value of COLLECTION_RESOURCE_MODE or COLLECTION_RECORD_MODE.
     */
    protected $storageStyle;
    /**
     * @var array Stores previously loaded Region Classes so that we can return them if they are requested several times
     */
    protected static $history = array();

    protected $regionCache = array();

    /**
     * @param array $items
     * @param int   $storageStyle
     */
    public function __construct($items = array(), $storageStyle = COLLECTION_RECORD_MODE) {
      $this->storageStyle = $storageStyle;
      if (db_table_exists("cache_geos")){
        // Check if we have a cache for geographical information, if so we will use that, otherwise we won't
        $query = db_select("cache_geos", "cg");
        $query->fields("cg");
        $result = $query->execute();
        if (!empty($result)){
          foreach ($result as $row){
            $this->regionCache[$row->entity_id] = $row;
          }
        }
      }
      if (!empty($items)) {
        $this->addSeveral($items);
      }
    }

    /**
     * Add a set of items to the collection.
     *
     * Operates in monad mode (returns itself at the end).
     *
     * @param DatabaseStatementInterface|array $items
     *
     * @return RegionCollection
     */
    public function addSeveral($items) {
      // @todo test whether items are appropriate for what we want to do
      if ($this->storageStyle === COLLECTION_RESOURCE_MODE) {
        $this->resource = $items;
      } else {
        foreach ($items as $item) {
          $this->add($item);
        }
      }
      return $this;
    }

    /**
     * Returns the contents of the collection.
     *
     * If the collection is in resource mode it will extract the items from the resource handler and then return it. As
     * such, this might be an expensive function to call in that case.
     *
     * @return array The contents of the collection.
     */
    public function getAll() {
      if ($this->storageStyle === COLLECTION_RECORD_MODE) {
        return $this->collected;
      } else {
        return $this->resource->fetchAllAssoc();
      }

    }

    /**
     * Add a single item to the collection.
     *
     * Operates in monad mode.
     *
     * @param mixed $item
     *
     * @return RegionCollection
     */
    public function add($item) {
      if (array_key_exists($item->entity_id, $this->regionCache)){
        if (!empty($this->regionCache[$item->entity_id])){
          foreach ($this->regionCache[$item->entity_id] as $field => $value){
            $item->$field = $value;
          }
        }
      }
      $this->collected[] = $item;
      return $this;
    }

    /**
     *
     *
     * @param array $longLat
     *
     * @return int|bool Either returns an entity_id (Drupal node id) or FALSE
     */
    public function findContainingRegion($longLat) {
      if (!empty($this->collected)) {
        $point = geoPHP::load('POINT(' . $longLat['long'] . ' ' . $longLat['lat'] . ')', 'wkt');
        /** @var RegionModel $region */
        foreach ($this->collected as $key => $region) {
          $callMode = is_callable(array($region, "containsPoint"));
          // We may have a RegionModel or we may not, if we do we want to call the model version of the function
          if ($callMode && $region->containsPoint($point)){
            return $region->getEntityId();
          }
          else if (!$callMode && !empty($region->field_campaigner_area_polygon_wkt)) {
            $polygon = geoPHP::load($region->field_campaigner_area_polygon_wkt, 'wkt');
            if ($point->within($polygon))
              return $region->entity_id;
          }
        }
      }
      return FALSE;
    }

    /**
     * Sorts the collection array by estimated distance to the target latitude/longitude.
     *
     * @param $longLat
     *
     * @return RegionCollection
     */
    public function sortByDistance($longLat) {
      // We have to use a global as the only alternative is to use a Closure and we can't be sure we are using PHP > 5.3
      global $currentLongLat, $latLongSortIdentifier;
      $latLongSortIdentifier++;
      $currentLongLat = $longLat;
      usort($this->collected, array("RegionModel", "getDistance"));
      return $this;
    }

    public function getFirst() {
      $keys = array_keys($this->collected);
      return $this->collected[$keys[0]];
    }


    /**
     * Takes a region class and finds the relevant data from the database and produces a region collection with that data.
     *
     * @param int $regionClass  A region class tid
     * @param int $storageStyle Either COLLECTION_RECORD_MODE or COLLECTION_RESOURCE_MODE
     *
     * @return RegionCollection The RegionCollection object generated
     */
    public static function getByRegionClass($regionClass, $storageStyle = COLLECTION_RECORD_MODE) {
      $geophp = geophp_load();
      if (!isset(RegionCollection::$history[$regionClass])) {
        if (!class_exists("RegionModel")) {
          require dirname(__FILE__) . '/../models/RegionModel.php';
        }
        $query = new EntityFieldQuery();
        $query->entityCondition("entity_type", "node");
        $query->entityCondition("bundle", "campaigner_region");
        $result  = $query->execute();
        $nodes   = $nodes = node_load_multiple(array_keys($result['node']));
        $regions = array();
        if (!empty($nodes)) {
          foreach ($nodes as $region) {
            if (empty($region->field_campaigner_region_class) || $region->field_campaigner_region_class['und'][0]['tid'] === $regionClass) {
              $regionModel = RegionCollection::getRegionModelFromNode($region);
              $regions[$region->nid] = $regionModel;
            }
          }
        }

        $collection = new RegionCollection(array(), $storageStyle);
        $collection->addSeveral($regions);
        RegionCollection::$history[$regionClass] = $collection;
      }
      return RegionCollection::$history[$regionClass];

    }

    /**
     * Finds all regions from the database and produces a region collection with that data.
     *
     * @param int $storageStyle Either COLLECTION_RECORD_MODE or COLLECTION_RESOURCE_MODE
     *
     * @return RegionCollection The RegionCollection object generated
     */
    public static function fetchAll($storageStyle = COLLECTION_RECORD_MODE) {
      $geophp = geophp_load();
      if (!class_exists("RegionModel")) {
        require dirname(__FILE__) . '/../models/RegionModel.php';
      }
      $query = new EntityFieldQuery();
      $query->entityCondition("entity_type", "node");
      $query->entityCondition("bundle", "campaigner_region");
      $result  = $query->execute();
      $nodes   = $nodes = node_load_multiple(array_keys($result['node']));
      $regions = array();
      if (!empty($nodes)) {
        foreach ($nodes as $region) {
          $regionModel = RegionCollection::getRegionModelFromNode($region);
          $regions[$region->nid] = $regionModel;
        }

      }
      $collection = new RegionCollection(array(), $storageStyle);
      $collection->addSeveral($regions);

      return $collection;
    }

    /**
     * Helper function takes a node and loads all the data we need to make a RegionModel in.
     *
     * @param $region
     *
     * @return RegionModel
     */
    public static function getRegionModelFromNode($region){
      $regionData          = $region->field_campaigner_area_polygon['und'][0];
      $regionData['title'] = $region->title;
      $regionData['entity_id'] = $region->nid;
      $regionModel         = new RegionModel();
      foreach ($regionData as $key => $datum) {
        $regionModel->$key = $datum;
      }
      return $regionModel;
    }

    public static function getByEntityId($entityId) {
      $geophp = geophp_load();
      if (!class_exists("RegionModel")) {
        require dirname(__FILE__) . '/../models/RegionModel.php';
      }
      $region = node_load($entityId);
      $regionModel = RegionCollection::getRegionModelFromNode($region);
      $collection = new RegionCollection(array());
      $collection->addSeveral(array($regionModel));
      return $collection;
    }


  }