<?php
// $Id: vimeo.inc,v 1.2 2008/05/11 03:46:09 alexua Exp $

define('VIDEO_CCK_VIMEO_MAIN_URL', 'http://www.vimeo.com/');
define('VIDEO_CCK_VIMEO_API_INFO', 'http://vimeo.com/api');
define('VIDEO_CCK_VIMEO_COLOR_DEFAULT', '#01AAEA');

/**
 * hook video_cck_PROVIDER_info
 * this returns information relevant to a specific 3rd party video provider
 * @return
 *   an array of strings requested by various admin and other forms
 *   'name' => the translated name of the provider
 *   'url' => the url to the main page for the provider
 *   'settings_description' => a description of the provider that will be posted in the admin settings form
 *   'supported_features' => an array of rows describing the state of certain supported features by the provider.
 *      These will be rendered in a table, with the columns being 'Feature', 'Supported', 'Notes'.
 */
function video_cck_vimeo_info() {
  $name = t('Vimeo');
  $features = array(
    array(t('Custom player color'), t('Yes'), t('You may customize the player\'s skin by choosing your own color.')),
    array(t('Thumbnails'), t('Yes'), t('You may select the size of thumbnail to request from Vimeo.')),
    array(t('Full screen mode'), t('Yes'), t('You may customize the player to enable or disable full screen playback. Full screen mode is enabled by default.')),
  );
  return array(
    'provider' => 'vimeo',
    'name' => $name,
    'url' => VIDEO_CCK_VIMEO_MAIN_URL,
    'settings_description' => t('These settings specifically affect videos displayed from !provider. You can learn more about its !api here.', array('!provider' => l($name, VIDEO_CCK_VIMEO_MAIN_URL, array('target' => '_blank')), '!api' => l(t('API'), VIDEO_CCK_VIMEO_API_INFO, array('target' => '_blank')))),
    'supported_features' => $features,
  );
}

/**
 * hook video_cck_PROVIDER_settings
 * this should return a subform to be added to the video_cck_settings() admin settings page.
 * note that a form field will already be provided, at $form['PROVIDER'] (such as $form['vimeo'])
 * so if you want specific provider settings within that field, you can add the elements to that form field.
 */
function video_cck_vimeo_settings() {
  $form['vimeo']['color'] = array(
    '#type' => 'fieldset',
    '#title' => t('Embedded video player color'),
    '#description' => t('If allowed, this color, in hexidecimal form (#RRGGBB), will be used to change the skin of the Vimeo player.'),
    '#collapsible' => true,
    '#collapsed' => true,
  );
  $form['vimeo']['color']['video_cck_vimeo_color_override'] = array(
    '#type' => 'checkbox',
    '#title' => t('Override player color'),
    '#default_value' => variable_get('video_cck_vimeo_color_override', FALSE),
  );
  $form['vimeo']['color']['video_cck_vimeo_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Color'),
    '#default_value' => variable_get('video_cck_vimeo_color', VIDEO_CCK_VIMEO_COLOR_DEFAULT),
  );
  $form['vimeo']['player_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Embedded video player options'),
    '#collapsible' => true,
    '#collapsed' => true,
  );
  $form['vimeo']['player_options']['video_cck_vimeo_on_screen_info'] = array(
    '#type' => 'checkboxes',
    '#title' => t('On-screen info'),
    '#default_value' => variable_get('video_cck_vimeo_on_screen_info', array('portrait', 'title', 'byline')),
		'#options' => array(
      'portrait' => t('Show video author\'s portrait'),
      'title' => t('Show video title'),
      'byline' => t('Show byline'),
    ),
    '#description' => t('Provide additional video information on the Vimeo player.'),
  );
  $form['vimeo']['player_options']['video_cck_vimeo_full_screen'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow fullscreen'),
		'#default_value' => variable_get('video_cck_vimeo_full_screen', 1),
		'#description' => t('Allow users to view video using the entire computer screen.'),
  );
  $form['vimeo']['video_cck_vimeo_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Vimeo API Key'),
    '#default_value' => variable_get('video_cck_vimeo_api_key', ''),
  );
  $form['vimeo']['video_cck_vimeo_api_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Vimeo API Shared Secret'),
    '#default_value' => variable_get('video_cck_vimeo_api_secret', ''),
  );
  $form['vimeo']['video_cck_vimeo_thumb_size'] = array(
    '#type' => 'select',
    '#title' => t('Vimeo Thumbnail Size'),
    '#options' => array('96' => '96', '100' => '100', '160' => '160', '200' => '200', '460' => '460'),
    '#default_value' => variable_get('video_cck_vimeo_thumb_size', '160'),
  );
  return $form;
}

/**
 * hook video_cck_PROVIDER_extract
 * this is called to extract the video code from a pasted URL or embed code.
 * @param $embed
 *   an optional string with the pasted URL or embed code
 * @return
 *   either an array of regex expressions to be tested, or a string with the video code to be used
 *   if the hook tests the code itself, it should return either the string of the video code (if matched), or an empty array.
 *   otherwise, the calling function will handle testing the embed code against each regex string in the returned array.
 */
function video_cck_vimeo_extract($embed = '') {
  // http://vimeo.com/123456
  // http://www.vimeo.com/123456
  return array(
    '@vimeo\.com/([^\"\&]+)@i',
  );
}

