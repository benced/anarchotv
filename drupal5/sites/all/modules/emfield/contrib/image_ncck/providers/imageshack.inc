<?php
// $Id: imageshack.inc,v 1.1 2007/08/19 00:04:43 aaron Exp $

define('IMAGE_NCCK_IMAGESHACK_MAIN_URL', 'http://www.imageshack.us/');

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

function image_ncck_imageshack_info() {
  $name = t('ImageShack');
  $features = array();
  return array(
    'provider' => 'imageshack',
    'name' => $name,
    'url' => IMAGE_NCCK_IMAGESHACK_MAIN_URL,
    'settings_description' => t('These settings specifically affect images displayed from !imageshack.', array('!imageshack' => l($name, IMAGE_NCCK_IMAGESHACK_MAIN_URL, array('target' => '_blank')))),
    'supported_features' => $features,
  );
}

function image_ncck_imageshack_data($field, $item) {
  $data = array();

  $url = stristr($item['embed'], 'my.php?image=')? _image_ncck_imageshack_scrape_image_url($item['embed']) : $item['embed'];
  if(preg_match('!([^/.:@]+)\.imageshack\.us/([^/]+)/([^/]+)/(.+)$!i', $url, $matches)) {
    $data = array(
      'server' => $matches[1],
      'a1' => $matches[2],
      'a2' => $matches[3],
      'file' => $matches[4],
      'title' => '',
    );
  }
  return $data;
}

function image_ncck_imageshack_extract($embed = '') {
  // http://img255.imageshack.us/my.php?image=dscn0036ky1.jpg
  // http://img255.imageshack.us/img255/7682/dscn0036ky1.jpg
  if(preg_match('!([^/.:@]+)\.imageshack\.us/(([^/]+)/([^/]+)/|my.php\?image=)?(.+)$!i', $embed, $matches)) {
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
function image_ncck_imageshack_embedded_link($code, $data) {
  return "http://{$data['server']}.imageshack.us/my.php?image={$code}";
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
function image_ncck_imageshack_image_url($code, $data) {
  if (func_num_args() == 7) {
    $arg = func_get_arg(5);
    $code = &$arg['data']['file'];
    $data = &$arg['data'];
  }
  return "http://{$data['server']}.imageshack.us/{$data['a1']}/{$data['a2']}/{$code}";
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
function image_ncck_imageshack_image_title($code, $data) {
  return ''; // Not provided by on ImageShack.
}

/**
 * Scrape the actual image URL from the ImageShack page.
 *
 * @param String $url
 *   ImageShack page.
 * @return String
 *   ImageShack image URL.
 */
function _image_ncck_imageshack_scrape_image_url($url) {
  static $urls;
  if (isset($urls[$url])) {
    return $urls[$url];
  }

  $html = file_get_contents($url);
  if (preg_match('!<img id="thepic" .+? src="(.+?)"!is', $html, $matches)) {
    return $urls[$url] = $matches[1];
  }
}