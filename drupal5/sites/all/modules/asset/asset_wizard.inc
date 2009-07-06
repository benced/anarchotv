<?php
// $Id: asset_wizard.inc,v 1.1.2.19 2008/07/30 08:09:29 wmostrey Exp $

define('ASSET_WIZARD_NEXT', 'Next');
define('ASSET_WIZARD_PREVIOUS', 'Back');
define('ASSET_WIZARD_CANCEL', 'Cancel');
define('ASSET_WIZARD_DELETE', 'Delete');
define('ASSET_WIZARD_SELECT', 'Select');
define('ASSET_WIZARD_FINAL_STEP', 4);
define('ASSET_WIZARD_FINISH_REDIRECT', 'node');
define('ASSET_THUMBNAIL', 'image');

/**
 * Form Functions =============================================================
 */

/**
 * Main form builder function for the asset wizard.
 */
function asset_wizard_form($form_values = null) {
  // We need some values passed around:
  //   $op for the "delete" button on the asset selection screen
  //   $fieldname for the direct "select" button
  if (!isset($_GET['noreset'])) $_SESSION['asset_op'] = $_GET['op'];
  if (isset($_GET['textfield'])) {
    list($fieldname, $lost) = split("\]\[", $_GET['textfield']);
    $fieldname = str_replace(array("_", "["), "-", $fieldname);
    $fieldname = "edit-" . $fieldname . "-";
    $_SESSION['asset_fieldname'] = $fieldname;
    // drupal_set_message("op2: ". $_SESSION['asset_op']);
  }
  elseif (isset($_GET['textarea']) || $_GET['q']=="asset/wizard/tinymce") {
    unset($_SESSION['asset_fieldname']);
    $_SESSION['asset_op'] = "tinymce";
  }
  // Allow the aid to be passed in the querystring
  if ($form_values == null && is_numeric($_GET['aid'])) {
    $form_values['step'] = 2;
    $form_values['aid'] = $_GET['aid'];
  }
  // Step Handling
  if (!isset($form_values)) {
      $step = 1;
  }
  else {
    list($lost, $lost, $field) = split("-", $fieldname);
    switch($form_values['op']) {
      case t('Back'):
        if ($field==ASSET_THUMBNAIL) {
          $step = 1;
        }
        elseif (isset($form_values['delete_cancel'])) {
          unset($_SESSION['messages']);
          drupal_goto(filter_xss($_GET['q']), 'dir=' . $form_values['dir']);
        }
        else  {
          $step = $form_values['step'] > 1 ? $form_values['step'] - 1 : 1;
        }
        break;
      case t('Next'):
      case t('Finish'):
        if ($step == 1 && !$form_values['aid']) {
          // dont proceed without a valid aid.
          break;
        }
        if ($field==ASSET_THUMBNAIL && $form_values['step'] == 1) {
          // When inserting an image preview, don't offer the option to select a size
          $step = $form_values['step'] + 2;
        }
        else {
          $step = $form_values['step'] + 1;
        }
        break;
      case t('Edit'):
        $asset = asset_load(array('aid' => $form_values['aid']));
        $form['edit-asset'] = array(
          '#type' => 'hidden',
          '#value' => '1',
        );
        $form['aid'] = array(
          '#type' => 'hidden',
          '#value' => $form_values['aid'],
        );
        $fileinfo = pathinfo($asset->filename);
        if (strlen($fileinfo['extension'])==3) {
          // This is a regular file
          $form['filename'] = array(
            '#title' => t('Filename'),
            '#type' => 'textfield',
            '#default_value' => $asset->filename,
            '#description' => t("Warning: you are changing the actual filename. If you're not sure what you're doing, you should leave this unchanged."),
          );
        }
        else {
          // Not a regular file, could be a youtube movie for instance (filename $instance.youtube)
          $form['filename'] = array(
            '#title' => t('Filename'),
            '#type' => 'item',
            '#value' => $asset->filename,
            '#description' => t("You can only change the filename of regular files, not of YouTube movies for instance."),
          );
        }
        $form['old-filename'] = array(
          '#type' => 'hidden',
          '#value' => $asset->filename,
        );
        $form['old-parent'] = array(
          '#type' => 'hidden',
          '#value' => $asset->dirname,
        );
        $form = array_merge($form, asset_wizard_default_fields($form_values));
        $form['title']['#default_value'] = $asset->title;
        $form['author']['#default_value'] = $asset->author;
        break;  
      case t('Delete'):
        if (!$form_values['deleteme']) {
          $form_values['step'] = 1;
          $form['delete_warning'] = array(
            '#value' => "&nbsp;". t("You are about to delete the selected asset. Press the delete button to confirm deletion."),
          );
          $form['deleteme'] = array(
            '#type' => 'hidden',
            '#value' => 1,
          );
        }
        elseif ($form_values['deleteme']==1) {
          $form['deleteme']   = array(
            '#type' => 'hidden',
            '#value' => 0,
          );
        }
      default:
        // only back,next,finish buttons should effect the step
        $step = $form_values['step'];
        break;
    }
  }
  $form['step'] = array('#type' => 'hidden', '#value' => $step);
  drupal_set_title("Asset Wizard - Step $step");

  // Extract asset info from the macro or aid
  if ($form_values['macro']) {
    $macros = asset_get_macros(' '.$form_values['macro'].' ');
    $macro = array_pop($macros);
    if ($macro['aid']) {
      $asset = asset_load($macro['aid']);
    }
  }elseif ($form_values['aid']) {
    $asset = asset_load($form_values['aid']);
    $macro['aid'] = $form_values['aid'];
  }
  if (is_object($asset) && $step > 1) {
    $form[] = array('#value'=>'<div class="asset-selected-preview">'.asset_js_preview($asset->aid,false,true).'</div>');    
  }

  // Build the form based on what step we are on
  switch($step) {
    case 1:
      asset_wizard_form_asset_selection($form, $form_values, $asset);
      break;
    case 2:
      $form['macro'] = array('#type'=>'hidden', '#value' => asset_build_macro($macro));
      $form['help'] = array('#value' => t('Select a formatting option, or <em>Choose</em> or <em>Upload</em> a new asset.'));
      $formatters = asset_get_formatters();
      $ext = strtolower($asset->extension);
      $default = variable_get('asset_default_formatter_' . $asset->type . '_' . $ext, 'asset:link');
      if (is_array($formatters[$asset->type][$ext])) {
        foreach ($formatters[$asset->type][$ext] as $formatter) {
          $form['formatter'][] = array(
            '#type' => 'radio',
            '#title' => $formatter['name'],
            '#return_value' => $formatter['module'].':'.$formatter['format'],
            '#default_value' => !isset($_GET['format']) ? $default : $_GET['format'],
            '#description' => $formatter['description'] ? $formatter['description'] : NULL,
            '#parents' => array('formatter'),
          );
        }
      }
      if (is_array($formatters[$asset->type]['*'])) {
        foreach ($formatters[$asset->type]['*'] as $formatter) {
          $form['formatter'][] = array(
            '#type' => 'radio',
            '#title' => $formatter['name'],
            '#return_value' => $formatter['module'].':'.$formatter['format'],
            '#default_value' => !isset($_GET['format']) ? $default : $_GET['format'],
            '#description' => $formatter['description'] ? $formatter['description'] : NULL,
            '#parents' => array('formatter'),
          );
        }
      }
      if (is_array($formatters['*']['*'])) {
        foreach ($formatters['*']['*'] as $formatter) {
          $form['formatter'][] = array(
            '#type' => 'radio',
            '#title' => $formatter['name'],
            '#return_value' => $formatter['module'].':'.$formatter['format'],
            '#default_value' => !isset($_GET['format']) ? $default : $_GET['format'],
            '#description' => $formatter['description'] ? $formatter['description'] : NULL,
            '#parents' => array('formatter'),
          );
        }
      }
      break;
    case 3:
      list($macro['formatter'],$macro['format']) = explode(':',$form_values['formatter']);
      $form['macro'] = array(
        '#type'=>'hidden', 
        '#value' => asset_build_macro($macro),
      );
      $default_options['title'] = array(
        '#type' => 'textfield',
        '#title' => t('Title'),
        '#default_value' => $asset->title,
        '#size' => 40,
      );
      list($lost, $lost, $field) = split("-", $fieldname);
      if (!preg_match('@link@i', $form_values['formatter'])) {
      	if ($field!=ASSET_THUMBNAIL) {
      	  // Don't allow the option to select a width/height when inserting a node thumbnail
          $default_options['width'] = array(
            '#type' => 'textfield',
            '#title' => t('Width'),
            '#default_value' => !isset($_GET['width']) ? $asset->width : filter_xss($_GET['width']),
          );
          $default_options['height'] = array(
            '#type' => 'textfield',
            '#title' => t('Height'),
            '#default_value' => !isset($_GET['height']) ? $asset->height : filter_xss($_GET['height']),
          );
          $default_options['resizable'] = array(
            '#type' => 'hidden',
            '#value' => 'true',
          );
      	}
      	else {
          $default_options['width'] = array(
            '#type' => 'hidden',
            '#title' => t('Width'),
            '#default_value' => $asset->height,
          );
          $default_options['height'] = array(
            '#type' => 'hidden',
            '#title' => t('Height'),
            '#default_value' => $asset->width,
          );
      	}
        $default_options['align'] = array(
          '#type' => 'select',
          '#title' => t('Alignment'),
          '#options' => array(
            'none' => t('None'),
            'left' => t('Left'),
            'center' => t('Center'),
            'right' => t('Right'),
          ),
          '#default_value' => !isset($_GET['align']) ? 'none' : filter_xss($_GET['align']),
        );
      }
      $module_options = (array)module_invoke($macro['formatter'],'asset_formatter','options',$asset,$macro);
      $form['options'] = array_merge($default_options, $module_options);
      $form['options']['#tree'] = true;
      break;
      
    case 4:
      if (is_array($form_values['options'])) {
        foreach($form_values['options'] as $k => $v) {
          if (!in_array($k, array('aid','formatter','format')) && $v) {
            $macro[$k] = $v;
          }
        }
      }
      $form['macro'] = array(
        '#type'=>'textarea',
        '#title'=>'Paste the following code into your text.',
        '#value'=>asset_build_macro($macro),
      );
      // Create the preview code for the CCK field
      $is_image = array('jpg', 'jpeg', 'png', 'gif', 'tif', 'tiff', 'bmp');
      $a = asset_load(array('aid' => $macro['aid']));
      // Preview the image
      if (in_array($a->extension, $is_image)) {
        if (variable_get('asset_imagecache', 0)) {
          $presets = imagecache_presets();
          $preset = $presets[variable_get('asset_imagecache', 0)]['presetname'];
          $preview = theme('imagecache', $preset, str_replace("//","/", $a->filepath));
        }
        else {
          $asset = array(
            'aid' => $macro['aid'],
            'format' => 'image',
            'formatter' => 'asset',
            'width' => '120',
            'height' => '80',
          );
          $preview = asset_render_macro($asset);
        }
      }
      // Try to auto-detect the format
      else {
        $preview = asset_preview($macro['aid']);
      }
      $form['preview'] = array(
        '#type'=>'hidden', 
        '#value' => $preview,
      );
      $form['author'] = array(
        '#type' => 'hidden',
        '#value' => $a->author,
      );
      $form[] = array('#value' => asset_wizard_finish());
      break;
  }
  
  // This is important. If we're on the final step, we tell drupal to use the 
  // normal redirect functionality. That means the browser goes to whatever page
  // after the final submit. All previous steps don't redirect, so just post 
  // back to this form.
  $form['#redirect'] = ($step == ASSET_WIZARD_FINAL_STEP) ? NULL : FALSE;
  if ($form['#redirect'] == FALSE) {
    if ($step == 1) {
      $form['#redirect'] = NULL;
    }
  }
  $form['#multistep'] = TRUE;
  $form['#tree'] = TRUE;

  $form['footer']['#tree'] = false;
  // We only want to display the delete button on the "Choose" page
  // if ($step == 1 && empty($_SESSION['asset_op'])) {
  if ($step != 1 || !empty($_SESSION['asset_op']) || $_GET['op']==t("Delete Folder")) {
  	if ($_GET['op']==t("Delete Folder")) {
  	  $form['delete_cancel'] = array(
  	    '#type' => 'hidden',
  	    '#value' => 1,
  	  );
  	  $form['dir'] = array(
  	    '#type' => 'hidden',
  	    '#value' => filter_xss($_GET['dir']),
  	  );
  	}
    $form['footer']['previous'] = array(
      '#type' => 'submit',
      '#value' => t(ASSET_WIZARD_PREVIOUS),
      '#attributes' => (isset($form_values['op']) || $step > 1 || $_GET['op']==t("Delete Folder") ? array() : array('disabled'=>'disabled'))
    );
  }
  if ($step == ASSET_WIZARD_FINAL_STEP - 1) {
    $form['footer']['finish'] = array(
      '#type' => 'submit',
      '#value' => t('Finish'),
    );
  }
  elseif ($step != 1 || !empty($_SESSION['asset_op'])) {
    $form['footer']['next'] = array(
      '#type' => 'submit',
      '#value' => t(ASSET_WIZARD_NEXT),
    );
  }
  if ($step == 1 && $_SESSION['asset_op']!="Upload" && $_SESSION['asset_op']!="New Folder") {
  	if ($_SESSION['asset_op']!="tinymce") {
      $form['footer']['select'] = array(
        '#type' => 'button',
        '#value' => t('Select'),
      );
  	}
    $form['footer']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
    );
    $form['footer']['edit'] = array(
      '#type' => 'submit',
      '#value' => t('Edit'),
    );
  }
  $form['footer']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t(ASSET_WIZARD_CANCEL),
    '#attributes' => array('alt' => t('Cancel and Return')),
  );
  return $form;
}

