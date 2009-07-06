<?php

function asset_views_tables(){
  $tables['asset_node'] = array(
    'name' => 'asset_node',
    'join' => array(
      'type' => 'inner',
      'left' => array(
        'table' => 'node',
        'field' => 'nid',
      ),
      'right' => array(
        'field' => 'nid',
      ),
    ),
  );
  
  
  $tables['asset'] = array(
    'name' => 'asset',
    'join' => array(
      'type' => 'inner',
      'left' => array(
        'table' => 'asset_node',
        'field' => 'aid',
      ),
      'right' => array(
        'field' => 'aid',
      ),
    ),
    'fields' => array(
      'aid' => array(
        'name' => t('Asset'),
        'help' => t('Display the formatted asset. NOTE: If using one of the formatters under <em><strong>Only Available for Specific Extensions</strong></em>, be sure to apply the Asset:Extension filter for the specific extensions that apply to that formatter.'),
        'option' => array(
          '#type' => 'select', 
          '#options' => 'asset_views_options_formatters',
        ),
        'handler' => array(
          'asset_views_handler_field_asset' => t('No Link'),
          'asset_views_handler_field_asset_link' => t('Link to asset'),          
          'asset_views_handler_field_asset_node_link' => t('Link to node'),
        ),
      ),
      'filename' => array(
        'name' => t('Asset: Filename'),
        'help' => t('Display the filename of the asset.'),
      ),
      'extension' => array(
        'name' => t('Asset: Extension'),
        'help' => t('Display the extension of the asset.'),
      ),
      'dirname' => array(
        'name' => t('Asset: Directory'),
        'help' => t('Display the directory of the asset.'),
      ),
      'filesize' => array(
        'name' => t('Asset: File Size'),
        'help' => t('Display the file size of the asset.'),
      ),
    ),
    'filters' => array(
      'extension' => array(
        'name' => t('Asset: File Extension'),
        'help' => t('Filter by file extension.'),
        'operator' => views_handler_operator_andor(),
        'value' => array(
          '#type' => 'select',
          '#options' => 'asset_views_options_file_extensions',
          '#multiple' => true,
          '#size' => 4,
        ),
      ),
      'type' => array(
        'name' => t('Asset: File Type'),
        'help' => t('Filter by file type.'),
        'operator' => views_handler_operator_andor(),
        'value' => array(
          '#type' => 'select',
          '#options' => 'asset_views_options_file_types',
          '#multiple' => true,
          '#size' => 4,
        ),
      ),
      'dirname' => array(
        'name' =>t ('Asset: Directory'),
        'help' => t('Filter by an assets parent directory.'),
        'operator' => views_handler_operator_eqneq(),
      ),
      'distinct' => array(
        'name' => t('Asset: Distinct'),
        'operator' => array('=' => 'is'),
        'list' => array('distinct' => 'distinct'),
        'handler' => 'asset_views_handler_filter_distinct',
        'value-type' => 'array',
        'help' => t('This filter ensures that each asset may only be listed once, even if it matches multiple criteria.'),
      ),
    ),
  );
  return $tables;
}


function asset_views_handler_field_asset_node_link($fieldinfo, $fielddata, $value, $data) {
  $link = 'node/'.$data->nid;
  return asset_views_handler_field_asset($fieldinfo, $fielddata, $value, $data, $link);
}

function asset_views_handler_field_asset_link($fieldinfo, $fielddata, $value, $data) {
  $asset = asset_load($value);
  $link = $asset->url;
  return asset_views_handler_field_asset($fieldinfo, $fielddata, $value, $data, $link);
}

function asset_views_handler_field_asset($fieldinfo, $fielddata, $value, $data, $link = false) {
  static $formatters;
  if(!$formatters){
    $formatters = asset_get_formatters();
  }
  
  $asset = asset_load($value);
  switch($fielddata['options']){
    case 'full':
      list($module,$format) = explode(':',asset_get_default_formatter($asset->type, $asset->extension, false));
      break;
    case 'teaser':
      list($module,$format) = explode(':',asset_get_default_formatter($asset->type, $asset->extension, true));
      break; 
    default:   
      list($module,$format) = explode(':',$fielddata['options']);      
      $found = false;
      foreach((array)$formatters[$asset->type][$asset->extension] as $formatter){
        if($formatter['module'] == $module && $formatter['format'] == $format){
          $found = true;
          break;
        }
      }
      foreach((array)$formatters[$asset->type]['*'] as $formatter){
        if($formatter['module'] == $module && $formatter['format'] == $format){
          $found = true;
          break;
        }
      }
      foreach((array)$formatters['*']['*'] as $formatter){
        if($formatter['module'] == $module && $formatter['format'] == $format){
          $found = true;
          break;
        }
      }
      if(!$found){
        $module = 'asset';
        $format = 'link';
      }
      break;
  }
  
  if($link){
    if($format == 'link'){
      $html = $asset->title ? $asset->title : $asset->filename;
    }else{
      $html = module_invoke($module, 'asset_formatter', 'render', $asset, array('format'=>$format));
    }
    return l($html, $link, array(), null, null, false, true);    
  }else{
    return module_invoke($module, 'asset_formatter', 'render', $asset, array('format'=>$format));
  }
}

function asset_views_options_formatters(){
  $all = t('Available for all Extensions');
  $specific = t('Only available for specific Extensions');
  
  $options[$all]['full'] = t('Default Full-Text');
  $options[$all]['teaser'] = t('Default Teaser');
  foreach (module_implements('asset_formatter') as $module) {
    $list = module_invoke($module, 'asset_formatter', 'info');
    foreach ($list as $key => $data) {
      foreach($data['types'] as $type=>$exts){
        if($exts == array('*')){
          $options[$all][$module.':'.$key] = $data['name'] . ' (' . join($exts,',').')';
        }else{
          $options[$specific][$module.':'.$key] = $data['name'] . ' (' . join($exts,',').')';
        }
      }
    }
  }
  return $options;
}

function asset_views_options_file_extensions(){
  $result = db_query('SELECT DISTINCT(extension) FROM {asset} ORDER BY extension');
  $options = array();
  while($row = db_fetch_object($result)){
    $options[] = $row->extension;
  }
  return drupal_map_assoc($options);
}

function asset_views_options_file_types(){
  $result = db_query('SELECT DISTINCT(type) FROM {asset} ORDER BY type');
  $options = array();
  while($row = db_fetch_object($result)){
    $options[] = $row->type;
  }
  return drupal_map_assoc($options);
}

function asset_views_handler_filter_distinct($op, $filter, $filterinfo, &$query) {
  if (!$query->no_distinct) {
    $query->set_distinct();
    $query->add_groupby('asset_node.aid');
  }
}

