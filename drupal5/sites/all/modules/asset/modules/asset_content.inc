<?php
// $id$

/**
 * Implementation of cck hook_field_info().
 */
function asset_field_info() {
  return array(
    'asset' => array('label' => 'Asset'),
  );
}

/**
 * Implementation of cck hook_field_settings().
 */
function asset_field_settings($op, $field) {
  switch ($op) {
    case 'database columns':
      return array(
        'aid' => array(
          'type' => 'int',
          'length' => 10,
          'unsigned' => TRUE,
          'not null' => TRUE,
          'default' => 0,
        ),
        'options' => array(
          'type' => 'longtext',
          'not null' => TRUE,
          'default' => "''",
        ),
        'caption' => array(
          'type' => 'longtext',
          'not null' => TRUE,
          'default' => "''",
        ),
        'copyright' => array(
          'type' => 'longtext',
          'not null' => TRUE,
          'default' => "''",
        ),
      );
  }
}

/**
 * Implementation of cck hook_field_formatter_info()
 */
function asset_field_formatter_info() {
  return array(
    'default' => array(
      'label' => 'Default',
      'field types' => array('asset'),
    ),
  );
}

/**
 * Prepare an individual item for viewing in a browser.
 */
function asset_field_formatter($field, $item, $formatter, $node) {
  $teaser = (arg(0) == 'node' && is_numeric(arg(1))) ? FALSE : TRUE;
  if(!empty($item['aid'])) {
    return asset_lightbox(array($item), FALSE, $teaser);
  }
}

/**
 * Declare information about a widget.
 */
function asset_widget_info() {
  return array(
    'asset' => array(
      'label' => 'Asset',
      'field types' => array('asset'),
    ),
  );
}

/**
 * Handle the parameters for a widget.
 */
function asset_widget_settings($op, $widget) {
  switch ($op) {
    case 'callbacks':
      return array(
        'default value' => CONTENT_CALLBACK_NONE,
      );
  }
}

/**
 * Define the behavior of a widget.
 */
