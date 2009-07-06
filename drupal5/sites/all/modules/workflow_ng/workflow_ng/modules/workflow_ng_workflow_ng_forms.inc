<?php

/**
 * @file Some workflow-ng configuration forms
 */

/**
 * Returns an array of all used PHP arguments in $code
 * PHP arguments are $name rather than [name:...]
 * 
 * @param $code The $code, which will be processed
 * @param $argument_info The argument definitions we work with
 * @return The array of names of the used arguments
 */
function workflow_ng_custom_php_get_used_php_arguments($code, $argument_info) {
  $used_args = array();
  foreach ($argument_info as $name => $argument) {
    if (strpos($code, '$'. $name) !== FALSE) {
      $used_args[] = $name;
    }
  }
  return $used_args;
}

/**
 * For a given list of available argument, formats them into a nice table.
 *
 * @param argument_info An array containing available arguments
 * @return An HTML table containing the formatting docs.
 */
function theme_php_argument_help($argument_info, $allow_saves) {
  $entity_info = workflow_ng_gather_data('entity_info', FALSE);
  $headers = array(t('Argument'), t('Type'), t('Description'), );
  if ($allow_saves) {
    $headers[] = t('Saveable');
  }
  $rows = array();
  foreach ($argument_info as $name => $argument) {
    $row = array();
    $row[] = '$'. $name;
    $row[] = $argument['#entity'];
    $row[] = isset($argument['#label']) ? $argument['#label'] : '';
    if ($allow_saves) {
      if (isset($entity_info[$argument['#entity']]['#save']) && function_exists($entity_info[$argument['#entity']]['#save'])) {
        $row[] = (isset($argument['#saved']) && $argument['#saved']) ? t('Yes (Automatic)') : t('Yes');
      }
      else {
        $row[] = t('No');
      }
    }
    $rows[] = $row;
  }

  $output = theme('table', $headers, $rows, array('class' => 'description'));
  return $output;
}

/**
 * Adds PHP argument help to $form
 */
function workflow_ng_custom_php_help(&$form, $argument_info, $allow_saves = TRUE) {
  if (isset($argument_info) && count($argument_info)) {
    $form['php_argument_help'] = array(
      '#type' => 'fieldset',
      '#title' => t('PHP Arguments'),
      '#description' => t('You can make use of all available arguments.'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['php_argument_help']['content'] = array(
      '#value' => theme('php_argument_help', $argument_info, $allow_saves),
    );
  }
}

/**
 * Custom PHP action configuration form
 */
function workflow_ng_action_custom_php_form($settings = array(), $argument_info) {
  $form = array();
  workflow_ng_custom_php_help($form, $argument_info, TRUE);
  $form['php'] = array(
    '#type' => 'textarea',
    '#rows' => 12,
    '#title' => t('PHP code'),
    '#default_value' => $settings['php'],
    '#description' => t('The code that should be executed. You can use Tokens as variable values (e.g.: print "hello [user:user]";), or use variables themselves (e.g.: drupal_set_message("This is node $node->nid");), or a mixture of the two. To save variables, return an array of variables to save (e.g. return array("node" => $node);). Should not include &lt;?php ?&gt; delimiters.'),
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

/**
 * Custom PHP condition configuration form
 */
function workflow_ng_condition_custom_php_form($settings = array(), $argument_info) {
  $form = array();
  workflow_ng_custom_php_help($form, $argument_info, FALSE);
  $form['php'] = array(
    '#type' => 'textarea',
    '#rows' => 12,
    '#title' => t('PHP code'),
    '#default_value' => $settings['php'],
    '#description' => t('The code that should be executed. You can use Tokens as variable values (e.g.: return [node:nid] < 3 || [node:nid] > 5), or use variables themselves (e.g.: return $author->name != "bot";), or a mixture of the two. Be sure to always use `return` to indicate the final condition value. Should not include &lt;?php ?&gt; delimiters.'),
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

/**
 * Custom PHP action configuration form submission
 */
function workflow_ng_action_custom_php_submit($form_id, $form_values) {
  return array(
    'php' => $form_values['php'],
    'used_arguments' => workflow_ng_token_get_used_arguments($form_values['php'], $form_values['arguments']),
    'used_php_arguments' => workflow_ng_custom_php_get_used_php_arguments($form_values['php'], $form_values['arguments']),
  );
}

/**
 * Custom PHP condition configuration form validation
 * Checks for at least one return statement
 */
function workflow_ng_condition_custom_php_validate($form_id, $form_values) {
  if (strpos($form_values['php'], 'return') === FALSE) {
    form_set_error('php', t('Condition must include a return statement.'));
  }
}

/**
 * Custom PHP condition configuration form submission
 */
function workflow_ng_condition_custom_php_submit($form_id, $form_values) {
  return array(
    'php' => $form_values['php'],
    'used_arguments' => workflow_ng_token_get_used_arguments($form_values['php'], $form_values['arguments']),
    'used_php_arguments' => workflow_ng_custom_php_get_used_php_arguments($form_values['php'], $form_values['arguments']),
  );
}