/**
 * Form builder for step 1 of the asset wizard.  This step is the most intensive
 * so it has been separated out for clarity.
 */
function asset_wizard_form_asset_selection(&$form, &$form_values, $current_asset=NULL) {
  global $user;
  static $types, $ops;
  
  if (!isset($types)) {
    foreach(module_implements('asset_type') as $module) {
      $types[$module] = module_invoke($module, 'asset_type', 'info');
      foreach($types[$module] as $delta=>$type) {
        $ops[$type['value']] = array('module'=>$module,'delta'=>$delta);
      } 
    }
  }
  
  if ($_GET['op']==t("Delete Folder")) {
   $dir = filter_xss($_GET['dir']);
    if (empty($dir) || strtolower($dir)==strtolower($user->name)) {
      drupal_set_message(t('You can not delete the main folder.'), 'error');
      drupal_goto(filter_xss($_GET['q']), 'dir=' . $dir);
    }
    else {
      // First check if this folder has sub-folders, these need to be deleted first
      $subdirs = db_query("SELECT aid FROM {asset} WHERE type='directory' AND dirname='%s' LIMIT 1", $dir);
      if (db_num_rows($subdirs)) {
         // There is at least one sub-folder
        drupal_set_message(t('This folder contains at least one sub-folder.<br/>You need to delete these first before continuing.'), 'error');
        drupal_goto(filter_xss($_GET['q']), 'dir=' . $dir);
      }
      else {
        // 	No subfolders, let's display the warning
        drupal_set_message('Warning: you are about to delete this folder and all assets within this folder.');
        $form['warning'] = array(
          '#value' => t('Click the Next button if you are sure you want to delete this folder and its content.<br/>Click the Back button if you want to go back.<br/><br/>This operation CAN NOT be undone.'),
        );
        $form['delete_confirm'] = array(
          '#type' => 'hidden',
          '#value' => 1,
        );
        return;
      }
    }
  }
        
  $op = false;
  if ($form_values['op'] == t('Back')) {
    // if a user got here by clicking back, then they are trying to get to the
    // main selection screen. 
    $op = false;
  }elseif (isset($form_values['op']) && isset($ops[$form_values['op']])) {
    $op = $form_values['op'];
  }elseif ($ops[$_GET['op']]) {
    $op = $_GET['op'];
    // unset op, so that links that re-use the query string wont use it again.
    unset($_GET['op']);
  }
  
  if ($op) {
    $module = $ops[$op]['module'];
    $delta = $ops[$op]['delta'];

    $form = array_merge($form, asset_wizard_default_fields($form_values));
    $form = array_merge($form, module_invoke($module, 'asset_type', 'form', $delta, $form_values));
    $form['module'] = array('#type'=>'value', '#value'=>$module);
    $form['delta'] = array('#type'=>'value', '#value'=>$delta);
  }
  else {
    // normal asset selection form
    // set directory
    if ($current_asset && asset_check_directory($current_asset->dirname)) {
      $dir = $current_asset->dirname;
    }
    elseif (isset($_GET['dir']) && asset_check_directory($_GET['dir'])) {
      $dir = $_GET['dir'];
    }
    elseif (asset_check_directory($user->name, FILE_CREATE_DIRECTORY, NULL, array('parent' => '', 'title' => $user->name))) {
      $dir = $user->name;
    }
    else {
      $dir = '';
    }

    $form['parent'] = array(
      '#type' => 'hidden',
      '#value' => ($form_values['parent'] ? $form_values['parent'] : $dir),
    );
        
    // copy querystring args for building links 
    $query = $_GET;
    unset($query['q']);
    
    // create directory crumbs and '..' entry
    $crumbs = array();
    if (file_create_path($dir) != file_directory_path()) {
      $parts = explode('/',$dir);
      while($current = array_pop($parts)) {
        $query['dir'] = $parts ? join('/',$parts).'/'.$current : $current;
        $crumbs[] = l($current,$_GET['q'],array(),asset_build_query($query));
      }
      $query['dir'] = substr($dir, 0, strrpos($dir,'/'));
      $list = '<li class="parent">'.l('..',$_GET['q'],array(),asset_build_query($query)).'</li>';
    }
    $query['dir'] = '';
    $crumbs[] = l('assets',$_GET['q'],array(),asset_build_query($query));
  
    // build directory list and filename options array
    $sql = 'SELECT DISTINCT(a.aid),a.filename FROM {asset} a ';
    $args = array();
    
    // for everyone but users with 'administer assets' access control, 
    // only get assets with proper permissions.
    if (user_access('administer assets')) {
      $sql .= 'WHERE a.dirname=\'%s\' ';
      $args = array($dir);
    }
    else {
      // Permissions are not functioning correctly, as per #543 and #489
      $sql .= 'LEFT JOIN {asset_role} r ON a.aid=r.aid '
             . 'WHERE (a.uid = %d '
             . 'OR a.status = 1 '
             . 'OR (r.rid IN (%s) AND r.status = 1) ) '
             . 'AND a.dirname=\'%s\' ';
      $args = array($user->uid, join(array_keys($user->roles),','), $dir);
      //$sql .= 'WHERE a.dirname="%s" ';
      //$args = array($dir);
    }
    $sql.="ORDER BY a.filename";
    $result = db_query($sql, $args);
    $options = array();
    while($asset = db_fetch_object($result)) {
      $asset = asset_load($asset->aid);
      // add to list if directory or add to options if file
      if ($asset->type == 'directory') {
        // if wizard was loaded with an aid, remove it
        if ($query['aid']) {
          unset($query['aid']);
        }
        $query['dir'] = ($asset->dirname ? $asset->dirname.'/' : '') . $asset->filename;
        $list  .= '<li class="folder">'.l($asset->title,$_GET['q'],array(),asset_build_query($query)).'</li>';
      }
      else {
        $options[$asset->aid] = $asset->filename;
      }
    } 
  
    $form['dir_crumb'] = array('#value' => '/ '.join(' / ',array_reverse($crumbs)));

    if ($list) {
      $form['folder_list'] = array('#value' =>  '<ul class="folder-list">'.$list.'</ul>');
    }
    $form['aid'] = array(
      '#type' => 'select',
      '#size' => 10,
      '#options' => $options,
    );
    if ($current_asset->aid) {
      $form['aid']['#default_value'] = $current_asset->aid;
    }
    $form['asset_preview'] = array('#value' => '<div class="asset-preview"></div>');
    $form['#tree'] = false;
    $form['#theme'] = 'asset_wizard_selection_form';
  
    //return $form;
  }  
}

