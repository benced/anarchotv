<?php
// $Id: states_views.inc,v 1.1.2.1.2.1 2008/03/05 10:02:36 fago Exp $

 /*
 * @file
 * Provides views integration for the states module
  */

/**
 * Implementation of hook_views_tables
 */
function states_views_tables() {
  $tables['node_state'] = array(
    'name' => 'node_state',
    'join' => array(
      'left' => array(
        'table' => 'node',
        'field' => 'vid'
      ),
      'right' => array(
        'field' => 'vid'
      ),
    ),
    'fields' => array(
      'machine' => array(
        'name' => t('States: State machine name'),
        'handler' => array(
          '' => t('machine readable name'),
          'states_views_field_machine' => t('human readable name'),
        ),
        'sortable' => true,
        'help' => t('This field displays the state machine\'s name.'),
      ),
      'state' => array(
        'name' => t('States: State machine state'),
        'sortable' => true,
        'help' => t('This field displays the state machine\'s state.'),
      ),
    ),
    'sorts' => array(
      'machine' => array(
        'name' => t('States: State machine name'),
        'sortable' => true,
        'help' => t('Sort after the state machine\'s name.'),
      ),
      'state' => array(
        'name' => t('States: State machine state'),
        'sortable' => true,
        'help' => t('Sort after the state machine\'s state.'),
      ),
    ),
    'filters' => array(
      'machine' => array(
        'name' => t('States: State machine'),
        'operator' => 'views_handler_operator_or',
        'value' => array(
          '#title' => t('Machine'),
          '#type' => 'select',
          '#options' => 'states_views_machines_list',
          '#multiple' => TRUE,
        ),
        'value-type' => 'array',
        'help' => t('This allows you to filter by a machine name.'),
      ),
    ),
  );
  foreach (states_get_machines('all') as $name => $info) {
    $tables['node_state_'. $name] = array(
      'name' => 'node_state',
      'join' => array(
        'left' => array(
         'table' => 'node',
          'field' => 'vid'
        ),
        'right' => array(
          'field' => 'vid'
        ),
        'extra' => array('machine' => $name),
      ),
      'filters' => array(
        'state' => array(
          'field' => 'state',
          'name' => t('States: !name', array('!name' => $info['#label'])),
          'operator' => 'views_handler_operator_or',
          'help' => t('This allows you to filter by a machine state.'),
        ),
      ),
    );
    if (is_array($info['#states'])) {
      $tables['node_state_'. $name]['filters']['state'] += array(
        'value' => array(
          '#title' => t('State'),
          '#type' => 'select',
          '#options' => drupal_map_assoc($info['#states']),
          '#multiple' => TRUE,
        ),
        'value-type' => 'array',
      );
    }
    else {
      $tables['node_state_'. $name]['filters']['state']['value'] = 'string';
    }
  }
  return $tables;
}


function states_views_machines_list() {
  return states_get_machines('labels');
}

function states_views_field_machine($fieldinfo, $fielddata, $value, $data) {
  $machines = states_get_machines('labels');
  return $machines[$value];
}
