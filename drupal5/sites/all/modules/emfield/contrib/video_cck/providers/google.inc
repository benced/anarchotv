<?php
// $Id: google.inc,v 1.7 2008/05/11 04:00:41 alexua Exp $

define('VIDEO_CCK_GOOGLE_MAIN_URL', 'http://video.google.com/');
define('VIDEO_CCK_GOOGLE_XML', 'http://video.google.com/videofeed');
define('VIDEO_CCK_GOOGLE_DOMAIN_DEFAULT', 'com');

function video_cck_google_info() {
  $name = t('Google');
  $features = array(
    array(t('Autoplay'), t('Yes'), ''),
    array(t('RSS Attachment'), t('Yes'), ''),
    array(t('Thumbnails'), t('Yes'), ''),
  );
  return array(
    'provider' => 'google',
    'name' => $name,
    'url' => VIDEO_CCK_GOOGLE_MAIN_URL,
    'settings_description' => t('These settings specifically affect videos displayed from !google.', array('!google' => l($name, VIDEO_CCK_GOOGLE_MAIN_URL, array('target' => '_blank')))),
    'supported_features' => $features,
  );
}

function video_cck_google_settings() {
  $form = array();
  $form['video_cck_google_domain'] = array(
    '#type' => 'textfield',
    '#title' => t('Google video domain'),
    '#default_value' => variable_get('video_cck_google_domain', VIDEO_CCK_GOOGLE_DOMAIN_DEFAULT),
    '#description' => t('Google hosts their videos on various servers throughout the world. By default, videos from Google will be displayed from the United States, at their google.<b><em>com</em></b> servers. If you wish to display the video from another Google server, please enter the domain here, without the initial dot. For instance, you might enter <b><em>it</em></b> for the Italian Google servers at google.it, or <b><em>ca</em></b> for their Canadian servers.'),
  );
  return $form;
}

function video_cck_google_extract($embed) {
  return array(
    '@http://video\.google(?:\.[a-z]{2,4})+/videoplay\?docid=([^\&]*)\&@i',
    '@http://video\.google(?:\.[a-z]{2,4})+/videoplay\?docid=([^\&]*)\&@i',
	'@http://video\.google(?:\.[a-z]{2,4})+/videoplay\?docid=(.*)@i',
    '@http://video\.google(?:\.[a-z]{2,4})+/googleplayer\.swf\?docId=(-?\d*)@i',
    '@http://video\.google(?:\.[a-z]{2,4})+/url\?docid=([^\&]*)\&@i',
  );
}

function video_cck_google_request($embed, $cacheable = TRUE) {
  $args = array('docid' => $embed);
  return module_invoke('emfield', 'request_xml', 'google', VIDEO_CCK_GOOGLE_XML, $args, $cacheable);
}

function video_cck_google_video_link($video_code) {
  return 'http://video.google.' . variable_get('video_cck_google_domain', VIDEO_CCK_GOOGLE_DOMAIN_DEFAULT) . '/videoplay?docid=' . $video_code;
}

function theme_video_cck_google_flash($embed, $width, $height, $autoplay) {
  if ($embed) {
    $autoplay = $autoplay ? '&autoPlay=true' : '';
    // this will be executed by not Internet Explorer browsers
    $output = '<!--[if !IE]> <-->
  <object type="application/x-shockwave-flash" width="'. $width .'" height="'. $height .'"
  data="http://video.google.' . variable_get('video_cck_google_domain', VIDEO_CCK_GOOGLE_DOMAIN_DEFAULT) . '/googleplayer.swf?docId='. check_plain($embed) . $autoplay . '">
  <!--> <![endif]-->' . "\n";

    // this will be executed by Internet Explorer
    $output .= '<!--[if IE]>
  <object type="application/x-shockwave-flash" width="'. $width .'" height="'. $height .'"
  classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000"
  codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,40,0">
  <![endif]-->' . "\n";

    // params will be passed to both IE or not IE browsers
    $output .= '<param name="movie" value="http://video.google.' . variable_get('video_cck_google_domain', VIDEO_CCK_GOOGLE_DOMAIN_DEFAULT) . '/googleplayer.swf?docId=' . check_plain($embed) . $autoplay . '" />' . "\n";
    // following a list of params simply copied from old embed tag params. I don't know if this are really needed.
    $output .= '<param name="quality" value="best" />
    <param name="bgcolor" value="#ffffff" />
    <param name="allowScriptAccess" value="sameDomain" />
    <param name="scale" value="noScale" />
    <param name="wmode" value="transparent" />
    <param name="salign" value="TL" />
    <param name="FlashVars" value="playerMode=embedded" />
    <p>'. t('Your browser is not able to display this multimedia content.') .'</p>
  </object>';
  }
  return $output;
}

