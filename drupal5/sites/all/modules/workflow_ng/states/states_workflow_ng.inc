<?php

/*
 * Implementation of hook_condition_info()
 */
function states_condition_info() {
  if (states_get_machines()) {
    return array(
      'states_state_change' => array(
        '#label' => t('Machine state has changed'),
        '#arguments' => array(
          'entity_unchanged' => array('#entity' => array('user', 'node'), '#label' => t('The unchanged argument')),
          'entity' => array('#entity' => array('user', 'node'), '#label' => t('The updated argument')),
        ),
        '#description' => t('On update events, choose two compare two times the same argument, where one argument has to be the unchanged one and one the updated one.'),
        '#module' => 'States',
      ),
    );
  }
}

/*
 * Condition: Compare two states and detect a state change
 */
function states_state_change($entity_unchanged, $entity, $settings) {
  $state1 = states_entity_get_machine_state($entity_unchanged, $settings['machine_name']);
  $state2 = states_entity_get_machine_state($entity, $settings['machine_name']);
  return $state1 !== $state2;
}

function states_state_change_form($settings = array(), $argument_info) {
  $form = array();
  $form['machine_name'] = array(
    '#type' => 'select',
    '#title' => t('Choose a state machine'),
    '#options' => states_get_machines('labels'),
    '#default_value' => $settings['machine_name'],
    '#description' => t('Choose, which state machine of the choosen arguments should be used.'),
  );
  $form['argument_info'] = array('#type' => 'value', '#value' => $argument_info);
  return $form;
}

function states_state_change_validate($form_id, $form_values, $form) {
  $info = $form_values['argument_info'];
  if ($info[$form_values['map']['entity_unchanged']]['#entity'] != $info[$form_values['map']['entity']]['#entity']) {
    form_set_error('map][entity', t('You must choose two arguments of the same entity type.'));
  }
  else {
    $machine_info = states_get_machines('all', $form_values['machine_name']);
    if ($machine_info['#entity'] != $info[$form_values['map']['entity_unchanged']]['#entity']) {
      form_set_error('machine_name', t('The entity type of the choosen state machine doesn\'t match the entity type of your choosen arguments.'));
    }
  }
}

function states_state_change_submit($form_id, $form_values) {
  return array('machine_name' => $form_values['machine_name']);
}

/*
 * Implementation of hook_event_info()
 * Declares further events, that will be invoked, when a state change happens
 */
function states_event_info() {
  $events = array();
  foreach (states_get_machines() as $name => $info) {
    if ($info['#entity'] == 'node') {
      $events['machine_'. $name. '_change'] = array(
        '#label' => t('!label change', array('!label' => $info['#label'])),
        '#module' => 'States',
        '#arguments' => array(
          'node' => array('#entity' => 'node', '#label' => t('content')),
          'node_unchanged' => array('#entity' => 'node', '#label' => t('unchanged content')),
          'author' => array('#entity' => 'user', '#label' => t('content author'), '#handler' => 'workflow_ng_events_argument_node_author'),
          'author_unchanged' => array('#entity' => 'user', '#label' => t('unchanged content\'s author'), '#handler' => 'workflow_ng_events_argument_unchanged_node_author'),
          'user' => array('#entity' => 'user', '#label' => t('acting user'), '#handler' => 'workflow_ng_events_argument_global_user'),
        ),
      );
    }
    else {
      $events['machine_'. $name. '_change'] = array(
        '#label' => t('!label change', array('!label' => $info['#label'])),
        '#module' => 'States',
        '#arguments' => array(
          'account' => array('#entity' => 'user', '#label' => t('user')),
          'account_unchanged' => array('#entity' => 'user', '#label' => t('unchanged user')),
          'user' => array('#entity' => 'user', '#label' => t('acting user'), '#handler' => 'workflow_ng_events_argument_global_user'),
        ),
      );
    }
  }
  return $events;
}

/*
 * Implementation of hook_action_info()
 */
function states_action_info() {
  if (states_get_machines()) {
    return array(
      'states_generate_events' => array(
        '#label' => t('Generate state change events'),
        '#arguments' => array(
          'entity_unchanged' => array('#entity' => array('user', 'node'), '#label' => t('The unchanged argument')),
          'entity' => array('#entity' => array('user', 'node'), '#label' => t('The updated argument')),
        ),
        '#fixed' => TRUE,
      ),
      'states_action_machine_set_state' => array(
        '#label' => t('Set a machine state'),
        '#arguments' => array(
          'entity' => array('#entity' => array('user', 'node'), '#label' => t('Entity')),
        ),
        '#module' => 'States',
      ),
    );
  }
}

/*
 * Detect *all* state changes and generate events
 */
