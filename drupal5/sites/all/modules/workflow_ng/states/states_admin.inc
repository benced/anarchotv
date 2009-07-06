<?php
// $Id: states_admin.inc,v 1.1.2.1.2.5 2008/03/08 11:01:40 fago Exp $

/**
 * @file
 * Provides an admin interface to list/create/edit state machines.
 */

// Entity types.
define('STATES_ADMIN_ENTITY_NODE', 0);
define('STATES_ADMIN_ENTITY_USER', 1);

// State creation / editing steps
define('STATES_ADMIN_STEP_BASIC'  , 1); // label textbox, entity type dropdown, allow any state checkbox
define('STATES_ADMIN_STEP_STATES' , 2); // states list
define('STATES_ADMIN_STEP_OPTIONS', 3); // initial state downdown/textbox, allow any entity checkbox
define('STATES_ADMIN_STEP_FILTERS', 4); // node type / user role checkboxes
define('STATES_ADMIN_STEP_REVIEW' , 5); // confirmation of previous steps

// Admin interface path
define('STATES_ADMIN_UI_PATH', 'admin/workflow-ng/state-machines');

/**
 * Implementation of hook_states
 * Lists the states created through the admin interface
 */
function states_states() {
  $machines = array();
  $result = db_query('SELECT * FROM {states_custom}');
  while ($row = db_fetch_object($result)) {
    $state = array(
      '#label' => $row->label,
      '#states' => unserialize($row->states),
      'can_edit' => TRUE,
      'custom_sid' => $row->sid,
    );
    if ($row->init_state != '') {
      $state['#init_state'] = $row->init_state;
    }
    if ($row->entity_type == STATES_ADMIN_ENTITY_NODE) {
      $state['#entity'] = 'node';
      if ($row->entity_filter !== '') {
        $state['#types'] = unserialize($row->entity_filter);
      }
    }
    else if ($row->entity_type == STATES_ADMIN_ENTITY_USER) {
      $state['#entity'] = 'user';
      if ($row->entity_filter !== '') {
        $state['#roles'] = unserialize($row->entity_filter);
      }
    }
    $machines['custom_state_'. $row->sid] = $state;
  }
  return $machines;
}

/**
 * Implementation of hook_perm().
 */
function states_perm() {
  return array('administer custom state machines');
}

/**
 * Implementation of hook_menu().
 */
function states_menu($may_cache) {
  $items = array();
  if ($may_cache) {
    $items[] = array(
      'path' => STATES_ADMIN_UI_PATH,
      'title' => t('Custom state machines'),
      'description' => t('View, edit, create and delete custom state machines.'),
      'callback' => 'drupal_get_form',
      'callback arguments' => array('states_admin_overview', NULL),
      'access' => user_access('administer custom state machines'),
      'type' => MENU_NORMAL_ITEM,
      'weight' => -1, 
    );
    $items[] = array(
      'path' => STATES_ADMIN_UI_PATH .'/0/list',
      'title' => t('Overview'),
      'weight' => -5,
      'type' => MENU_DEFAULT_LOCAL_TASK,
    );
    $items[] = array(
      'path' => STATES_ADMIN_UI_PATH .'/0/add',
      'title' => t('Create a new state machine'),
      'type' => MENU_LOCAL_TASK,
      'callback' => 'drupal_get_form',
      'callback arguments' => array('states_admin_create'),
      'access' => user_access('administer custom state machines'),
    );
  }
  else if (arg(0) == 'admin' && arg(1) == 'workflow-ng' && arg(2) == 'state-machines') {
    $machines = states_get_machines();
    if (isset($machines[arg(3)]) && $machines[arg(3)]['can_edit'] == TRUE) {
      $label = $machines[arg(3)]['#label'];
      $items[] = array(
        //this item lets the menu system create a nicer breadcrumb
        'path' => STATES_ADMIN_UI_PATH .'/'. arg(3),
        'title' => check_plain($label),
        'type' => MENU_CALLBACK,
      );
      $items[] = array(
        'path' => STATES_ADMIN_UI_PATH .'/'. arg(3) .'/edit',
        'title' => t('Edit'),
        'type' => MENU_CALLBACK,
        'callback' => 'drupal_get_form',
        'callback arguments' => array('states_admin_edit', arg(3)),
        'access' => user_access('administer custom state machines'),
      );
      $items[] = array(
        'path' => STATES_ADMIN_UI_PATH .'/'. arg(3) .'/delete',
        'title' => t('Delete'),
        'type' => MENU_CALLBACK,
        'callback' => 'drupal_get_form',
        'callback arguments' => array('states_admin_delete', arg(3)),
        'access' => user_access('administer custom state machines'),
      );
    }
  }
  return $items;
}

