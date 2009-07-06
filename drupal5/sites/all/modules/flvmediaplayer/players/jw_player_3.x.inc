<?php

  /**
   * @file this defines the forms and variables for the 4.x version of the player
   */


/**
 * this builds out the settings form for the player
 * @param string $node_type is the node type in use
 * @param array $config is a drupal node object
 */
function flvmediaplayer_player_form($config = null) {
 
  $form = array();
  $form['layout']['flvmp_player_height'] = array(
    '#title' => t('Player height'),
    '#type' => 'textfield',
    '#default_value' => $config['player_height'] ? $config['player_height'] : 400,
    '#description' => t('Height of the player'),
    '#size' => 4,
  ); 
  
  $form['layout']['flvmp_player_width'] = array(
    '#title' => t('Player width'),
    '#type' => 'textfield',
    '#default_value' => $config['player_width'] ? $config['player_width'] : 280,
    '#description' => t('Width of the player'),
    '#size' => 4,
  );  
  
  // Display options
  $form['flvmp_logo'] = array(
    '#title' => t('Logo / Watermark file to display on player'), 
    '#type' => 'textfield',
    '#default_value' => $config['logo'],
    '#description' => t('URL to an image to use as a watermark or logo on your player.'),
  );
  $options = array(
    'none' => t('No alteration'),
    'fit' => t('Strech X and Y to fill player frame'),
    'stretch' => t('Fit vertical size of player frame'),
  );
  $form['flvmp_stretch'] = array(
    '#title' => t('Display handling'), 
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => $config['stretch'],
    '#description' => t('Define how the movie fits in to the player.'),
  );
  $form['flvmp_playback_fullscreen'] = array(
    '#title' => t('Allow fullscreen'), 
    '#type' => 'checkbox',
    '#default_value' => $config['playback_fullscreen'],
    '#description' => t('Enable fullscreen display'),
  );  
         
  // --------------------------------------------
  // Player colour options
  $form['color'] = array(
    '#title' => t('Player colours'),
    '#theme' => 'flvmediaplayer_color_table',
    '#type' => 'fieldset',
    '#cols' => 2,
    '#collapsible' => true,
    '#collapsed' => true,   
  );                    
  $form['color']['flvmp_back_color'] = array(
    '#title' => t('Background color'), 
    '#type' => 'textfield',
    '#size' => 6,
    '#default_value' => $config['back_color'],
    '#description' => t('Background color of the flash player.'),
  );          
  $form['color']['flvmp_front_color'] = array(
    '#title' => t('Front color'), 
    '#type' => 'textfield',
    '#size' => 6,
    '#default_value' => $config['front_color'],
    '#description' => t('Front color of the flash player.'),
  );          
  $form['color']['flvmp_light_color'] = array(
    '#title' => t('Light color'), 
    '#type' => 'textfield',
    '#size' => 6,
    '#default_value' => $config['light_color'],
    '#description' => t('Light color of the flash player.'),
  );          
  $form['color']['flvmp_screen_color'] = array(
    '#title' => t('Screen color'), 
    '#type' => 'textfield',
    '#size' => 6,
    '#default_value' => $config['screen_color'],
    '#description' => t('Screen color of the flash player.'),
  );

  // --------------------------------------------
  // Play List Behavior
  $form['playlist'] = array(
    '#title' => t('Playlist controls'),
    '#type' => 'fieldset',
    '#collapsible' => true,
    '#collapsed' => true, 
    '#description' => t('Controls playlist behavior. If there are no playlists associated with the file, ignored.'),
  ); 

  $form['playlist']['flvmp_shuffle' ] = array(
    '#title' => t('Shuffle'), 
    '#type' => 'checkbox',
    '#default_value' => $config['shuffle'],
    '#description' => t('Shuffle the playlist during playback'),
  );
  
  $options = array(
    'false' => t('No repeat'),
    'true' => t('Repeat'),
    'list' => t('Play as a list')
  );
  $form['playlist']['flvmp_repeat' ] = array(
    '#title' => t('Playlist Repeat'), 
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => $config['repeat'],
    '#description' => t('How playlists are handled, default is <em>list</em> which plays a playlist once through.'),
  );
  
  $form['playlist']['flvmp_pl_scroll' ] = array(
    '#title' => t('Playlist autoscroll'), 
    '#type' => 'checkbox',
    '#default_value' => $config['pl_scroll'],
    '#description' => t('Check to automatically scroll through the playlist on rollover, instead of using a scrollbar'),
  );
  
  $form['playlist']['flvmp_pl_thumb' ] = array(
    '#title' => t('Thumbnails in playlists'), 
    '#type' => 'checkbox',
    '#default_value' => $config['pl_thumb'],
    '#description' => t('Check to automatically scroll through the playlist on rollover, instead of using a scrollbar'),
  );
  
 $form['playlist']['flvmp_pl_height' ] = array(
    '#title' => t('Playlist height'), 
    '#type' => 'textfield',
    '#size' => 4,
    '#default_value' => $config['pl_height'],
    '#description' => t('Set this smaller than the height to show a playlist below the display. If you set it the same as the height, the controlbar will auto-hide on top of the video.'),
  );
  $form['playlist']['flvmp_pl_width' ] = array(
    '#title' => t('Playlist width'), 
    '#type' => 'textfield',
    '#size' => 4,
    '#default_value' => $config['pl_width'],
    '#description' => t('Set this smaller as the width to show a playlist to the right of the display.'),
  );
                         
  // enable javascript?
  $form['flvmp_playback_allowscriptaccess' ] = array(
    '#title' => t('Enable javascript'), 
    '#type' => 'checkbox',
    '#default_value' => $config['playback_allowscriptaccess'],
    '#description' => t('You can allow javascript access to the player. Default is on. You need this for recommended content '),
  );
  
  // implement flvmediaplayer_file_use
  $options = module_invoke_all('flvmediaplayer_file_use', 'define');                               
  // select where to build      
  $form['flvmp_file_location_hook'] = array(
    '#type' => 'select',
    '#title' => t('Media file'),
    '#options' => $options,
    '#default_value' => $config['file_location_hook'],
    '#description' => t('Select a module to load the video for this player. If this module does not return a file, the player will not be rendered.'),
  );
    
  // handle off line media 
  $form['flvmp_no_media' ] = array(
    '#title' => t('Media offline'), 
    '#type' => 'checkbox',
    '#default_value' => $config['no_media'],
    '#description' => t('If no media file is present, should a "media offline" notice be posted? If additional flash vars are present, this will be ignored'),
  );
  
  // display in body of the node
  $form['flvmp_display' ] = array(
    '#title' => t('Render in node body'), 
    '#type' => 'checkbox',
    '#default_value' => $config['display'],
    '#description' => t('You can render the player in the node body, or do custom theming. Default is in node body.'),
  );
  
  // display on teasers
  $form['flvmp_teaser' ] = array(
    '#title' => t('Display in teaser'), 
    '#type' => 'checkbox',
    '#default_value' => $config['teaser'],
    '#description' => t('Include player in teaser. This is set per content type, is not overriden.'),
  );
  
  // --------------------------------------------
  //  Embed Options
  
  $form['embed'] = array(
    '#type' => 'fieldset',
    '#title' => t('Embed options'),
    '#collapsible' => true,
    '#collapsed' => true,      
  );
  
  $form['embed']['flvmp_embed' ] = array(
    '#title' => t('Embed code'), 
    '#type' => 'checkbox',
    '#default_value' => $config['embed'],
    '#description' => t('Display embed code for this player for users to place player on their site.'),
  );
  
  $form['embed']['flvmp_embed_teaser' ] = array(
    '#title' => t('Embed code in teaser'), 
    '#type' => 'checkbox',
    '#default_value' => $config['embed_teaser'],
    '#description' => t('Display embed code for this player in node teasers'),
  );
  
  // use external config file
  $form['embed']['flvmp_embed_ex-config' ] = array(
    '#title' => t('Embed uses external config'), 
    '#type' => 'checkbox',
    '#default_value' => $config['embed_ex-config'],
    '#description' => t('An external configuration file allows you to adjust the paremters of a player after it has been embeded. This is good if you want to change the appearnce of off site players. This does add server overhead, however the configuration files are cached by Drupal. Default is on for embeds.'),
  );
  
  // --------------------------------------------
  //  External Config
  
  $form['external'] = array(
    '#type' => 'fieldset',
    '#title' => t('External communication'),
    '#collapsible' => true,
    '#collasped' => true,
    '#description' => t('External configuration files'),        
  );
  
  // implement recommende content url
  $options = module_invoke_all('flvmediaplayer_recommended', 'define', null, null);
  $form['external']['flvmp_recommended'] = array(
    '#type' => 'select',
    '#title' => t('recommended content display'),
    '#options' => $options,
    '#default_value' => $config['recommended'],
    '#description' => t('Display recommended content at the end of the video using this recommendation set.'),
  );
  
  // enable tracking if trackit module is installed
  if (module_exists('trackit')) {
    $form['external']['flvmp_tracker' ] = array(
      '#title' => t('Use Trackit'), 
      '#type' => 'checkbox',
      '#default_value' => $config['tracker'],
      '#description' => t('Enable this to track player views with the Trackit module'),
    );
  }
  
  // use external config file
  $form['external']['flvmp_external_config' ] = array(
    '#title' => t('External Config'), 
    '#type' => 'checkbox',
    '#default_value' => $config['external_config'],
    '#description' => t('An external configuration file allows you to adjust the paremters of a player after it has been embeded. This is good if you want to change the appearnce of off site players. This does add server overhead, however the configuration files are cached by Drupal. Default is off for the player.'),
  );
  
  return $form;
}