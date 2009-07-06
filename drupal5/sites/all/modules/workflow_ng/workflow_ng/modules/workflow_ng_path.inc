<?php

/**
 * @file Workflow-ng integration for the path module 
 */

/**
 * Implementation of hook_module_info
 */
function path_module_info() {
  return array(
    'path_workflow_ng_forms' => array(
      'file' => 'workflow_ng_path_forms.inc',
      'file path' => drupal_get_path('module', 'workflow_ng'). '/modules/',
    ),
  );
}

/*
 * Implementation of hook_condition_info()
 */
function path_condition_info() {
  return array(
    'workflow_ng_condition_url_has_alias' => array(
      '#label' => t('Path has alias(es)'),
      '#module' => 'Path',
    ),
    'workflow_ng_condition_alias_exists' => array(
      '#label' => t('URL alias exists)'),
      '#module' => 'Path',
    ),
  );
}

/*
 * Condition implementation: Check if the path has an alias
 */
function workflow_ng_condition_url_has_alias($settings, &$arguments, &$log) {
  extract( workflow_ng_token_replace_all(array('src'), $settings, $arguments, $log) );
  return (bool)drupal_lookup_path('alias', $src);
}

/*
 * Condition implementation: Check if the URL alias already exists
 */
function workflow_ng_condition_alias_exists($settings, &$arguments, &$log) {
  extract( workflow_ng_token_replace_all(array('dst'), $settings, $arguments, $log) );

  $dst = workflow_ng_clean_path($dst, $settings['replace']);
  return (bool)drupal_lookup_path('source', $dst);
}

/*
 * Implementation of hook_action_info()
 */
function path_action_info() {
  return array(
    'workflow_ng_action_path_alias' => array(
      '#label' => t('Create or delete an URL alias'),
      '#module' => 'Path',
    ),
    'workflow_ng_action_node_path_alias' => array(
      '#label' => t('Create or delete a content\'s URL alias'),
      '#arguments' => array(
        'node' => array('#entity' => 'node', '#label' => t('Content')),
      ),
      '#module' => 'Path',
      '#description' => t('This action only works if the acting user has %perm1 and %perm2 permsissions. If this doesn\'t suit, use the generic "Create or delete an URL alias" action together with the existing system path "node/{ID}".', array('%perm1' => t('create url aliases'), '%perm2' => t('administer url aliases'))),
    ),
  );
}

/*
 * Action Implementation
 */
function workflow_ng_action_path_alias($settings, &$arguments, &$log) {
  extract( workflow_ng_token_replace_all(array('src', 'dst'), $settings, $arguments, $log) );
  $dst = workflow_ng_clean_path($dst, $settings['replace']);

  path_set_alias($src, $dst);
}

/*
 * Action Implementation
 */
function workflow_ng_action_node_path_alias($node, $settings, &$arguments, &$log) {
  extract( workflow_ng_token_replace_all(array('dst'), $settings, $arguments, $log) );

  $node->path = workflow_ng_clean_path($dst, $settings['replace']);
  return array('node' => $node);
}

/*
 * Cleans the given path by replacing non ASCII characters with the given 
 * $replace parameter
 * 
 * Furthermore it allows customization by overriding the variable workflow_ng_clean_path in
 * the settings.php
 */
function workflow_ng_clean_path($path, $replace) {
  if ($replace) {
    $array = variable_get('workflow_ng_clean_path', array('/[^a-zA-Z0-9\/\-_]+/', $replace));
    $array[2] = $path;
    //replace it and remove trailing and leading replacement characters
    return rtrim(ltrim(call_user_func_array('preg_replace', $array), $replace), $replace);
  }
  else {
    return $path;     
  }
}