function states_admin_overview() {
  drupal_set_title(t('State machines overview'));
  states_clear_machine_cache();

  $form['nodes_header'] = array('#value' => '<h3>'. t('Content state machines') .'</h3>');
  $form['nodes'] = states_admin_overview_table('node');
  $form['users_header'] = array('#value' => '<h3>'. t('User state machines') .'</h3>');
  $form['users'] = states_admin_overview_table('user');
  $form['#redirect'] = FALSE;
  return $form;
}

function states_admin_overview_table($entity) {
  $machines = states_get_machines();

  $header = array(t('Label'), t('States'), t('Initial state'), t('Attribute name'), $entity=='node' ? t('Content types') : t('User roles'), t('Operations'));

  $rows = array();
  foreach ($machines as $name => $machine) {
    if ($machine['#entity'] == $entity && isset($machine['can_edit']) && $machine['can_edit'] == TRUE) {
      $states = t('(any)');
      if ($machine['#states'] != '*') {
        $states = theme('item_list', $machine['#states']);
      }
      $filter = t('(any)');
      if ($entity == 'node' && isset($machine['#types'])) {
        $filter = array();
        $node_names = node_get_types('names');
        foreach ($machine['#types'] as $value) {
          $filter[] = $node_names[$value];
        }
        if (count($filter) > 0) {
          $filter = theme('item_list', $filter);
        }
        else {
          $filter = t('(none)');
        }
      }
      else if ($entity == 'user' && isset($machine['#roles'])) {
        $filter = array();
        $role_names = user_roles();
        foreach ($machine['#roles'] as $value) {
          $filter[] = $role_names[$value];
        }
        if (count($filter) > 0) {
          $filter = theme('item_list', $filter);
        }
        else {
          $filter = t('(none)');
        }
      }
      $ops = array();
      $path = STATES_ADMIN_UI_PATH .'/'. $name;
      $ops[] = l(t('edit'), $path .'/edit');
      $ops[] = l(t('delete'), $path .'/delete');
      $rows[] = array(
        check_plain($machine['#label']),
        $states,
        isset($machine['#init_state']) ? check_plain($machine['#init_state']) : t('(none)'),
        check_plain(isset($machine['#attribute_name']) ? isset($machine['#attribute_name']) : $name),
        $filter,
        implode(' ', $ops),
      );
    }
  }

  if (count($rows)) {
    return array('#value' => theme('table', $header, $rows, array('class' => 'state-configurations')));
  }

  return array('#value' => t('None.'));
}

function states_admin_trim_value(&$value) {
  $value = trim($value);
}

/**
 * Helper function for states_admin_create. Addresses a bug in the D5 Forms API;
 * multistep forms do not honour default values for checkboxes.
 */
function states_admin_checkboxes_multistep_fix($elements) {
  foreach (element_children($elements) as $key) {
    if ($elements[$key]['#type'] == 'checkbox' && isset($elements[$key]['#default_value']) && $elements[$key]['#default_value'] != 0) {
      $elements[$key]['#attributes']['checked'] = 'checked';
    }
  }
  return $elements;
}

function states_admin_create($form_values = NULL) {
  if (!isset($form_values)) {
    $form_values = array(
      'step' => 0,
      'saved' => serialize(array()),
      // Some decent default values for a new state machine:
      'label' => '',
      'entity' => STATES_ADMIN_ENTITY_NODE,
      'any_state' => 0,
      'state_list' => array(),
      'initial_state' => 0,
      'any_entity' => 0,
      'filters' => array(),
    );
  }
  return states_admin_form($form_values);
}