/**
 * This is the base form for asset types.  
 */
function asset_wizard_default_fields($form_values) {
  $parentdir = $form_values['parent'] ? $form_values['parent'] : $_GET['dir'];
  $parentdir = trim(str_replace(file_directory_path(), '', $parentdir), '/'); 
  $parent = asset_load(array('dirname' => dirname($parentdir), 'filename' => basename($parentdir)));
  $form['parent'] = array(
    '#type' => 'hidden',
    '#value' => file_create_path($form_values['parent'] ? $form_values['parent'] : $_GET['dir']),
  );
  $form['parent'] = array(
    '#type' => 'select',
    '#title' => t('Directory'),
    '#default_value' => $form_values['parent'] ? $form_values['parent'] : $_GET['dir'],
    '#options' => asset_wizard_directory_options(),
    '#weight' => -5,
  );
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
  );
  $form['author'] = array(
    '#type' => 'textfield',
    '#title' => t('Author'),
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#rows' => 3,
  );
  $form['permissions'] = array(
    '#theme' => 'asset_upload_permissions',
    '#tree' => false,
  );
  $default_status = 1;
  if (isset($form_values['aid']) && is_numeric($form_values['aid'])) {
    $default_status = db_result(db_query("SELECT status FROM {asset} WHERE aid=%d LIMIT 1", $form_values['aid']));
  }
  $form['permissions']['status'] = array(
    '#type' => 'radios',
    '#title' => t('Permissions'),
    '#description' => t('Select the permissions for this asset.  Selecting Public will allow anyone with asset permissions to use this asset.  By selecting Private, you can restrict which roles, if any, can access this asset.  If you select Private and do not select any roles, you will be the only user with access to this asset.'),
    '#required' => true,
    '#default_value' => $default_status,
    '#options' => array(
      ASSET_PUBLIC => t('Public'),
      ASSET_PRIVATE => t('Private'),
    ),
  );
  $form['permissions']['roles'] = array(
    '#type' => 'checkboxes',
    '#options' => user_roles(0, 'create assets'),
    '#default_value' => $parent->roles,
  );
  
  $form['preview'] = array(
    '#type'=>'hidden', 
    '#value' => $preview,
  );
  
  return $form;
    
}

