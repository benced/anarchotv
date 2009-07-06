<?php

// $Id: flvmediaplayer_menu.inc,v 1.1.2.6 2008/12/19 21:49:31 arthuregg Exp $

/**
 * Implementation of hook_menu().
 * called from flvmediaplayer_menu()
 *
 * @param boolean $may_cache
 * @return array
 */
function flvmediaplayer_menu_inc($may_cache) {
  $items = array();

  if ($may_cache) {
    // default page
    $items[] = array (
      'path' => 'admin/settings/flvmediaplayer',
      'title' => t('FLV Media Player'),
      'description' => t('FLV Media Player configuration.'),
      'callback' => 'drupal_get_form',
      'callback arguments' => array('flvmediaplayer_admin_settings'),
      'access' => user_access('administer flvmediaplayer'),
    );

    $items[] = array (
      'path' => 'admin/settings/flvmediaplayer/default',
      'title' => t('FLV Media Player settings'),
      'description' => t('Settings for the FLV Media Player.'),
      'callback' => 'drupal_get_form',
      'callback arguments' => array('flvmediaplayer_admin_settings'),
      'access' => user_access('administer flvmediaplayer'),
      'type' => MENU_DEFAULT_LOCAL_TASK,
    );

    // profiles
    $items[] = array (
      'path' => 'admin/settings/flvmediaplayer/profiles',
      'title' => t('Profiles'),
      'description' => t('FLV Media Player profiles.'),
      'callback' => 'flvmediaplayer_admin_profiles',
      'access' => user_access('administer flvmediaplayer'),
      'type' => MENU_LOCAL_TASK,
    );

    // get all the individual profiles
    if ($profiles = flvmediaplayer_profile_get_configs()) {
      foreach ($profiles as $pid => $name) {
        $items[] = array (
          'path' => 'admin/settings/flvmediaplayer/profiles/'. $pid,
          'title' => $name,
          'description' => t('FLV Media Player profile.'),
          'callback' => 'flvmediaplayer_admin_profiles',
          'callback arguments' => array($pid),
          'access' => user_access('administer flvmediaplayer'),
          'type' => MENU_LOCAL_TASK,
        );
      }
    }

    // switches install to expert mode
    $items[] = array(
      'path' => 'admin/settings/flvmediaplayer/expert',
      'callback' => 'flvmediaplayer_admin_expert',
      'access' => user_access('administer flvmediaplayer'),
    );

    // runs the helper form set
    $items[] = array(
      'path' => 'admin/settings/flvmediaplayer/wizard',
      'callback' => 'drupal_get_form',
      'callback arguments' => 'flvmediaplayer_admin_settings_wizard_form',
      'access' => user_access('administer flvmediaplayer'),
    );
  }

  // export pages
  $items[] = array(
    'path' => 'admin/settings/flvmediaplayer/export',
    'callback' => 'drupal_get_form',
    'callback arguments' => array('flvmediaplayer_export_form', arg(4)),
    'access' => user_access('administer flvmediaplayer'),
    'type' => MENU_LOCAL_TASK,
    'title' => t('Export'),
    'description' => t('Export flvmediaplayer profile'),
    'weight' => 9
  );

  // export pages
  $items[] = array(
    'path' => 'admin/settings/flvmediaplayer/import',
    'callback' => 'drupal_get_form',
    'callback arguments' => array('flvmediaplayer_import_form', arg(4)),
    'access' => user_access('administer flvmediaplayer'),
    'type' => MENU_LOCAL_TASK,
    'title' => t('Import'),
    'description' => t('Import flvmediaplayer profile'),
    'weight' => 9
  );

  // XML external configuration page. This is legacy, but kept
  // for backwards compatibility
  $items[] = array (
    'path' => 'flvmediaplayer',
    'callback' => 'flvmediaplayer_config_xml_page',
    'access' => user_access('content'),
    'type' => MENU_CALLBACK,
  );

  // XML external configuration page
  $items[] = array (
    'path' => 'node/'. arg(1) .'/flvmediaplayer',
    'callback' => 'flvmediaplayer_config_xml_page',
    'callback arguments' => array(arg(0), arg(1), arg(3)),
    'access' => user_access('content'),
    'type' => MENU_CALLBACK,
  );
  return $items;
}