/**
 * The custom state creation / editing multistep form
 */
function states_admin_form($form_values) {
  // Merge the saved values into the form_values
  $saved = unserialize($form_values['saved']);
  unset($form_values['saved']); // remove the saved values from the form values so they aren't in both parts of the merge
  if (isset($form_values['unset'])) {
    // Changing the any_state option will invalidate the initial_state option and the state_list (this has to be done before the unset; silly FAPI checkbox)
    if ($form_values['unset'] == 'any_state' && (($saved['any_state'] == 1) != ($form_values['any_state'] == 1))) {
      if ($form_values['any_state'] == 1) {
        $form_values['state_list'] = '*';
        $form_values['initial_state'] = '';
      }
      else {
        $form_values['state_list'] = array();
        $form_values['initial_state'] = 0;
      }
    }
    // To allow checkboxes to be unticked, they have to be zeroed in the saved values, as they only appear in the new values if they are ticked
    // Similarly, checkbox groups only appear if one or more was ticked. If they were all unticked, they don't appear in new values.
    $saved_unset = $form_values['unset'];
    unset($form_values['unset']);
    if (is_array($saved[$saved_unset])) {
      $saved[$saved_unset] = array();
    }
    else {
      $saved[$saved_unset] = 0;
    }
  }
  $form_values = array_merge($saved, $form_values);

  // Do any processing of the results from the previous step
  $from_step = $form_values['step'];

  // The state list is changed from a newline delimited string into an array. It is the only value not left as-is until the
  // form submission. This is because it is much more useful to us as an array.
  if ($from_step == STATES_ADMIN_STEP_STATES) {
    $states = explode("\n", $form_values['state_list']);
    array_walk($states, 'states_admin_trim_value');
    $form_values['state_list'] = $states;
  }

  // Changing the entity type option will invalidate the filters option (as node filters and role filters are very different)
  if (($saved['entity'] == STATES_ADMIN_ENTITY_NODE) != ($form_values['entity'] == STATES_ADMIN_ENTITY_NODE)) {
    $form_values['filters'] = array();
  }

  // Move to the next step or previous step
  if ($form_values['op'] == t('Back')) {
    $step = $from_step - 1;
    if ($step == STATES_ADMIN_STEP_FILTERS && $form_values['any_entity'] == 1) {
      // The filters step is skipped if any entity is allowed
      $step = STATES_ADMIN_STEP_OPTIONS;
    }
    if ($step == STATES_ADMIN_STEP_STATES && $form_values['any_state'] == 1) {
      // The state list step is skipped if any state is allowed
      $step = STATES_ADMIN_STEP_BASIC;
    }
  }
  else {
    $step = $from_step + 1;
    if ($step == STATES_ADMIN_STEP_STATES && $form_values['any_state'] == 1) {
      // The state list step is skipped if any state is allowed
      $step = STATES_ADMIN_STEP_OPTIONS;
    }
    if ($step == STATES_ADMIN_STEP_FILTERS && $form_values['any_entity'] == 1) {
      // The filters step is skipped if any entity is allowed
      $step = STATES_ADMIN_STEP_REVIEW;
    }
  }
  
  // Store persistent variables
  $form_values['step'] = $step;
  $form['saved'] = array(
    '#type' => 'hidden',
    '#value' => serialize($form_values),
  );

  // Make the new page of the wizard
  switch ($step) {
    case STATES_ADMIN_STEP_BASIC:
      $form['label'] = array(
        '#type' => 'textfield',
        '#title' => t('Label'),
        '#maxlength' => 127,
        '#description' => t('Choose an appropriate label for the new state machine.'),
        '#required' => TRUE,
        '#default_value' => $form_values['label'],
      );
      $form['entity'] = array(
        '#type' => 'select',
        '#title' => t('Entity'),
        '#options' => array(
          STATES_ADMIN_ENTITY_NODE => t('Content'),
          STATES_ADMIN_ENTITY_USER => t('User'),
        ),
        '#description' => t('Select the type of entity you want the state machine to track.'),
        '#required' => TRUE,
        '#default_value' => $form_values['entity'],
      );
      $form['any_state'] = array(
        '#type' => 'checkbox',
        '#title' => t('Allow any state'),
        '#default_value' => $form_values['any_state'],
        '#description' => t('Allows the state machine to have any state, rather than requiring one from a defined pool.'),
      );
      $form['unset'] = array(
        '#type' => 'hidden',
        '#value' => 'any_state',
      );
      $form = states_admin_checkboxes_multistep_fix($form); // Workaround FAPI checkbox multistep bug
      break;

    case STATES_ADMIN_STEP_STATES:
      $form['state_list'] = array(
        '#type' => 'textarea',
        '#title' => t('Machine states'),
        '#description' => t('List the possible states of the state machine, one per line.'),
        '#required' => TRUE,
        '#default_value' => implode("\n", $form_values['state_list']),
      );
      break;

    case STATES_ADMIN_STEP_OPTIONS:
      if ($form_values['any_state'] == 1) {
        $form['initial_state'] = array(
          '#title' => t('Initial state'),
          '#type' => 'textfield',
          '#maxlength' => 127,
          '#description' => t('Choose a initial state for entities, or leave blank to leave entities with no state.'),
          '#default_value' => $form_values['initial_state'],
        );
      }
      else {
        $states = $form_values['state_list'];
        array_unshift($states, t('(none)'));
        $form['initial_state'] = array(
          '#title' => t('Initial state'),
          '#type' => 'select',
          '#options' => $states,
          '#description' => t('Choose a initial state for entities.'),
          '#required' => TRUE,
          '#default_value' => $form_values['initial_state'],
        );
      }
      $form['any_entity'] = array(
        '#type' => 'checkbox',
        '#default_value' => $form_values['any_entity'],
      );
      $form['unset'] = array(
        '#type' => 'hidden',
        '#value' => 'any_entity',
      );
      if ($form_values['entity'] == STATES_ADMIN_ENTITY_NODE) {
        $form['any_entity']['#title'] = t('Allow any content type');
        $form['any_entity']['#description'] = t('Allows the state machine to work on all content types, rather than ones from a defined pool.');
      }
      else {
        $form['any_entity']['#title'] = t('Allow any user role');
        $form['any_entity']['#description'] = t('Allows the state machine to work on all user roles, rather than ones from a defined pool.');
      }
      $form = states_admin_checkboxes_multistep_fix($form); // Workaround FAPI checkbox multistep bug
      break;

    case STATES_ADMIN_STEP_FILTERS:
      $form['filters'] = array(
        '#type' => 'checkboxes',
        '#process' => array('expand_checkboxes' => array(), 'states_admin_checkboxes_multistep_fix' => array()), // Workaround FAPI checkbox multistep bug
        '#default_value' => $form_values['filters'],
      );
      if ($form_values['entity'] == STATES_ADMIN_ENTITY_NODE) {
        $form['filters']['#title'] = t('Permitted content types');
        $form['filters']['#description'] = t('The state machine is only valid for the checked content types.');
        $form['filters']['#options'] = node_get_types('names');
      }
      else {
        $form['filters']['#title'] = t('Permitted user roles');
        $form['filters']['#description'] = t('The state machine is only valid for the checked user roles.');
        $form['filters']['#options'] = user_roles();
      }
      $form['unset'] = array(
        '#type' => 'hidden',
        '#value' => 'filters',
      );
      break;

    case STATES_ADMIN_STEP_REVIEW:
      $form['review_label'] = array(
        '#prefix' => '<h3>'. t('You are about to create the following state machine:') .'</h3>',
        '#type' => 'item',
        '#title' => t('Label'),
        '#value' => $form_values['label'],
      );
      if (isset($form_values['sid_to_update'])) {
        $form['review_label']['#prefix'] = '<h3>'. t('You are about to update the state machine to the following:') .'</h3>';
      }

      $form['review_entity'] = array(
        '#type' => 'item',
        '#title' => t('Entity'),
        '#value' => $form_values['entity'] == STATES_ADMIN_ENTITY_NODE ? t('Content') : t('User'),
      );

      $form['review_states'] = array(
        '#type' => 'item',
        '#title' => t('States'),
        '#value' => t('(any)'),
      );
      if ($form_values['any_state'] != 1) {
        $form['review_states']['#value'] = theme('item_list', $form_values['state_list']);
      }

      $form['review_initial'] = array(
        '#type' => 'item',
        '#title' => t('Initial state'),
        '#value' => t('(none)'),
      );
      if ($form_values['any_state'] == 1) {
        if ($form_values['initial_state'] != '') {
          $form['review_initial']['#value'] = $form_values['initial_state'];
        }
      }
      else {
        if ($form_values['initial_state'] > 0) {
          $states = $form_values['state_list'];
          $form['review_initial']['#value'] = $states[$form_values['initial_state'] - 1];
        }
      }

      $form['review_filter'] = array(
        '#type' => 'item',
        '#value' => t('(any)'),
      );
      if ($form_values['entity'] == STATES_ADMIN_ENTITY_NODE) {
        $form['review_filter']['#title'] = t('Content types');
        if ($form_values['any_entity'] != 1) {
          $node_list = array_filter($form_values['filters']);
          foreach ($node_list as $key => $value) {
            $node_list[$key] = node_get_types('name', $value);
          }
          if (count($node_list) > 0) {
            $form['review_filter']['#value'] = theme('item_list', $node_list);
          }
          else {
            $form['review_filter']['#value'] = t('(none)');
          }
        }
      }
      else {
        $form['review_filter']['#title'] = t('User roles');
        if ($form_values['any_entity'] != 1) {
          $role_names = user_roles();
          $role_list = array_filter($form_values['filters']);
          foreach ($role_list as $key => $value) {
            $role_list[$key] = $role_names[$value];
          }
          if (count($role_list) > 0) {
            $form['review_filter']['#value'] = theme('item_list', $role_list);
          }
          else {
            $form['review_filter']['#value'] = t('(none)');
          }
        }
      }
      break;
  }
  
  // Put on the form elements common to every page (the next and back buttons)
  if ($step != 1) {
    $form['back'] = array(
      '#type' => 'submit',
      '#value' => t('Back'),
    );
  }
  if ($step == STATES_ADMIN_STEP_REVIEW) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => isset($form_values['sid_to_update']) ? t('Update') : t('Create'),
    );
  }
  else {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Next'),
    );
  }
  
  // Set the form settings and finish
  $form['#redirect'] = FALSE;
  $form['#multistep'] = TRUE;
  $form['#validate'] = array('states_admin_form_validate' => array());
  $form['#submit'] = array('states_admin_form_submit' => array());
  return $form;
}

