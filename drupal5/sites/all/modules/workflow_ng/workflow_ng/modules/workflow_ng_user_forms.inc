<?php

/**
 * @file Workflow-ng configuration forms for the user module 
 */

/*
 * Condition User: form to select roles to check for
 */
function workflow_ng_condition_user_hasrole_form($settings = array(), $argument_info) {
  $form['hasrole'] = _workflow_ng_roles($settings, FALSE);
  $form['hasrole']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Match against any or all of the selected roles'),
    '#options' => array('OR' => t('any'), 'AND' => t('all')),
    '#description' => t('If matching against all selected roles the user must have <em>all</em> the roles checked in the list above.'),
    '#default_value' => $settings['operation'],
  );
  return $form;
}

function workflow_ng_condition_user_hasrole_submit($form_id, $form_values) {
  return array(
    'roles' => array_filter(array_keys(array_filter($form_values['roles']))),
    'operation' => $form_values['operation'],
  );
}

/*
 * Action user: add user roles form
 */
function workflow_ng_action_user_addrole_form($settings = array(), $argument_info) {
  return _workflow_ng_roles($settings);
}

function workflow_ng_action_user_addrole_submit($form_id, $form_values) {
  $settings = array('roles' => array_filter(array_keys(array_filter($form_values['roles']))));
  return $settings;
}

/*
 * User Action: form for selecting roles to be deleted
 */
function workflow_ng_action_user_removerole_form($settings = array(), $argument_info) {
  return _workflow_ng_roles($settings);
}

function workflow_ng_action_user_removerole_submit($form_id, $form_values) {
  $settings = array('roles' => array_filter(array_keys(array_filter($form_values['roles']))));
  return $settings;
}

/*
 * Helper function to create role form
 */
function _workflow_ng_roles($settings = array(), $roles_only = TRUE) {
  $roles = user_roles($roles_only);
  if ($roles_only) {
    unset($roles[DRUPAL_AUTHENTICATED_RID]);
  }

  $form = array();
  $form['roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select role(s)'),
    '#options' => $roles,
    '#default_value' => $settings['roles'],
    '#required' => TRUE,
  );
  return $form;
}

function workflow_ng_action_load_user_form($settings = array(), $argument_info, $info) {
  $form = array();
  workflow_ng_ui_new_arguments_form($form, $info);
  $form['username'] = array(
    '#type' => 'textfield', 
    '#title' => t('User name'), 
    '#maxlength' => 60, 
    '#autocomplete_path' => 'user/autocomplete', 
    '#default_value' => $settings['username'], 
    '#weight' => -1, 
    '#description' => t('Name of the user to be loaded.')
  );
  $form['userid'] = array(
    '#type' => 'textfield', 
    '#title' => t('User id'),
    '#default_value' => $settings['userid'], 
    '#description' => t('Id of the user to be loaded.')
  );

  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

function workflow_ng_action_load_user_validate($form_id, $form_values) {
  if (!$form_values['username'] && !$form_values['userid']) {
    form_set_error('username', t('You have to enter the user name or the user id.'));
  }
}


function workflow_ng_action_load_user_submit($form_id, $form_values) {
  //returns the needed settings
  return workflow_ng_token_get_settings(array('username', 'userid'), $form_values);
}