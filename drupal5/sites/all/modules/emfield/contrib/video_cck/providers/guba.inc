<?php
// $Id: guba.inc,v 1.1 2008/05/11 04:20:59 alexua Exp $

define('VIDEO_CCK_GUBA_MAIN_URL', 'http://www.guba.com/');
define('VIDEO_CCK_GUBA_API_INFO', 'http://www.guba.com/restApiDoc/restReference.html');
define('VIDEO_CCK_GUBA_API_APPLICATION_URL', 'http://www.guba.com/developers.html');

function video_cck_guba_info() {
  $name = t('GUBA');
  $features = array(
    array(t('Autoplay'), t('Yes'), ''),
    array(t('RSS Attachment'), t('No'), ''),
    array(t('Show related videos'), t('No'), t('This is embedded in the video itself when enabled; currently not available with other providers. Set the feature above.')),
    array(t('Thumbnails'), t('Yes'), t('May not currently resize thumbnails. Must have an API key for thumbnails at the moment, although research is underway to determine an alternative to this. Set your API key above.')),
  );
  return array(
    'provider' => 'guba',
    'name' => $name,
    'url' => VIDEO_CCK_GUBA_MAIN_URL,
    'settings_description' => t('These settings specifically affect videos displayed from !guba. You can learn more about its !api here.', array('!guba' => l($name, VIDEO_CCK_GUBA_MAIN_URL, array('target' => '_blank')), '!api' => l(t('API'), VIDEO_CCK_GUBA_API_INFO, array('target' => '_blank')))),
    'supported_features' => $features,
  );
}

function video_cck_guba_settings() {
  $form['guba']['api'] = array(
    '#type' => 'fieldset',
    '#title' => t('GUBA API'),
    '#description' => t('If you wish to be able to display GUBA thumbnails automatically, you will first need to apply for an API Developer Key from the !guba. Note that you do not need this key to display GUBA videos themselves.', array('!guba' => l('GUBA Developer Profile page', VIDEO_CCK_GUBA_API_APPLICATION_URL, array('target' => '_blank')))),
    '#collapsible' => true,
    '#collapsed' => true,
  );
  $form['guba']['api']['video_cck_guba_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('GUBA API Key'),
    '#default_value' => variable_get('video_cck_guba_api_key', ''),
    '#description' => t('Please enter your GUBA Developer Key here.'),
  );
  $form['guba']['api']['video_cck_guba_api_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('GUBA API Secret'),
    '#default_value' => variable_get('video_cck_guba_api_secret', ''),
    '#description' => t('If you have a secret for the GUBA API, enter it here.'),
  );
  return $form;
}


function video_cck_guba_request($method, $args = array(), $cached = TRUE) {
  //$args = array('docid' => $embed);
  return module_invoke('emfield', 'request_xml', 'guba', "http://www.guba.com/rest/video/" . $method . "/details", $args, $cacheable);
}

function video_cck_guba_extract($embed = '') {
  return array(
    '@http://www\.guba\.com/watch/([^"\?]+)@i',
    '@http://www\.guba\.com/f/root\.swf\?video_url=http://free\.guba\.com/uploaditem/([^"\?/]*)@i',
  );
}

function video_cck_guba_embedded_link($video_code) {
  return 'http://www.guba.com/watch/' . $video_code;
}

function theme_video_cck_guba_flash($embed, $width, $height, $autoplay) {
  $autoplay = $autoplay ? '&autoPlay=true' : '&autoPlay=false';
  $output .= '<embed src="http://www.guba.com/f/root.swf?video_url=http://free.guba.com/uploaditem/'. $embed .'/flash.flv&isEmbeddedPlayer=false' . $autoplay . '" quality="best" bgcolor="#FFFFFF" menu="true" width="' . $width . '" height="' . $height . '" name="root" id="root" align="middle" scaleMode="noScale" allowScriptAccess="always" allowFullScreen="true" type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer"></embed>';
  return $output;
}

function video_cck_guba_thumbnail($field, $item, $formatter, $node, $width, $height) {
  $thumbid = $item['value'];
  $thm = 'http://img.guba.com/public/video/f/58/' . $thumbid . '-m.jpg';
  return $thm;
}

function video_cck_guba_video($embed, $width, $height, $field, $item, $autoplay) {
  $output = theme('video_cck_guba_flash', $embed, $width, $height, $autoplay);
  return $output;
}

function video_cck_guba_preview($embed, $width, $height, $field, $item, $autoplay) {
  $output = theme('video_cck_guba_flash', $embed, $width, $height, $autoplay);
  return $output;
}