/**
 * The custom state creation / editing multistep form validation handler
 */
function states_admin_form_validate($form_id, $form_values) {
  // Merge the saved variables into the new ones
  $saved = unserialize($form_values['saved']);
  unset($form_values['saved']);
  $form_values = array_merge($saved, $form_values);

  $step = $form_values['step'];
  switch ($step) {
    case STATES_ADMIN_STEP_STATES:
      $states = array();
      if (isset($form_values['state_list'])) {
        $states = explode("\n", $form_values['state_list']);
      }
      array_walk($states, 'states_admin_trim_value');
      foreach ($states as $state) {
        if (strlen($state) > 127) {
          form_set_error('state_list', t('The state "!state" exceeds the maximum length of 127.', array('!state' => $state)));
        }
      }
      $states_count = array_count_values($states);
      foreach ($states_count as $state => $count) {
        if ($count != 1) {
          form_set_error('state_list', t('The state "!state" appears more than once.', array('!state' => $state)));
        }
      }
      break;

    case STATES_ADMIN_STEP_FILTERS:
      $filters = array();
      if (isset($form_values['filters'])) {
        $filters = $form_values['filters'];
      }
      $filters_count = array_count_values($filters);
      if (count($filters) == $filters_count[0]) {
        drupal_set_message(t('You left all boxes unticked. While this is allowed, was it really intended?'));
      }
      break;
  }
}

