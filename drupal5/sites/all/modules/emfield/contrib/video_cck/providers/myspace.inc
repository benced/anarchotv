<?php
// $Id: myspace.inc,v 1.5 2008/02/22 18:46:26 aaron Exp $

define('VIDEO_CCK_MYSPACE_MAIN_URL', 'http://vids.myspace.com/index.cfm?fuseaction=vids.videos');

function video_cck_myspace_info() {
  $name = t('MySpace');
  $features = array(
    array(t('Autoplay'), t('No'), ''),
    array(t('RSS Attachment'), t('No'), ''),
    array(t('Thumbnails'), t('Yes'), ''),
  );
  return array(
    'provider' => 'myspace',
    'name' => $name,
    'url' => VIDEO_CCK_MYSPACE_MAIN_URL,
    'settings_description' => t('These settings specifically affect videos displayed from !provider.', array('!provider' => l($name, VIDEO_CCK_MYSPACE_MAIN_URL, array('target' => '_blank')))),
    'supported_features' => $features,
  );
}

function video_cck_myspace_settings() {
  $form = array();
  return $form;
}

function video_cck_myspace_request($code, $cacheable = TRUE) {
  $args = array();
  return module_invoke('emfield', 'request_xml', 'myspace', "http://vids.myspace.com/index.cfm?fuseaction=vids.individual&videoid=$code", $args, $cacheable);
}

/**
 *  this scrapes the thumbnail from the video page and caches it
 */
function _video_cck_myspace_scrape_thumbnail($video, $cached = true) {
  if (!$cached || !$cache = cache_get('myspace:thumbnail:' . $video, 'cache')) {
    $vidid = substr($video, 0, 10);
    if ($str = file_get_contents("http://vids.myspace.com/index.cfm?fuseaction=vids.individual&videoid=$vidid")) {
      // grab videos channel-id first
      $channelID = preg_replace('/.*tvchanid=([0-9]+);.*/si','\1',$str);

      if (is_numeric($channelID)) {
        // now from channel-page grab videos thumbnail
        $str2 = file_get_contents("http://vids.myspace.com/index.cfm?fuseaction=vids.viewVideos&channelid=$channelID");

        if ($str2) {
          $picturelink = preg_replace('@.*href="[^"]+'.$vidid.'"[^>]+><img[^>]+src="([^"]+)".*@si','\1',$str2);

          if ($picturelink) {
            $thumbnail = $picturelink;
            cache_set('myspace:thumbnail:' . $video, 'cache', $thumbnail, time() + 3600);
          }
        }
      }
    }
  }
  else {
    $thumbnail = $cache->data;
  }
  return $thumbnail;
}

function video_cck_myspace_extract($embed) {
  // <a href="http://myspacetv.com/index.cfm?fuseaction=vids.individual&videoid=14686340">What's That Floating In The Water Part 1</a><br><embed src="http://lads.myspace.com/videos/vplayer.swf" flashvars="m=14686340&v=2&type=video" type="application/x-shockwave-flash" width="430" height="346"></embed><br><a href="http://myspacetv.com/index.cfm?fuseaction=vids.addToProfileConfirm&videoid=14686340&title=What's That Floating In The Water Part 1">Add to My Profile</a> | <a href="http://myspacetv.com/index.cfm?fuseaction=vids.home">More Videos</a>
  return array(
    '@src="myspace\.com/index.cfm\?fuseaction=vids\.individual&videoid=([^&"]+)@i',
    '@myspace\.com/index\.cfm\?fuseaction=vids\.individual&videoid=([^&"]+)@i',
    '@src="myspacetv\.com/index.cfm\?fuseaction=vids\.individual&videoid=([^&"]+)"@i',
    '@myspacetv\.com/index\.cfm\?fuseaction=vids\.individual&videoid=([^&"]+)@i',
  );
}

function video_cck_myspace_video_link($video_code) {
  return 'http://vids.myspace.com/index.cfm?fuseaction=vids.individual&videoid=' . $video_code;
}

function theme_video_cck_myspace_flash($embed, $width, $height) {
  static $count = 0;
  if ($embed) {
    $output .= "    <object type=\"application/x-shockwave-flash\" height=\"$height\" width=\"$width\" data=\"http://lads.myspace.com/videos/vplayer.swf\" id=\"emfield-video-myspace-$count\" >
      <param name=\"movie\" value=\"http://lads.myspace.com/videos/vplayer.swf\" />
      <param name=\"allowScriptAcess\" value=\"sameDomain\"/>
      <param name=\"quality\" value=\"best\"/>
      <param name=\"bgcolor\" value=\"#FFFFFF\"/>
      <param name=\"scale\" value=\"noScale\"/>
      <param name=\"salign\" value=\"TL\"/>
      <param name=\"FlashVars\" value=\"m=$embed&type=video\" />
      <param name=\"wmode\" value=\"transparent\" />
    </object>\n";
  }
  $count++;
  return $output;
}

function video_cck_myspace_thumbnail($field, $item, $formatter, $node, $width, $height) {
  return _video_cck_myspace_scrape_thumbnail($item['value']);
}

function video_cck_myspace_video($embed, $width, $height, $field, $item) {
  $output = theme('video_cck_myspace_flash', $embed, $width, $height);
  return $output;
}

function video_cck_myspace_preview($embed, $width, $height, $field, $item) {
  $output = theme('video_cck_myspace_flash', $embed, $width, $height);
  return $output;
}
