$(document).ready(function(){

  $('.messages').each(function(){
    $('<a class="okay">Okay</a>')
      .prependTo(this)
      .click(function(){
        $(this).parent().slideUp('slow');
        return false;
      });
  });
  
  $("#edit-cancel").click(cancelAction);
  //$("#edit-finish").click(insertToEditor);
  $("#edit-aid").change(function(){
    $(".asset-preview").load(Drupal.settings.asset.basePath + 'index.php?q=asset/js/preview/'+$(this).val());
  });
  
  // replace buttons with links for better styling
  $("#1asset-popup-footer input[@type=submit]").each(function(){
    var button = this;
    var html = '<a class="button-replacement" id="button-' + button.id + '" href="#"><span>' + button.value + '</span></a>';
    $(html).prependTo($("#asset-popup-footer")).click(function(){
      $("#asset-wizard-form").append('<input type="hidden" name="op" value="' + button.value + '" />');
      $("#asset-wizard-form")[0].submit();
      return false;
    });
    $(button).hide();
  });
  
  if($("#edit-folder").length){
    $("#edit-folder").val($("#edit-folder")[0].alt).focus(function(){
      $(this).val('');
      $(this).unbind('focus');
    });
  }
  
  $("#button-edit-cancel").click(cancelAction);
  $("#button-edit-cancel span").html($("#edit-cancel")[0].alt);
  
  $(window).bind('resize', function(){
    var h = window.innerHeight ||
        $.boxModel && document.documentElement.clientHeight || 
        document.body.clientHeight;
    
    h = h - parseInt($('.toolbar:eq(0)').height()) - parseInt($('#asset-popup-footer').height());
    //$('#asset-popup-main').height(h);
  }).trigger('resize');
  
  $('input[@name=status]').change(updatePermissions);
  updatePermissions();
  locationBar();
  
  initLoader();
});

function locationBar(){
  if($('.location ul').size() < 1){
    return false;
  }
 
  $('<span>'+$('.location li:eq(0) a').html()+'</span>').prependTo($('.location'));
  $('.location').click(
    function(){
      $('ul',this).toggle();
    }
  );
}

function updatePermissions(){
  if($('#permissions-private').size() < 1){
    return false;
  }
  
  if($('#permissions-private')[0].checked){
    $('.roles input').removeAttr('disabled');
  }else{
    $('.roles input').attr('disabled','disabled');
  }
}