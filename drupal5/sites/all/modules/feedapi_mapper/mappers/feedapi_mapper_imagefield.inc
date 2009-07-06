<?php
// $Id: feedapi_mapper_imagefield.inc,v 1.1.2.1 2008/09/22 17:08:56 alexb Exp $

/**
 * @file
 * Implements a FeedAPI mapper on behalf of the ImageField module.
 * @link http://drupal.org/project/imagefield
 */

/**
 * Implementation of hook_feedapi_mapper().
 */
function imagefield_feedapi_mapper($op, $node, $field_name, $feed_element = array(), $sub_field = '') {
  if (!($field = feedapi_mapper_content_is_cck_type($field_name, array('image')))) {
    // Not an imagefield field.
    return;
  }

  switch ($op) {
    case 'describe':
      return t('Maps image URLs to CCK ImageFields. The image will be downloaded and saved as a local file. You can choose whether to keep the original file name or use a hash value of the URL. The latter is better for images that are returned by scripts (like getimage.php?foo=), otherwise they would all be saved as "getimage.php".');

    case 'list':
      $sub_fields = array(
        'original' => t('Original file name'),
        'md5' => t('Hashed URL'),
      );
      return $sub_fields;

    case 'map':
      if ($feed_element) {
        $items = (array)$node->$field_name;
        foreach ((array)$feed_element as $element) {
          if (valid_url($element) && $file = _feedapi_mapper_imagefield_file_insert($element, $sub_field, $node->nid)) {
            // Always add as last, Image Field takes care of deleting the
            // first image for non-multiple fields.
            $items[] = $file;
            if (!$field['multiple']) {
              break;
            }
          }
        }
        $node->$field_name = $items;
      }
      return $node;
  }
}

function _feedapi_mapper_imagefield_file_insert($url, $filename_option, $nid = NULL) {
  static $blacklist;
  if (!isset($blacklist)) {
    // The blacklist is an associative array consisting of an MD5 fingerprint
    // and keyed by file size.
    $blacklist = variable_get('imagefield_mapper_blacklist', array());
  }

  // Download the image.
  $result = drupal_http_request($url);
  if ($result->code != 200) {
    return FALSE;
  }

  // Check for blacklisted image.
  $filesize = strlen($result->data);
  if (isset($blacklist[$filesize]) && md5($result->data) == $blacklist[$filesize]) {
    return FALSE;
  }

  // Build the filename.
  $mime_type = $result->headers['Content-Type'];
  $filename = _feedapi_mapper_imagefield_get_filename($filename_option, $url, $mime_type);
  if (!$filename) {
    return FALSE;
  }
  $filepath = file_directory_temp() .'/'. $filename;

  // Avoid creating dupes while updating.
  if ($nid && db_result(db_query("SELECT fid FROM {files} WHERE nid = %d AND filename = '%s' AND filesize = %d", $nid, $filename, $filesize))) {
    return FALSE;
  }

  if ($filepath = file_save_data($result->data, $filepath)) {
    // Construct the $file object. The file will be moved to the configured
    // directory and the record will be inserted by the repective imagefield
    // functions. File id 'upload' is required while updating a node.
    // @see imagefield_insert_file(), imagefield_update_file()
    return array(
      'fid' => 'upload',
      'filename' => $filename,
      'filepath' => $filepath,
      'filemime' => $mime_type,
      'filesize' => $filesize,
    );
  }

  return FALSE;
}

/**
 * Get local filename from image URL.
 *
 * @param $filename_option
 *   The selected filename generation option.
 * @param $url
 *   The feed URL.
 * @param &$mime_type
 *   The MIME type of the downloaded image.
 * @return
 *   The local filename or FALSE if the file was no image.
 */
function _feedapi_mapper_imagefield_get_filename($filename_option, $url, &$mime_type) {
  // Figure out the proper file extension from the MIME type.
  static $types = array(
    'image/gif' => '.gif',
    'image/jpeg' => '.jpg',
    'image/png' => '.png',
    'image/psd' => '.psd',
    'image/bmp' => '.bmp',
    'image/tiff' => '.tif',
    'image/jp2' => '.jp2',
    'image/iff' => '.iff',
    'image/vnd.wap.wbmp' => '.wbmp',
    'image/xbm' => '.xbm',
    'image/vnd.microsoft.icon' => '.ico',
  );
  if (!isset($types[$mime_type])) {
    return FALSE;
  }

  $ext = $types[$mime_type];

  // Create filename.
  switch ($filename_option) {
    case 'original':
      $components = parse_url($url);
      $filename = basename($components['path']);
      if (strrchr($filename, '.') != $ext) {
        $filename .= $ext;
      }
      return $filename;

    case 'md5':
      return md5($url) . $ext;
  }
}