/**
 * Validate callback for asset_wizard_form().
 */
function asset_wizard_form_validate($form_id, $form_values) {
  // If the user presses 'cancel' or 'back', we should do no further
  // validation. Also, if they press 'cancel' we should actually
  // goto the finish page, because the 'submit' stage won't be called
  // if the user hasn't filled in one of the mandatory fields. In fact,
  // in that case, Drupal's built in form validation will have set
  // errors that we don't need to show the user.
  if ($form_values['op'] == t(ASSET_WIZARD_CANCEL)) {
    // Clear errors from Drupal's built in validation.
    drupal_get_messages('error');
    // Now go to the 'finish page'
    drupal_goto(ASSET_WIZARD_FINISH_REDIRECT);
    return;
  } elseif ($form_values['op'] == t(ASSET_WIZARD_PREVIOUS)) {
    // Clear messages, and do no further validation
    drupal_get_messages('error');
    return;
  }
  elseif ($form_values['op'] == t(ASSET_WIZARD_DELETE)) {
    if (!$form_values['aid']) {
      form_set_error('aid', 'You need to select an asset to delete.');  
    }
    elseif ($form_values['deleteme']==1) {
      $asset = asset_load(array('aid' => $form_values['aid']));
      @unlink($asset->filepath);
      db_query('DELETE FROM {asset} WHERE aid=%d LIMIT 1', $form_values['aid']);
      db_query('DELETE FROM {asset_node} WHERE aid=%d', $form_values['aid']);
      drupal_set_message(t('The selected asset has been deleted.'));
    }
  }
  elseif ($form_values['op'] == t('Edit')) {
    if (!$form_values['aid']) {
      form_set_error('aid', 'You need to select an asset to edit.');  
    }
  }
  elseif ($form_values['op'] == t('Select') && isset($_SESSION['asset_fieldname'])) {
    $delta = split("-", $_SESSION['asset_fieldname']);
    $delta_count = substr_count($_SESSION['asset_fieldname'], "-");
    $delta_count--;
    $delta = $delta[$delta_count];
    $fieldname = str_replace(array("edit-field-", "-$delta-", "-"), array("", "", "_"), $_SESSION['asset_fieldname']);
    if (!$form_values['aid']) {
      form_set_error('aid', 'You need to select an asset before inserting it.');
      return;
    }
    // An asset is selected straight from step 1 to insert directly into CCK
    // Create the preview code for the CCK field
    $is_image = array('jpg', 'jpeg', 'png', 'gif', 'tif', 'tiff', 'bmp');
    $a = asset_load(array('aid' => $form_values['aid']));
    // Preview the image
    if (in_array($a->extension, $is_image)) {
      if (variable_get('asset_imagecache', 0)) {
        $presets = imagecache_presets();
        $preset = $presets[variable_get('asset_imagecache', 0)]['presetname'];
        $preview = theme('imagecache', $preset, str_replace("//","/", $a->filepath));
      }
      else {
        $asset = array(
          'aid' => $form_values['aid'],
          'format' => 'image',
          'formatter' => 'asset',
          'width' => '120',
          'height' => '80',
        );
        $preview = asset_render_macro($asset);
      }
    }
    else {
      // $preview = asset_preview($form_values['aid']); // This doens't work: inserting javascript code with javascript
      $preview = "Media placeholder";
    }
    $fieldname = "field_" . $fieldname;
    print 
    '<script>
    window.opener.document.getElementById("' . $_SESSION['asset_fieldname'] . 'value").value = "' . $a->filename . '";
    window.opener.document.getElementById("' . $_SESSION['asset_fieldname'] . 'aid").value = "' . $form_values['aid'] . '";
    window.opener.document.getElementById("preview_' . $fieldname . '_' . $delta . '").innerHTML = "' . str_replace("\"", "'", $preview) . '";
    window.opener.document.getElementById("' . $_SESSION['asset_fieldname'] . 'caption").value = "' . $a->title . '";
    window.opener.document.getElementById("' . $_SESSION['asset_fieldname'] . 'copyright").value = "' . $a->author . '";
    window.close();
    </script>';
    // Let's just execute this
    exit;
  }

  // Do whatever validation here. It's probably a good idea to do a
  // switch/case on the wizard step. It may be a good idea to validate
  // everything on each call, as that will catch anyone hacking the
  // form with directly injected form posts, although at slightly
  // more processing.
  switch($form_values['op']) {
    case t('Next'):
      if ($form_values['delta']=="directory") {
        if (!$form_values['title']) {
          form_set_error('folder', t('You must specify a folder name.'));
          break;
        }
        if (preg_match('/[^A-Za-z0-9-_.]/',$form_values['title'])) {
          form_set_error('title', t('The folder name may only contain alpha-numeric characters and dashes(-), underscores(_) and periods(.).'));
          break;
        }
        $dir = file_create_path($form_values['parent'].'/'.$form_values['folder']);
        if (!asset_check_directory($dir, FILE_CREATE_DIRECTORY, 'folder', $form_values)) {
          form_set_error('folder', t('Error creating %dir.',array('%dir'=>$dir)));
        }
      }
      if (isset($form_values['delete_confirm'])) {
        // First we delete all assets from this directory
        db_query("DELETE FROM {asset} WHERE type='local' AND dirname='%s'", $form_values['dir']);
        // Then we delete the directory itself
        $path = pathinfo($form_values['dir']);
        db_query("DELETE FROM {asset} WHERE type='directory' AND filename='%s' LIMIT 1", $path['basename']);
        asset_rmdir(file_directory_path() . '/' . $form_values['dir']);
        // We're all done
        unset($_SESSION['messages']);
        drupal_set_message("The directory \"" . $path['basename'] . "\" and its content have been removed");
        drupal_goto(filter_xss($_GET['q']), 'dir=');
      }
      if (isset($form_values['edit-asset'])) {
        db_query('DELETE FROM {asset_role} WHERE aid=%d', $form_values['aid']);
        foreach($form_values['roles'] as $role => $access) {
          db_query('INSERT INTO {asset_role} (aid, rid, status) VALUES (%d, %d, %d)', array($form_values['aid'], $role, $access)); 
        }
        db_query("UPDATE {asset} SET filename='%s', title='%s', author='%s', status=%d WHERE aid=%d LIMIT 1", $form_values['filename'], $form_values['title'], $form_values['author'], $form_values['status'], $form_values['aid']);
        $newdir = 0;
        if ($form_value['old-parent'] != $form_values['parent']) $newdir = 1;
        if ($form_values['old-filename'] != $form_values['filename']) $newdir = 1;
        if ($newdir) {
          empty($form_values['parent']) ? $dir = "" : $dir = $form_values['parent'] . "/";
          empty($form_values['old-parent']) ? $olddir = "" : $olddir = $form_values['old-parent'] . "/";
          if (rename(file_directory_path() . '/' . $olddir . $form_values['old-filename'], file_directory_path() . '/' . $dir . $form_values['filename'])) {
            db_query("UPDATE {asset} SET dirname='%s' WHERE aid=%d LIMIT 1", $form_values['parent'], $form_values['aid']);
            drupal_set_message(t('Your changes have been saved.'));
            drupal_goto($_GET['q'], 'dir='. $form_values['parent'] .'&noreset=1');
            break;
          }
          else {
            drupal_set_message(t('There was an error copying the file to the new directory.'), 'error');
            break;
          }
        }
        drupal_set_message(t('Your changes have been saved.'));
        break;
      }
      if ($form_values['step']==1 && isset($form_values['module'])) {
        // allow validate to return an aid
        $retval = module_invoke($form_values['module'], 'asset_type', 'validate', $form_values['delta'], $form_values);
        if (is_numeric($retval)) {
          $_POST['aid'] = $retval;
          $form_values['aid'] = $retval;
        }        
      }
      if ($form_values['step']==1 && !isset($_GET['op']) && !$form_values['aid']) {
        form_set_error('aid',t('Please select a file.'));
      }
      if ($form_values['step']==2 && !$form_values['formatter']) {
        form_set_error('formatter',t('Please select a formatter.'));
      }
      break;
  }
}

