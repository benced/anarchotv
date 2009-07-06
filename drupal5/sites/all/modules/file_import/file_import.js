// $Id: file_import.js,v 1.1 2008/02/15 01:02:27 neochief Exp $

String.prototype.capitalize = function(){
    return this.replace(/\w+/g, function(a){
        return a.charAt(0).toUpperCase() + a.slice(1).toLowerCase();
    });
};

String.prototype.capitalizeFirst = function(){
    return this.replace(/^./g, function(a){
        return a.charAt(0).toUpperCase() + a.slice(1).toLowerCase();
    });
};

function file_import_form_change()
{
	$('table').append('<tr><td></td><td></td><td id="c0"></td><td id="c1"></td><td id="c2"></td></tr>');

	$('#edit_titles legend').remove();
	$('#c0').append($('#edit_titles').children());
	$('#edit_titles').remove();

	$('#edit_titles legend').remove();
	$('#c1').append($('#replace_titles').children());
	$('#replace_titles').remove();

	$('#target_node legend').remove();
	$('#c2').append($('#target_node').children());
	$('#target_node').remove();


	$('#node_select_button').click(function(){
      $(".form-checkbox").each(function (i) {
      	  if ($(this).attr('checked'))
      	  {
      	  	  $('#edit-node-'+$(this).attr('value')).attr('value',$('#node_select_combo').attr('value'));
      	  }
      });
		return false;
	});

	$('#clear_titles').click(function(){
      $(".form-checkbox").each(function (i) {
      	  if ($(this).attr('checked'))
      	  {
      	  	  $('#edit-title-'+$(this).attr('value')).attr('value','');
      	  }
      });
		return false;
	});

	$('#cfl_titles').click(function(){
      $(".form-checkbox").each(function (i) {
      	  if ($(this).attr('checked'))
      	  {
      	  	  a = '#edit-title-'+$(this).attr('value');
      	  	  $(a).attr('value',$(a).attr('value').capitalizeFirst());
      	  }
      });
		return false;
	});

	$('#cfla_titles').click(function(){
      $(".form-checkbox").each(function (i) {
      	  if ($(this).attr('checked'))
      	  {
      	  	  a = '#edit-title-'+$(this).attr('value');
      	  	  $(a).attr('value',$(a).attr('value').capitalize());
      	  }
      });
		return false;
	});
	
	$('#replace_titles_button').click(function(){
      $(".form-checkbox").each(function (i) {
      	  if ($(this).attr('checked'))
      	  {
      	  	  value = $('#edit-title-'+$(this).attr('value')).attr('value');
      	  	  from = $('#edit-replace-from').attr('value');
      	  	  to = $('#edit-replace-to').attr('value');
      	  	  var re = new RegExp(from, "g");
      	  	  value = value.replace(re,to);
      	  	  $('#edit-title-'+$(this).attr('value')).attr('value',value);
      	  }
      });
		return false;
	});

}