/**
 * hook video_cck_PROVIDER_embedded_link($video_code)
 * returns a link to view the video at the provider's site
 *  @param $video_code
 *    the string containing the video to watch
 *  @return
 *    a string containing the URL to view the video at the original provider's site
 */
function video_cck_vimeo_embedded_link($video_code) {
  return 'http://www.vimeo.com/'. $video_code;
}

function video_cck_vimeo_convert_color($color = NULL) {
  if ($color{0} == '#') {
    return substr($color, 1);
  }
  return $color;
}

/**
 * the embedded flash displaying the Vimeo video
 */
function theme_video_cck_vimeo_flash($embed, $width, $height, $autoplay) {
  $output = '';
  if ($embed) {
		$fullscreen = variable_get('video_cck_vimeo_full_screen', 1);
		$on_screen_info = variable_get('video_cck_vimeo_on_screen_info', array('portrait', 'title', 'byline'));
		$show_portrait = $on_screen_info['portrait'] ? 1 : 0;
		$show_title = $on_screen_info['title'] ? 1 : 0;
		$show_byline = $on_screen_info['byline'] ? 1 : 0;
    if (variable_get('video_cck_vimeo_color_override', FALSE)) {
      $color = video_cck_vimeo_convert_color(variable_get('video_cck_vimeo_color', VIDEO_CCK_VIMEO_COLOR_DEFAULT));
    }
    $output = '<object type="application/x-shockwave-flash" width="'. $width .'" height="'. $height .'" data="http://www.vimeo.com/moogaloop.swf?clip_id='. $embed .'&amp;server=www.vimeo.com&amp;fullscreen='. $fullscreen .'&amp;show_title='. $show_title .'&amp;show_byline='. $show_byline .'&amp;show_portrait='. $show_portrait .'&amp;color='. $color .'">';
    $output .= '<param name="quality" value="best" />';
    $output .= '<param name="allowfullscreen" value="'. ($fullscreen ? 'true' : 'false') .'" />';
    $output .= '<param name="scale" value="showAll" />';
    $output .= '<param name="movie" value="http://www.vimeo.com/moogaloop.swf?clip_id='. $embed .'&amp;server=www.vimeo.com&amp;fullscreen='. $fullscreen .'&amp;show_title='. $show_title .'&amp;show_byline='. $show_byline .'&amp;show_portrait='. $show_portrait .'&amp;color='. $color .'" /></object>';
  }
  return $output;
}

/**
 * hook video_cck_PROVIDER_video
 * this actually displays the full/normal-sized video we want, usually on the default page view
 *  @param $embed
 *    the video code for the video to embed
 *  @param $width
 *    the width to display the video
 *  @param $height
 *    the height to display the video
 *  @param $field
 *    the field info from the requesting node
 *  @param $item
 *    the actual content from the field
 *  @return
 *    the html of the embedded video
 */
function video_cck_vimeo_video($embed, $width, $height, $field, $item, $autoplay) {
  $output = theme('video_cck_vimeo_flash', $embed, $width, $height, $autoplay);
  return $output;
}

/**
 * hook video_cck_PROVIDER_video
 * this actually displays the preview-sized video we want, commonly for the teaser
 *  @param $embed
 *    the video code for the video to embed
 *  @param $width
 *    the width to display the video
 *  @param $height
 *    the height to display the video
 *  @param $field
 *    the field info from the requesting node
 *  @param $item
 *    the actual content from the field
 *  @return
 *    the html of the embedded video
 */
function video_cck_vimeo_preview($embed, $width, $height, $field, $item, $autoplay) {
  $output = theme('video_cck_vimeo_flash', $embed, $width, $height, $autoplay);
  return $output;
}


/**
 * hook video_cck_PROVIDER_thumbnail
 * returns the external url for a thumbnail of a specific video
 * TODO: make the args: ($embed, $field, $item), with $field/$item provided if we need it, but otherwise simplifying things
 *  @param $field
 *    the field of the requesting node
 *  @param $item
 *    the actual content of the field from the requesting node
 *  @return
 *    a URL pointing to the thumbnail
 */
function video_cck_vimeo_thumbnail($field, $item, $formatter, $node, $width, $height) {
  $vimeo_id = (substr($item['value'],-1,1)=='/') ? substr($item['value'],0,-1) : $item['value'];
  $vimeo_signature = variable_get('video_cck_vimeo_api_secret', '') .
    'api_key' . variable_get('video_cck_vimeo_api_key', '') .
    'method' . 'vimeo.videos.getThumbnailUrl' .
    'size' . variable_get('video_cck_vimeo_thumb_size', '160') .
    'video_id' . $vimeo_id;
  $vimeo_apicall = 'http://www.vimeo.com/api/rest?' .
    'api_key=' . variable_get('video_cck_vimeo_api_key', '') .
    '&method=' . 'vimeo.videos.getThumbnailUrl' .
    '&size=' . variable_get('video_cck_vimeo_thumb_size', '160') .
    '&video_id=' . $vimeo_id .
    '&api_sig=' . md5($vimeo_signature);
  $api_file = file_get_contents($vimeo_apicall);
  $data = array();
  preg_match("#<thumbnail .*>(.*)</thumbnail>#i",$api_file,$data);
  return $data[1];
}


