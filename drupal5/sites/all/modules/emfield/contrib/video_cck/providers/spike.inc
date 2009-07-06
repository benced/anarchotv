<?php
// $Id: spike.inc,v 1.2 2008/05/02 16:10:01 alexua Exp $

define('VIDEO_CCK_IFILM_MAIN_URL', 'http://www.spike.com/');

function video_cck_spike_info() {
  $name = t('Spike TV');
  $features = array(
    array(t('Autoplay'), t('No'), t('In the works...')),
    array(t('RSS Attachment'), t('No'), ''),
    array(t('Thumbnails'), t('In the works...'), ''),
  );
  return array(
    'provider' => 'spike tv',
    'name' => $name,
    'url' => VIDEO_CCK_SPIKE_MAIN_URL,
    'settings_description' => t('These settings specifically affect videos displayed from !spike.', array('!spike' => l($name, VIDEO_CCK_SPIKE_MAIN_URL, array('target' => '_blank')))),
    'supported_features' => $features,
  );
}

function video_cck_spike_settings() {
  $form = array();
  return $form;
}

function video_cck_spike_extract($embed) {
  return array(
    // http://www.spike.com/video/2836119/collection/19459/channel/comedy
    // <embed width="448" height="365" src="http://www.spike.com/efp" quality="high" bgcolor="000000" name="efp" align="middle" type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer" flashvars="flvbaseclip=2836119&"> </embed> <h1><a href="http://www.spike.com/video/2836119">"I Lost It" with Tom Cruize and Jesus</a></h1><span>Posted Mar 25, 2007</span><p>What happens when Jesus, Abe Lincoln, Tom Cruise, Dustin Hoffman, Buddha and Einstein play a friendly game of poker?</p>
    '@spike\.com/video/([^/&"\?]*)@i',
  );
}

function video_cck_spike_request($embed, $cacheable = TRUE) {
  return NULL;
}

function video_cck_spike_video_link($video_code) {
  return 'http://www.spike.com/video/' . $video_code;
}

function theme_video_cck_spike_flash($embed, $width, $height, $autoplay) {
  // TODO: figure out autoplay...
  //<embed width="448" height="365" src="http://www.spike.com/efp" quality="high" bgcolor="000000" name="efp" align="middle" type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer" flashvars="flvbaseclip=2952560&"> </embed>
 if ($embed) {
    $autoplay = $autoplay ? ' autoplay="true"' : '';
    $output = '<embed width="' . $width . '" height="' . $height . '" src="http://www.spike.com/efp" quality="high" bgcolor="000000" name="efp" align="middle" type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer" flashvars="flvbaseclip=' . $embed .'"> </embed>';
  }
  return $output;
}

function video_cck_spike_thumbnail($field, $item, $formatter, $node, $width, $height) {
  $picturelink = substr($item['value'], 0, 7);
  return 'http://img1.ifilmpro.com/resize/image/stills/films/resize/istd/' . $picturelink . '.jpg?width=' . $width;
}

function video_cck_spike_video($embed, $width, $height, $field, $item, $autoplay) {
  $output = theme('video_cck_spike_flash', $embed, $width, $height, $autoplay);
  return $output;
}

function video_cck_spike_preview($embed, $width, $height, $field, $item, $autoplay) {
  $output = theme('video_cck_spike_flash', $embed, $width, $height, $autoplay);
  return $output;
}
