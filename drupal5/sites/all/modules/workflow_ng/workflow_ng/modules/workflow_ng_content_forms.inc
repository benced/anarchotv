<?php

/**
 * @file workflow-ng forms for CCK
 */

function workflow_ng_content_get_fields_by_type($type) {
  $fields = array();
  foreach (content_fields() as $field) {
    if ($field['type'] == $type) {
      $fields[$field['field_name']] = $field['field_name'];
    }
  }
  return $fields;
}

function workflow_ng_action_load_referenced_user_form($settings = array(), $argument_info) {
  $form = array();
  $form['field'] = array(
    '#type' => 'select',
    '#title' => t('Field'),
    '#default_value' => $settings['field'],
    '#options' => workflow_ng_content_get_fields_by_type('userreference'),
    '#description' => t('If the field has multiple values, only the first user will be loaded.'),
    '#required' => TRUE,
  );
  return $form;
}

function workflow_ng_action_load_referenced_user_submit($form_id, $form_values) {
  //returns the needed settings
  return array('field' => $form_values['field']);
}

function workflow_ng_action_load_referenced_node_form($settings = array(), $argument_info) {
  $form = array();
  $form['field'] = array(
    '#type' => 'select',
    '#title' => t('Field'),
    '#default_value' => $settings['field'],
    '#options' => workflow_ng_content_get_fields_by_type('nodereference'),
    '#description' => t('If the field has multiple values, only the first node will be loaded.'),
    '#required' => TRUE,
  );
  return $form;
}

function workflow_ng_action_load_referenced_node_submit($form_id, $form_values) {
  //returns the needed settings
  return array('field' => $form_values['field']);
}
