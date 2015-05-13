<?php
/**
 * @file
 * campaigner_features.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function campaigner_features_taxonomy_default_vocabularies() {
  return array(
    'campaigner_location_method' => array(
      'name' => 'Location method',
      'machine_name' => 'campaigner_location_method',
      'description' => 'The method by which the user\'s location is determined',
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
