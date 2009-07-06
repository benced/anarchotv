<?php

// $Id: views_defaults.inc,v 1.1.2.3 2008/08/17 08:19:32 drewish Exp $

/**
 * Implementation of hook_views_default_views().
 */
function  image_gallery_views_default_views() {

  /**
   * Image gallery with taxonomy argument
   */
  $view = new stdClass();
  $view->name = 'image_gallery';
  $view->description = t('Basic image gallery. Embedded by the image_gallery module.');
  $view->access = array (
);
  $view->view_args_php = '';
  $view->page = TRUE;
  $view->page_title = 'Image galleries';
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = t('There are no images in this gallery.');
  $view->page_empty_format = '1';
  $view->page_type = 'image_gallery';
  $view->url = 'image';
  $view->use_pager = TRUE;
  $view->nodes_per_page = '6';
  $view->sort = array (
    array (
      'tablename' => 'node',
      'field' => 'sticky',
      'sortorder' => 'DESC',
      'options' => '',
    ),
    array (
      'tablename' => 'node',
      'field' => 'created',
      'sortorder' => 'DESC',
      'options' => 'normal',
    ),
  );
  $view->argument = array (
    array (
      'type' => 'taxid',
      'argdefault' => '7',
      'title' => '%1',
      'options' => '',
      'wildcard' => '',
      'wildcard_substitution' => '',
    ),
  );
  $view->field = array (
    array (
      'tablename' => 'image_node',
      'field' => 'nid',
      'label' => '',
      'handler' => 'image_views_handler_image_img_link',
      'options' => 'thumbnail',
    ),
    array (
      'tablename' => 'node',
      'field' => 'title',
      'label' => '',
      'handler' => 'views_handler_field_nodelink',
      'options' => 'link',
    ),
  );
  $view->filter = array (
    array (
      'tablename' => 'node',
      'field' => 'status',
      'operator' => '=',
      'options' => '',
      'value' => '1',
    ),
    array (
      'tablename' => 'node',
      'field' => 'type',
      'operator' => 'OR',
      'options' => '',
      'value' => array (
  0 => 'image',
),
    ),
    array (
      'tablename' => 'term_data',
      'field' => 'vid',
      'operator' => 'AND',
      'options' => '',
      'value' => array (
  0 => _image_gallery_get_vid(),
),
    ),
  );
  $view->exposed_filter = array (
  );
  $view->requires = array(node, image_node, term_data);
  $views[$view->name] = $view;


  return $views;
}
