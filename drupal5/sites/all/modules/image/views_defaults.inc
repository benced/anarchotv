<?php

// $Id: views_defaults.inc,v 1.1.2.1 2008/01/06 23:00:54 drewish Exp $

/**
 * Implementation of hook_views_default_views().
 */
function  image_views_default_views() {

  /**
   * Latest image block & page
   */
  $view = new stdClass();
  $view->name = 'image_latest';
  $view->description = 'Block and gallery page of recent images.';
  $view->access = array (
);
  $view->view_args_php = '';
  $view->page = TRUE;
  $view->page_title = 'Latest images';
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = '';
  $view->page_empty_format = '1';
  $view->page_type = 'image_gallery';
  $view->url = 'image/recent';
  $view->use_pager = TRUE;
  $view->nodes_per_page = '6';
  $view->menu = TRUE;
  $view->menu_title = 'Latest images';
  $view->menu_tab = FALSE;
  $view->menu_tab_weight = '0';
  $view->menu_tab_default = FALSE;
  $view->menu_tab_default_parent = NULL;
  $view->menu_tab_default_parent_type = 'tab';
  $view->menu_parent_tab_weight = '0';
  $view->menu_parent_title = '';
  $view->block = TRUE;
  $view->block_title = 'Latest image';
  $view->block_header = '';
  $view->block_header_format = '1';
  $view->block_footer = '';
  $view->block_footer_format = '1';
  $view->block_empty = '';
  $view->block_empty_format = '1';
  $view->block_type = 'image_gallery';
  $view->nodes_per_block = '1';
  $view->block_more = TRUE;
  $view->block_use_page_header = FALSE;
  $view->block_use_page_footer = FALSE;
  $view->block_use_page_empty = FALSE;
  $view->sort = array (
    array (
      'tablename' => 'node',
      'field' => 'created',
      'sortorder' => 'DESC',
      'options' => 'normal',
    ),
  );
  $view->argument = array (
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
  );
  $view->exposed_filter = array (
  );
  $view->requires = array(node, image_node);
  $views[$view->name] = $view;


  /**
   * Random image block
   */
  $view = new stdClass();
  $view->name = 'image_random';
  $view->description = 'Random image';
  $view->access = array (
);
  $view->view_args_php = '';
  $view->page = FALSE;
  $view->page_title = '';
  $view->page_header = '';
  $view->page_header_format = '1';
  $view->page_footer = '';
  $view->page_footer_format = '1';
  $view->page_empty = '';
  $view->page_empty_format = '1';
  $view->page_type = 'image_gallery';
  $view->url = '';
  $view->use_pager = TRUE;
  $view->nodes_per_page = '10';
  $view->block = TRUE;
  $view->block_title = 'Random image';
  $view->block_header = '';
  $view->block_header_format = '1';
  $view->block_footer = '';
  $view->block_footer_format = '1';
  $view->block_empty = '';
  $view->block_empty_format = '1';
  $view->block_type = 'image_gallery';
  $view->nodes_per_block = '1';
  $view->block_more = FALSE;
  $view->block_use_page_header = FALSE;
  $view->block_use_page_footer = FALSE;
  $view->block_use_page_empty = FALSE;
  $view->sort = array (
    array (
      'tablename' => 'node',
      'field' => 'random',
      'sortorder' => 'ASC',
      'options' => '',
    ),
  );
  $view->argument = array (
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
  );
  $view->exposed_filter = array (
  );
  $view->requires = array(node, image_node);
  $views[$view->name] = $view;


  return $views;
}
