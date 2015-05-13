<?php
/**
 * @file
 * campaigner_country_regions.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function campaigner_country_regions_taxonomy_default_vocabularies() {
  return array(
    'campaigner_region_class' => array(
      'name' => 'Region class',
      'machine_name' => 'campaigner_region_class',
      'description' => 'Grouping for regions',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
      'rdf_mapping' => array(
        'rdftype' => array(
          0 => 'skos:ConceptScheme',
        ),
        'name' => array(
          'predicates' => array(
            0 => 'dc:title',
          ),
        ),
        'description' => array(
          'predicates' => array(
            0 => 'rdfs:comment',
          ),
        ),
      ),
    ),
  );
}
