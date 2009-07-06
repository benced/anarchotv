// $Id: editor_plugin.js,v 1.1.2.3 2007/12/19 18:50:23 wmostrey Exp $
/* Import plugin specific language pack */
tinyMCE.importPluginLanguagePack('drupalasset', 'en');

var TinyMCE_DrupalAssetPlugin = {
	getInfo : function() {
		return {
			longname : 'DrupalAsset',
			author : 'Roger Lopez, adapted from code by Benjamin Shell',
			authorurl : 'http://www.digett.com',
			infourl : 'http://drupal.org/project/asset',
			version : tinyMCE.majorVersion + "." + tinyMCE.minorVersion
		};
	},

	initInstance : function(inst) {
		if (!tinyMCE.settings['drupalasset_skip_plugin_css'])
			tinyMCE.importCSS(inst.getDoc(), tinyMCE.baseURL + "/plugins/drupalasset/drupalasset.css");
	},

	getControlHTML : function(cn) {
		switch (cn) {
			case "drupalasset":
				return tinyMCE.getButtonHTML(cn, 'lang_drupalasset_desc', '{$pluginurl}/images/doc-option-add.png', 'mceDrupalAsset');
		}

		return "";
	},

	execCommand : function(editor_id, element, command, user_interface, value) {
		// Handle commands
		switch (command) {
			case "mceDrupalAsset":
				var name = "";
				var nid = "", alt = "", captionTitle = "", captionDesc = "", link = "", align = "", width = "", height = "";
				var action = "insert";
				var template = new Array();
				var inst = tinyMCE.getInstanceById(editor_id);
				var focusElm = inst.getFocusElement();

				// get base url
				var base_url = tinyMCE.baseURL;
				base_url = base_url.substring(0, base_url.indexOf('modules'));

				template['file'] = base_url + 'index.php?q=asset/wizard/tinymce';
				template['width'] = 600;
				template['height'] = 400;
				template['html'] = false;

				// Is selection a asset
				if (focusElm != null && focusElm.nodeName.toLowerCase() == "img") {
					name = tinyMCE.getAttrib(focusElm, 'class');

					if (name.indexOf('mceItemDrupalAsset') == -1) // Not a DrupalImage
						return true;

                                        // Get the rest of the DrupalImage attributes
                                        // align = tinyMCE.getAttrib(focusElm, 'align');
                                        align = tinyMCE.getAttrib(focusElm, 'class'); // class="mceItemDrupalAsset asset-right"
                                        align = align.split(" ");
                                        align = align[1].split("-");
                                        align = align[1];
                                        format = tinyMCE.getAttrib(focusElm, 'src'); // http://yoursite.tld/asset/img/1468/asset/image. asset = formatter, image = format
                                        format = format.split("asset/img/");
                                        format = format[1].split("/");
                                        format = format [1] + ":" + format[2]; // we now have asset:image, just perfect
                                        width = tinyMCE.getAttrib(focusElm, 'width');
                                        height = tinyMCE.getAttrib(focusElm, 'height');
                                        alt = decodeURIComponent(tinyMCE.getAttrib(focusElm, 'title')); // using 'title' because this doesn't seem to work with 'alt'
                                        var miscAttribs = TinyMCE_DrupalAssetPlugin._parsePipeAttributes(alt);  // parse the deliminated attributes in the alt tag
                                        aid = miscAttribs['aid'];

          template['file'] += '&aid=' + miscAttribs['aid'] + '&align=' + align + '&width=' + width + '&height=' + height + '&format=' + format;
                                        action = "update";
				}

				//WARNING: "resizable : 'yes'" below is painfully important otherwise
				// tinymce will try to open a new window in IE using showModalDialog().
				// And for some reason showModalDialog() doesn't respect the target="_top"
				// attribute.
				tinyMCE.openWindow(template, {editor_id : editor_id, nid : nid, captionTitle : captionTitle, captionDesc : captionDesc, width : width, height : height, action : action, scrollbars: 'yes', resizable: 'yes' });
				//tinyMCE.openWindow(template, {editor_id : editor_id, nid : nid, captionTitle : captionTitle, captionDesc : captionDesc, link : link, align : align, width : width, height : height, action : action, resizable : 'yes', scrollbars : 'yes'});
			return true;
	   }

	   // Pass to next handler in chain
	   return false;
	},

	cleanup : function(type, content) {
		switch (type) {
			case "insert_to_editor_dom":
				break;
			case "get_from_editor_dom":
				break;
			case "insert_to_editor": // called when TinyMCE loads existing data or when updating code using Edit HTML Source plugin
				// Parse all drupalasset filter tags and replace them with asset placeholders
				var startPos = 0;
				var index = 0;
				while ((startPos = content.indexOf('[asset|', startPos)) != -1) {
					// Find end of object
					var endPos = content.indexOf(']', startPos);
					var attribs = TinyMCE_DrupalAssetPlugin._parsePipeAttributes(content.substring(startPos + 7, endPos));
          attribs['height'] = attribs['height'] || 100;
          attribs['width'] = attribs['width'] || 100;
          if(attribs['align']){
            alignClass = 'asset-' + attribs['align'];
          }else{
            alignClass = '';
          }
					endPos++;

					miscAttribs = encodeURIComponent(TinyMCE_DrupalAssetPlugin._buildPipeAttributes(attribs)); // escape(miscAttribs);

					// Insert asset
					var contentAfter = content.substring(endPos);
					content = content.substring(0, startPos);
          if(attribs['format'] == 'link'){
            content += '<a href="#" title="' + miscAttribs + '" class="mceItemDrupalAsset">';
            content += attribs['title'] + '</a>';
          //}else if(attribs['resizable'] == 'true'){
          }else{
            // Reference: these are the default parameters that are valid for the TinyMCE asset tags:
            // img[class|src|border=0|alt|title|hspace|vspace|width|height|align]
            //content += '<img src="' + (tinyMCE.getParam("theme_href") + '/images/spacer.gif') + '"';
            content += '<img src="'+TinyMCE_DrupalAssetPlugin._previewSrc(attribs)+'"';
            content += ' style="width:' + attribs["width"] + 'px; height:' + attribs["height"] + 'px;"';
            content += ' width="' + attribs["width"] + '" height="' + attribs["height"] + '"';
            content += ' alt="' + miscAttribs + '" title="' + miscAttribs + '" name="mceItemDrupalAsset" class="mceItemDrupalAsset ' + alignClass + '"';
            if(attribs['resizable'] != 'true'){
              content += ' readonly="readonly"';
            }
            content += ' />';
//          }else{
//            content += '<div style="background-image:url('+TinyMCE_DrupalAssetPlugin._previewSrc(attribs)+');';
//            content += ' width:' + attribs["width"] + 'px; height:' + attribs["height"] + 'px;"';
//            content += ' alt="' + miscAttribs + '" title="' + miscAttribs + '" name="mceItemDrupalAsset" class="mceItemDrupalAsset ' + alignClass + '"></div>';
          }
					content += contentAfter;
					index++;

					startPos++;
				}
				break;

			case "get_from_editor": // called when TinyMCE exits or when the Edit HTML Source plugin is clicked
				// Parse all image placeholders and replace them with drupalasset filter tags
				var startPos = -1;
        while ((startPos = content.indexOf('<img', startPos+1)) != -1) {
          var endPos = content.indexOf('/>', startPos);
          var attribs = TinyMCE_DrupalAssetPlugin._parseHTMLAttributes(content.substring(startPos + 4, endPos));
          endPos += 2;

          if (attribs['name'] != "mceItemDrupalAsset") {
            continue;
          }

          var miscAttribs = decodeURIComponent(attribs["alt"]);
          var arrayMiscAttribs = TinyMCE_DrupalAssetPlugin._parsePipeAttributes(miscAttribs);
          arrayMiscAttribs['width'] = attribs['width'];
          arrayMiscAttribs['height'] = attribs['height'];

          var contentBefore = content.substring(0, startPos);
          var contentAfter = content.substring(endPos);
          var drupalHTML = '';
          drupalHTML += '[asset|' + TinyMCE_DrupalAssetPlugin._buildPipeAttributes(arrayMiscAttribs) + ']';
          content = contentBefore + drupalHTML + contentAfter;
        }

        while ((startPos = content.indexOf('<a', startPos+1)) != -1) {
          var endPos = content.indexOf('</a>', startPos);
          var attribs = TinyMCE_DrupalAssetPlugin._parseHTMLAttributes(content.substring(startPos + 2, endPos));
          endPos += 4;

          if (!attribs['class'] || attribs['class'].indexOf("mceItemDrupalAsset") == -1) {
            continue;
          }

          var miscAttribs = decodeURIComponent(attribs["title"]);
          if(!miscAttribs){
            continue;
          }
          var arrayMiscAttribs = TinyMCE_DrupalAssetPlugin._parsePipeAttributes(miscAttribs);
          arrayMiscAttribs['width'] = attribs['width'];
          arrayMiscAttribs['height'] = attribs['height'];

          var contentBefore = content.substring(0, startPos);
          var contentAfter = content.substring(endPos);
          var drupalHTML = '';
          drupalHTML += '[asset|' + TinyMCE_DrupalAssetPlugin._buildPipeAttributes(arrayMiscAttribs) + ']';
          content = contentBefore + drupalHTML + contentAfter;
        }

				break;
		}

		// Pass through to next handler in chain
		return content;
	},

	handleNodeChange : function(editor_id, node, undo_index, undo_levels, visual_aid, any_selection) {
		if (node == null)
			return;

    var id = editor_id + "_drupalasset";

		do {
			// This code looks at the name of the image to see if the drupalasset button should be selected.
			// However, by default 'name' is not accepted by TinyMCE as a parameter for the img tag, so it must
			// be added using the initialization string.  As far as THIS code goes, it could look at 'className'
			// instead, therefore avoiding this requirement, however the regular image button looks at the
			// 'name' value to see if it starts with 'mce_'.  If it does, it considers it an internal image and
			// does not highlight the regular image button.  If 'className' is used here instead, BOTH buttons
			// highlight when a drupalimage is selected.
			if (node.nodeName == "IMG" && tinyMCE.getAttrib(node, 'class').indexOf('mceItemDrupalAsset') == 0) {
				tinyMCE.switchClass(editor_id + '_drupalasset', 'mceButtonSelected');
        $("#" + id + " img").src($("#"+id+" img").src().replace('-add.png','-edit.png'));
        //console.log('handleNodeChange: '+node+','+tinyMCE.getAttrib(node, 'width')+'x'+tinyMCE.getAttrib(node,'height'));
        if(tinyMCE.getAttrib(node, 'readonly') == 'readonly'){
          var attribs = TinyMCE_DrupalAssetPlugin._parsePipeAttributes(decodeURIComponent(node.title));
          node.height = attribs['height'];
          node.width = attribs['width'];
        }
				return true;
			}else if(node.nodeName == "A" && tinyMCE.getAttrib(node, 'class').indexOf('mceItemDrupalAsset') == 0){
        tinyMCE.switchClass(editor_id + '_drupalasset', 'mceButtonSelected');
        $("#" + id + " img").src($("#"+id+" img").src().replace('-add.png','-edit.png'));
      }
		} while ((node = node.parentNode));

		tinyMCE.switchClass(editor_id + '_drupalasset', 'mceButtonNormal');
    $("#" + id + " img").src($("#"+id+" img").src().replace('-edit.png','-add.png'));

		return true;
	},

	// pipes | must be escaped with a backslash like this: \|
	// note: values also cannot contain ] because the functions that call
	// this function use the ] symbol to find the end of the drupalasset filter code
	_parsePipeAttributes : function(attribute_string) {
		var attributes = new Array();
		var keyvalue_arr = new Array();
		// if it weren't for the escaping, the regExp string would look like this:
		// var regExp = new RegExp("([a-zA-Z]*)=([^\|]*)", "g"); // doesn't work without global (g)
		var regExp = new RegExp("([a-zA-Z]*)=([^|](?:\\.|[^\\|]*)*)*", "g"); // doesn't work without global (g)
		var matches = attribute_string.match(regExp);
		for (var i = 0; i < matches.length; i++ ) {
			keyvalue_arr = matches[i].split('=');
			attributes[keyvalue_arr[0]] = keyvalue_arr[1];
		}
		return attributes;
	},

  _buildPipeAttributes : function (attributes) {
    var strAttr = '';
    for ( key in attributes){
      strAttr += '|' + key + '=' + attributes[key];
    }
    return strAttr.substr(1);
  },


	/*
	 * Parses HTML attributes into a key=>value array.  Take a look at the example strings and see how
	 * standard HTML entities within any value, such as the title and desc (which are combined in the
	 * alt tag as a piped string) need to be converted to: &quot; &amp; &lt; &gt;
	 * simple example string:
	 *     name="mceItemDrupalImage" width="200" height="150" src="/images/spacer.gif"
	 *     alt="nid=123|title=My Photos|desc=" class="mceItemDrupalImage" align="right"
	 * advanced example string:
	 *     name="mceItemDrupalImage" width="200" height="150" src="/images/spacer.gif"
	 *     alt="nid=123|title=&quot;To be or not to be&quot;|desc=That is the question." class="mceItemDrupalImage" align="right"
	 * Any pipes | or closing brackets would also be a problem, not for this parsing function, but
	 * when parsing the pipe deliminated string.  These characters need to be escaped with a backslash.
	 * Unlike the quotes, this cannot be accomplished automatically within this TinyMCE plugin.  Any
	 * user or Drupal module that inserts drupalasset filter strings in a post, whether using TinyMCE
	 * or not, must backslash any pipes or closing brackets.
	*/
	_parseHTMLAttributes : function(attribute_string) {
		var attributes = new Array();
		var innerMatches = new Array();
		var regExp = '([a-zA-Z0-9]+)[\s]*=[\s]*"([^"](?:\\.|[^\\"]*)*)"';

		var outerRegExp = new RegExp(regExp, "g"); // doesn't work without global (g)
		var outerMatches = attribute_string.match(outerRegExp);
		var innerRegExp = new RegExp(regExp); // doesn't work with global (g)
		for (var i = 0; i < outerMatches.length; i++ ) {
			innerMatches = innerRegExp.exec(outerMatches[i]);
			attributes[innerMatches[1]] = innerMatches[2];
		}
		return attributes;
	},

  _previewSrc : function(attribs) {
    var src = 'asset/img/' + attribs['aid'];
    if(attribs['formatter']){
      src += '/' + attribs['formatter'];
      if(attribs['format']){
        src += '/' + attribs['format'];
      }
    }
    return src;
  }
};

tinyMCE.addPlugin("drupalasset", TinyMCE_DrupalAssetPlugin);
