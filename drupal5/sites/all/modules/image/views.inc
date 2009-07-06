<?php
// $Id: views.inc,v 1.4.2.1 2008/01/06 23:00:54 drewish Exp $

/**
 * Implementation of hook_views_tables()
 */
function image_views_tables() {
  $tables['image_node'] = array(
    'name' => 'node',
    'fields' => array(
      'nid' => array(
        'name' => t('Image: Display Image'),
        'handler' => array(
          'image_views_handler_image_img' => t('Image'),
          'image_views_handler_image_img_link' => t('Image with link'),
        ),
        'option' => array(
          '#type' => 'select',
          '#options' => 'image_views_handler_filter_image_size',
        ),
        'notafield' => true,
        'sortable' => false,
      ),
    ),
  );
  $tables['image_image'] = array(
    'name' => 'image',
    'join' => array(
      'left' => array(
        'table' => 'node',
        'field' => 'nid',
      ),
      'right' => array(
        'field' => 'nid',
      ),
    ),
    'fields' => array(
      'fid' => array(
        'name' => t('Image: File Id'),
        'sortable' => true,
        'help' => t('File Id which represents the file.'),
      ),
    ),
  );
  $tables['image_files'] = array(
    'name' => 'files',
    'join' => array(
      'left' => array(
        'table' => 'image_image',
        'field' => 'fid',
      ),
      'right' => array(
        'field' => 'fid'
      ),
    ),
    'fields' => array(
      'filename' => array(
        'name' => t('Image: File name'),
        'handler' => array(
          'views_handler_file_filename' => t('Plain'),
          'views_handler_file_filename_download' => t('With download link'),
        ),
        'sortable' => true,
        'addlfields' => array('filepath'),
        'option' => 'string',
        'help' => t('Display file name'),
      ),
      'filepath' => array(
        'name' => t('Image: File path'),
        'sortable' => false,
        'help' => t('Display Path to File.'),
      ),
      'filesize' => array(
        'name' => t('Image: File size'),
        'handler' => 'views_handler_file_size',
        'sortable' => true,
        'help' => t('Display the file size of the associated file.'),
      ),
      'filemime' => array(
        'name' => t('Image: Mime type'),
        'sortable' => true,
        'help' => t('This filter allows nodes to be filtered by mime type.'),
      ),
    ),
  );
  return $tables;
}

/**
 * Views handler for displaying the image.
 */
function image_views_handler_image_img($fieldinfo, $fielddata, $value, $data) {
  $node = node_load($data->nid);
  return image_display($node, $fielddata['options']);
}

/**
 * Views handler for displaying the image in a link to the the image node
 */
function image_views_handler_image_img_link($fieldinfo, $fielddata, $value, $data) {
  $node = node_load($data->nid);
  return l(image_display($node, $fielddata['options']), "node/{$node->nid}", array(), NULL, NULL, FALSE, TRUE);
}

/**
 * Views - Generate a list of all the valid sizes that are available
 */
function image_views_handler_filter_image_size($op) {
  foreach (_image_get_sizes() as $key => $size) {
    $a[$key] = $size['label'];
  }
  return $a;
}

/**
 * Implementation of hook_views_style_plugins()
 */
function image_views_style_plugins() {
  $items['image_gallery'] = array(
    'name' => t('Image: Gallery'),
    'theme' => 'image_gallery_view',
    'validate' => 'views_ui_plugin_validate_list',
    'needs_fields' => TRUE,
  );
  return $items;
}

/**
 * Display the nodes of a view as an image gallery.
 */
function theme_image_gallery_view($view, $nodes, $type) {
  drupal_add_css(drupal_get_path('module', 'image_gallery') .'/image_gallery.css');

  $fields = _views_get_fields();

  // get the size of images
  // TODO: If the user adds more than one image field with different sizes... this breaks
  foreach ($view->field as $field) {
    if ($field['tablename'] == 'image_node') {
      $size = image_get_sizes($field['options']);
    }
  }
  $width = $size['width'];
  // We'll add height to keep thumbnails lined up.
  $height = $size['height'] + 25;

  $content .= '<ul class="images clear-block">'; 

  foreach ($nodes as $node) {
    $item = '';

    foreach ($view->field as $field) {
     if ($fields[$field['id']]['visible'] !== FALSE) {
        if ($field['label']) {
          $item .= "<div class='view-label view-label-$field[queryname]'>". $field['label'] .'</div>';
        }
        $item .= "<div class='view-field view-data-$field[queryname]'>". views_theme_field('views_handle_field', $field['queryname'], $fields, $field, $node, $view) .'</div>';
      }
    }
    $content .= "<li class='' style='height : {$height}px; width : {$width}px;'><div class='view-item view-item-$view->name'>$item</div></li>\n"; 
  }

  $content .= "</ul>\n";

  return $content;
}
