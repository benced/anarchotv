<?php

/*
 * Implementation of hook_action_info()
 */
function cclinks_action_info() {
  return array(
    'cclinks_set_schedule_date' => array(
      '#label' => t('Set event scheduling date'),
      '#arguments' => array(
        'entity' => array('#entity' => array('node'), '#label' => t('Content')),
      ),
      '#module' => 'cclinks',
    ),
  );
}

/*
 * Action: Schedule event generation
 */
function cclinks_set_schedule_date($node, $settings, &$arguments, &$log) {
  $date = workflow_ng_token_replace($settings['date'], $settings['date_args'], $arguments, $log);
  cclinks_link_apply_schedule_date($node, $settings['event'], $date);
}

/*
 * Configuration form
 */
function cclinks_set_schedule_date_form($settings = array(), $argument_info) {
  $form['event'] = array(
   '#type' => 'select',
   '#title' => t('Choose the link for which the event will be scheduled'),
   '#options' => cclinks_get_link_options(),
   '#default_value' => $settings['event'],
  );
  $form['date'] = array(
   '#type' => 'textarea', 
   '#title' => t('Date'),
   '#default_value' => isset($settings['date']) ? $settings['date'] : date('Y-m-d H:i:s', time()),
   '#description' => t('Format: %time. To generate the event without delay just enter "%now". To abort a schedule leave it empty.', array('%time' => date('Y-m-d H:i:s'), '%now' => t('now'))),
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

function cclinks_set_schedule_date_submit($form_id, $form_values) {
  $vars = drupal_map_assoc(array('date', 'event'));
  $settings = array('date_args' => workflow_ng_token_get_used_arguments($form_values['date'], $form_values['arguments']));
  return array_intersect_key($form_values, $vars) + $settings;
}

/*
 * Returns an array suitable for use with #options
 */
function cclinks_get_link_options() {
  $options = array();
  foreach (cclinks_get_events() as $event_name => $settings) {
    $options[$event_name] = check_plain($settings['label']);
  }
  return $options;
}