function states_generate_events($entity_unchanged, $entity, $settings) {
  if (entity_is_node($entity)) {
    $machines = states_get_machines('node');
    if (isset($machines[$entity->type])) {
      states_check_change_and_generate_event($machines[$entity->type], $settings, $entity, $entity_unchanged);
    }
  }
  //entity is a user
  else if ($machines = states_user_has_machines($entity)) {
    states_check_change_and_generate_event($machines, $settings, $entity, $entity_unchanged);
  }
}


/*
 * Checks the states of both entites for all machines and invokes the event if necessary
 */
function states_check_change_and_generate_event($machines, $settings, $entity, $entity_unchanged) {
  if (isset($settings['insert'])) {
    foreach ($machines as $name => $info) {
      $state = states_entity_get_machine_state($entity, $name);
      //on insert, there is a state change if the init state hasn't been applied
      if ($state !== $info['#init_state']) {
        workflow_ng_invoke_event('machine_'. $name. '_change', $entity, $entity);
      }
    }
  }
  else {
    foreach ($machines as $name => $info) {
      $state1 = states_entity_get_machine_state($entity_unchanged, $name);
   	 	$state2 = states_entity_get_machine_state($entity, $name);
      if ($state1 !== $state2) {
        workflow_ng_invoke_event('machine_'. $name. '_change', $entity, $entity_unchanged);
      }
    }
  }
}

/*
 * Action: Sets a new state for a machine 
 */
function states_action_machine_set_state($entity, $settings, $arguments, &$log) {
  $value = workflow_ng_token_replace($settings['value'], $settings['value_args'], $arguments, $log); 
  states_entity_set_machine_state($entity, $settings['machine_name'], $value ? $value : NULL);

  return array('entity' => $entity);
}

function states_action_machine_set_state_form($settings, $argument_info) {
  $form['machine_name'] = array(
    '#type' => 'select',
    '#title' => t('Choose a state machine'),
    '#options' => states_get_machines('labels'),
    '#default_value' => $settings['machine_name'],
    '#description' => t('Choose, which state machine of the choosen arguments should be used.'),
  );
  $form['value'] = array(
    '#type' => 'textarea',
    '#title' => t('Value'),
    '#default_value' => $settings['value'],
    '#description' => t('The value to set. Be sure to only use valid machine state values.'),
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

function states_action_machine_set_state_validate($form_id, $form_values, $form) {
  $info = $form_values['arguments'];
  $machine_info = states_get_machines('all', $form_values['machine_name']);
  if ($machine_info['#entity'] != $info[$form_values['map']['entity']]['#entity']) {
    form_set_error('machine_name', t('The choosen machine doesn\'t track the state for entities of this type.'));
  }
}

function states_action_machine_set_state_submit($form_id, $form_values) {
  $settings = array('machine_name' => $form_values['machine_name']);
  foreach (array('value') as $key) {
    $settings[$key] = $form_values[$key];
    $settings[$key .'_args'] = workflow_ng_token_get_used_arguments($form_values[$key], $form_values['arguments']);
  }
  return $settings;
}


/*
 * Implementation of hook_configuration()
 * Add some fixed configurations, that generate new events for state changes
 */
function states_configuration() {
  if (states_get_machines()) {
    $configurations = array();
    $configurations['states1'] = array(
      '#label' => t('Generate state change events for nodes'),
      '#event' => 'node_update',
      '#module' => 'states',
      '#fixed' => TRUE,
    );
    $configurations['states1']['action'] = workflow_ng_use_action('states_generate_events', array(
      '#argument map' => array('node_unchanged' => 'entity_unchanged', 'node' => 'entity'),
    ));
    $configurations['states12'] = array(
      '#label' => t('Generate state change events for new nodes'),
      '#event' => 'node_insert',
      '#module' => 'states',
      '#fixed' => TRUE,
    );
    $configurations['states12']['action'] = workflow_ng_use_action('states_generate_events', array(
      '#argument map' => array('node_unchanged' => 'entity_unchanged', 'node' => 'entity'),
      '#settings' => array('insert' => TRUE),
    ));
    $configurations['states2'] = array(
      '#label' => t('Generate state change events for new users'),
      '#event' => 'user_insert',
      '#module' => 'states',
      '#fixed' => TRUE,
    );
    $configurations['states2']['action'] = workflow_ng_use_action('states_generate_events', array(
      '#argument map' => array('account' => 'entity_unchanged', 'account' => 'entity'),
      '#settings' => array('insert' => TRUE),
    ));
    $configurations['states22'] = array(
      '#label' => t('Generate state change events for users'),
      '#event' => 'user_update',
      '#module' => 'states',
      '#fixed' => TRUE,
    );
    $configurations['states22']['action'] = workflow_ng_use_action('states_generate_events', array(
      '#argument map' => array('account_unchanged' => 'entity_unchanged', 'account' => 'entity'),
    ));
    return $configurations;
  }
}