/**
 * The custom state creation / editing multistep form submission handler
 */
function states_admin_form_submit($form_id, $form_values) {
  // Merge the saved variables into the new ones
  $saved = unserialize($form_values['saved']);
  unset($form_values['saved']);
  $form_values = array_merge($saved, $form_values);
  
  $step = $form_values['step'];
  if ($step == STATES_ADMIN_STEP_REVIEW && $form_values['op'] != t('Back')) {
    $label = $form_values['label'];
    $entity_type = $form_values['entity'];

    $entity_filter = '';
    if ($form_values['any_entity'] != 1) {
      $entity_filter = serialize(array_keys($form_values['filters']));
    }

    $states = '*';
    if ($form_values['any_state'] != 1) {
      $states = $form_values['state_list'];
    }

    $init_state = '';
    if ($form_values['any_state'] == 1) {
      $init_state = $form_values['initial_state'];
    }
    else {
      if ($form_values['initial_state'] > 0) {
        $init_state = $states[$form_values['initial_state'] - 1];
      }
    }

    if (isset($form_values['sid_to_update'])) {
      db_query("UPDATE {states_custom} SET label = '%s', entity_type = %d, entity_filter = '%s', states = '%s', init_state = '%s' WHERE sid = %d",
        $label, $entity_type, $entity_filter, serialize($states), $init_state, $form_values['sid_to_update']);
      drupal_set_message(t('State machine updated'));
    }
    else {
      db_query("INSERT INTO {states_custom} (label, entity_type, entity_filter, states, init_state) VALUES ('%s', %d, '%s', '%s', '%s')",
        $label, $entity_type, $entity_filter, serialize($states), $init_state);
      drupal_set_message(t('New state machine created'));
    }

    drupal_goto(STATES_ADMIN_UI_PATH);
  }
}

