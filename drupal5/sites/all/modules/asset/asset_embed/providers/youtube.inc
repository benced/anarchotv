<?php
// $Id: youtube.inc,v 1.1.2.1 2008/07/24 10:44:54 wmostrey Exp $

function asset_embed_youtube_info() {
  return array('name' => 'YouTube');
}

function asset_embed_youtube_settings() {
  $form = array();
  return $form;
}

function asset_embed_youtube_extract_id($text) {
  $matches = array();
  // first try to match the embed tag
  preg_match('/param[^>]*value=\"http:\/\/www.youtube.com\/v\/([^\"]*)\"/', $text, $matches);
  
  // then the URL
  if (!isset($matches[1])) {
    // http://youtube.com/watch?v=9193duFdUec
    preg_match('/http:\/\/www.youtube.com\/watch\?v=(.*)^\"/', $text, $matches);
  }    
  return (isset($matches[1]) ? array('id' => $matches[1]) : false);
}

function asset_embed_youtube_thumbnail_src($asset, $attr = array()) {
  $id = $asset->embed['id'];
  $img = $attr['thumbnail'] ? $attr['thumbnail'] : 'default';
  $url = "http://img.youtube.com/vi/$id/$img.jpg";
  return $url;
}

function theme_asset_embed_youtube_thumbnail($asset, $attr = array()) {
  $url = asset_embed_youtube_thumbnail_src($asset, $attr);
  $alt = $title = $asset->title;
  $img = '<img src="'. check_url($url) .'" alt="'. check_plain($alt) .'" title="'. check_plain($title) .'" />';      
  return $img;
}

function theme_asset_embed_youtube_fullsize($asset, $attr = array()) {
  $id = $asset->embed['id'];
  
  return '<object width="'. $attr['width'] .'" height="'. $attr['height'] .'">'
       .'<param name="movie" value="http://www.youtube.com/v/'. $id .'"></param>'
       .'<param name="wmode" value="transparent"></param>'
       .'<embed src="http://www.youtube.com/v/'. $id .'" type="application/x-shockwave-flash" '
       .'wmode="transparent" width="'. $attr['width'] .'" height="'. $attr['height'] .'">'
       .'</embed></object>';
}

function asset_embed_youtube_formatter_options($asset, $format) {
  $form = array();
  switch ($format) {
    case 'thumbnail':
      $form['thumbnail'] = array(
        '#type' => 'radios',
        '#title' => t('Select an image'),
        '#options' => array(
          1 => theme('asset_embed_youtube_thumbnail', $asset, array('thumbnail' => 1)),
          2 => theme('asset_embed_youtube_thumbnail', $asset, array('thumbnail' => 2)),
          3 => theme('asset_embed_youtube_thumbnail', $asset, array('thumbnail' => 3)),
        ),
        '#default_value' => 1,
        '#weight' => -1,
      );
      break;
  }
  return $form;
}
