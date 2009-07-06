<?php

/**
 * @file Workflow-ng integration for the comment module 
 */

/** 
 * Implementation of hook_event_info()
 */
function comment_event_info() {
  return array(
    'comment_insert' => array(
      '#label' => t('Comment has been created'),
      '#module' => 'Comment',
      '#arguments' => workflow_ng_events_hook_comment_arguments(t('created comment')),
      '#redirect' => TRUE,
    ),
    'comment_update' => array(
      '#label' => t('Comment has been updated'),
      '#module' => 'Comment',
      '#arguments' => workflow_ng_events_hook_comment_arguments(t('updated comment')),
      '#redirect' => TRUE,
    ),
    'comment_delete' => array(
      '#label' => t('Comment has been deleted'),
      '#module' => 'Comment',
      '#arguments' => workflow_ng_events_hook_comment_arguments(t('deleted comment')),
      '#redirect' => TRUE,
    ),
  );
}

/*
 * Returns some arguments suitable for hook comment
 */
function workflow_ng_events_hook_comment_arguments($comment_label) {
  return array(
    'comment' => array('#entity' => 'comment', '#label' => $comment_label),
    'comment_author' => array('#entity' => 'user', '#label' => $comment_label .' '. t('author'), '#handler' => 'workflow_ng_events_argument_comment_author'),
    'node' => array('#entity' => 'node', '#label' => t('commented content'), '#handler' => 'workflow_ng_events_argument_comment_node'),
    'node_author' => array('#entity' => 'user', '#label' => t('commented content author'), '#handler' => 'workflow_ng_events_argument_comment_node_author'),
  ) + workflow_ng_events_global_user_argument();
}

/*
 * Implementation of hook_comment
 */
function workflow_ng_comment($comment, $op) {
  if (in_array($op, array('insert', 'update', 'delete'))) {
    if (is_array($comment)) {
      $comment = (object)$comment;
    }
    workflow_ng_invoke_event('comment_'. $op, $comment);
  }
}


/*
 * Gets the author of the comment
 */
function workflow_ng_events_argument_comment_author($comment) {
  return user_load(array('uid' => $comment->uid));
}

/*
 * Gets the comment's node
 */
function workflow_ng_events_argument_comment_node($comment) {
  return node_load($comment->nid);
}

/*
 * Gets the comment's node's author
 */
function workflow_ng_events_argument_comment_node_author($comment) {
  return workflow_ng_events_argument_node_author(workflow_ng_events_argument_comment_node($comment));
}
