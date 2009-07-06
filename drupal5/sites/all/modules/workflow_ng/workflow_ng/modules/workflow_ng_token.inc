<?php

/**
 * @file Workflow-ng integration for the token module
 * It exposes some further tokens and provides some API functions, that can
 * be used by conditions and actions to easily provide token integration
 */

/**
 * Implementation of hook_token_values()
 */
function workflow_ng_token_values($entity_type, $object = NULL) {
  $values = array();
  if ($entity_type == 'global') {
    $values['url'] = check_plain($_REQUEST['q']);
    $values['path'] = check_plain($_GET['q']);
    $arguments = explode('/', $values['path']); 
    foreach ($arguments as $key => $arg) {
      $values['path-arg-'. ($key+1)] = $arg;
    }
  }
  else if ($entity_type == 'node') {
    $node = $object;
    $values['status'] = $node->status ? t('yes') : t('no');
    $values['promote'] = $node->promote ? t('yes') : t('no');
    $values['sticky'] = $node->sticky ? t('yes') : t('no');
    if (module_exists('path')) {
      $values['path'] = $node->path ? $node->path : 'node/'. $node->nid;
    }
  }
  return $values;
}

/**
 * Implementation of hook_token_list()
 */
function workflow_ng_token_list($entity_type = 'all') {
  $tokens = array();
  $tokens['global']['url'] = t('The current active url, e.g. "admin".');
  $tokens['global']['path'] = t('The current active drupal path, e.g. "node/2". If URL aliases are used, this will be always the path behind the alias."');
  $tokens['global']['path-arg-1'] = t('The first component of the current active path, e.g. for the path "node/2" it will be "node".');
  $tokens['global']['path-arg-x'] = t('The Xst argument of the current active path.');

  if ($entity_type == 'node') {
    $tokens['node']['status'] = t('Whether the node is published. (@yes/@no)', array('@yes' => t('yes'), '@no' => t('no')));
    $tokens['node']['promote'] = t('Whether the node is promoted to the front page. (@yes/@no)', array('@yes' => t('yes'), '@no' => t('no')));
    $tokens['node']['sticky'] = t('Whether the node is sticky at the top of lists. (@yes/@no)', array('@yes' => t('yes'), '@no' => t('no')));
    $tokens['node']['path'] = t('The URL alias of the node WARNING - raw user input.');
  }
  return $tokens;
}


/*
 * Some useful funtions that conditions and actions may use when token is enabled 
 */


/*
 * Returns an array of all used arguments in $text
 * 
 * @param $text The $text, which will be processed
 * @param $argument_info The argument definitions we work with
 * @return The array of names of the used arguments
 */
function workflow_ng_token_get_used_arguments($text, $argument_info) {
  $used_args = array();
  foreach ($argument_info as $name => $argument) {
    if (strpos($text, '['. $name . ':') !== FALSE) {
      $used_args[] = $name;
    }
  }
  return $used_args;
}

/*
 * Performs the replacement of the replacement patterns.
 * If a used argument isn't loaded yet, it will be automatically loaded by workflow_ng_element_get_argument()
 * 
 * @param $text The $text, which will be processed
 * @param $used_arguments The array of names of used arguments
 * @param $arguments The workflow-ng arguments data structure
 * @param $log An array of log entries. Set it to FALSE to disable logging
 * @return The text with replaced patterns.
 */
function workflow_ng_token_replace($text, $used_arguments, &$arguments, &$log) {
  static $token_cache = array();

  if (is_array($used_arguments)) {
    foreach($used_arguments as $name) {
      $data = workflow_ng_element_get_argument($arguments, $name, $log);
      $entity = $arguments['info'][$name]['#entity'];
  
      $token_id = _token_get_id($entity, $data);
      if (isset($token_cache[$token_id]) && $token_cache[$token_id] != $name) {
        //this is another argument although it has the same id
        //so we need to flush the token cache manually 
        token_get_values('global', NULL, TRUE);
      }
  
      $text = token_replace($text, $entity, $data, '['. $name . ':', ']');
  
      //remember that this argument has been used and token has cached it...
      $token_cache[$token_id] = $name;
    }
  }
  return $text;
}

/*
 * Adds the token replacement help to $form
 */
function workflow_ng_token_replacement_help(&$form, $argument_info) {
  if (isset($argument_info) && count($argument_info)) {
    $form['token_help'] = array(
      '#type' => 'fieldset',
      '#title' => t('Replacement patterns'),
      '#description' => t('You can make use of the replacement patterns of all available arguments.'),
    );
    foreach ($argument_info as $name => $argument) {
      $form['token_help'][$name] = array(
        '#type' => 'fieldset',
        '#title' => t('Replacement patterns for @name', array('@name' => $argument['#label'])),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form['token_help'][$name]['content'] = array(
        '#value' => theme('token_help', $argument['#entity'], '['. $name . ':', ']'),
      );
    }
  }
  $form['arguments'] = array('#type' => 'value', '#value' => $argument_info);
}

/**
 * Prepares the settings array for the given elements
 * 
 * @param $names An array of element names, e.g. array('message')
 */
function workflow_ng_token_get_settings($names, $form_values) {
  $settings = array();
  foreach ($names as $key) {
    $settings[$key] = $form_values[$key];
    $settings[$key .'_args'] = workflow_ng_token_get_used_arguments($form_values[$key], $form_values['arguments']);
  }
  return $settings;
}

/**
 * Replaces the tokens for all given elements
 * 
 * @param $names An array of element names, e.g. array('message')
 * @return An array of replaced strings, keyed with the element names
 */
function workflow_ng_token_replace_all($names, $settings, &$arguments, &$log) {
  $return = array();
  foreach ($names as $key) {
    if (array_key_exists($key .'_args', $settings)) {
      $return[$key] = isset($settings[$key]) ? workflow_ng_token_replace($settings[$key], $settings[$key .'_args'], $arguments, $log) : NULL;      
    }
    else {
      $return[$key] = $settings[$key];
      workflow_ng_handle_error_msg(t('Workflow-ng: Applying token replacements failed. To fix this try editing and saving all conditions and actions.'), $configuration_name);
    }
  }
  return $return;
}
