<?php
/**
 * @file
 * campaigner_features.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function campaigner_features_user_default_permissions() {
  $permissions = array();

  // Exported permission: access campaigner content.
  $permissions['access campaigner content'] = array(
    'name' => 'access campaigner content',
    'roles' => array(),
    'module' => 'campaigner',
  );

  return $permissions;
}
