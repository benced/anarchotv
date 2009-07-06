// $Id: README.txt,v 1.2.2.1.2.3 2008/11/23 22:49:11 arthuregg Exp $


-----------------------------------------------
| INSTALLATION                                |
-----------------------------------------------

This is a module which helps generate the configuration for the Jeroen Wijering Flash Media player. You need to
download the JW Media player from:

http://www.jeroenwijering.com/?item=JW_FLV_Player

Make sure to pay attention to the licencing!

This module can support the 3x player, but it is now focused on the 4x functionality.

Place the mediaplayer.swf file in this module directory and set your configuration options
per content type at admin/settings/flvmediaplayer. You can also place the player where
you choose and set the path on each profile that you build.

After installing, navigate to 

admin > settings > flvmediaplayer

A configuration wizard will help you configure a profile. If you do not need
the wizard, click on the "expert settings" option. 

-----------------------------------------------
| USAGE                                       |
-----------------------------------------------

Go to admin > settings > flvmediaplayer 

Allow the wizard to help you setup a profile

Go to admin > settings > flvmediaplayer

Set your profile as the default profile on the content types that you wish
You can also import and export profiles here

Advanced usage: 
 * use external configuration options to make it easier to update your players
   after they have already been embeded
 * use [flvmediaplayer] in your posts to place a player where you want it
 * use the profile option on embedded players to assign a different profile for 
   your embedded content- for example on your site you don't use a logo
   but you want your embedded players to have a logo.
 * render a player on your page with theme('flvmediaplayer_display', $node, $profile, $params); 


-----------------------------------------------
| REQUIREMENTS                                |
-----------------------------------------------

Requires SWFObject_api module

You may also find XSPF Playlist (http://drupal.org/project/xspf_playlist) helpful

-----------------------------------------------
| ADVANCED USAGE                              |
-----------------------------------------------

Implementing the hook_flvmediaplayer_use($op, $name, $config) 

switch ($op) {
  case 'define':
    return a keyed array which is modulename--itemID => description. This allows a given
    module to implement multiple items for flvmediaplayer to use. 
 

switch ($op) {
  case 'view':
    return a single keyed array which has a path to the file
    
  

Implementing the hook_flvmediaplayer_no_flash($op, $name, $config) 

switch ($op) {
  case 'define':
    return a keyed array which is modulename--itemID => description. This allows a given
    module to implement multiple items for flvmediaplayer to use. 
 

switch ($op) {
  case 'view':
    return html to be displayed in place of the player






