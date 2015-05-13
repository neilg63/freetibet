<?php
/**
 * @file
 * campaigner_upgrade.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function campaigner_upgrade_field_group_info() {
  $export = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_share_button|node|campaigner_petition|form';
  $field_group->group_name = 'group_share_button';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'campaigner_petition';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_widgets';
  $field_group->data = array(
    'label' => 'Share Button',
    'weight' => '54',
    'children' => array(
      0 => 'field_share_description',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => '',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_share_button|node|campaigner_petition|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_social_proof|node|campaigner_petition|form';
  $field_group->group_name = 'group_social_proof';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'campaigner_petition';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_widgets';
  $field_group->data = array(
    'label' => 'Social Proof',
    'weight' => '53',
    'children' => array(
      0 => 'field_social_proof_fields',
      1 => 'field_social_proof_title',
      2 => 'field_social_proof_refresh_rate',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => '',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_social_proof|node|campaigner_petition|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_thermometer|node|campaigner_petition|form';
  $field_group->group_name = 'group_thermometer';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'campaigner_petition';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_widgets';
  $field_group->data = array(
    'label' => 'Thermometer',
    'weight' => '52',
    'children' => array(
      0 => 'field_target',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => '',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_thermometer|node|campaigner_petition|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_widgets|node|campaigner_petition|form';
  $field_group->group_name = 'group_widgets';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'campaigner_petition';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Widgets',
    'weight' => '33',
    'children' => array(
      0 => 'group_social_proof',
      1 => 'group_thermometer',
      2 => 'group_share_button',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => '',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_widgets|node|campaigner_petition|form'] = $field_group;

  return $export;
}