/**
 * hook emfield_PROVIDER_data
 *
 * provides an array to be serialised and made available with $item elsewhere
 */
function video_cck_google_data($field, $item) {
  $data = array();
  // create some 'field' version control
  //$data['video_cck_google_version'] = 1;

  $rss = video_cck_google_request($item['value']);

  if (
    is_array($rss['ITEM']) &&
    is_array($rss['ITEM']['MEDIA:GROUP']) && 
    is_array($rss['ITEM']['MEDIA:GROUP']['MEDIA:CONTENT']) && 
    is_array($rss['ITEM']['MEDIA:GROUP']['MEDIA:CONTENT'][1])
  ) {
    $video = $rss['ITEM']['MEDIA:GROUP']['MEDIA:CONTENT'][1];
    $data['filepath'] = $video['URL'];
    $data['filemime'] = $video['TYPE'];
    $data['medium'] = $video['MEDIUM'];
    $data['expression'] = $video['EXPRESSION'];
    $data['duration'] = $video['DURATION'];
    $data['width'] = $video['WIDTH'];
    $data['height'] = $video['HEIGHT'];
  }

  if (is_array($rss['MEDIA:GROUP'])) {
    if (is_array($rss['MEDIA:GROUP']['MEDIA:THUMBNAIL']) && is_array($rss['MEDIA:GROUP']['MEDIA:THUMBNAIL'][1])) {
      $thumbnail = $rss['MEDIA:GROUP']['MEDIA:THUMBNAIL'][1];
      $data['thumbnail']['filepath'] = $thumbnail['URL'];
      $data['thumbnail']['width'] = $thumbnail['WIDTH'];
      $data['thumbnail']['height'] = $thumbnail['HEIGHT'];
    }
    if (is_array($rss['MEDIA:GROUP']['MEDIA:PLAYER']) && is_array($rss['MEDIA:GROUP']['MEDIA:PLAYER'][1])) {
      $data['player']['filepath'] = $rss['MEDIA:GROUP']['MEDIA:PLAYER'][1]['URL'];
    }
  }
  
  if ($data['thumbnail']['filepath'] == '') {
    // for whatever reason the thumbnail failed try the old method
    // we'll parse it from the description, where it's repeated.
    $desc = $rss['ITEM']['DESCRIPTION'][0];
    $regex = '@<img src="([^"]*)"@';
    if (preg_match($regex, $desc, $matches)) {
      $data['thumbnail']['filepath'] = $matches[1];
    }
  }

  // google rss doesn't actually have <embeded> media, just MRSS, and no size
  // so not actually really really necessary or really useful but for completeness
  $play = 'http://video.google.'. variable_get('video_cck_google_domain', VIDEO_CCK_GOOGLE_DOMAIN_DEFAULT) .'/googleplayer.swf?docId='. check_plain($item['embed']);
  $response = emfield_request_header('google', $play, FALSE, FALSE);

  if ($response->code == 200) {
    $data['filesize'] = $response->headers['Content-Length'];
  }

  return $data;
}

function video_cck_google_rss($item, $teaser) {
  if ($item['value']) {
    if ($item['data']['video_cck_google_data_version'] >= 1000000) {
      $data = $item['data'];
    }
    else {
      $data = video_cck_google_data(NULL, $item);
    }

    return $data;
  }
}

function video_cck_google_thumbnail($field, $item, $formatter, $node, $width, $height) {
  if ($item['data']['video_cck_google_data_version'] >= 1) {
    $data = $item['data'];
  }
  else {
    $data = video_cck_google_data($field, $item);
  }

  return $data['thumbnail']['filepath'];

  return NULL;
}

function video_cck_google_video($embed, $width, $height, $field, $item, $autoplay) {
  $output = theme('video_cck_google_flash', $embed, $width, $height, $autoplay);
  return $output;
}

function video_cck_google_preview($embed, $width, $height, $field, $item, $autoplay) {
  $output = theme('video_cck_google_flash', $embed, $width, $height, $autoplay);
  return $output;
}