/**
 * Submit callback for asset_wizard_form().
 */
function asset_wizard_form_submit($form_id, $form_values) {
  // If the user presses 'back' or 'cancel' don't do any submission work...
  if ($form_values['op'] == t(ASSET_WIZARD_PREVIOUS) || $form_values['op'] == t(ASSET_WIZARD_CANCEL)) {
    return FALSE;
  }
  if ($form_values['op'] == t('New Folder')) {
    $dir = file_create_path($form_values['parent'].'/'.$form_values['folder']);
    if (!asset_check_directory($dir, FILE_CREATE_DIRECTORY, 'folder')) {
      drupal_set_message(t('Error creating %dir.',array('%dir'=>$dir)));
    }
    return FALSE;
  }

  if ($form_values['step'] == 1 && isset($form_values['module'])) {
    // allow module submits to return an aid
    $retval = module_invoke($form_values['module'], 'asset_type', 'submit', $form_values['delta'], $form_values);
    if (is_numeric($retval)) {
      $_POST['aid'] = $retval;
    }
    return FALSE;
  }
  // If we haven't processed the form and completed fully, we have
  // to return FALSE so that Drupal redisplays our form.
  return FALSE;
}

function asset_wizard_directory_options($dir = '') {
  global $user;
  // build directory list and filename options array
  $sql = 'SELECT DISTINCT(a.aid),a.dirname FROM {asset} a ';
  $args = array();
  // for everyone but users with 'administer assets' access control, 
  // only get assets with proper permissions.
  if (user_access('administer assets')) {
    $sql .= 'WHERE a.dirname LIKE \'%s%\' ';
    $args = array($dir);
  }
  else {
    $sql .= 'LEFT JOIN {asset_role} r ON a.aid=r.aid '
           . 'WHERE (a.uid = %d '
           . 'OR a.status = 1 '
           . 'OR (r.rid IN (%s) AND r.status = 1) ) '
           . 'AND a.dirname LIKE \'%s%\' ';
    $args = array($user->uid, join(array_keys($user->roles),','), $dir);
    // $sql .= 'WHERE a.dirname="%s" ';
    // $args = array($dir);
  }
  $sql .= ' AND a.type = \'directory\' ORDER BY a.dirname ASC';
  $result = db_query($sql, $args);
  $options = array();
  while($asset = db_fetch_object($result)) {
    $asset = asset_load($asset->aid);
    $dir = $asset->dirname . ($asset->dirname ? '/' : '') . $asset->filename;
    $parts = explode('/',$dir);
    if (count($parts)) {
      $name = str_repeat('--', count($parts)) . ' ' . $asset->title;
    }
    else {
      $name = $asset->title;
    }
    $options[$dir] = $name;
  }
  if (user_access('administer assets')) $options[''] = 'assets';
  ksort($options);
  return $options; 
}

