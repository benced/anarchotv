<?php

/**
 * @file Workflow-ng integration for the system module
 */

/**
 * Implementation of hook_module_info
 */
function system_module_info() {
  return array(
    'system_workflow_ng_forms' => array(
      'file' => 'workflow_ng_system_forms.inc',
      'file path' => drupal_get_path('module', 'workflow_ng'). '/modules/',
    ),
  );
}

/**
 * Implementation of hook_event_info()
 */
function system_event_info() {
  return array(
    'init' => array(
      '#label' => t('User is going to view a page'),
      '#module' => 'System',
      '#description' => t('Note that if drupal\'s page cache is enabled, this event won\'t be generated for pages served from cache.'),
      '#arguments' => workflow_ng_events_global_user_argument(),
    ),
  );
}

/*
 * Implementation of hook_menu
 * As system_menu exists already, we implement workflow_ng_menu
 */
function workflow_ng_menu($may_cache) {
  if (!$may_cache) {
    workflow_ng_invoke_event('init');
  }
}


/*
 * Implementation of hook_condition_info()
 */
function system_condition_info() {
  return array(
    'workflow_ng_condition_token_compare' => array(
      '#label' => t('Textual comparison'),
      //we are not working with defined arguments, instead we make use of all available arguments
      '#arguments' => array(),
      '#description' => t('This condition just compares two texts. It returns TRUE, if both texts are equal.').
        ' '. t('You may use token replacements in both texts, e.g. the texts %text1 and %text2 are only equal, if the argument called "node" is of the type page.', array('%text1' => "[node:type]", '%text2' => 'page')),
      '#module' => 'System',
    ),
    'workflow_ng_condition_token_numeric' => array(
      '#label' => t('Numeric comparison'),
      '#arguments' => array(),
      '#description' => t('This does a numeric comparison between two tokens. Select greater than, less than or equal to.'),
      '#module' => 'System',
    ),
    'workflow_ng_condition_check_boolean' => array(
      '#label' => t('Check a boolean value'),
      '#arguments' => array(
        'boolean' => array('#entity' => 'boolean', '#label' => t('Boolean')),
      ),
      '#description' => t('Check a boolean value, i.e. TRUE or FALSE.'),
      '#module' => 'System',
    ),
  );
}

/*
 * Condition Implementation: Token enabled comparison
 */
function workflow_ng_condition_token_compare($settings, &$arguments, &$log) {
  extract( workflow_ng_token_replace_all(array('text1', 'text2'), $settings, $arguments, $log) );
  return $settings['regex'] ? ereg($text2, $text1) : $text1 == $text2;
}

/*
 * Condition implementation: token enabled numeric comparison
 */
function workflow_ng_condition_token_numeric($settings, &$arguments, &$log) {
  extract( workflow_ng_token_replace_all(array('text1', 'text2'), $settings, $arguments, $log) );
  $text1 = (float)$text1;
  $text2 = (float)$text2;

  switch ($settings['operation']) {
    case 'greater':
      $result = $text1 > $text2;
      break;
    case 'equal':
      $result = $text1 == $text2;
      break;
    case 'less':
      $result = $text1 < $text2;
      break;
  }
  return $result;
}

/*
 * Condition implementation: Boolean check
 */
function workflow_ng_condition_check_boolean($boolean) {
  return (bool)$boolean;
}





/*
 * Implementation of hook_action_info()
 */
function system_action_info() {
  return array(
    'workflow_ng_action_set_breadcrumb' => array(
      '#label' => t('Set breadcrumb'),
      '#module' => 'System',
    ),
    'workflow_ng_action_mail_to_user' => array(
      '#label' => t('Send a mail to a user'),
      '#arguments' => array(
        'user' => array('#entity' => 'user', '#label' => t('Recipient')),
      ),
      '#module' => 'System',
      '#description' => t('You may use the token replacements for the mail\'s message body as well as the mail\'s subject.'),
    ),
    'workflow_ng_action_mail_to_users_of_role' => array(
      '#label' => t('Send mail to all users of a role'),
      '#arguments' => array(),
      '#module' => 'System',
      '#description' => t('You may use the token replacements for the mail\'s message body as well as the mail\'s subject.'),
    ),
    'workflow_ng_action_mail' => array(
      '#label' => t('Send a mail to an arbitrary mail address'),
      '#module' => 'System',
      '#description' => t('You may use the token replacements for the mail\'s message body, to and from fields as well as for the mail\'s subject.'),
    ),
    'workflow_ng_action_drupal_message' => array(
      '#label' => t('Show a configurable message on the site'),
      '#module' => 'System',
    ),
    'workflow_ng_action_drupal_goto' => array(
      '#label' => t('Page redirect'),
      '#module' => 'System',
    ),
    'workflow_ng_action_watchdog' => array(
      '#label' => t('Log to watchdog'),
      '#module' => 'System',
    ),
  );
}

/**
 * Action: Send mail to all users of a specific role group(s).
 */
