var align;
var width;
var height;

function preinit() {
  tinyMCE.setWindowArg('mce_windowresize', false);
  tinyMCE.setWindowArg('mce_replacevariables', false);
}

function initLoader() {
}

function initProperties() {
}

function insertToEditor() {
  var content = $("#edit-macro").val();
  if(!content){
    return false;
  }
  content = window.opener.TinyMCE_DrupalAssetPlugin.cleanup('insert_to_editor',content);
  // Insert the image
  tinyMCE.execCommand("mceInsertContent", true, content);
  tinyMCE.selectedInstance.repaint();
  
  // Close the dialog
  tinyMCEPopup.close();
  return false;
}

function cancelAction() {
  // Close the dialog
  tinyMCEPopup.close();
}

// While loading
preinit();