function asset_wizard_location_bar($dir = '') {
  global $user;
  $defaults = array_unique(array($dir, 'PUBLIC', $user->name));
  $qstr = $_GET;
  unset($qstr['q']);

  foreach($defaults as $path) {
    $info = asset_pathinfo($path);
    $asset = asset_load(array('dirname' => $info['dirname'], 'filename' => $info['filename']));
    $qstr['dir'] = $asset->filepath;
    $links[] =  l($asset->title,$_GET['q'],array(),asset_build_query($qstr));
  }
  
  return theme('item_list', $links);
}

/**
 * Build a list of available asset_types that can be used.
 */
function asset_wizard_toolbar($dir = '') {
  $qarray = $_GET;
  $qarray['dir'] = $dir;
  
  $buttons = array();

  // Create Choose button
  $choose = t('Choose');
  $html = theme('image', drupal_get_path('module','asset').'/icon.png', $choose, $choose, NULL, FALSE) . '<br/>' . $choose;
  $qstring = drupal_query_string_encode($qarray, array('q','aid','op'));
  $buttons[] = l($html, 'asset/wizard/'.arg(2), array(), $qstring, null, false, true);
  
  // Create other buttons
  foreach(module_implements('asset_type') as $module) {
    $types = module_invoke($module, 'asset_type', 'info');
    foreach($types as $delta=>$type) {
      if (user_access('create ' . $delta . ' assets') || user_access('administer assets')) {
        if ($type['src']) {
          $html = theme('image', $type['src'], $type['value'], $type['value'], NULL, FALSE) . '<br/>' . $type['value'];
        }else {
          $html = $type['value'];
        }
        $qarray['op'] = $type['value'];
        $qstring = drupal_query_string_encode($qarray, array('q','aid'));
        $buttons[] = l($html, 'asset/wizard/'.arg(2), array(), $qstring, null, false, true);
      }
    }
    
    // add class to last button from each module
    $i = count($buttons) - 1;
    if (!is_array($buttons[$i])) {
      $buttons[$i] = array('data' => $buttons[$i], 'class' => 'last');
    }
  }
  
  return theme('item_list', $buttons);  
}

