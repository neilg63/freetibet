<?php

/*
* Evaluate, read and set attributes of source SVG XML
*/

class SvgimgData {
  
  /*
  * @var string
  */
  protected $svg;
  
  /*
  * @var array
  */
  protected $size = array('width' => 0, 'height' => 0);
  
  /*
  * @var SimpleXMLElement
  */
  protected $xml;
  
  /*
  * @var bool
  */
  protected $hasSize = false;
  
  /*
  * @var bool
  */
  protected $valid = false;
  
  function __construct($svg) {
    if (is_string($svg) && strpos($svg,'<svg') !== false && strpos($svg,'</svg>') > 10) {
      $this->parseSVG($svg);
      if ($this->xml instanceof SimpleXMLElement) {
        $this->parseSize();
        $this->valid = true;
      }
    }
  }
  
  /*
  * @param string $svg
  * @return self
  */
  function parseSVG($svg) {
    $xml_xml_decl_pos = strpos($svg, '<?xml');
    $has_xml_decl = (is_int($xml_xml_decl_pos) && $xml_xml_decl_pos >= 0 && $xml_xml_decl_pos < 10);
    // ImageMagick fails to parse files without xml declaration
    if (!$has_xml_decl) {
      $svg = '<?xml version="1.0" encoding="UTF-8" standalone="no"?>' . $svg;
    }
    $this->xml = simplexml_load_string($svg);
  
    $this->xml->registerXPathNamespace('svg', 'http://www.w3.org/2000/svg');
    $this->xml->registerXPathNamespace('xlink', 'http://www.w3.org/1999/xlink');
    $this->svg = $svg;
    return $this;
  }
  
  /*
  * @return self
  */
  function parseSize() {
    $attrs = $this->xml->attributes();
    $hasWidth = false;
    $hasHeight = false;
    if ( isset($attrs['width'])) {
      $this->size['width'] = (int) (string) $attrs['width'];
      $hasWidth = true;
    }
    if (isset($attrs['height'])) {
      $this->size['height'] = (int) (string) $attrs['height'];
      $hasHeight = true;
    }
    if ($hasWidth && $hasHeight) {
      $this->hasSize = true;
    }
    return $this;
  }
  
  /*
  * @return array
  */
  function size() {
    return $this->size;
  }
  
  /*
  * @return bool
  */
  function hasSize() {
    return $this->hasSize;
  }
  
  /*
  * @return SimpleXMLElement
  */
  function xml() {
    return $this->xml;
  }
  
  /*
  * @return string
  */
  function svg() {
    return $this->svg;
  }
  
  /*
  * @return bool
  */
  function valid() {
    return $this->valid;
  }
  
  /*
  * @param array $size
  * @return self
  */
  function setSize(array $size) {
    if (isset($size['width']) && isset($size['height']) && is_numeric($size['width']) && is_numeric($size['height'])) {
      if ($this->xml instanceof SimpleXMLElement) {
        $this->xml['width'] = (int) $size['width'];
        $this->xml['height'] = (int) $size['height'];
        $this->svg = $this->xml->asXml();
      }
      $this->size = $size;
    }
    return $this;
  }
  
}
