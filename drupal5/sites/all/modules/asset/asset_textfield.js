// Declare global variables
var myDoc;
var myForm;
var mytextfield;
var myhiddenfield;

function initLoader() {
  // Save the references to the parent form and textfield to be used later. 
  myDoc      = window.opener.document; // global (so don't use var keyword)
  myForm     = '';
  mytextfield = '';
  myhiddenfield = '';
  
  var args = getArgs(); // get the querystring arguments
  var textfield = args.textfield;
  var aidfield = args.aidfield;

  // Let's calculate the delta to enter the preview
  if(textfield!=undefined) {
    psplit1 = textfield.split("[", 2);
    psplit2 = psplit1[1].split("]", 2);
    fieldtype = psplit1[0];
    delta = psplit2[0];
  }
  
  // Reference the form object for this textfield.
  if (myDoc.getElementsByTagName) {
    var f = myDoc.getElementsByTagName('form');
    for (var i=0; i<f.length; i++) {
      if (f[i][textfield]) {
        myForm = f[i];
        mytextfield = f[i][textfield];
      }
      if (f[i][aidfield]) {
        myhiddenfield = f[i][aidfield];
      }
    }
  }
}

function insertToEditor() {
  var content = $("#edit-macro").val();
  // If content is empty, something went wrong. Aborting.
  if(!content){
    return false;
  }
  var preview = $("#edit-preview").val();
  var author = $("#edit-author").val();
  // We only need the aid
  // Example content string: [asset|aid=7|format=link|formatter=asset|title=test|align=none]
  asset = content.split("|", 5);
  aid = asset[1].split("=", 2);
  content = aid[1];
  subject = asset[4].split("=", 2);
  subject = subject[1];
  
  // Set the textfield's value to the aid
  mytextfield.value = subject;
  myhiddenfield.value = content;
  // Set the image preview
  window.opener.document.getElementById("preview_" + fieldtype + "_" + delta).innerHTML = preview;
  // Set the caption (title) and copyright (author)
  fieldtype = fieldtype.replace("_", "-");
  window.opener.document.getElementById("edit-" + fieldtype + "-" + delta + "-caption").value = subject;
  window.opener.document.getElementById("edit-" + fieldtype + "-" + delta + "-copyright").value = author;
  // Close the dialog
  cancelAction();
  return false;
}

function cancelAction(){
  window.close();
}

/*
 * getArgs() by Jim K - From Orielly JSB pp 244
 *
 * This function parses comma separated name=value 
 * argument pairs from the query string of the URL. 
 * It stores the name=value pairs in 
 * properties of an object and then returns that object
 * 
 * ex usage: 
 * var args = getArgs(); //Get the arguments
 * alert(args.CSSPATH);
*/
function getArgs() {
  var args = new Object();

  var query = location.search.substring(1); // Get Query String
  var pairs = query.split("&"); // Split query at the ampersand
  
  for(var i = 0; i < pairs.length; i++) { // Begin loop through the querystring
    var pos = pairs[i].indexOf('='); // Look for "name=value"
    if (pos == -1) continue; // if not found, skip to next
    
    var argname = pairs[i].substring(0,pos); // Extract the name
    var value = pairs[i].substring(pos+1); // Extract the value
    args[argname] = unescape(value); // Store as a property
  }
  return args; // Return the Object
}
