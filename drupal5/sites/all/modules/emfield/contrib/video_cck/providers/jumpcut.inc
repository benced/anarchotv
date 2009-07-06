<?php
// $Id: jumpcut.inc,v 1.3 2007/09/06 22:18:25 aaron Exp $

define('VIDEO_CCK_JUMPCUT_MAIN_URL', 'http://jumpcut.com/');

function video_cck_jumpcut_info() {
  $name = t('JumpCut');
  $features = array(
    array(t('Autoplay'), t('No'), ''),
    array(t('RSS Attachment'), t('No'), ''),
    array(t('Thumbnails'), t('No'), ''),
  );
  return array(
    'provider' => 'jumpcut',
    'name' => $name,
    'url' => VIDEO_CCK_JUMPCUT_MAIN_URL,
    'settings_description' => t('These settings specifically affect videos displayed from !provider.', array('!provider' => l($name, VIDEO_CCK_JUMPCUT_MAIN_URL, array('target' => '_blank')))),
    'supported_features' => $features,
  );
}

function video_cck_jumpcut_settings() {
  $form = array();
  return $form;
}

function video_cck_jumpcut_extract($embed) {
  return array(
    '@jumpcut\.com/view\?id=(.*)@i',
    '@jumpcut\.com/view\?id=([^\&]*)\&@i',
  );
}

function video_cck_jumpcut_video_link($video_code) {
  return 'http://jumpcut.com/view?id=' . $video_code;
}

function theme_video_cck_jumpcut_flash($embed, $width, $height) {
  if ($embed) {
    $output .= '<embed src="http://jumpcut.com/media/flash/jump.swf?id=' . $embed . '&asset_type=movie&asset_id=' . $embed . '&eb=1" width="' . $width . '" height="' . $height . '" type="application/x-shockwave-flash"></embed>';
  }
  return $output;
}

function video_cck_jumpcut_thumbnail($field, $item, $formatter, $node, $width, $height) {
  return '';
}

function video_cck_jumpcut_video($embed, $width, $height, $field, $item) {
  $output = theme('video_cck_jumpcut_flash', $embed, $width, $height);
  return $output;
}

function video_cck_jumpcut_preview($embed, $width, $height, $field, $item) {
  $output = theme('video_cck_jumpcut_flash', $embed, $width, $height);
  return $output;
}

