// $Id: asset_tinymce_helper.js,v 1.1.2.4 2008/08/21 08:12:18 wmostrey Exp $

$(document).ready(
  function(){
    $('a.asset-textarea-link').each(
      function(){
        var textarea_id = this.id.replace('asset-link-', '');
        var mcelink_id = 'wysiwyg4' + textarea_id.replace('edit-', '');
        var mceLink = $('#' + mcelink_id);
        var assetLink = this;
        
        mceLink.click(
          function(){
            // this seems kinda backward but since this fires before mceToggle
            // when there is no editor, then that means that the editor will
            // be created on this click, and so the link needs to be hidden.
            if(tinyMCE.getEditorId(textarea_id) == null){
              // editor will be created so hide asset link
              $(assetLink).hide();
            }else{
              // editor will be removed so show asset link
              $(assetLink).show();
            }
          }
        ).click();
      }
    );  // each
  }
);
