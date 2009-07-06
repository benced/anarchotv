<?php

/**
 * @file Furthere workflow-ng integration that doesn't fit really to any other module
 */

/**
 * Implementation of hook_module_info
 */
function workflow_ng_module_info() {
  return array(
    'workflow-ng_workflow_ng_forms' => array(
      'file' => 'workflow_ng_workflow_ng_forms.inc',
      'file path' => drupal_get_path('module', 'workflow_ng'). '/modules/',
    ),
    'workflow_ng_workflow_ng_configuration' => array(
      'file' => 'workflow_ng_configurations.inc',
      'file path' => drupal_get_path('module', 'workflow_ng'),
    ),
  );
}

/**
 * Implementation of hook_action_info()
 */
function workflow_ng_action_info() {
  return array(
    'workflow_ng_action_custom_php' => array(
      '#label' => t('Execute custom PHP code'),
      '#arguments' => array(),
      '#module' => 'Workflow-ng',
    ),
  );
}

/**
 * Implementation of hook_condition_info()
 */
function workflow_ng_condition_info() {
  return array(
    'workflow_ng_condition_custom_php' => array(
      '#label' => t('Execute custom PHP code'),
      '#arguments' => array(),
      '#module' => 'Workflow-ng',
    ),
  );
}

/**
 * Returns an array containing the values of the desired PHP arguments
 * @param $used_arguments Array containing names of arguments to fetch (usually from workflow_ng_custom_php_get_used_php_arguments)
 * @param $arguments The available arguments
 * @param $log Workflow-NG log
 * @return Array containing the values for each of the arguments in $used_arguments
 */
function workflow_ng_custom_php_get_php_argument_data($used_arguments, &$arguments, &$log) {
  $argument_data = array();
  if (is_array($used_arguments)) {
    foreach($used_arguments as $name) {
      $data = workflow_ng_element_get_argument($arguments, $name, $log);
      $argument_data[$name] = $data;
    }
  }
  return $argument_data;
}

/**
 * Evalutes the given PHP code, with the given variables defined
 *
 * @param $code The PHP code to run
 * @param $argument_data Array contained variables to be extracted to the code
 * @return The return value of eval
 */
function workflow_ng_custom_php_eval($code, $argument_data) {
  extract($argument_data);
  return eval($code);
}

/**
 * Run any PHP code as an action
 */
function workflow_ng_action_custom_php($settings, &$arguments, &$log) {
  $php = workflow_ng_token_replace($settings['php'], $settings['used_arguments'], $arguments, $log);
  $argument_data = workflow_ng_custom_php_get_php_argument_data($settings['used_php_arguments'], $arguments, $log);
  $result = workflow_ng_custom_php_eval($php .';', $argument_data);

  //this action has no arguments defined, so just returning the arguments to save won't work
  //so we set the arguments to be saved manually
  foreach (element_children($result) as $argument_name) {
    workflow_ng_set_argument_to_save($argument_name, $result[$argument_name], $arguments);
    unset($result[$argument_name]);
  }
  return $result;
}

/**
 * Run any PHP code as a condition
 */
function workflow_ng_condition_custom_php($settings, &$arguments, &$log) {
  $php = workflow_ng_token_replace($settings['php'], $settings['used_arguments'], $arguments, $log);
  $argument_data = workflow_ng_custom_php_get_php_argument_data($settings['used_php_arguments'], $arguments, $log);
  $status = workflow_ng_custom_php_eval($php .';', $argument_data);

  return $status;
}
