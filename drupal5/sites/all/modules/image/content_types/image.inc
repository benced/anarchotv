<?php
// $Id: image.inc,v 1.1.2.1 2008/08/17 08:44:25 drewish Exp $

/**
 * Callback function to supply a list of content types.
 */
function image_image_panels_content_types() {
  $items['image'] = array(
    'title' => t('Image'),
    'weight' => 10,
    // Only provides a single content type.
    'single' => TRUE,
    'content_types' => 'panels_image_content_types',
    'render callback' => 'panels_image_render',
    'add callback' => 'panels_image_add',
    'edit callback' => 'panels_image_edit',
    'add validate callback' => 'panels_image_edit_validate',
    'edit validate callback' => 'panels_image_edit_validate',
    'title callback' => 'panels_image_title',
  );
  return $items;
}

/**
 * Return all content types available.
 */
function panels_image_content_types() {
  return array(
    'image' => array(
      'title' => t('Image'),
      'weight' => 5,
      'icon' => 'icon_node.png',
      'path' => panels_get_path('content_types/node'),
      'description' => t('Add a image as content.'),
      'category' => array(t('Images'), 5),
    ),
  );
}

/**
 * Output function for the 'image' content type. Outputs an image node
 * based on the module and delta supplied in the configuration.
 */
function panels_image_render($conf, $panel_args) {
  $nid = $conf['nid'];

  if (!is_numeric($nid)) {
    return;
  }

  $node = node_load($nid);
  if (!node_access('view', $node)) {
    return;
  }

  $block->module = 'node';
  $block->delta = $node->nid;

  if (!empty($conf['image_size'])) {
    $node->panel_image_size = $conf['image_size'];
  }
  if (!empty($conf['caption_type'])) {
    if ($conf['caption_type'] == 'custom' && !empty($conf['caption_text'])) {
      $node->panel_image_caption = check_plain($conf['caption_text']);
    }
    elseif ($conf['caption_type'] == 'body') {
      $node->panel_image_caption = check_markup($node->body, $node->format, FALSE);
    }
  }
  $block->content = node_view($node, FALSE, FALSE, FALSE);
  return $block;
}

/**
 * Returns the form for a new image.
 */
function panels_image_add($id, $parents, $conf = array()) {
  $form = panels_image_edit($id, $parents, $conf);
  $form['nid'] = array(
    '#prefix' => '<div class="no-float">',
    '#suffix' => '</div>',
    '#title' => t('Enter the title or NID of an image'),
    '#type' => 'textfield',
    '#maxlength' => 512,
    '#autocomplete_path' => 'panels/node/autocomplete',
    '#weight'  => -10,
  );
  $form['validate_me'] = array('#type' => 'value', '#value' => TRUE);
  return $form;
}

/**
 * Returns an edit form for the custom type.
 */
function panels_image_edit($id, $parents, $conf) {
  $form['nid'] = array(
    '#type' => 'value',
    '#default_value' => $conf['nid'],
  );
  if (function_exists('image_get_sizes')) {
    foreach (image_get_sizes() as $key => $size) {
      $dimensions = $size['width'] .'x'. $size['height'];
      $label = $size['label'];
      if ($dimensions != 'x') {
        $label .= ' '. $dimensions;
      }
      $image_sizes[$key] = $label;
    }
    $form['image_size'] = array(
      '#type' => 'select',
      '#title' => t('Image size'),
      '#options' => $image_sizes,
      '#description' => t('What size of the image should be inserted?'),
    );
    if (isset($conf['image_size'])) {
      $form['image_size']['#default_value'] = $conf['image_size'];
    }
  }
  $form['caption_type'] = array(
    '#type' => 'select',
    '#title' => t('Image caption'),
    '#options' => array(
      'none' => t('None'),
      'body' => t('Existing caption'),
      'custom' => t('Custom caption'),
    ),
    '#default_value' => isset($conf['caption_type']) ? $conf['caption_type'] : 'none',
    '#description' => t('Which caption (if any) should be displayed?'),
  );
  $form['caption_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom caption'),
    '#default_value' => isset($conf['caption_text']) ? $conf['caption_text'] : '',
  );
  return $form;
}

/**
 * Validate a image.
 */
function panels_image_edit_validate($form, $form_values) {
  if (!$form_values['validate_me']) {
    return;
  }

  $nid = $form_values['nid'];
  $preg_matches = array();
  $match = preg_match('/\[nid: (\d+)\]/', $nid, $preg_matches);
  if (!$match) {
    $match = preg_match('/^nid: (\d+)/', $nid, $preg_matches);
  }

  if ($match) {
    $nid = $preg_matches[1];
  }
  if (is_numeric($nid)) {
    $node = db_fetch_object(db_query(db_rewrite_sql("SELECT n.nid FROM {node} n WHERE n.nid = %d AND n.type = 'image'"), $nid));
  }
  else {
    $node = db_fetch_object(db_query(db_rewrite_sql("SELECT n.nid FROM {node} n WHERE LOWER(n.title) = LOWER('%s') AND n.type = 'image'"), $nid));
  }
  if ($node) {
    form_set_value($form['nid'], $node->nid);
  }

  if (!($node || preg_match('/^[@%]\d+$/', $nid))) {
    form_error($form['nid'], t('Invalid image'));
  }
}

/**
 * Returns the administrative title for a image.
 */
function panels_image_title($conf) {
  $node = node_load($conf['nid']);
  if ($node) {
    $title = '<strong>'. t('Image:') .'</strong> '. check_plain($node->title);
    $title .= ' ['. $conf['image_size'] .']';
    return $title;
  }
  else {
    return t('Deleted/missing node @nid', array('@nid' => $conf['nid']));
  }
}