/**
 * Script tag to add to content to finish the wizard.
 */
function asset_wizard_finish() {
  return '<script type="text/javascript">$(insertToEditor);</script>';
}

/**
 * Theming Functions ==========================================================
 */
 
/**
 * Much trimmed down version of theme_page();
 */
function theme_asset_popup($content) {
  $title = drupal_get_title();
  // build $styles to not include theme styles
  $styles = '';
  $css = drupal_add_css();
  foreach($css as $media => $types) {
    foreach($types as $type => $files) {
      if ($type != 'theme') {
        foreach($types[$type] as $file => $preprocess) {
          $styles .= '<style type="text/css" media="'. $media .'">@import "'. base_path() . $file .'";</style>' ."\n";
        }
      }
    }
  }
  $scripts = drupal_get_js();
  $head = drupal_get_html_head();
  $messages = theme('status_messages');
  return <<<POPUP
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language ?>" xml:lang="<?php print $language ?>">
<head>
  <title>$title</title>
  $head
  $styles
  $scripts
</head>
<body id="asset-popup">
  <h1>$title</h1>
  $messages
  <div class="content">$content</div>
</body>
</html>
POPUP;

}
  
/**
 * Main theme function for asset_wizard_form.
 */
function theme_asset_wizard_form($form, $main_class = NULL) {
  $footer = '<div id="asset-popup-footer" class="clear-block">'.drupal_render($form['footer']).'</div>';
  $help = ($form['help'] ? '<div class="help">'.drupal_render($form['help']).'</div>' : '');
  $toolbar = '<div class="toolbar">' . asset_wizard_toolbar($form['parent']['#value']) .'</div>';
  //$location = '<div class="location">' . asset_wizard_location_bar($form['parent']['#value']) .'</div>';
  $messages = theme('status_messages');
  $main = '<div id="asset-popup-main" class="'.$main_class.'">'.$messages.$help.drupal_render($form).'</div>';
  return $toolbar.$main.$footer;
}

