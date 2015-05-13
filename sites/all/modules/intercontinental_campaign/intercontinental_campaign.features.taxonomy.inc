<?php
/**
 * @file
 * intercontinental_campaign.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function intercontinental_campaign_taxonomy_default_vocabularies() {
  return array(
    'intercontinental' => array(
      'name' => 'Intercontinental',
      'machine_name' => 'intercontinental',
      'description' => 'Intercontinental campaign',
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
