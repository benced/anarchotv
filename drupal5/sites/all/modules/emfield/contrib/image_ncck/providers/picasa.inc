<?php
// $Id: picasa.inc,v 1.3 2008/05/11 20:30:26 alexua Exp $

define('IMAGE_NCCK_PICASA_MAIN_URL', 'http://picasaweb.google.com/');
define('IMAGE_NCCK_PICASA_API_INFO', 'http://code.google.com/apis/picasaweb/gdata.html');
define('IMAGE_NCCK_PICASA_REST_ENDPOINT', 'http://picasaweb.google.com/data/feed/api/user/');

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

function image_ncck_picasa_info() {
  $name = t('Picasa');
  $features = array(
    array(t('Import photosets'), t('No'), ''),
  );
  return array(
    'provider' => 'picasa',
    'name' => $name,
    'url' => IMAGE_NCCK_PICASA_MAIN_URL,
    'settings_description' => t('These settings specifically affect images displayed from !picasa.', array('!picasa' => l($name, IMAGE_NCCK_PICASA_MAIN_URL, array('target' => '_blank')), '!api' => l(t('API'), IMAGE_NCCK_PICASA_API_INFO, array('target' => '_blank')))),
    'supported_features' => $features,
    'import_sets_word' => t('photosets'),
  );
}




/**
 * this is a wrapper for image_cck_request_xml
 */
function image_ncck_picasa_request($data = array(), $kind = '', $cached = TRUE) {
  $args['kind'] = $kind;
  $xml =  module_invoke('emfield', 'request_xml', 'picasa', IMAGE_NCCK_PICASA_REST_ENDPOINT . "{$data['userid']}/album/{$data['album']}/photoid/{$data['photoid']}", $args, $cached, FALSE, ":{$data['userid']}:{$data['album']}?{$data['photoid']}");
  return $xml;
}

function image_ncck_picasa_data($field, $item) {
  $data = array();
  if(preg_match('!picasaweb\.google\.com/([^/]*)/([^/]*)/photo\#(.*)!i', $item['embed'], $matches)){
    $data = array(
      'userid' => $matches[1],
      'album' => $matches[2],
      'photoid' => $matches[3],
    );
  }
  // use the page id, since we'll have that in most cases (except in embed pastes, which gets parsed during extraction)
  // we use this to get an rss feed w/ all the info for the image. interesting reading ;)
  $xml = image_ncck_picasa_request($data, 'tag');

  $data['title'] = $xml['MEDIA:GROUP']['MEDIA:DESCRIPTION'][0];
  $data['original'] = $xml['MEDIA:GROUP']['MEDIA:CONTENT'][1][URL];
  $data['small'] = $xml['MEDIA:GROUP']['MEDIA:THUMBNAIL'][1][URL];
  $data['medium'] = $xml['MEDIA:GROUP']['MEDIA:THUMBNAIL'][3][URL];
  $data['large'] =  $xml['MEDIA:GROUP']['MEDIA:THUMBNAIL'][5][URL];
  $data['width'] = $xml['MEDIA:GROUP']['MEDIA:THUMBNAIL'][5][WIDTH];
  $data['height'] = $xml['MEDIA:GROUP']['MEDIA:THUMBNAIL'][5][HEIGHT];
  return $data;
}

function image_ncck_picasa_extract($embed = '') {
  // http://picasaweb.google.com/kaos777/YearlyKos/photo#5119063656501095090
  if(preg_match('!picasaweb\.google\.com/([^/]*)/([^/]*)/photo\#(.*)!i', $embed, $matches)) {
    return $matches[3];
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
function image_ncck_picasa_embedded_link($code, $data) {
  $userid = $data['userid'];
  $album= $data['album'];
  $photoid= $data['photoid'];
  return "http://picasaweb.google.com/$userid/$album/photo#$photoid";
}

/**
 *  implement image_ncck_PROVIDER_image_url
 *
 *  @param $code
 *    the code of the image
 *  @param $data
 *    any stored data for the image, which may already have the url of the image to display
 *  @return
 *    the url directly to emfield for the image to display
 */
function image_ncck_picasa_image_url($code, $width, $height, $formatter, $field, $item, $node) {
    if (func_num_args() == 7) {
    $arg = func_get_arg(5);
    $code = &$arg['data']['original'];
    $data = &$arg['data'];
    $size = _image_ncck_picasa_guess_size($width, $height);
  }
  //http://lh5.google.com/kaos777/RwqRRgxcKrI/AAAAAAAAAB0/pl5FboT6x2M/IMG_0468.JPG
 if(preg_match('!([^/]*)\.ggpht\.com/([^/]*)/([^/]*)/([^/]*)/([^/]*)/(.*)!i', $code, $matches)){
    $info = array(
      'server' => $matches[1],
     'userid' => $matches[2],
      's1' => $matches[3],
      's2' => $matches[4],
      's3' => $matches[5],
      'image'  => $matches[6],
    );
  }

  return "http://{$info['server']}.google.com/{$info['userid']}/{$info['s1']}/{$info['s2']}/{$info['s3']}/s$size/{$info['image']}";
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
function image_ncck_picasa_image_title($code, $data) {
  if (func_num_args() == 7) {
    $arg = func_get_arg(5);
    $code = &$arg['data']['title'];
    $data = &$arg['data'];
    $title = $code;
  }
  return "$title";
}

function _image_ncck_picasa_guess_size($width, $height) {
  $max = max($width, $height);
   foreach (array('144' => 144, '288' => 288, '400' => 400, '800' => 800) as $size => $value) {
    if ($max <= $value)  {
      return $size;
        }
  }
  // we would use the original size if we could, but we can't since google won't serve it
  return '800';
}