/**
 * Theme function for the selection step of the wizard.
 */
function theme_asset_wizard_selection_form($form) {
//  $output .= '<div class="toolbar">' . drupal_render($form['toolbar']) . '</div>';
  $output .= '<div class="crumb-left"><div class="crumb-right">'
          . drupal_render($form['dir_crumb']) . '</div></div>';
          
  $output .= '<div class="asset-tree-widget clear-block">'
          . drupal_render($form['folder_list'])
          . drupal_render($form['aid'])
          . drupal_render($form['asset_preview'])
          . '</div>';
          
  $form[] = array('#value' => $output, '#weight' => -10);
  return theme('asset_wizard_form', $form, 'asset-selection-form');          
}

/**
 * Simple <input type="image"> function
 */
function theme_asset_image_button($element) {
  // Make sure not to overwrite classes.
  $element['#button_type'] = 'image';
  if (isset($element['#attributes']['class'])) {
    $element['#attributes']['class'] = 'form-'. $element['#button_type'] .' '. $element['#attributes']['class'];
  }
  else {
    $element['#attributes']['class'] = 'form-'. $element['#button_type'];
  }

  return '<input type="image" '. (empty($element['#name']) ? '' : 'name="'. $element['#name'] .'" ')  .'id="'. $element['#id'].'" value="'. check_plain($element['#value']) .'" '. drupal_attributes($element['#attributes']) ." />\n";
}
 
/**
 * Theme function for the permissions field.
 */
function theme_asset_upload_permissions($element) {
  $roles = db_query("SELECT rid, status FROM {asset_role} WHERE aid=%d", $element['status']['#post']['aid']);
  while($role = db_fetch_object($roles)) {
  	if ($role->status) {
      $element['roles'][$role->rid]['#value'] = $role->rid;
  	}
  }
  $roles = drupal_render($element['roles']);
  $element['status'][ASSET_PRIVATE]['#suffix'] = '<div class="roles">' . $roles . '</div>';
  $element['status'][ASSET_PRIVATE]['#id'] = 'permissions-private';
  $element['status'][ASSET_PUBLIC]['#id'] = 'permissions-public';
  return drupal_render($element);
}

function asset_rmdir($folder) {
  if (is_dir($folder)) {
    foreach (scandir ( $folder )  as $value ) {
      if ($value != "." && $value != "..") {
        $value = $folder . "/" . $value;
        if (is_dir($value)) {
          asset_rmdir($value);
        }
        elseif (is_file($value)) {
          @unlink ( $value );
         }
       } 
     } 
     return rmdir ( $folder );  
   } 
   else {
     return FALSE; 
   }
}
