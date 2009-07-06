<?php

/**
 * @file Workflow-ng configuration forms for the path module 
 */

function workflow_ng_condition_url_has_alias_form($settings = array(), $argument_info) {
  $form['src'] = array(
    '#type' => 'textfield',
    '#title' => t('Existing system path'),
    '#default_value' => $settings['src'],
    '#maxlength' => 64,
    '#size' => 45,
    '#description' => t('Specify the existing path for which you want to check if an URL alias exists.'),
    '#field_prefix' => url(NULL, NULL, NULL, TRUE) . (variable_get('clean_url', 0) ? '' : '?q='),
    '#required' => TRUE,
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

function workflow_ng_condition_url_has_alias_submit($form_id, $form_values) {
  return workflow_ng_token_get_settings(array('src'), $form_values);
}

function workflow_ng_condition_alias_exists_form($settings = array(), $argument_info) {
  $form['dst'] = array(
    '#type' => 'textfield',
    '#title' => t('Path alias'),
    '#default_value' => $settings['dst'],
    '#maxlength' => 64,
    '#size' => 45,
    '#description' => t('Specify the path alias which you want to check if it already exists.'),
    '#field_prefix' => url(NULL, NULL, NULL, TRUE) . (variable_get('clean_url', 0) ? '' : '?q='),
    '#required' => TRUE,
  );
  $form['replace'] = array(
    '#type' => 'textfield',
    '#title' => t('Before checking, replace non ascii characters with'),
    '#default_value' => isset($settings['replace']) ? $settings['replace'] : '-',
    '#maxlength' => 5,
    '#size' => 5,
    '#description' => t('Leave this textfield empty to disable the replacement of non ascii characters.'),
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

function workflow_ng_condition_alias_exists_submit($form_id, $form_values) {
  return workflow_ng_token_get_settings(array('dst'), $form_values);
}

function workflow_ng_action_path_alias_form($settings = array(), $argument_info) {
  $form['src'] = array(
    '#type' => 'textfield',
    '#title' => t('Existing system path'),
    '#default_value' => $settings['src'],
    '#maxlength' => 64,
    '#size' => 45,
    '#description' => t('Specify the existing path you wish to alias. For example: node/28, forum/1, taxonomy/term/1+2.') .' '.
       t('Leave it empty to delete URL aliases pointing to the given path alias.'),
    '#field_prefix' => url(NULL, NULL, NULL, TRUE) . (variable_get('clean_url', 0) ? '' : '?q='),
  );
  $form['dst'] = array(
    '#type' => 'textfield',
    '#title' => t('Path alias'),
    '#default_value' => $settings['dst'],
    '#maxlength' => 64,
    '#size' => 45,
    '#description' => t('Specify an alternative path by which this data can be accessed. For example, type "about" when writing an about page. Use a relative path and don\'t add a trailing slash or the URL alias won\'t work.') .' '.
       t('Leave it empty to delete URL aliases pointing to the given system path.'),
    '#field_prefix' => url(NULL, NULL, NULL, TRUE) . (variable_get('clean_url', 0) ? '' : '?q='),
  );
  $form['replace'] = array(
    '#type' => 'textfield',
    '#title' => t('Replace non ascii characters with'),
    '#default_value' => isset($settings['replace']) ? $settings['replace'] : '-',
    '#maxlength' => 5,
    '#size' => 5,
    '#description' => t('Leave this textfield empty to disable the replacement of non ascii characters.'),
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

function workflow_ng_action_path_alias_validate($form_id, $form_values, $form) {
  if (empty($form_values['src']) && empty($form_values['dst'])) {
    form_set_error('src', t('You have to enter at least eiter an existing system path or a path alias.'));
  }
}

function workflow_ng_action_path_alias_submit($form_id, $form_values) {
  $settings = workflow_ng_token_get_settings(array('src', 'dst'), $form_values);
  return array('replace' => $form_values['replace']) + $settings;
}

function workflow_ng_action_node_path_alias_form($settings = array(), $argument_info) {
  $form = workflow_ng_action_path_alias_form($settings, $argument_info);

  //remove the unneeded src field
  unset($form['src']);

  return $form;
}

function workflow_ng_action_node_path_alias_submit($form_id, $form_values) {
  $settings = workflow_ng_token_get_settings(array('dst'), $form_values);
  return array('replace' => $form_values['replace']) + $settings;
}
