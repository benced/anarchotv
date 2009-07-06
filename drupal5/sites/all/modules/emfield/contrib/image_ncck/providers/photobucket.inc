<?php
// $Id: photobucket.inc,v 1.1 2007/08/18 18:25:58 aaron Exp $

define('IMAGE_NCCK_PHOTOBUCKET_MAIN_URL', 'http://www.photobucket.com/');

/**
 * hook image_ncck_PROVIDER_info
 * this returns information relevant to a specific 3rd party image provider
 * @return
 *   an array of strings requested by various admin and other forms
 *   'name' => the translated name of the provider
 *   'url' => the url to the main page for the provider
 *   'settings_description' => a description of the provider that will be posted in the admin settings form
 *   'supported_features' => an array of rows describing the state of certain supported features by the provider.
 *      These will be rendered in a table, with the columns being 'Feature', 'Supported', 'Notes'.
 */

function image_ncck_photobucket_info() {
  $name = t('Photobucket');
  $features = array();
  return array(
    'provider' => 'photobucket',
    'name' => $name,
    'url' => IMAGE_NCCK_PHOTOBUCKET_MAIN_URL,
    'settings_description' => t('These settings specifically affect images displayed from !photobucket.', array('!photobucket' => l($name, IMAGE_NCCK_PHOTOBUCKET_MAIN_URL, array('target' => '_blank')))),
    'supported_features' => $features,
  );
}

function image_ncck_photobucket_data($field, $item) {
  $data = array();

  if(preg_match('![si]([^/.:@]+)\.photobucket\.com/albums/([^/]+)/([^/]+)/(\?action=view&current=)?(.+)$!i', $item['embed'], $matches)) {
    $data = array(
      'server' => $matches[1],
      'album' => $matches[2],
      'owner' => $matches[3],
      'file' => $matches[5],
    );
    $data['title'] = image_ncck_photobucket_image_title($data['file'], $data);
  }
  return $data;
}

function image_ncck_photobucket_extract($embed = '') {
  // http://s201.photobucket.com/albums/aa274/layoutqueenie/?action=view&current=baileys_in_gardens.jpg
  // http://i201.photobucket.com/albums/aa274/layoutqueenie/baileys_in_gardens.jpg
  if(preg_match('![si]([^/.:@]+)\.photobucket\.com/albums/([^/]+)/([^/]+)/(\?action=view&current=)?(.+)$!i', $embed, $matches)) {
    return $matches[5];
  }
  return array();
}

/**
 * hook image_ncck_PROVIDER_embedded_link($code)
 * returns a link to view the content at the provider's site
 *  @param $code
 *    the string containing the content to watch
 *  @return
 *    a string containing the URL to view the image at the original provider's site
 */
function image_ncck_photobucket_embedded_link($code, $data) {
  return "http://s{$data['server']}.photobucket.com/albums/{$data['album']}/{$data['owner']}/?action=view&current={$code}";
}

/**
 *  implement image_ncck_PROVIDER_image_url
 *
 *  @param $code
 *    the code of the image
 *  @param $data
 *    any stored data for the image, which may already have the title
 *  @return
 *    the url directly to the image to display
 */
function image_ncck_photobucket_image_url($code, $data) {
  if (func_num_args() == 7) {
    $arg = func_get_arg(5);
    $code = &$arg['data']['file'];
    $data = &$arg['data'];
  }

  return "http://i{$data['server']}.photobucket.com/albums/{$data['album']}/{$data['owner']}/{$code}";
}

/**
 *  implement image_ncck_PROVIDER_image_title
 *
 *  @param $code
 *    the code of the image
 *  @param $data
 *    any stored data for the image, which may already have the title
 *  @return
 *    the title as the 3rd party provider knows it, if accessible to us. otherwise, ''
 */
function image_ncck_photobucket_image_title($code, $data) {
  if ($data['title']) {
    return $data['title'];
  }
  $url = image_ncck_photobucket_embedded_link($code, $data);
  return _image_ncck_photobucket_scrape_image_title($url);
}

/**
 * Visit the image URL and scrape the image title from HTML.
 *
 * @param String $url
 *   Image URL.
 * @return String
 *   Image title.
 */
function _image_ncck_photobucket_scrape_image_title($url) {
  static $title;
  if (isset($title[$url])) {
    return $title[$url];
  }

  $html = file_get_contents($url);
  return $title[$url] = preg_match('@<span id="photoTitle">(.+?)</span>@is', $html, $matches)? $matches[1] : '';
}
