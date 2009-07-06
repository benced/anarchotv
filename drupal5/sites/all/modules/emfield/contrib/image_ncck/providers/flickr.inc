<?php
// $Id: flickr.inc,v 1.5 2008/02/19 16:26:20 aaron Exp $

define('IMAGE_NCCK_FLICKR_MAIN_URL', 'http://www.flickr.com/');
define('IMAGE_NCCK_FLICKR_API_INFO', 'http://flickr.com/services/api');
define('IMAGE_NCCK_FLICKR_API_APPLICATION_URL', 'http://www.flickr.com/services/api/keys');
define('IMAGE_NCCK_FLICKR_REST_ENDPOINT', 'http://api.flickr.com/services/rest/');

/**
 * hook image_ncck_PROVIDER_info
 * this returns information relevant to a specific 3rd party video provider
 * @return
 *   an array of strings requested by various admin and other forms
 *   'name' => the translated name of the provider
 *   'url' => the url to the main page for the provider
 *   'settings_description' => a description of the provider that will be posted in the admin settings form
 *   'supported_features' => an array of rows describing the state of certain supported features by the provider.
 *      These will be rendered in a table, with the columns being 'Feature', 'Supported', 'Notes'.
 */

function image_ncck_flickr_info() {
  $name = t('Flickr');
  $features = array(
    array(t('Import photosets'), t('Yes'), t('If you have the Embedded Media Import module activated, you may allow @name photosets to be imported into content.', array('@name' => $name))),
  );
  return array(
    'provider' => 'flickr',
    'name' => $name,
    'url' => IMAGE_NCCK_FLICKR_MAIN_URL,
    'settings_description' => t('These settings specifically affect images displayed from !flickr.', array('!flickr' => l($name, IMAGE_NCCK_FLICKR_MAIN_URL, array('target' => '_blank')), '!api' => l(t('API'), IMAGE_NCCK_FLICKR_API_INFO, array('target' => '_blank')))),
    'supported_features' => $features,
    'import_sets_word' => t('photosets'),
  );
}

/**
 *  This allows flickr photosets to be imported into nodes
 */
function image_ncck_flickr_import($url, $limit = 0, $page = 0) {
  $codes = array();

  // http://www.flickr.com/photos/nikkiana/sets/72157601948647678/
  if (preg_match('@flickr\.com/photos/([^/]*)/([^/]*)/([^/]*)/@i', $url, $matches)) {
    $page++;  // flickr starts current page at 1
    $codes['#matches'] = $matches;
    $args = array('photoset_id' => $matches[3]);
    if ($limit) {
      $args['per_page'] = $limit;
    }
    $args['page'] = $page;
    $xml = image_ncck_flickr_request('flickr.photosets.getPhotos', $args);
//    print_r($xml);
    $codes['#pages'] = $xml['photoset']['pages'];
    $codes['#page'] = $xml['photoset']['page'] - 1;
    $codes['#total'] = $xml['photoset']['total'];
    $codes['#per_page'] = $xml['photoset']['per_page'];
    $codes['#set'] = array();
    foreach ($xml['photoset']['photo'] as $photo) {
      $data = image_ncck_flickr_data(NULL, array('value' => $photo['id']));
      $codes['#set'][] = array(
        '#code' => $photo['id'],
        '#title' => $photo['title'],
        '#link' => image_ncck_flickr_embedded_link($photo['id'], $xml['photoset']['owner']),
        '#thumb' => image_ncck_flickr_image_url($photo['id'], 100, 100, NULL, NULL, NULL),
        '#body' => $data['description'],
        '#tags' => $data['tags'],
//         '#body' => image_ncck_flickr_description($photo['id']),
//         '#tags' => image_ncck_flickr_tags($photo['id']),
      );
    }
/*
    $data['owner'] = $xml['photo']['owner']['nsid'];
    $data['title'] = $xml['photo']['title']['_content'];*/
  }
// print_r($codes);
  return $codes;
}

/**
 * hook image_ncck_PROVIDER_settings
 * this should return a subform to be added to the image_ncck_settings() admin settings page.
 * note that a form field will already be provided, at $form['PROVIDER'] (such as $form['flickr'])
 * so if you want specific provider settings within that field, you can add the elements to that form field.
 */
function image_ncck_flickr_settings() {
  $form['flickr']['api'] = array(
    '#type' => 'fieldset',
    '#title' => t('Flickr API'),
    '#description' => t('You will first need to apply for an API Developer Key from the !flickr.', array('!flickr' => l(t('Flickr Developer Profile page'), IMAGE_NCCK_FLICKR_API_APPLICATION_URL, array('target' => '_blank')))),
    '#collapsible' => true,
    '#collapsed' => (variable_get('image_ncck_flickr_api_key', '') != ''),
  );
  $form['flickr']['api']['image_ncck_flickr_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Flickr API Key'),
    '#default_value' => variable_get('image_ncck_flickr_api_key', ''),
    '#description' => t('Please enter your Flickr Developer Key here.'),
  );
  $form['flickr']['api']['image_ncck_flickr_api_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Flickr API Secret'),
    '#default_value' => variable_get('image_ncck_flickr_api_secret', ''),
    '#description' => t('If you have a secret for the Flickr API, enter it here.'),
  );
  return $form;
}