function workflow_ng_action_mail_to_users_of_role($settings, &$arguments, &$log) {
  $recipients = $settings['recipients'];
  $names = array('from', 'subject');
  extract( workflow_ng_token_replace_all(array('message' => 'message') + $names, $settings, $arguments, $log) );

  foreach ($names as $key) {
    $$key = str_replace(array("\r", "\n"), '', $$key);
  }

  // All authenticated users, which is everybody.
  if ($recipients[DRUPAL_AUTHENTICATED_RID]) {
    $result = db_query('SELECT mail FROM {users} WHERE uid > 0');
  }
  else {
    $roles = count($recipients) ? implode(',', $recipients) : 0;
    // Avoid sending emails to members of two or more target role groups.
    $result = db_query('SELECT DISTINCT u.mail FROM {users} u INNER JOIN {users_roles} r ON u.uid = r.uid WHERE r.rid IN ('. $roles .')');
  }

  // Now, actually send the mails.
  while ($account = db_fetch_object($result)) {
    if (drupal_mail('workflow-ng-mass-user-mail-by-role', $account->mail, $subject, $message, $from)) {
      watchdog('action', t('Sent email to %recipient', array('%recipient' => $account->mail)), WATCHDOG_NOTICE);
    }
    else {
      watchdog('error', t('Unable to send email to %recipient', array('%recipient' => $account->mail)), WATCHDOG_ERROR);
    }
  }
}

/*
 * Action Implementation: Show a configureable message
 */
function workflow_ng_action_drupal_message($settings, &$arguments, &$log) {
  extract( workflow_ng_token_replace_all(array('message'), $settings, $arguments, $log) );
  drupal_set_message($message, $settings['error'] ? 'error' : 'status');
}


/*
 * Action Implementation: Page redirect
 */
function workflow_ng_action_drupal_goto($settings, &$arguments, &$log) {
  extract( workflow_ng_token_replace_all(array('path', 'query', 'fragment'), $settings, $arguments, $log) );

  if ($settings['force']) {
    if (strpos($query, 'destination') === FALSE) {
      if (!empty($query)) {
        $query .= '&';
      }
      $query .= $_REQUEST['destination'] ? 'destination='. urlencode($_REQUEST['destination']) : NULL;
    }
    unset($_REQUEST['destination']);
  }
  if ($settings['override']) {
    //override the destination parameter, so another drupal_goto goes to our url :)
    $_REQUEST['destination'] = urlencode($path) . ($query ? '?'. urlencode($query) : '').($fragment ? '#'. urlencode($fragment) : '');
  }
  else {
    drupal_goto($path, $query ? $query : NULL, $fragment ? $fragment : NULL);
  }
}

/*
 * Action Implementation: Watchdog
 */
function workflow_ng_action_watchdog($settings, &$arguments, &$log) {
  extract( workflow_ng_token_replace_all(array('message', 'link'), $settings, $arguments, $log) );

  watchdog($settings['type'], $message, $settings['severity'], $link);
}

/*
 * Action Implementation: Send a mail to a user
 */
function workflow_ng_action_mail_to_user($user, $settings, &$arguments, &$log) {
  //we also handle $settings['to'] if it's set
  $names = isset($settings['to']) ? array('from', 'subject', 'to') : array('from', 'subject');
  extract( workflow_ng_token_replace_all(array('message' => 'message') + $names, $settings, $arguments, $log) );

  foreach ($names as $key) {
    $$key = str_replace(array("\r", "\n"), '', $$key);
  }
  $to = isset($to) ? $to : $user->mail;

  if (drupal_mail('workflow_ng_action_mail', $to, $subject, $message, $from)) {
    watchdog('action', t('Sent email to %recipient', array('%recipient' => ($to == $user->mail) ? $user->name : $to)), WATCHDOG_NOTICE, ($to == $user->mail) ? theme('username', $user) : NULL);
  }
  else {
    watchdog('error', t('Unable to send email to %recipient', array('%recipient' => ($to == $user->mail) ? $user->name : $to)), WATCHDOG_NOTICE, ($to == $user->mail) ? theme('username', $user) : NULL);
  }
}

/*
 * Action Implementation: workflow_ng_action_mail
 * This action makes use of the workflow_ng_action_mail_to_user action implementation
 */
function workflow_ng_action_mail($settings, &$arguments, &$log) {
  workflow_ng_action_mail_to_user(NULL, $settings, $arguments, $log);
}

/*
 * Action Implementation: Set breadcrumb
 */
function workflow_ng_action_set_breadcrumb($settings, &$arguments, &$log) {
  extract( workflow_ng_token_replace_all(array('titles', 'paths'), $settings, $arguments, $log) );

  $titles = explode("\n", $titles);
  $paths = explode("\n", $paths);

  $trail = array(l(t('Home'), ''));
  for ($i = 0; $i < count($titles); $i++) {
    // Skip empty titles
    if ($title = trim($titles[$i])) {
      // Output plaintext instead of a link if there is a title
      // without a path.
      $path = trim($paths[$i]);
      if (strlen($path) > 0 && $path != '<none>') {
        $trail[] = l($title, trim($paths[$i]));
      }
      else {
        $trail[] = check_plain($title);
      }
    }
  }
  drupal_set_breadcrumb($trail);
}
