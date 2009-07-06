<?php
// $Id: scrape.php,v 1.4.2.7 2007/09/22 05:10:21 bradfordcp Exp $

/**
 * @file
 * The scrape portion of the tracker module.
 *
 * Accepts the input from the users torrent client
 * and acts accordingly.
 */

// Require the bt functions
require_once('bt_common.inc');

// Include bootstrap.inc and run the bootstrap
include_once("includes/bootstrap.inc");
drupal_bootstrap(DRUPAL_BOOTSTRAP_DATABASE);

// Initializes $conf so we can use variable_get
$conf = variable_init(isset($conf) ? $conf : array());

// Do not continue parsing if the module is not enabled
$active = db_result(db_query("SELECT status FROM {system} WHERE name = '%s'", 'bt_tracker'));

if ($active == 1) {
  $response = array('files' => array());
  $hashes = array();
  
  $request_variables = split('&', $_SERVER['QUERY_STRING']);
  foreach ($request_variables as &$request_var) {
    preg_match('/info_hash=/', $request_var, $matches);
    
    if (count($matches) == 0 ) {
      unset($request_var);
    }
    else {
      // Strip 'info_hash=' from the begining of the value and urldecode it.
      $hashes[] = urldecode(substr($request_var, 10));
    }
  }
  
  if (count($hashes[0]) == 0) {
    bt_message('Please supply an info_hash', BITTORRENT_MESSAGE_ERROR);
  }
  
  foreach ($hashes as $info_hash) {  
    $info_hash;
    
    $valid = db_result(db_query("SELECT nid FROM {bt_torrents} WHERE info_hash = %b", $info_hash));
    if ($valid) {
      if (variable_get('bt_tracker_scrape_scope', 0) == 0) {
        $response['files'][$info_hash] = db_fetch_array(db_query("SELECT bt.seeders AS complete, bt.leechers AS incomplete, bt.downloaded FROM {bt_torrents} bt WHERE bt.info_hash = %b", $info_hash));
      }
      else {
        $response['files'][$info_hash]['complete'] = db_result(db_query("SELECT COUNT(btau.ip) FROM {bt_tracker_active_users} btau WHERE btau.bytes_left = 0 AND btau.info_hash = %b", $info_hash));
        $response['files'][$info_hash]['incomplete'] = db_result(db_query("SELECT COUNT(btau.ip) FROM {bt_tracker_active_users} btau WHERE (NOT btau.bytes_left = 0) AND btau.info_hash = %b", $info_hash));
        $response['files'][$info_hash]['completed'] = db_result(db_query("SELECT bt.downloaded FROM {bt_torrents} bt WHERE bt.info_hash = %b", $info_hash));
      }
      
      $struct = strip_excess(bdecode(db_result(db_query('SELECT metadata FROM {bt_torrents} WHERE info_hash = %b', $info_hash))));
      $response['files'][$info_hash]['name'] = $struct['info']['name'];
    }  
  }

  if (count($response['files']) == 0) {
    bt_message('Invalid info hash(s)', BITTORRENT_MESSAGE_ERROR);
  }
  
  $response['flags'] = array();
  $response['flags']['min_request_interval'] = variable_get('bt_tracker_scrape_interval', 900);
  
  bencode_response_raw(bencode($response));
}
else {
  // Let the client know that the tracker is offline.
  bt_message('Tracker is Offline', BITTORRENT_MESSAGE_ERROR);
}