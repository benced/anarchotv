<?php

/**
 * @file Workflow-ng integration for the user module 
 */

/**
 * Implementation of hook_module_info
 */
function user_module_info() {
  return array(
    'user_workflow_ng_forms' => array(
      'file' => 'workflow_ng_user_forms.inc',
      'file path' => drupal_get_path('module', 'workflow_ng'). '/modules/',
    ),
  );
}

/** 
 * Implementation of hook_event_info()
 */
function user_event_info() {
  return array(
    'user_insert' => array(
      '#label' => t('User has registered'),
      '#module' => 'User',
      '#arguments' => workflow_ng_events_hook_user_arguments(t('registered user')),
      '#redirect' => TRUE,
    ),
    'user_update' => array(
      '#label' => t('User has updated his account details'),
      '#module' => 'User',
      '#arguments' => workflow_ng_events_hook_user_arguments(t('updated user')) + array('account_unchanged' => array('#entity' => 'user', '#label' => t('unchanged user'))),
      '#redirect' => TRUE,
    ),
    'user_view' => array(
      '#label' => t('User page has been viewed'),
      '#module' => 'User',
      '#description' => t('Note that if drupal\'s page cache is enabled, this event won\'t be generated for pages served from cache.'),
      '#arguments' => workflow_ng_events_hook_user_arguments(t('viewed user')),
    ),
    'user_delete' => array(
      '#label' => t('User has been deleted'),
      '#module' => 'User',
      '#arguments' => workflow_ng_events_hook_user_arguments(t('deleted user')),
      '#redirect' => TRUE,
    ),
    'user_login' => array(
      '#label' => t('User has logged in'),
      '#module' => 'User',
      '#arguments' => array('account' => array('#entity' => 'user', '#label' => t('logged in user'))),
      '#redirect' => TRUE,
    ),
    'user_logout' => array(
      '#label' => t('User has logged out'),
      '#module' => 'User',
      '#arguments' => array('account' => array('#entity' => 'user', '#label' => t('logged out user'))),
      '#redirect' => TRUE,
    ),
  );
}

/**
 * Returns the argument definition for the acting user (global $user)
 */
function workflow_ng_events_global_user_argument() {
  return array('user' => array('#entity' => 'user', '#label' => t('acting user'), '#handler' => 'workflow_ng_events_argument_global_user'));
}

/**
 * Implementation of hook_user
 * As user_user exists already, we implement workflow_ng_user()
 */
function workflow_ng_user($op, $edit, &$account, $category = NULL) {
  static $account_unchanged;

  //we don't support updates for other categories than 'account'
  if ($op == 'update' && $category != 'account') {
    return;
  }
  else if ($op == 'update') {
    //save the unchanged account for the use with op after_update
    $account_unchanged = $account;
  }
  else if ($op == 'after_update') {
    workflow_ng_invoke_event('user_update', array('account' => &$account, 'account_unchanged' => $account_unchanged));
  }
  else if (in_array($op, array('insert', 'login', 'logout', 'view', 'delete'))) {
    workflow_ng_invoke_event('user_'. $op, array('account' => &$account));
  }
}

/**
 * Returns some arguments suitable for hook_user
 */
function workflow_ng_events_hook_user_arguments($account_label) {
  return array(
    'account' => array('#entity' => 'user', '#label' => $account_label),
  ) + workflow_ng_events_global_user_argument();
}

/**
 * Gets the user account of the "acting" user - which is always the global user
 */
function workflow_ng_events_argument_global_user() {
  global $user;
  return user_load(array('uid' => $user->uid));
}


/*
 * Implementation of hook_condition_info()
 */
function user_condition_info() {
  return array(
    'workflow_ng_condition_user_comparison' => array(
      '#label' => t('Compare two users'),
      '#arguments' => array(
        'user1' => array('#entity' => 'user', '#label' => t('First user to compare')),
        'user2' => array('#entity' => 'user', '#label' => t('Second user to compare')),
      ),
      '#description' => t('Evaluates to TRUE, if both compared users are the same user account.'),
      '#module' => 'User',
    ),
    'workflow_ng_condition_user_hasrole' => array(
      '#label' => t('User has role(s)'),
      '#arguments' => array(
        'user' => array('#entity' => 'user', '#label' => t('User to test')),
      ),
      '#description' => t('Whether the user has the selected role(s).'),
      '#module' => 'User',
    ),
  );
}

/*
 * A simple user comparison
 */
function workflow_ng_condition_user_comparison($user1, $user2) {
  return $user1->uid == $user2->uid;
}

/*
 * Condition user: condition to check whether user has particular roles
 */
function workflow_ng_condition_user_hasrole($user, $settings) {
  $roles = $settings['roles'];
  $operation = $settings['operation'];
  
  switch ($operation) {
    case 'OR':
      foreach ($roles as $rid) {
        if (isset($user->roles[$rid])) {
          return TRUE;
        }
      }
      return FALSE;
    case 'AND':
      foreach ($roles as $rid) {
        if (!isset($user->roles[$rid])) {
          return FALSE;
        }
      }
      return TRUE;
  }
}



/*
 * Implementation of hook_action_info()
 */
function user_action_info() {
  return array(
    'workflow_ng_action_user_addrole' => array(
      '#label' => t('Add user role'),
      '#arguments' => array(
        'user' => array('#entity' => 'user', '#label' => t('User whose roles should be changed')),
      ),
      '#module' => 'User',
    ),
    'workflow_ng_action_user_removerole' => array(
      '#label' => t('Remove user role'),
      '#arguments' => array(
        'user' => array('#entity' => 'user', '#label' => t('User whose roles should be changed')),
      ),
      '#module' => 'User',
    ),
    'workflow_ng_action_load_user' => array(
      '#label' => t('Load a user'),
      '#new arguments' => array(
        'user_loaded' => array('#entity' => 'user', '#label' => t('Loaded user')),
      ),
      '#description' => t('Enter an id or a name of the user to load.'),
      '#module' => 'User',
     ),
  );
}

/*
 * Action user: adds roles to a particular user
 */
function workflow_ng_action_user_addrole($user, $settings) {
  // Get role list (minus the anonymous)
  $role_list = user_roles(1);
  $roles = $settings['roles'];
  foreach ($roles as $rid) {
    $user->roles[$rid] = $role_list[$rid];
  }
  user_save($user, array('roles' => $user->roles));
  // Delete that user's menu cache:
  cache_clear_all($user->uid .':', 'cache_menu', TRUE);
  return array('user' => $user);
}

/*
 * User Action: deletes roles from a given user
 */
function workflow_ng_action_user_removerole($user, $settings) {
  $roles = $settings['roles'];
  foreach ($roles as $rid) {
    // If the user has this role, delete it
    if (isset($user->roles[$rid])) {
      unset($user->roles[$rid]);
    }
  }
  user_save($user, array('roles' => $user->roles));
  // Delete that user's menu cache:
  cache_clear_all($user->uid .':', 'cache_menu', TRUE);
  return array('user' => $user);
}

/**
 * Loads a user
 */
function workflow_ng_action_load_user($settings, &$arguments, &$log) {
  extract(workflow_ng_token_replace_all(array('username', 'userid'), $settings, $arguments, $log));

  $userarray = array('name' => $username, 'uid' => $userid);
  return array('#new arguments' => array('user_loaded' => user_load(array_filter($userarray))));
}

