<?php

/**
 * @file Workflow-ng configuration forms for the system module
 */

/*
 * Condition: Token enabled comparison configuration form
 */
function workflow_ng_condition_token_compare_form($settings = array(), $argument_info) {
  $form = array();
  $form['text1'] = array(
    '#type' => 'textfield',
    '#title' => 'Two texts to compare',
    '#default_value' => isset($settings['text1']) ? $settings['text1'] : '',
  );
  $form['text2'] = array(
    '#type' => 'textfield',
    '#title' => '',
    '#default_value' => isset($settings['text2']) ? $settings['text2'] : '',
  );
  $form['regex'] = array(
    '#type' => 'checkbox',
    '#title' => t('Evaluate text2 as a regular expression.'),
    '#default_value' => isset($settings['regex']) ? $settings['regex'] : 0,
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

function workflow_ng_condition_token_compare_submit($form_id, $form_values) {
  //returns the needed settings
  $settings = workflow_ng_token_get_settings(array('text1', 'text2'), $form_values);
  return $settings + array('regex' => $form_values['regex']);
}

/*
 * Condition: token enabled numeric comparison configuration form
 */
function workflow_ng_condition_token_numeric_form($settings = array(), $argument_info) {
  $form = array();
  $form['text1'] = array(
    '#type' => 'textfield',
    '#title' => t('Text to compare'),
    '#default_value' => isset($settings['text1']) ? $settings['text1'] : '',
  );
  $form['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#options' => array('greater' => t('Greater than'), 'equal' => t('Equal to'), 'less' => t('Less than')),
    '#default_value' => $settings['operation'],
  );
  $form['text2'] = array(
    '#type' => 'textfield',
    '#title' => '',
    '#default_value' => isset($settings['text2']) ? $settings['text2'] : '',
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

function workflow_ng_condition_token_numeric_submit($form_id, $form_values) {
  $settings = workflow_ng_token_get_settings(array('text1', 'text2'), $form_values);
  return $settings + array('operation' => $form_values['operation']);
}


function workflow_ng_action_mail_to_users_of_role_form($settings = array(), $argument_info) {
  // Select only non-anonymous user roles because anonymous users won't have emails.
  $roles = user_roles(TRUE);

  $form['recipients'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Recipient Role Groups'),
    '#prefix' => t('WARNING: This may cause problems if there are too many users of these roles on your site, as your server may not be able to handle all the mail requests all at once.'),
    '#required' => TRUE,
    '#default_value' => $settings['recipients'],
    '#options' => $roles,
    '#description' => t('Select the roles whose users should receive this email.'),
  );
  $form['from'] = array(
    '#type' => 'textfield',
    '#title' => t('Sender'),
    '#default_value' => isset($settings['from']) ? $settings['from'] : variable_get('site_mail', ini_get('sendmail_from')),
    '#description' => t('The mail\'s "from" address.'),
    '#required' => TRUE,
  );
  $form['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => $settings['subject'],
    '#description' => t('The mail\'s subject.'),
  );
  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#default_value' => $settings['message'],
    '#description' => t('The mail\'s message body.'),
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

function workflow_ng_action_mail_to_users_of_role_submit($form_id, $form_values) {
  $form_values['recipients'] = array_filter($form_values['recipients']);
  return workflow_ng_token_get_settings(array('from', 'subject', 'message'), $form_values) + array('recipients' => $form_values['recipients']);
}

/*
 * Action "Show a configureable message" configuration form
 */
function workflow_ng_action_drupal_message_form($settings = array(), $argument_info) {
  $form = array();
  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#default_value' => $settings['message'],
    '#description' => t('The message that should be displayed.'),
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  $form['error'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display as error message'),
    '#default_value' => $settings['error'],
  );
  return $form;
}

function workflow_ng_action_drupal_message_submit($form_id, $form_values) {
  //returns the needed settings
  $settings = workflow_ng_token_get_settings(array('message'), $form_values);
  return $settings + array('error' => $form_values['error']);
}

/*
 * Action "Page redirect" configuration form
 */
function workflow_ng_action_drupal_goto_form($settings = array(), $argument_info, $info) {
  $form['redirect'] = array(
    '#type' => 'item',
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
    '#title' => t('To'),
    '#description' => '<div style="display:block">'. t('Enter a Drupal path, path alias, or external URL to redirect to. Enter (optional) queries after "?" and (optional) anchor after "#". Most redirects will not contain queries or fragment anchors.') .'</div>',
  );
  $form['redirect']['path'] = array(
    '#type' => 'textfield',
    '#size' => 30,
    '#default_value' => drupal_get_path_alias($settings['path']),
  );
  $form['redirect'][] = array(
    '#value' => '?',
  );
  $form['redirect']['query'] = array(
    '#type' => 'textfield',
    '#size' => 12,
    '#default_value' => $settings['query'],
  );
  $form['redirect'][] = array(
    '#value' => '#',
  );
  $form['redirect']['fragment'] = array(
    '#type' => 'textfield',
    '#size' => 12,
    '#default_value' => $settings['fragment'],
  );
  $form['force'] = array(
    '#type' => 'checkbox',
    '#title' => t('Force redirecting to the given path, even if a destination parameter is given'),
    '#description' => t('Per default drupal doesn\'t redirect to the given path, if a destination parameter is set. Instead it redirects to the given destination paramter. Most times, the destination parameter is set by appending it to the URL, e.g. !example_url', array('!example_url' => 'http://example.com/user/login?destination=node/2')),
    '#default_value' => $settings['force'],
    '#weight' => 7,
  );
  $form['override'] = array(
    '#type' => 'checkbox',
    '#title' => t('Override another path redirect by setting the destination parameter'),
    '#description' => t('If checked, the path redirect isn\'t initiated by this action, but the destination parameter gets set. So if something else redirects, the redirect will use the configured path.'),
    '#default_value' => $settings['override'],
    '#weight' => 8,
  );
  if (!isset($settings['override'])) {
    $event = $info['event'];
    //a formAPI bug lets default_value not work, so we have to care about this ourself
    if (isset($event['#redirect']) && $event['#redirect']) {
      $form['override']['#attributes']['checked'] = 'checked';
    }
  }
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

function workflow_ng_action_drupal_goto_submit($form_id, $form_values) {
  //returns the needed settings
  $settings = workflow_ng_token_get_settings(array('path', 'query', 'fragment'), $form_values);
  return $settings + array('force' => $form_values['force'], 'override' => $form_values['override']);
}

/*
 * Action "Watchdog" configuration form
 */
function workflow_ng_action_watchdog_form($settings = array(), $argument_info) {
  $form['severity'] = array(
    '#type' => 'select',
    '#title' => t('Severity'),
    '#options' => array(
      WATCHDOG_NOTICE => t('Notice'),
      WATCHDOG_WARNING => t('Warning'),
      WATCHDOG_ERROR => t('Error'),
    ),
    '#default_value' => isset($settings['type']) ? $settings['type'] : WATCHDOG_NOTICE,
    '#required' => TRUE,
  );
  $form['type'] = array(
    '#type' => 'textfield',
    '#title' => t('Category'),
    '#default_value' => isset($settings['type']) ? $settings['type'] : 'workflow-ng',
    '#description' => t('The category to which this message belongs.'),
    '#required' => TRUE,
  );
  $form['message'] = array(
    '#type' => 'textfield',
    '#title' => t('Message'),
    '#default_value' => isset($settings['message']) ? $settings['message'] : '',
    '#description' => t('The message to log. You may make use of the replacements.'),
    '#required' => TRUE,
  );
  $form['link'] = array(
    '#type' => 'textfield',
    '#title' => t('Link (optional)'),
    '#default_value' => isset($settings['link']) ? $settings['link'] : '',
    '#description' => t('A link to associate with the message. You may make use of the replacements.'),
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

function workflow_ng_action_watchdog_submit($form_id, $form_values) {
  $vars = drupal_map_assoc(array('severity', 'type'));
  $settings = workflow_ng_token_get_settings(array('message', 'link'), $form_values);
  return array_intersect_key($form_values, $vars) + $settings;
}

/*
 * Action "Send a mail to a user" configuration form
 */
function workflow_ng_action_mail_to_user_form($settings = array(), $argument_info) {
  $form['from'] = array(
    '#type' => 'textfield',
    '#title' => t('Sender'),
    '#default_value' => isset($settings['from']) ? $settings['from'] : variable_get('site_mail', ini_get('sendmail_from')),
    '#description' => t('The mail\'s "from" address.'),
    '#required' => TRUE,
  );
  $form['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => $settings['subject'],
    '#description' => t('The mail\'s subject.'),
  );
  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#default_value' => $settings['message'],
    '#description' => t('The mail\'s message body.'),
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

function workflow_ng_action_mail_to_user_submit($form_id, $form_values) {
  return workflow_ng_token_get_settings(array('from', 'subject', 'message'), $form_values);
}

/*
 * Action "Send a mail to an arbitrary mail address" configuration form
 */
function workflow_ng_action_mail_form($settings = array(), $argument_info) {
  $form = workflow_ng_action_mail_to_user_form($settings, $argument_info);
  $form['to'] = array(
    '#type' => 'textfield',
    '#title' => t('Recipient'),
    '#default_value' => $settings['to'],
    '#description' => t('The mail\'s recipient address. You may separate multiple addresses with ",".'),
    '#required' => TRUE,
    '#weight' => -1,
  );
  return $form;
}

function workflow_ng_action_mail_submit($form_id, $form_values) {
  return workflow_ng_token_get_settings(array('to', 'from', 'subject', 'message'), $form_values);
}

/*
 * Action "Set breadcrumb" configuration form
 */
function workflow_ng_action_set_breadcrumb_form($settings = array(), $argument_info) {
  $form['titles'] = array(
    '#type' => 'textarea',
    '#title' => t('Titles'),
    '#required' => TRUE,
    '#description' => t('A list of titles for the breadcrumb links, one on each line.'),
    '#default_value' => $settings['titles'],
  );
  $form['paths'] = array(
    '#type' => 'textarea',
    '#title' => t('Paths'),
    '#required' => TRUE,
    '#description' => t('A list of Drupal paths for the breadcrumb links, one on each line.'),
    '#default_value' => $settings['paths'],
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

function workflow_ng_action_set_breadcrumb_submit($form_id, $form_values) {
  return workflow_ng_token_get_settings(array('titles', 'paths'), $form_values);
}
