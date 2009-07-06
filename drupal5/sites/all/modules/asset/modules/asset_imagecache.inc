<?php
// $Id: asset_imagecache.inc,v 1.1.2.5 2008/08/07 06:58:58 wmostrey Exp $

function imagecache_asset_settings(){
  $form[] = array('#value' => '<p>Select which imagecache presets should be available to use as asset formatters.</p>');
  $presets = imagecache_presets();
  foreach($presets as $preset){
    $formats[$preset['presetname']] = array(
      'name' => $preset['presetname'] . ' (imagecache)',
      'types' => array('jpg','gif','png'),
    );
    $form['asset_imagecache_preset_'.$preset['presetid']] = array(
      '#type' => 'checkbox',
      '#title' => $preset['presetname'],
      '#default_value' => variable_get('asset_imagecache_preset_'.$preset['presetid'], 1),
    );      
  }
  return system_settings_form($form);
}

function imagecache_asset_formatter($op='info',$asset=null,$attr=array()){
  switch($op){
    case 'info':
      $presets = imagecache_presets();
      foreach($presets as $preset){
        if(variable_get('asset_imagecache_preset_'.$preset['presetid'], 1)){
          $formats[$preset['presetname']] = array(
            'name' => $preset['presetname'] . ' (imagecache)',
            'types' => array('local' => array('jpg','gif','png')),
          );
        }
      }
      return $formats;
    case 'options':
      $path = imagecache_create_path($attr['format'], $asset->filepath);
      if(!file_exists($path)){
        // this is the easiest albeit indirect way i can see to generate the image
        $result = drupal_http_request(url(file_create_url($path)));
      }
      $size = image_get_info(imagecache_create_path($attr['format'], $asset->filepath));
      $form['width'] = array('#type' => 'hidden', '#value' => $size['width']);        
      $form['height'] = array('#type' => 'hidden', '#value' => $size['height']);
      $form['resizable'] = array();
      $form['_width'] = array(
        '#type' => 'item',
        '#title' => t('Width, preset by imagecache'),
        '#value' => $size['width'] . 'px',
      );
      $form['_height'] = array(
        '#type' => 'item',
        '#title' => t('Height, preset by imagecache'),
        '#value' => $size['height'] . 'px',
      );   
      return $form;
      break;
    case 'img':
      return imagecache_create_path($attr['format'], $asset->filepath);
    case 'preview':
      return theme('imagecache', $attr['format'], $asset->filepath, '', '', array('width' => '100'));
    case 'render':
      return theme('imagecache', $attr['format'], $asset->filepath);
  }
}

function _asset_imagecache_get_size($name,$path){
  $preset = imagecache_preset_by_name($name);
  $actions = imagecache_preset_actions($preset);
  $action = array_pop($actions);
  $rule = $action['data'];
  
  if(!$rule['height'] || !$rule['width']){
    $info = image_get_info(file_create_path($path));
    if(!$rule['height']){
      $aspect = $rule['width'] / $info['width'];
      $rule['height'] = $info['height'] * $aspect; 
    }else if(!$rule['width']){
      $aspect = $rule['height'] / $info['height'];
      $rule['width'] = $info['width'] * $aspect; 
    }
  }
  return $rule;
}
