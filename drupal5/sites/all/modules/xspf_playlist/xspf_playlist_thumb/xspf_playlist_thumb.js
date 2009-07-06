// ;$Id: xspf_playlist_thumb.js,v 1.1.2.3 2008/07/04 21:23:47 nedjo Exp $

// show and hide the configuration options
$(document).ready( function() {
  // hide all the thumbnail configurations on load
  $('.xspf_playlist_config_thumb').hide();
  
  // show the selected one
  var show = $('#edit-xspf-upload-content-thumb').val();
  $('#'+show+'_config').show();
  $('#'+show+'_config').addClass('active');
  
  // now bind clicks to selects
  $('#edit-xspf-upload-content-thumb').click(function () {
    // hide active items
    $('.xspf_playlist_config_thumb.active').each(function () {
      $(this).removeClass('active');
      $(this).hide('slow');
    });
    
    // show selected
    show = $(this).val();
    $('#'+show+'_config').show('slow');
     $('#'+show+'_config').addClass('active');             
   });
  
  
});


// This is used for the thumb picker
if (Drupal.jsEnabled) {
  $(document).ready(function () {
    // activate the correct thumbs by checking the value of the drop down
    $('.xspf_thumb_select').each( function () {
      // find the id we need
      get_xid = new RegExp(/edit-xspf-playlist-thumb-([0-9]*)/);
		  var the_xid = $(this).attr('id').match(get_xid);
		  // update the thumb 
		  $('#xspf_thumb_'+the_xid[1]+'_'+$(this).val()).addClass('picked');
		  
		  // now update the file display:
		  $('#xspf_'+the_xid[1]+' .xspf_playlist-selected').html('Using file: '+$('#'+$(this).attr('id')+' option:selected').html() );
		  
    });
       
    // hide the selector so we don't need to worry about it
    $('.xspf_thumb_select').css('display', 'none');

    //  bind the clicks on the thumbnails 
		$('.xspf_playlist-picker > .xspf_playlist-thumb').click( function () {
		  // remove any current picks 	  
		  $(this).siblings('.xspf_playlist-thumb').removeClass('picked');
		  
		  // get the id of this image and it's xid
		  get_ids = new RegExp(/xspf_thumb_([0-9]*)_([0-9]*)/);
		  var ids = $(this).attr('id').match(get_ids);

		  // assign the id to the drop down
      $('#edit-xspf-playlist-thumb-'+ids[1]).val(ids[2]);
		  
		  // assign the file name 
      $(this).siblings('.xspf_playlist-selected').html('Using file: '+ $(this).children('img').attr('src'));

      // make the image picked
      $(this).addClass('picked');
		});
		
 });
}