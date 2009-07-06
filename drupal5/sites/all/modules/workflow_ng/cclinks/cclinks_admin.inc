<?php

/**
 * @file Configurable Content Links - Admin pages
 */


//admin interface path
define('CCLINKS_PATH', 'admin/workflow-ng/cclinks');

function cclinks_help($section) {
  if ($section == CCLINKS_PATH) {
    return '<p>'. t('Don\'t forget to adjust the !link permissions for your content links.', array('!link' => l(t('access control'), 'admin/user/access', array(), drupal_get_destination())));
  }
}


/**
 * Called by cclinks' of hook_menu().
 */
function cclinks_admin_menu($items, $may_cache) {
  if ($may_cache) {
    $items[] = array(
      'path' => CCLINKS_PATH,
      'title' => t('Content links'),
      'callback' => 'cclinks_overview',
      'access' => user_access('administer workflow-ng'),
      'description' => t('Create configurable content links, which generate events when they are pressed.'),
      'type' => MENU_NORMAL_ITEM,
      'weight' => -1,
    );
    $items[] = array(
      'path' => CCLINKS_PATH .'/list',
      'title' => t('Overview'),
      'weight' => -5,
      'type' => MENU_DEFAULT_LOCAL_TASK,
    );
    $items[] = array(
      'path' => CCLINKS_PATH .'/add',
      'title' => t('Add a content link'),
      'callback' => 'drupal_get_form',
      'callback arguments' => array('cclinks_edit_form'),
      'access' => user_access('administer workflow-ng'),
      'type' => MENU_LOCAL_TASK,
    );
    $items[] = array(
      'path' => CCLINKS_PATH .'/edit',
      'title' => t('Edit a content link'),
      'callback' => 'drupal_get_form',
      'callback arguments' => array('cclinks_edit_form'),
      'access' => user_access('administer workflow-ng'),
      'type' => MENU_CALLBACK,
    );
    $items[] = array(
      'path' => CCLINKS_PATH .'/delete',
      'title' => t('Delete content link'),
      'callback' => 'drupal_get_form',
      'callback arguments' => array('cclinks_delete_form'),
      'access' => user_access('administer workflow-ng'),
      'type' => MENU_CALLBACK,
    );
  }
  return $items;
}

/*
 * Shows all configured events
 */
function cclinks_overview() {
  $cclinks = cclinks_get_events();
  if (count($cclinks) == 0) {
    $output = t('There are no configured links yet.');
    $output .= '<p>'. l(t('Add a link'), CCLINKS_PATH. '/add') . '</p>';
  }
  else {
    $headers = array(t('Label'), t('Active'), t('Schedules event'), t('Operations'));
    $rows = array(); 
    uasort($cclinks, '_cclinks_sort');
    foreach ($cclinks as $event_name => $settings) {
      $row = array();
      $row[] = check_plain($settings['label']);
      $row[] = $settings['active'] ? t('Active') : t('Inactive');
      $row[] = $settings['schedule'] ? t('Yes') : t('No');
      $row[] = l(t('edit'), CCLINKS_PATH. '/edit/'. $event_name);
      $row[] = l(t('delete'), CCLINKS_PATH. '/delete/'. $event_name);
      $rows[] = $row;
    }
    $output .= theme('table', $headers, $rows);
  }
  return $output;
}


/*
 * Shows an edit or add form for a cclink
 * If no id is given, an add form will be shown
 */
function cclinks_edit_form($event_name = '') {
  $cclinks = cclinks_get_events();
  if ($event_name && isset($cclinks[$event_name])) {
    $settings = $cclinks[$event_name];
  }
  else {
    $event_name = 'cclink'. cclinks_get_next_id();
    $settings = array();
  }
  $form['event_name'] = array('#type' => 'value', '#value' => $event_name);
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#description' => t('Choose a label which will be associated with this link. It will also be the label of the associated event, which will appear in workflow-ng.'),
    '#required' => TRUE,
    '#default_value' => $settings['label'],
  );
  $form['active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Active'),
    '#required' => FALSE,
    '#default_value' => isset($settings['active']) ? $settings['active'] : 1,
  );
  $form['types'] = array(
    '#type' => 'select',
    '#title' => t('Content types'),
    '#options' => node_get_types('names'),
    '#multiple' => TRUE,
    '#required' => TRUE,
    '#default_value' => $settings['types'],
  );
  $form['link_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Link label'),
    '#description' => t('The label of the link shown at the content.'),
    '#required' => TRUE,
    '#default_value' => $settings['link_label'],
  );
  $form['toggle_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Toggled link label'),
    '#description' => t('If given, toggle the link\'s label with this one after it got pressed. Leave it empty, to disable toggling.'),
    '#default_value' => $settings['toggle_label'],
  );
  $form['schedule'] = array(
    '#type' => 'checkbox',
    '#title' => t('Schedule event occurance'),
    '#description' => t('If activated, users have to specify the time when the workflow-ng event will occur after clicking the link.'),
    '#default_value' => $settings['schedule'],
  );
  $form['schedule_form'] = array(
    '#type' => 'fieldset',
    '#title' => t('Schedule Date Form'),
    '#description' => t('If scheduling is activated, this settings will apply to the form for entering the schedule date.'),
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['schedule_form']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Page title'),
    '#description' => t('The title of the page for entering the scheduling date.'),
  );
  $form['schedule_form']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Page title'),
    '#description' => t('The title of the page for entering the scheduling date.'),
    '#default_value' => $settings['schedule_form']['title'],
  );
  $form['schedule_form']['submit'] = array(
    '#type' => 'textfield',
    '#title' => t('Label of the submit button'),
    '#default_value' => isset($settings['schedule_form']['submit']) ? $settings['schedule_form']['submit'] : 'Submit',
  );
  $form['schedule_form']['abort'] = array(
    '#type' => 'textfield',
    '#title' => t('Label of the abort button'),
    '#default_value' => isset($settings['schedule_form']['abort']) ? $settings['schedule_form']['abort'] : 'Abort',
  );
  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Link weight'),
    '#default_value' => isset($settings['weight']) ? $settings['weight'] : 0,
  );
  $form['submit'] = array('#type' => 'submit', '#weight' => 10, '#value' => t('Submit'));
  return $form;
}

function cclinks_edit_form_submit($form_id, $form_values) {
  $vars = drupal_map_assoc(cclinks_setting_properties());
  cclinks_save_event($form_values['event_name'], array_intersect_key($form_values, $vars));
  drupal_set_message(t('Your changes have been saved.'));
  return CCLINKS_PATH;
}


/*
 * Shows a delete confirmation
 */
function cclinks_delete_form($event_name = '') {
  $cclinks = cclinks_get_events();
  if (isset($cclinks[$event_name])) {
    $form['event_name'] = array('#type' => 'value', '#value' => $event_name);
    $form['settings'] = array('#type' => 'value', '#value' => $cclinks[$event_name]);
    $text = t('Are you sure you want to delete the link %label?', array('%label' => $cclinks[$event_name]['label']));
    $path = isset($_GET['destination']) ? $_GET['destination'] : CCLINKS_PATH;
    return confirm_form($form, $text, $path, t('All configurations that might be configured for this event will be deleted too. This action cannot be undone.'), t('Delete'), t('Cancel'));
  }
}

function cclinks_delete_form_submit($form_id, $form_values) {
  if ($form_values['confirm']) {
    cclinks_delete_event($form_values['event_name']);
    drupal_set_message(t('The link %label has been successfully deleted.', array('%label' => $form_values['settings']['label'])));
  }
  return CCLINKS_PATH;
}

