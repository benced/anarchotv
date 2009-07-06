<?php

  /**
   * @file this defines the forms and variables for the 4.x version of the player
   */

/**
 * defines the options for the player
 * @param array $config
 * @return array
 */
function flvmediaplayer_player_form($config) {

  // define default player to use
  $form['flvmp_player_path'] = array(
    '#title' => t('Profile player'),
    '#type' => 'textfield',
    '#default_value' => $config['player_path'] ? $config['player_path'] : variable_get('flvmediaplayer_path', null),
    '#description' => t('Path to the flash player used with this configuration.'),
  );

  // set this so that we can default the value to on
  if (! isset($config['player_node_body'])) { $config['player_node_body'] = 1; }
  $form['flvmp_player_node_body'] = array(
    '#title' => t('Display player in node body'),
    '#type' => 'checkbox',
    '#description' => t('If you want to have the player automatically displayed on a node. If you are not using custom theming, this needs to be on.'),
    '#default_value' =>  $config['player_node_body'],
  );

  $form['file'] = array(
    '#type' => 'fieldset',
    '#title' => t('File settings'),
    '#description' => t('If you select an external file source, these values will be overridden'),
    '#collapsible' => true,
    '#colapsed' => true,
  );

  $form['file']['flvmp_file_title'] = array(
    '#title' => t('File title'),
    '#type' => 'textfield',
    '#default_value' => $config['file_title'],
    '#description' => t('Title of the video, shown in the display or playlist.'),
  );

  $form['file']['flvmp_file_author'] = array(
    '#title' => t('File author'),
    '#type' => 'textfield',
    '#default_value' => $config['file_author'],
    '#description' => t('Author of the video, shown in the display or playlist'),
  );

  $form['file']['flvmp_file_description'] = array(
    '#title' => t('File description'),
    '#type' => 'textfield',
    '#default_value' => $config['file_description'],
    '#description' => t('Text description of the file.'),
  );

  $form['file']['flvmp_file_location'] = array(
    '#title' => t('File location'),
    '#type' => 'textfield',
    '#default_value' => $config['file_location'],
    '#description' => t('Location of the mediafile or playlist to play, leave blank to be overridden'),
  );

  // get no flash options
  $options = flvmediaplayer_extend('flvmediaplayer_file_use', 'define');
  if (! empty($options)) {
    $form['file']['flvmp_file_location_hook'] = array(
      '#type' => 'select',
      '#title' => t('Media file'),
      '#options' => $options,
      '#default_value' => $config['file_location_hook'],
      '#description' => t('Select a module to load the video for this player. A specified file location will override this choice.'),
    );
  }

  $form['file']['flvmp_file_duration'] = array(
    '#title' => t('File duration'),
    '#type' => 'textfield',
    '#default_value' => $config['file_duration'],
    '#description' => t('Duration of the file in seconds'),
    '#size' => 4,
  );

  $form['file']['flvmp_file_image'] = array(
    '#title' => t('File image'),
    '#type' => 'textfield',
    '#default_value' => $config['file_image'],
    '#description' => t('Location of a preview image; shown in display and playlist, leave blank to be overrriden'),
  );

  $form['file']['flvmp_file_url'] = array(
    '#title' => t('Link URL'),
    '#type' => 'textfield',
    '#default_value' => $config['file_url'],
    '#description' => t('URL to an external page the display, controlbar and playlist can link to.'),
  );

  $form['file']['flvmp_file_start'] = array(
    '#title' => t('File start time'),
    '#type' => 'textfield',
    '#default_value' => $config['file_start'] ? $config['file_start'] : 0 ,
    '#size' => '4',
    '#description' => t('Position in seconds where playback will start.'),
  );

  $form['file']['flvmp_file_type'] = array(
    '#title' => t('File type'),
    '#type' => 'selected',
    '#options' => array('none', 'sound', 'image', 'video', 'youtube', 'camera', 'http', 'rtmp'),
    '#default_value' => $config['file_type'] ? $config['file_type'] : 'none',
    '#description' => t('Type of file, can be sound, image, video, youtube, camera, http or rtmp. Use this to override auto-detection.'),
  );

  $form['file']['flvmp_file_captions'] = array(
    '#title' => t('Captions file'),
    '#type' => 'textfield',
    '#default_value' => $config['file_captions'],
    '#description' => t('Location of a TT captions XML file.'),
  );

  // ----------------------------------------------------------

  $form['color'] = array(
    '#type' => 'fieldset',
    '#title' => t('Color settings'),
    '#collapsible' => true,
    '#colapsed' => true,
  );

  $form['color']['flvmp_color_background'] = array(
    '#title' => t('Background color'),
    '#type' => 'textfield',
    '#default_value' => $config['color_background'],
    '#description' => t('Background color of the controlbar and playlist. Default is ffffff'),
    '#size' => 6,
  );

  $form['color']['flvmp_color_frontcolor'] = array(
    '#title' => t('Front color'),
    '#type' => 'textfield',
    '#default_value' => $config['color_frontcolor'],
    '#description' => t('Color of all icons and texts in the controlbar and playlist. Default is 000000'),
    '#size' => 6,
  );

  $form['color']['flvmp_color_lightcolor'] = array(
    '#title' => t('Light color'),
    '#type' => 'textfield',
    '#default_value' => $config['color_lightcolor'],
    '#description' => t('Color of an icon or text when you rollover it with the mouse. Default is 000000'),
    '#size' => 6,
  );

  $form['color']['flvmp_color_screencolor'] = array(
    '#title' => t('Screen color'),
    '#type' => 'textfield',
    '#default_value' => $config['color_screencolor'],
    '#description' => t('Background color of the display. Default is 000000'),
    '#size' => 6,
  );

  // ----------------------------------------------------------

  $form['layout'] = array(
    '#type' => 'fieldset',
    '#title' => t('Player Layout settings'),
    '#collapsible' => true,
    '#colapsed' => true,
  );

  $form['layout']['flvmp_player_height'] = array(
    '#title' => t('Player height'),
    '#type' => 'textfield',
    '#default_value' => $config['player_height'] ? $config['player_height'] : 280,
    '#description' => t('Height of the player, supports % for flexible layout'),
    '#size' => 4,
  );

  $form['layout']['flvmp_player_width'] = array(
    '#title' => t('Player width'),
    '#type' => 'textfield',
    '#default_value' => $config['player_width'] ? $config['player_width'] : 400,
    '#description' => t('Width of the player, supports % for flexible layout'),
    '#size' => 4,
  );

  $form['layout']['flvmp_layout_height'] = array(
    '#title' => t('Display height'),
    '#type' => 'textfield',
    '#default_value' => $config['layout_height'] ? $config['layout_height'] : 280,
    '#description' => t('Height of the display (not the entire player!) in pixels. Default is 280.'),
    '#size' => 4,
  );

  $form['layout']['flvmp_layout_width'] = array(
    '#title' => t('Display width'),
    '#type' => 'textfield',
    '#default_value' => $config['layout_width'] ? $config['layout_width'] : 400,
    '#description' => t('Width of the display (not the entire player!) in pixels. Default is 400'),
    '#size' => 4,
  );

  $form['layout']['flvmp_layout_controlbar'] = array(
    '#title' => t('Controlbar position'),
    '#type' => 'select',
    '#options' => array('bottom', 'over', 'none'),
    '#default_value' => $config['layout_controlbar'],
    '#description' => t('Position of the controlbar. Can be set to bottom, over and none.'),
  );

  $form['layout']['flvmp_layout_controlbarsize'] = array(
    '#title' => t('Controlbar size'),
    '#type' => 'textfield',
    '#default_value' => $config['layout_controlbarsize'],
    '#description' => t('Height of the controlbar in pixels.'),
    '#size' => 4,
  );

  $form['layout']['flvmp_layout_logo'] = array(
    '#title' => t('Logo'),
    '#type' => 'textfield',
    '#default_value' => $config['layout_logo'],
    '#description' => t('Location of an external jpg,png or gif image to show in the display.'),
  );

  $form['layout']['flvmp_layout_playlist'] = array(
    '#title' => t('Playlist postion'),
    '#type' => 'select',
    '#options' => array('bottom', 'over', 'right', 'none'),
    '#default_value' => $config['layout_playlist'] ? $config['layout_playlist'] : 'none',
    '#description' => t('Position of the playlist. Can be set to bottom, over, right or none.'),
  );

  $form['layout']['flvmp_layout_playlistsize'] = array(
    '#title' => t('Playlist size'),
    '#type' => 'textfield',
    '#default_value' => $config['layout_playlistsize'],
    '#description' => t('Size of the playlist. When below or above, this refers to the height, when right, this refers to the width of the playlist.'),
    '#size' => 4,
  );

  $form['layout']['flvmp_layout_skin'] = array(
    '#title' => t('Skin'),
    '#type' => 'textfield',
    '#default_value' => $config['layout_skin'],
    '#description' => t('Location of a SWF file with the player graphics.'),
  );


  // ----------------------------------------------------------

  $form['playback'] = array(
    '#type' => 'fieldset',
    '#title' => t('Playback settings'),
    '#collapsible' => true,
    '#colapsed' => true,
  );

  $form['playback']['flvmp_playback_fullscreen'] = array(
    '#title' => t('Allow fullscreen'),
    '#type' => 'checkbox',
    '#return_value' => 'true',
    '#default_value' => $config['playback_fullscreen'],
    '#description' => t('Enable fullscreen display'),
  );

  $form['playback']['flvmp_playback_autostart'] = array(
    '#title' => t('Autostart'),
    '#type' => 'checkbox',
    '#default_value' => $config['playback_autostart'],
    '#description' => t('Automatically start the player on load.'),
  );

  $form['playback']['flvmp_playback_bufferlength'] = array(
    '#title' => t('Buffer length'),
    '#type' => 'textfield',
    '#default_value' => $config['playback_bufferlength'] ? $config['playback_bufferlength'] : 0.1,
    '#description' => t('Number of seconds of the file that has to be loaded before starting.'),
    '#size' => 3,
  );

  $form['playback']['flvmp_playback_displayclick'] = array(
    '#title' => t('Display click'),
    '#type' => 'select',
    '#options' => array('play', 'link', 'fullscreen', 'none', 'mute', 'next'),
    '#default_value' => $config['playback_displayclick'] ? $config['playback_displayclick'] : 'play',
    '#description' => t('What to do when one clicks the display. Can be play, link, fullscreen, none, mute, next.'),
  );

  $form['playback']['flvmp_playback_item'] = array(
    '#title' => t('Playlist start item'),
    '#type' => 'textfield',
    '#default_value' => $config['playback_item'] ? $config['playback_item'] : 0,
    '#description' => t('Playlist item that should start to play. Use this to set a specific start-item.'),
    '#size' => 3,
  );

  $form['playback']['flvmp_playback_mute'] = array(
    '#title' => t('Mute'),
    '#type' => 'checkbox',
    '#default_value' => $config['playback_mute'],
    '#description' => t('Mute all sounds on startup. Is saved as cookie.'),
  );

  // quality is a true false value
  $form['playback']['flvmp_playback_quality'] = array(
    '#title' => t('Quality'),
    '#type' => 'checkbox',
    '#return_value' => 'true',
    '#default_value' => $config['playback_quality'] ? $config['playback_quality'] : true,
    '#description' => t('Enables high-quality playback. This sets the smoothing of videos on/off, the deblocking of videos on/off and the dimensions of the camera small/large. Is saved as cookie.'),
  );

  $form['playback']['flvmp_playback_repeat'] = array(
    '#title' => t('Repeat'),
    '#type' => 'select',
    '#options' => array('none' => t('no'), 'list' => t('Play all items on the playlist'), 'always' => t('Repeat'), 'single' => t('Repeate selected item')),
    '#default_value' => $config['playback_repeat'] ? $config['playback_repeat'] : false,
    '#description' => t('Continously play the song/video/playlist.'),
  );

  $form['playback']['flvmp_playback_shuffle'] = array(
    '#title' => t('Shuffle'),
    '#type' => 'checkbox',
    '#default_value' => $config['playback_shuffle'],
    '#description' => t('Shuffle playback of playlistitems'),
  );

  $form['playback']['flvmp_playback_stretching'] = array(
    '#title' => t('Streching'),
    '#type' => 'select',
    '#options' => array('none' => t('No stretching'), 'exactfit' => t('Disproportionate stretch'), 'uniform' => t('Stretch with black borders'), 'fill' => t('Uniform, but completely fill the display')),
    '#default_value' => $config['playback_stretching'] ? $config['playback_stretching']  : 'uniform',
    '#description' => t('Defines how to resize images in the display.'),
  );

  $form['playback']['flvmp_playback_volume'] = array(
    '#title' => t('Volume'),
    '#type' => 'select',
    '#options' => array(0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100),
    '#default_value' => $config['playback_volume'] ? $config['playback_volume'] : 90 ,
    '#description' => t('Startup volume of the player. Is saved as cookie.'),
  );

  $form['playback' ]['flvmp_playback_allowscriptaccess'] = array(
    '#title' => t('Enable javascript'),
    '#type' => 'checkbox',
    '#return_value' => 'true',
    '#default_value' => $config['playback_allowscriptaccess'] ? $config['playback_allowscriptaccess'] : true,
    '#description' => t('You can allow javascript access to the player. Default is on. You need this for recommended content '),
  );


  // ----------------------------------------------------------

  $form['external'] = array(
    '#type' => 'fieldset',
    '#title' => t('External settings'),
    '#collapsible' => true,
    '#collapsed' => true,
  );

  // use external config file
  $form['external']['flvmp_external_config' ] = array(
    '#title' => t('External Config'),
    '#type' => 'checkbox',
    '#default_value' => $config['external_config'],
    '#description' => t('An external configuration file allows you to adjust the paremters of a player after it has been embeded. This is good if you want to change the appearnce of off site players. This does add server overhead, however the configuration files are cached by Drupal. Default is off. Value applies to the embedded player as well. This file is dynamically generated at <em>flvmediaplayer/node/X</em>'),
  );

  // use external config file
  $form['external']['flvmp_external_plugins' ] = array(
    '#title' => t('Plugins'),
    '#type' => 'textarea',
    '#rows' => 2,
    '#default_value' => $config['external_plugins'],
    '#description' => t('Add additional plugins to use with your player. Format should be <em>http://yoursite.com/plugin1,http://yoursite.com/plugin2,http://yoursite.com/plugin3</em>'),
  );

  // use additional flash_vars
  $form['external']['flvmp_external_flashvars' ] = array(
    '#title' => t('Additional flash vars'),
    '#type' => 'textarea',
    '#rows' => 2,
    '#default_value' => $config['external_flashvars'],
    '#description' => t('Additional data to load into the player. Enter each variables and value on a new line: <em>file=foo.text</em>'),
  );

  $form['external']['flvmp_external_abouttext'] = array(
    '#title' => t('About text'),
    '#type' => 'textfield',
    '#default_value' => $config['external_abouttext'],
    '#description' => t('Text to show in the rightclick menu. Please do not change this if you do not have a commercial license! When undefined it shows the player version.'),
  );

  $form['external']['flvmp_external_aboutlink'] = array(
    '#title' => t('About link'),
    '#type' => 'textfield',
    '#default_value' => $config['external_aboutlink'],
    '#description' => t('URL to link to from the rightclick menu. Do not change this if you do not have a commercial license!'),
  );

  $form['external']['flvmp_external_client'] = array(
    '#title' => t('Client'),
    '#type' => 'textfield',
    '#default_value' => $config['external_client'],
    '#description' => t('Version and platform of the Flash client plugin. Useful to check for e.g. MP4 playback or fullscreen capabilities.'),
  );

  $form['external']['flvmp_external_id'] = array(
    '#title' => t('Player ID'),
    '#type' => 'textfield',
    '#default_value' => $config['external_id'],
    '#description' => t('ID of the player within the javascript DOM. Useful for javascript interaction.'),
  );

  $form['external']['flvmp_external_linktarget'] = array(
    '#title' => t('Link target'),
    '#type' => 'textfield',
    '#default_value' => $config['external_linktarget'],
    '#description' => t("Browserframe where the links from display are opened in. Some possibilities are '_self' (same frame) , '_blank' (new browserwindow) or 'none' (links are ignored in the player, so javascript can handle it)."),
  );

  $form['external']['flvmp_external_streamer'] = array(
    '#title' => t('Streamer'),
    '#type' => 'textfield',
    '#default_value' => $config['external_streamer'],
    '#description' => t('Location of a server to use for streaming. Can be an RTMP application or external PHP/ASP file to use for HTTP streaming. If set to lighttpd, the player presumes a Lighttpd server is used to stream videos.'),
  );

  $form['external']['flvmp_external_tracecall'] = array(
    '#title' => t('Tracecall'),
    '#type' => 'textfield',
    '#default_value' => $config['external_tracecall'],
    '#description' => t('Name of a javascript function that can be used for tracing the player activity. All events from the view, model and controller are sent there.'),
  );

  $form['external']['flvmp_external_version'] = array(
    '#title' => t('Version'),
    '#type' => 'textfield',
    '#default_value' => $config['external_version'],
    '#description' => t('Exact major release, minor release and revision number of the player. sent to javascript with every call.'),
  );

  // --------------------------------------------
  //  Embed Options

  $form['embed'] = array(
    '#type' => 'fieldset',
    '#title' => t('Embedded player options'),
    '#collapsible' => true,
    '#collapsed' => true,
  );

  // use a specific profile with an embed
  $form['embed']['flvmp_embed_profile' ] = array(
    '#title' => t('Embeded player profile'),
    '#type' => 'select',
    '#options' => array_merge(array(0 => t('None')), flvmediaplayer_profile_get_configs('name')),
    '#default_value' => $config['embed_profile'],
    '#description' => t('Use a specific profile with the embed player.'),
  );

  $form['embed']['flvmp_embed_enable' ] = array(
    '#title' => t('Embed enable'),
    '#type' => 'checkbox',
    '#default_value' => $config['embed_enable'],
    '#description' => t('Display embed code for this player for users to place player on their site. Displays in the node body.'),
  );

  $form['embed']['flvmp_embed_teaser' ] = array(
    '#title' => t('Embed code in teaser'),
    '#type' => 'checkbox',
    '#default_value' => $config['embed_teaser'],
    '#description' => t('Display embed code for this player in node teasers.'),
  );

  // -----------------------------------------------
  // No flash

  $form['no_flash'] = array(
    '#title' => t('No Flash support'),
    '#type' => 'fieldset',
    '#collapsible' => true,
    '#collapsed' => true,
  );

  $form['no_flash']['flvmp_flash_noflash_text_filter']['flvmp_flash_noflash_text'] = array(
    '#title' => t('No flash display'),
    '#type' => 'textarea',
    '#default_value' => $config['flash_noflash_text'],
    '#description' => t('Enter text to be displayed if the browser does not have flash enabled, ie: <em>Sorry, you need to install flash to see this content.</em>'),
  );
  $form['no_flash']['flvmp_flash_noflash_text_filter']['filter'] = filter_form(1);


  $options = flvmediaplayer_extend('flvmediaplayer_no_flash', 'define');
  $options['none'] = t('None');
  if (! empty($options)) {
	  $form['no_flash']['flvmp_flash_noflash_hook'] = array(
	    '#title' => t('No flash display'),
	    '#type' => 'select',
	    '#options' => $options,
	    '#default_value' => $config['flash_noflash_hook'] ? $config['flash_noflash_hook'] : 'none' ,
	    '#description' => t('Select an item which display no flash content. This will be overriden by entered text.'),
	  );
  }

  return $form;
}