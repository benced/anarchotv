<?php
// $Id: dailymotion.inc,v 1.1.2.1 2008/07/24 10:44:54 wmostrey Exp $

function asset_embed_dailymotion_info() {
  return array('name' => 'Daily Motion');
}

function asset_embed_dailymotion_settings() {
  return array();
}

function asset_embed_dailymotion_extract_id($text) {
  $matches = array();
  // first try to match the embed tag
  preg_match('!param[^>]*value=\"http://www.dailymotion.com/swf/([^\"]*).*href=\"http://www.dailymotion.com/video/([^\"]*)!', $text, $matches);
  
  $data = array();
  if ($matches[1]) {
    $data['id'] = $matches[1];
  }
  if ($matches[2]) {
    $data['thumbnail_id'] = $matches[2];
  }    
  return $data;
}

function asset_embed_dailymotion_thumbnail_src($asset, $attr = array()) {
  $id = $asset->embed['thumbnail_id'];
  $url ='http://www.dailymotion.com/thumbnail/160x120/video/'. $id; 
  return $url;
}

function theme_asset_embed_dailymotion_thumbnail($asset, $attr = array()) {
  $url = asset_embed_dailymotion_thumbnail_src($asset, $attr);
  $alt = $title = $asset->title;
  $img = '<img src="'. check_url($url) .'" alt="'. check_plain($alt) .'" title="'. check_plain($title) .'" />';      
  return $img;
}

function theme_asset_embed_dailymotion_fullsize($asset, $attr = array()) {
  $id = $asset->embed['id'];

  return '<object width="'. $attr['width'] .'" height="'. $attr['height'] .'">'
       .'<param name="movie" value="http://www.dailymotion.com/swf/'. $id .'"></param>'
       .'<param name="allowfullscreen" value="false"></param>'
       .'<param name="wmode" value="transparent"></param>'
       .'<embed src="http://www.dailymotion.com/swf/'. $id .'" type="application/x-shockwave-flash" '
       .'wmode="transparent" width="'. $attr['width'] .'" height="'. $attr['height'] .'" allowfullscreen="false">'
       .'</embed></object>';
}

