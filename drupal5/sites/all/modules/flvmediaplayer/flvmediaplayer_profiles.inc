<?php

// $Id: flvmediaplayer_profiles.inc,v 1.1.2.8 2008/12/19 21:49:31 arthuregg Exp $

/* ************************************************************** */
/* PROFILE FUNCTIONS */
/* ************************************************************** */


/**
 * Creates the profile listing page
 * @param int $pid is a profile id
 * @return string
 */
function flvmediaplayer_admin_profiles ($pid = null) {
  // return a profile if it is passed in
  if ($pid) {
    // get config data to pass to the form
    $configuration = flvmediaplayer_profile_get_config($pid);
    drupal_set_title(t('Edit profile: !name', array('!name' => $configuration['profile_name'])));
    return drupal_get_form('flvmediaplayer_profile_edit_form', $configuration);
  }

  // set title for the profile lists
  drupal_set_title(t('Profile list'));
  // get the create form
  $output = drupal_get_form('flvmediaplayer_profile_create_form');
  // get existing profiles
  if ($configurations = flvmediaplayer_profile_get_configs()) {
    foreach($configurations as $pid => $name) {
      $rows[] = array($pid, l($name, 'admin/settings/flvmediaplayer/profiles/'. $pid));
    }
    $header = array(t('Profile Id'), t('Name (edit)'));
    $output .= theme('table', $header, $rows);
  }
  return $output;
}


/**
 * form for creating a new profile
 */
function flvmediaplayer_profile_create_form() {
  $form = array();
  $form['create'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create new profile'),
  );
  $form['create']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('Give your profile a distinct name. Use _ for spaces if you need them.'),
  );
  $form['create']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create'),
  );
  return $form;
}


/**
 * validates the name on the create
 */
function flvmediaplayer_profile_create_form_validate($form_id, $form_values) {
  if (strstr($form_values['name'], ' ')) {
    form_set_error('name', t('Please do not use spaces in the name.'));
  }
  // check to make sure this name is not in use
  if ($result = flvmediaplayer_profile_get_config($form_values['name'])) {
    print_r($result);
    form_set_error('name', t('This name is already in use, please choose another.'));
  }
}


/**
 * submits a new profile and returns user to the edit screen
 * @param string $from_id
 *   the form id being passed by drupal
 * @param array $form_values
 *   full drupal form array
 * @return string drupal goto
 */
function flvmediaplayer_profile_create_form_submit($form_id, $form_values) {
  // serialize the data from the form
  $data = serialize($form_values);
  $type = 'profile';
  $pid = db_next_id('flvmediaplayer_profiles');
  db_query('INSERT INTO {flvmediaplayer_profiles} (pid, type, name, config) VALUES (%d, "%s", "%s", "%s")', $pid, $type, $form_values['name'], $data);
  drupal_set_message(t('Created your profile !name', array('!name' => $form_values['name'])));
  return 'admin/settings/flvmediaplayer/profiles/'. $pid;
}


/**
 * handles the profile config save/delete
 * @param array $configuration
 *   the profile configuration array
 * @return array
 */
function flvmediaplayer_profile_edit_form($configuration) {
  $form = array();
  // create the fieldset
  $form['profiles'] = array(
    '#type' => 'fieldset',
    '#title' => $configuration['name'],
  );
  $form['profiles']['flvmp_name'] = array(
    '#type' => 'textfield',
    '#default_value' => $configuration['name'],
    '#description' => t('The name of your configuration'),
  );

  // grab the settings form
  $form['profiles']['data'] = flvmdiaplayer_get_player_form($configuration);

  // set the pid
  $form['profiles']['flvmp_pid'] = array(
    '#type' => 'hidden',
    '#value' => $configuration['pid'],
  );

  $form['profiles']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  $form['profiles']['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );
  return $form;
}


/**
 * handles the submit of an individual profile form
 * @param string $form_id
 * @param array $form_values
 *
 */
function flvmediaplayer_profile_edit_form_submit($form_id, $form_values) {
  switch ($form_values['op']) {
    case t('Delete'):
      db_query('DELETE FROM {flvmediaplayer_profiles} WHERE pid = %d', $form_values['pid']);
      drupal_set_message(t('Deleted your player profile'));
    break;

    case t('Save'):
      // remove all prefixes from the data we want
      foreach ($form_values as $name => $value) {
        if (strstr($name, 'flvmp_')) {
          // replace the prefixes from the incoming data
          $data[str_replace('flvmp_', '', $name)] = $value;
        }
      }
      // update the profile
      db_query('UPDATE {flvmediaplayer_profiles} SET name = "%s", config = "%s" WHERE pid = %d',
        $form_values['flvmp_name'], serialize($data), $data['pid']);
      // clear cached profile data associated with nodes
      cache_clear_all('flvmp_%'. $form_values['flvmp_name'], 'cache_flvmediaplayer', true);
      drupal_set_message(t('Cleared configuration cache'));
      drupal_set_message(t('Saved your player profile'));
    break;
  }
}