/**
 * this is a wrapper for image_ncck_request_xml that includes flickr's api key
 */
function image_ncck_flickr_request($method, $args = array(), $cached = TRUE) {
  // display an error if we don't have an api key yet
  image_ncck_flickr_error_check();

  $args['api_key'] = trim(variable_get('image_ncck_flickr_api_key', ''));
  if ($secret = trim(variable_get('image_ncck_flickr_api_secret', ''))) {
    $args['secret'] = md5($secret . $arghash);
  }
  $args['method'] = $method;
  $args['format'] = 'php_serial';

  $xml = module_invoke('emfield', 'request_xml', 'flickr', IMAGE_NCCK_FLICKR_REST_ENDPOINT, $args, $cached, FALSE, FALSE, TRUE);
  return $xml;
}

function image_ncck_flickr_data($field, $item) {
  $data = array();

  // use the page id, since we'll have that in most cases (except in embed pastes, which gets parsed during extraction)
  // we use this to get an rss feed w/ all the info for the video. interesting reading ;)
  $xml = image_ncck_flickr_request('flickr.photos.getInfo', array('photo_id' => $item['value']));

  $data['owner'] = $xml['photo']['owner']['nsid'];
  $data['title'] = $xml['photo']['title']['_content'];
  $data['description'] = $xml['photo']['description']['_content'];
  $data['tags'] = array();
  if (is_array($xml['photo']['tags']['tag'])) {
    foreach ($xml['photo']['tags']['tag'] as $tag) {
      $data['tags'][] = $tag['raw'];
    }
  }
  return $data;
}

/**
 *  This will log an error if we don't have a key yet. In addition, if the user is an admin, we'll display an error.
 */
function image_ncck_flickr_error_check() {
  static $checked;
  if (!$checked && (variable_get('image_ncck_flickr_api_key', '') == '')) {
    global $user;
    $error = t('You do not yet have a Flickr API key set. You will need to !apply and enter your key at the !settings before Flickr images may be displayed.', array('!apply' => l(t('apply for a Flickr API key'), IMAGE_NCCK_FLICKR_API_APPLICATION_URL, array('target' => '_blank')), '!settings' => l(t('settings administration page'), 'admin/content/emfield')));
    if (user_access('administer site configuration')) {
      drupal_set_message($error, 'error');
    }
    watchdog('Embedded Media Field', $error);
  }
  $checked = TRUE;
}

function image_ncck_flickr_extract($embed = '') {
  // http://flickr.com/photos/96898796@N00/194727976/
  if (preg_match('@flickr\.com/photos/([^/]*)/([^/]*)/@i', $embed, $matches)) {
    return $matches[2];
  }
  else if (preg_match('@flickr\.com/photos/([^/]*)/(.*)@i', $embed, $matches)) {
    return $matches[2];
  }
  return array();
}

/**
 * hook image_ncck_PROVIDER_embedded_link($code)
 * returns a link to view the content at the provider's site
 *  @param $code
 *    the string containing the content to watch
 *  @return
 *    a string containing the URL to view the video at the original provider's site
 */
function image_ncck_flickr_embedded_link($code, $data = array()) {
  if ($data['owner']) {
    $owner = $data['owner'];
  }
  else {
    $xml = image_ncck_flickr_request('flickr.photos.getInfo', array('photo_id' => $code));
    $owner = $xml['photo']['owner']['nsid'];
  }
  return 'http://www.flickr.com/photos/' . $owner . '/' .$code;
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
function image_ncck_flickr_image_url($code, $width, $height, $formatter = NULL, $field = NULL, $item = NULL, $node = NULL) {
  if ($code) {
    $photo = image_ncck_flickr_request('flickr.photos.getInfo', array('photo_id' => $code));
    $photo = $photo['photo'];
    $size = _image_ncck_flickr_guess_size($width, $height);
    $url = _image_ncck_flickr_image_url($photo, $size);
  }
  return $url;
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
function image_ncck_flickr_image_title($code, $data) {
  if ($data['title']) {
    return $data['title'];
  }
  $photo = image_ncck_flickr_request('flickr.photos.getInfo', array('photo_id' => $code));
  return $photo['photo']['title']['_content'] ? $photo['photo']['title']['_content'] : '';
}


function _image_ncck_flickr_image_url($photo, $size = NULL, $format = NULL) {
  // early images don't have a farm setting so default to 1.
  $farm = isset($photo['farm']) ? $photo['farm'] : 1;
  $server = $photo['server'];
  // photoset's use primary instead of id to specify the image.
  $id = isset($photo['primary']) ? $photo['primary'] : $photo['id'];
  $secret = $photo['secret'];

  return "http://farm{$farm}.static.flickr.com/{$server}/{$id}_{$secret}" .($size ? "_$size." : '.') . ($size == 'o' ? $format : 'jpg');
}

function _image_ncck_flickr_guess_size($width, $height) {
  $max = max($width, $height);
  foreach (array('s' => 75, 't' => 100, 'm' => 240, '' => 500, 'b' => 1024) as $size => $value) {
    if ($max <= $value) {
      return $size;
    }
  }
  return 'o';
}