function states_admin_edit($machine_name, $form_values = NULL) {
  if (!isset($form_values)) {
    $machines = states_get_machines();
    $machine = $machines[$machine_name];

    // Transform the state machine definition into form values
    $form_values = array(
      'step' => 0,
      'label' => $machine['#label'],
      'entity' => $machine['#entity'] == 'node' ? STATES_ADMIN_ENTITY_NODE : STATES_ADMIN_ENTITY_USER,
      'any_state' => $machine['#states'] == '*' ? 1 : 0,
      'state_list' => $machine['#states'],
      'any_entity' => (isset($machine['#types']) || isset($machine['#roles'])) ? 0 : 1,
      'sid_to_update' => $machine['custom_sid'],
      'filters' => array(),
    );
    if ($machine['#states'] == '*') {
      $form_values['initial_state'] = isset($machine['#init_state']) ? $machine['#init_state'] : '';
    }
    else {
      $form_values['initial_state'] = isset($machine['#init_state']) ? array_search($machine['#init_state'], $machine['#states']) : FALSE;
      if ($form_values['initial_state'] === FALSE) {
        $form_values['initial_state'] = 0;
      }
      else {
        $form_values['initial_state'] = $form_values['initial_state'] + 1;
      }
    }
    if ($machine['#entity'] == 'node' && isset($machine['#types']) && count($machine['#types']) > 0) {
      $form_values['filters'] = array_combine(array_values($machine['#types']), array_values($machine['#types']));
    }
    else if ($machine['#entity'] == 'user' && isset($machine['#roles']) && count($machine['#roles']) > 0) {
      $form_values['filters'] = array_combine(array_values($machine['#roles']), array_values($machine['#roles']));
    }
    $form_values['saved'] = serialize($form_values); // So that the form handler thinks nothing has changed
  }
  return states_admin_form($form_values);
}

function states_admin_delete($machine_name) {
  $machines = states_get_machines();
  $machine = $machines[$machine_name];

  $form = array('custom_sid' => array('#type' => 'value', '#value' => $machine['custom_sid']));
  return confirm_form($form,
    t('Are you sure you want to delete the state machine "%machine"?', array('%machine' => $machine['#label'])),
    array('path' => STATES_ADMIN_UI_PATH),
    t('This action cannot be undone.'), t('Delete'), t('Cancel')
  );
}

function states_admin_delete_submit($form_id, $form_values) {
  db_query("DELETE FROM {states_custom} WHERE sid = %d", $form_values['custom_sid']);
  drupal_set_message(t('State machine deleted'));
  return STATES_ADMIN_UI_PATH;
}
