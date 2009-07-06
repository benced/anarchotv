<?php
// $Id: metacafe.inc,v 1.3 2007/09/06 22:18:25 aaron Exp $

define('VIDEO_CCK_METACAFE_MAIN_URL', 'http://www.metacafe.com/');

function video_cck_metacafe_info() {
  $name = t('MetaCafe');
  $features = array(
    array(t('Autoplay'), t('Yes'), ''),
    array(t('RSS Attachment'), t('No'), ''),
    array(t('Thumbnails'), t('Yes'), ''),
  );
  return array(
    'provider' => 'metacafe',
    'name' => $name,
    'url' => VIDEO_CCK_METACAFE_MAIN_URL,
    'settings_description' => t('These settings specifically affect videos displayed from !provider.', array('!provider' => l($name, VIDEO_CCK_METACAFE_MAIN_URL, array('target' => '_blank')))),
    'supported_features' => $features,
  );
}

function video_cck_metacafe_settings() {
  $form = array();
  return $form;
}

/**
 *  we're going to handle our own matches, unless someone can come up with a regex that will match this better
 */
function video_cck_metacafe_extract($embed) {
  // http://www.metacafe.com/watch/479957/gorilla_prank/
  if ($embed && preg_match('@metacafe\.com/watch/(.[^/]*)/(.[^/]*)/@i', $embed, $matches)) {
    return $matches[1] . '/' . $matches[2];
  }
  else if ($embed && preg_match('@metacafe\.com/watch/(.[^/]*)/(.*)@i', $embed, $matches)) {
    return $matches[1] . '/' . $matches[2];
  }
  // <embed src="http://www.metacafe.com/fplayer/479957/gorilla_prank.swf" width="400" height="345" wmode="transparent" pluginspage="http://www.macromedia.com/go/getflashplayer" type="application/x-shockwave-flash"></embed><br><font size = 1><a href="http://www.metacafe.com/watch/479957/gorilla_prank/">Gorilla Prank</a> - <a href='http://www.metacafe.com/'>Celebrity bloopers here</a></font>
  else if ($embed && preg_match('@metacafe\.com/fplayer/(.[^/]*)/(.[^\.]*)\.@i', $embed, $matches)) {
    return $matches[1] . '/' . $matches[2];
  }
  return false;
}

function video_cck_metacafe_video_link($video_code) {
  return 'http://www.metacafe.com/watch/' . $video_code . '/';
}

function theme_video_cck_metacafe_flash($embed, $width, $height, $autoplay) {
  if ($embed) {
    $autoplay = $autoplay ? '?playerVars=autoPlay=yes' : '';
    $output .= '<embed src="http://www.metacafe.com/fplayer/' . $embed . '.swf' . $autoplay . '" width="' . $width . '" height="' . $height . '" wmode="transparent" pluginspage="http://www.macromedia.com/go/getflashplayer" type="application/x-shockwave-flash"></embed>';
  }
  return $output;
}

function video_cck_metacafe_thumbnail($field, $item, $formatter, $node, $width, $height) {
  $picturelink=substr($item['value'], 0, 6);
  return 'http://www.metacafe.com/thumb/' . $picturelink . '.jpg';
}

function video_cck_metacafe_video($embed, $width, $height, $field, $item, $autoplay) {
  $output = theme('video_cck_metacafe_flash', $embed, $width, $height, $autoplay);
  return $output;
}

function video_cck_metacafe_preview($embed, $width, $height, $field, $item, $autoplay) {
  $output = theme('video_cck_metacafe_flash', $embed, $width, $height, $autoplay);
  return $output;
}