function asset_widget($op, &$node, $field, &$items) {
  switch ($op) {
    case 'prepare form values':
      foreach ($items as $delta => $item) {
        // convert the field into a filter string
        if (isset($item['aid'])) {
          $items[$delta]['value'] = $item['options'];
        }
      }
      if ($_POST[$field['field_name']]) {
        $items = $_POST[$field['field_name']];
        foreach($items as $key => $item) {
          $a = asset_load(array('aid' => $item['aid']));
          $items[$key]['value'] = $a->filename;
        }
      }
      break;

    case 'form':
      // Start creating the form
      $field_name = $field['field_name'];
      $form = array($field_name => array('#tree' => TRUE));
      // If the field has multiple values
      if ($field['multiple']) {
        // Currently the fieldset is disabled because the asset javascript doesn't like it > name collision
        drupal_add_js(drupal_get_path('module', 'asset') .'/asset_more.js');
        $delta = 0;
        // Render link fields for all the entered values
        foreach ($items as $data) {
          if (is_array($data) && $data['aid']) {
            _asset_widget_form($form[$field['field_name']][$delta], $field, $data, $delta);
            $delta++;
          }
        }
        // Render one additional new asset field
        foreach (range($delta, $delta) as $delta) {
          _asset_widget_form($form[$field['field_name']][$delta], $field, array(), $delta);
        }
        // Create a wrapper for additional fields
        $form[$field['field_name']]['wrapper'] = array(
          '#type' => 'markup',
          '#value' => '<div id="' . str_replace('_', '-', $field['field_name']) . '-wrapper" class="clear-block"></div>',
        );
        // Add 'More' Javascript Callback
        $form[$field['field_name']]['more-url'] = array(
          '#type' => 'hidden',
          '#value' => url('asset/widget/js/' . $field['type_name'] . '/' . $field['field_name'], NULL, NULL, TRUE),
          '#attributes' => array('class' => 'more-links'),
          '#id' => str_replace('_', '-', $field['field_name']) . '-more-url',
        );
        // Add Current Field Count
        $form[$field['field_name']]['count'] = array(
          '#type' => 'hidden',
          '#value' => $delta,
          '#id' => str_replace('_', '-', $field['field_name']) . '-count',
        );
        // Add More Button
        $form[$field['field_name']]['more'] = array(
          '#type' => 'button',
          '#value' => t('More Assets'),
          '#name' => 'more',
          '#id' => str_replace('_', '-', $field['field_name']) . '-more',
        );
      }
      else {
        module_invoke('i18n', 'language_rtl') ? $rtl = "right" : $rtl = "left";
        // Show the one and only value field
        $form[$field_name][0]['value'] = array(
          '#type' => 'textfield',
          '#title' => t($field['widget']['label']),
          //'#description' => t($field['widget']['description']),
          '#default_value' => $items[0]['value'],
        );
        // Set the aid (hidden)
        $form[$field_name][0]['aid'] = array(
          '#type' => 'hidden',
          '#default_value' => $items[0]['aid'],
        );
        // Set the preview
        if(!empty($items[0]['value']) || (isset($_POST[$field_name][0]['aid']) && is_numeric($_POST[$field_name][0]['aid']))) {
          // When in preview mode, $items[0]['value'] is not set, so we need to get the $_POST data
          !empty($items[0]['value']) ? $aid = $items[0]['aid'] : $aid = filter_xss($_POST[$field_name][0]['aid']);
          $is_image = array('jpg', 'jpeg', 'png', 'gif', 'tif', 'tiff', 'bmp');
          $a = asset_load(array('aid' => $aid));
          // Preview the image
          if(in_array($a->extension, $is_image)) {
            if(variable_get('asset_imagecache', 0)) {
              $presets = _imagecache_get_presets();
              $preset = $presets[variable_get('asset_imagecache', 0)];
              $preview = theme('imagecache', $preset, str_replace("//","/", $a->filepath));
            }
            else {
              $asset = array(
                'aid' => $aid,
                'format' => 'image',
                'formatter' => 'asset',
                'width' => '120',
                'height' => '80',
              );
              $preview = asset_render_macro($asset);
            }
            $form[$field_name][0]['preview'] = array(
              '#prefix' => '<div id="preview_' . $field_name . '_0" style="float:'.$rtl.'">',
              '#suffix' => '</div>',
              '#value' => $preview,
            );
          }
          // Try to auto-detect the format
          else {
            $form[$field_name][0]['preview'] = array(
              '#prefix' => '<div id="preview_' . $field_name . '_0" style="float:'.$rtl.'">',
              '#suffix' => '</div>',
              '#value' => asset_preview($aid),
            );
          }
        }
        else {
          $form[$field_name][0]['preview'] = array(
            '#value' => '<div id="preview_' . $field_name . '_0" style="float:'.$rtl.'"></div>',
          );
        }
        // Set the caption
        $form[$field_name][0]['caption'] = array(
          '#type' => 'textfield',
          '#maxlength' => '256',
          '#default_value' => $items[0]['caption'],
          '#description' => 'Enter the caption for this asset',
          '#prefix' => '<div style="margin-'.$rtl.':130px"><table style="width:400px;margin-top:-15px;border-collapse:inherit"><tr><td style="padding:0px;margin:0px;text-align:center;padding-top:16px;width:10px" valign="top">'.l('<img src="'. base_path() . drupal_get_path('module', 'asset') . '/lullacons/doc-option-add.png' .'" alt="'. t("Insert asset") . '" title="'. t("Insert asset") .'"/>', 'asset/wizard/textfield', array(
    'class'=>'asset-textfield-link',
    'id'=>'asset-link-' . $field_name . '-0-value',
    'onclick'=>"window.open(this.href, 'asset_textfield_link', 'width=614,height=400,scrollbars=yes,status=yes,resizable=yes,toolbar=no,menubar=no'); return false",
    ), 'textfield=' . $field_name . '[0][value]&aidfield=' . $field_name . '[0][aid]&delta=0', NULL, FALSE, TRUE).'</td><td>',
          '#suffix' => '</td></tr></table></div>',

        );
        // Set the copyright
        $form[$field_name][0]['copyright'] = array(
          '#type' => 'textfield',
          '#maxlength' => '256',
          '#default_value' => $items[0]['copyright'],
          '#description' => 'Enter the copyright for this asset',
          '#prefix' => '<div style="margin-'.$rtl.':130px"><table style="width:400px;margin-top:-30px;border-collapse:inherit"><tr><td style="padding:0px;margin:0px;text-align:center;padding-top:16px;width:10px" valign="top">'.l('<img src="'. base_path() . drupal_get_path('module', 'asset') . '/lullacons/doc-option-remove.png' .'" alt="'. t("Delete asset") .'" title="'. t("Delete asset") .'"/>', '', array('onclick' => "
    document.getElementById('edit-" . str_replace("_","-",$field_name) . "-0-value').value='';
    document.getElementById('edit-" . str_replace("_","-",$field_name) . "-0-aid').value='';
    document.getElementById('edit-" . str_replace("_","-",$field_name) . "-0-caption').value='';
    document.getElementById('edit-" . str_replace("_","-",$field_name) . "-0-copyright').value='';
    document.getElementById('preview_" . $field_name . "_0').innerHTML='';
    return false;"), NULL, NULL, FALSE, TRUE).'</td><td>',
          '#suffix' => '</td></tr></table></div>',
        );
        $form[$field_name][0]['#prefix'] = '<div class="clear-block">';
        $form[$field_name][0]['#suffix'] = '</div>';
      }
      if(isset($field['widget']['description']) && !empty($field['widget']['description'])) {
        $form[$field_name][0]['description'] = array(
          '#value' => '<div class="description">' . $field['widget']['description'] . '</div>',
        );
      }
      return $form;

    case 'process form values':
      foreach ($items as $delta => $item) {
      	if($field['multiple']==1 && (empty($item['aid']) || !is_numeric($item['aid']))) {
      	  unset($items[$delta]);
      	  continue;
      	}
        if(!empty($item['aid']) && is_numeric($item['aid'])) {
          if(!empty($item['value'])) {
            $items[$delta]['options'] = $item['value'];
          }
          else {
            $a = asset_load(array('aid' => $item['aid']));
            $items[$delta]['options'] = $a->filename;
          }
          $items[$delta]['aid'] = $item['aid'];
          $items[$delta]['caption'] = $item['caption'] ? $item['caption'] : '';
          $items[$delta]['copyright'] = $item['copyright'] ? $item['copyright'] : '';
        }
      }
      break;
  }
}

function asset_widget_js($type_name, $field_name) {
  $field = content_fields($field_name, $type_name);
  $type = content_types($type);
  $delta = $_POST[$field_name]['count'];
  $form = array();
  $node_field = array();
  
  _asset_widget_form($form, $field, $node_field, $delta);
  
  // Assign parents matching the original form
  foreach (element_children($form) as $key) {
    $form[$key]['#parents'] = array($field_name, $delta, $key);
  }
  
  // Add names, ids, and other form properties
  foreach (module_implements('form_alter') as $module) {
    $function = $module .'_form_alter';
    $function('asset_widget_js', $form);
  }
  $form = form_builder('asset_widget_js', $form);
  
  $output = drupal_render($form);
  
  print drupal_to_js(array('status' => TRUE, 'data' => $output));
  exit;
}

function _asset_widget_form(&$form_item, $field, $node_field, $delta = 0) {
  module_invoke('i18n', 'language_rtl') ? $rtl = "right" : $rtl = "left";
  $field_name = $field['field_name'];
  $item_count = $delta + 1;
  $form_item = array(
    '#tree' => TRUE,
  );
  // Set the title
  $form_item['value'] = array(
    '#type' => 'textfield',
    '#title' => t($field['widget']['label']) . " " . $item_count,
    '#attributes' => array('disabled' => 'disabled'),
    '#default_value' => $node_field['value'],
  );
  // Set the aid (hidden)
  $form_item['aid'] = array(
    '#type' => 'hidden',
    '#default_value' => $node_field['aid'],
  );
  if(!empty($node_field['aid'])) {
    $aid = $node_field['aid'];
    // Set the preview
    $is_image = array('jpg', 'jpeg', 'png', 'gif', 'tif', 'tiff', 'bmp');
    $a = asset_load(array('aid' => $aid));
    // Preview the image
    if(in_array($a->extension, $is_image)) {
      if(variable_get('asset_imagecache', 0)) {
        $presets = _imagecache_get_presets();
        $preset = $presets[variable_get('asset_imagecache', 0)];
        $preview = theme('imagecache', $preset, str_replace("//","/", $a->filepath));
      }
      else {
        $asset = array(
          'aid' => $aid,
          'format' => 'image',
          'formatter' => 'asset',
          'width' => '120',
          'height' => '80',
         );
        $preview = asset_render_macro($asset);
      }
      $form_item['preview'] = array(
        '#prefix' => '<div id="preview_' . $field_name . '_' . $delta . '" style="float:' . $rtl . '">',
        '#suffix' => '</div>',
        '#value' => $preview,
      );
    }
    // Try to auto-detect the format
    else {
      if(asset_preview($aid)) {
        $form_item['preview'] = array(
          '#prefix' => '<div id="preview_' . $field_name . '_' . $delta . '" style="float:' . $rtl . '">',
          '#suffix' => '</div>',
          '#value' => asset_preview($aid),
        );
      } 
      else {
      // Display a placeholder for assets that don't have a preview: .mov, .doc, ...
        $form_item['preview'] = array(
          '#type' => 'markup', 
          '#value' => '<div id="preview_' . $field_name . '_' . $delta . '" style="float:' . $rtl . '"></div>',
        );
      }
    }
  }
  else {
    $form_item['preview'] = array(
      '#type' => 'markup', 
      '#value' => '<div id="preview_' . $field_name . '_' . $delta . '" style="float:'.$rtl.'"></div>',
    );
  }  
  // Set the caption
  $form_item['caption'] = array(
    '#type' => 'textfield',
    '#maxlength' => '256',
    '#default_value' => $node_field['caption'],
    '#description' => 'Enter the caption for this asset',
    '#prefix' => '<div style="margin-'.$rtl.':130px"><table style="width:400px;margin-top:-15px;border-collapse:inherit"><tr><td style="padding:0px;margin:0px;text-align:center;padding-top:16px;width:10px" valign="top">'.l('<img src="'. base_path() . drupal_get_path('module', 'asset') . '/lullacons/doc-option-add.png' .'" alt="'. t("Insert asset") . '" title="' . t("Insert asset") . '"/>', 'asset/wizard/textfield', array(
    'class'=>'asset-textfield-link',
    'id'=>'asset-link-' . $field_name . '-' . $delta . '-value',
    'onclick'=>"window.open(this.href, 'asset_textfield_link', 'width=614,height=400,scrollbars=yes,status=yes,resizable=yes,toolbar=no,menubar=no'); return false",
    ), 'textfield=' . $field_name . '[' . $delta . '][value]&aidfield=' . $field_name . '[' . $delta . '][aid]&delta=' . $delta, NULL, FALSE, TRUE).'</td><td>',
    '#suffix' => '</td></tr></table></div>',
  );
  // Set the copyright
  $form_item['copyright'] = array(
    '#type' => 'textfield',
    '#maxlength' => '256',
    '#default_value' => $node_field['copyright'],
    '#description' => 'Enter the copyright for this asset',
    '#prefix' => '<div style="margin-'.$rtl.':130px"><table style="width:400px;margin-top:-30px;border-collapse:inherit"><tr><td style="padding:0px;margin:0px;text-align:center;padding-top:16px;width:10px" valign="top">'.l('<img src="'. base_path() . drupal_get_path('module', 'asset') . '/lullacons/doc-option-remove.png' .'" alt="'. t("Delete asset") . '" title="' . t("Delete asset") . '"/>', '', array('onclick' => "
    document.getElementById('edit-" . str_replace("_","-",$field_name) . "-" . $delta . "-value').value='';
    document.getElementById('edit-" . str_replace("_","-",$field_name) . "-" . $delta . "-aid').value='';
    document.getElementById('edit-" . str_replace("_","-",$field_name) . "-" . $delta . "-caption').value='';
    document.getElementById('edit-" . str_replace("_","-",$field_name) . "-" . $delta . "-copyright').value='';
    document.getElementById('preview_" . $field_name . "_" . $delta . "').innerHTML='';
    return false;"), NULL, NULL, FALSE, TRUE).'</td><td>',
    '#suffix' => '</td></tr></table></div>',
  );
  if($delta==0 && isset($field['widget']['description']) && !empty($field['widget']['description'])) {
    $form_item['description'] = array(
      '#value' => '<div class="description">' . $field['widget']['description'] . '</div>',
    );
  }
  $form_item['#prefix'] = '<div class="clear-block">';
  $form_item['#suffix'] = '</div>';
}
