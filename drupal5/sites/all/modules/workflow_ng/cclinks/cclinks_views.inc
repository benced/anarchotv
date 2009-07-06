<?php
// $Id: cclinks_views.inc,v 1.1.2.1 2008/03/05 10:35:45 fago Exp $

 /*
 * @file
 * Provides views integration for the cclinks module
  */

/**
 * Implementation of hook_views_tables
 */
function cclinks_views_tables() {
  $tables['cclink_schedules'] = array(
    'name' => 'cclink_schedules',
    'join' => array(
      'left' => array(
        'table' => 'node',
        'field' => 'vid'
      ),
      'right' => array(
        'field' => 'vid'
      ),
    ),
    'fields' => array(
      'event' => array(
        'name' => t('CCLinks: Link label'),
        'handler' => 'cclinks_views_handler_event',
        'sortable' => false,
        'help' => t('This field displays the link\'s label.'),
      ),
      'date' => array(
        'name' => t('CCLinks: Scheduled date'),
        'sortable' => true,
        'handler' => views_handler_field_dates(),
        'option' => 'string',
        'help' => t('This field displays the scheduled date of a link\'s event.'),
      ),
    ),
    'sorts' => array(
      'date' => array(
        'name' => t('CCLinks: Scheduled date'),
        'handler' => 'views_handler_sort_date',
        'option' => views_handler_sort_date_options(),
        'help' => t('Sorts by the scheduling date.'),
      ),
    ),
    'filters' => array(
      'date' => array(
        'name' => t('CCLinks: Scheduled date'),
        'operator' => 'views_handler_operator_gtlt',
        'value' => views_handler_filter_date_value_form(),
        'handler' => 'views_handler_filter_timestamp',
        'option' => 'string',
        'help' => t('This filters by the scheduled date.')
          .' '. views_t_strings('filter date'),
      ),
      'event' => array(
        'name' => t('CCLinks: Link label'),
        'operator' => 'views_handler_operator_or',
        'value' => array(
          '#title' => t('Machine'),
          '#type' => 'select',
          '#options' => 'cclinks_get_labels',
          '#multiple' => TRUE,
        ),
        'value-type' => 'array',
        'help' => t('This allows you to filter by link label.'),
      ),
    ),
  );
  return $tables;
}

function cclinks_views_handler_event($fieldinfo, $fielddata, $value, $data) {
  $labels = cclinks_get_labels();
  return $labels[$value];
}
