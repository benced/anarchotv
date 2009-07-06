<?php
// $Id: brightcove.inc,v 1.5 2008/01/04 18:25:20 aaron Exp $

define('VIDEO_CCK_BRIGHTCOVE_MAIN_URL', 'http://www.brightcove.tv/');

/**
 * hook video_cck_PROVIDER_info
 * this returns information relevant to a specific 3rd party video provider
 * @return
 *   an array of strings requested by various admin and other forms
 *   'name' => the translated name of the provider
 *   'url' => the url to the main page for the provider
 *   'settings_description' => a description of the provider that will be posted in the admin settings form
 */
function video_cck_brightcove_info() {
  $name = t('Brightcove');
  $features = array(
    array(t('Autoplay'), t('Yes'), ''),
    array(t('RSS Attachment'), t('No'), ''),
    array(t('Thumbnails'), t('No'), ''),
  );
  return array(
    'provider' => 'brightcove',
    'name' => $name,
    'url' => VIDEO_CCK_BRIGHTCOVE_MAIN_URL,
    'settings_description' => t('These settings specifically affect videos displayed from !brightcove.', array('!brightcove' => l($name, VIDEO_CCK_BRIGHTCOVE_MAIN_URL, array('target' => '_blank')))),
    'supported_features' => $features,
  );
}

/**
 * hook video_cck_PROVIDER_settings
 * this should return a subform to be added to the video_cck_settings() admin settings page.
 * note that a form field will already be provided, at $form['PROVIDER'] (such as $form['brightcove'])
 * so if you want specific provider settings within that field, you can add the elements to that form field.
 */
function video_cck_brightcove_settings() {
  $form = array();
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
function video_cck_brightcove_extract($embed = '') {
  return array(
    //<embed src='http://www.brightcove.tv/playerswf' bgcolor='#FFFFFF' flashVars='allowFullScreen=true&initVideoId=1155177265&servicesURL=http://www.brightcove.tv&viewerSecureGatewayURL=https://www.brightcove.tv&cdnURL=http://admin.brightcove.com&autoStart=false' base='http://admin.brightcove.com' name='bcPlayer' width='425' height='350' allowFullScreen='true' allowScriptAccess='always' seamlesstabbing='false' type='application/x-shockwave-flash' swLiveConnect='true' pluginspage='http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash'></embed>

    '@brightcove\.tv/playerswf\' bgcolor\=\'\#FFFFFF\' flashVars\=\'initVideoId\=([^\&]*)\&@i',

    // was:
    // &initVideoId=753308735&
    // '@\&initVideoId=([^\&]*)\&@',

    // http://www.brightcove.tv/title.jsp?title=753308735
    '@brightcove\.tv/title\.jsp\?title=(.*)@i',
  );
}

/**
 * hook video_cck_PROVIDER_video_link($video_code)
 * returns a link to view the video at the provider's site
 *  @param $video_code
 *    the string containing the video to watch
 *  @return
 *    a string containing the URL to view the video at the original provider's site
 */
function video_cck_brightcove_video_link($video_code) {
  return 'http://www.brightcove.tv/title.jsp?title=' . $video_code;
}

/**
 * the embedded flash displaying the brightcove video
 */
function theme_video_cck_brightcove_flash($embed, $width, $height, $autoplay) {
  if ($embed) {
    $autostart = $autoplay ? 'autoStart=true' : 'autoStart=false';
    $output .= "<embed src='http://www.brightcove.tv/playerswf' bgcolor='#FFFFFF' flashVars='initVideoId=$embed&servicesURL=http://www.brightcove.tv&viewerSecureGatewayURL=https://www.brightcove.tv&cdnURL=http://admin.brightcove.com&$autostart' base='http://admin.brightcove.com' name='bcPlayer' width='$width' height='$height' allowFullScreen='true' allowScriptAccess='always' seamlesstabbing='false' type='application/x-shockwave-flash' swLiveConnect='true' pluginspage='http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash'></embed>";
  }
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
function video_cck_brightcove_thumbnail($field, $item, $formatter, $node, $width, $height) {
  return '';
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
function video_cck_brightcove_video($embed, $width, $height, $field, $item, $autoplay) {
  $output = theme('video_cck_brightcove_flash', $embed, $width, $height, $autoplay);
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
function video_cck_brightcove_preview($embed, $width, $height, $field, $item, $autoplay) {
  $output = theme('video_cck_brightcove_flash', $embed, $width, $height, $autoplay);
  return $output;
}
