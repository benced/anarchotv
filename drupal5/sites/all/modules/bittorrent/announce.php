<?php
// $Id: announce.php,v 1.4.2.9 2008/02/04 18:51:49 bradfordcp Exp $

/**
 * @file
 * The announce portion of the tracker module.
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

$_tracker_name = (string) variable_get('bt_tracker_name', variable_get('site_name', 'BitTorrent') .' Tracker');
$_tracker_version = "0.6";
$_tracker_scope = variable_get('bt_tracker_scope', 0);
$_maximum_simultaneous_downloads = variable_get('bt_tracker_maximum_simultaneous_downloads', 0);
$_minimum_ratio = variable_get('bt_tracker_minimum_ratio', 0);

// Do not continue parsing if the module is not enabled
$active = db_result(db_query("SELECT status FROM {system} WHERE name = '%s'", 'bt_tracker'));

if ($active == 1) {
  // This will contain the values of the request for easier access later. 
  $request = array();
  
  $_required_keys = array('info_hash', 'peer_id', 'port', 'uploaded', 'downloaded', 'left');
  $_optional_keys = array('ip', 'numwant', 'num want', 'num_want', 'no_peer_id', 'key', 'trackerid', 'event');
  
  // Add passkey to the list of required keys when the scope is set to private.
  if ($_tracker_scope == 2) {
    $_required_keys[] = 'passkey';
  }
  else {
    $_optional_keys[] = 'passkey';
  }
  
  // Check for missing keys in the querystring
  foreach ($_required_keys as $key) {
    if (!array_key_exists($key, $_GET)) {
      bt_message('Missing Key: '. $key, BITTORRENT_MESSAGE_ERROR);
    }
    else {
      $request[$key] = $_GET[$key];
    }
  }
  
  // Check for optional keys in the querystring
  foreach ($_optional_keys as $key) {
    if (array_key_exists($key, $_GET)) {
      $request[$key] = $_GET[$key];
    }
  }
  
  /**
   * passkey
   *   This is the passkey that is assigned by the tracker to each user. In the case of 
   *   this tracker it is a 20-byte string which is the result of a sha1 of various information.
   *   It is created in such a way such that now two users have the same passkey.
   */
  if (array_key_exists('passkey', $request)) {
    if (strlen(urldecode($request['passkey'])) != 20) {
      bt_message('Invalid passkey: '. $request['passkey'], BITTORRENT_MESSAGE_ERROR);
    }
  }
  
  /**
   * info_hash
   *   20-byte SHA1 hash of the value of the info key from the Metainfo file.
   *   Note that the value will be a bencoded dictionary, given the definition
   *   of the info key above. Note: This string is always urlencoded, as opposed
   *   to peer_id, which needs to be unencoded.
   */
  if (strlen($request['info_hash']) != 20) {
    bt_message('Invalid info_hash ('. strlen($request['info_hash']) .' - '. urlencode($request['info_hash']) .')', BITTORRENT_MESSAGE_ERROR);
  }
  
  /**
   * no_peer_id
   *   Optional. Indicates the client accepts a no_peer_id response. The peer_id key is then
   *   not required to be in the response of the tracker.
   */  
  if (!array_key_exists('no_peer_id', $request)) {
    $request['no_peer_id'] = 0;
  }
  
  /**
   * peer_id
   *   20-byte string used as a unique ID for the client, generated by the client
   *   at startup. This is allowed to be any value, and may be binary data. There
   *   are currently no guidelines for generating this peer ID. However, one may
   *   rightly presume that it must at least be unique for your local machine,
   *   thus should probably incorporate things like process ID and perhaps a
   *   timestamp recorded at startup.
   */
  if ((strlen($request['peer_id']) != 20)) {
    bt_message('Invalid peer_id ('. strlen($request['peer_id']) .' - '. urlencode($request['peer_id']) .')', BITTORRENT_MESSAGE_ERROR);
  }  
  
  /**
   * key
   *   Optional. An additional identification that is not shared with any users.
   *   It is intended to allow a client to prove their identity should their IP
   *   address change.
   */
  if (array_key_exists('key', $request)) {
    $request['key'] = strtoupper(stripslashes($request['key']));
  }
  else {
    $request['key'] = NULL;
  }
  
  /**
   * ip
   *   Optional. The true IP address of the client machine, in dotted quad
   *   format or rfc3513 defined hexed IPv6 address. Notes: In general this
   *   parameter is not necessary as the address of the client can be determined
   *   from the IP address from which the HTTP request came.
   */
  if (array_key_exists('ip', $request)) {
    $request['ip'] = stripslashes($request['ip']);
    if (empty($request['ip']) || !preg_match('/^(\d{1,3}\.){3}\d{1,3}$/s', $request['ip'])) {
      $request['ip'] = $_SERVER["REMOTE_ADDR"];
    }
  }
  else {
    $request['ip'] = $_SERVER["REMOTE_ADDR"];
  }
  
  /**
   * port
   *   The port number that the client is listening on. Ports reserved for
   *   BitTorrent are typically 6881-6889. Clients may choose to give up if
   *   it cannot establish a port within this range.
   */
  $request['port'] = 0 + stripslashes($request['port']);
  if (!$request['port'] || $request['port'] > 0xffff) {
    bt_message("Invalid port: ". $request['port'], BITTORRENT_MESSAGE_ERROR);
  }
  
  
  /**
   * uploaded
   *   The total amount uploaded (since the client sent the 'started' event
   *   to the tracker) in base ten ASCII. While not explicitly stated in the
   *   official specification, the concensus is that this should be the total
   *   number of bytes uploaded.
   */
  $request['uploaded'] = 0 + stripslashes($request['uploaded']);
  
  
  /**
   * downloaded
   *   The total amount downloaded (since the client sent the 'started' event
   *   to the tracker) in base ten ASCII. While not explicitly stated in the
   *   official specification, the consensus is that this should be the total
   *   number of bytes downloaded.
   */
  $request['downloaded'] = 0 + stripslashes($request['downloaded']);
  
  
  /**
   * left
   *   The number of bytes this client still has to download, encoded in base
   *   ten ASCII.
   */
  $request['left'] = 0 + stripslashes($request['left']);
  
  
  /**
   * event
   *   If specified, must be one of started, completed, stopped, (or empty
   *   which is the same as not being specified). If not specified, then this
   *   request is one performed at regular intervals.
   *     - started: The first request to the tracker must include the event
   *       key with the started value.
   *     - stopped: Must be sent to the tracker if the client is shutting down
   *       gracefully.
   *     - completed: Must be sent to the tracker when the download completes.
   *       However, must not be sent if the download was already 100% complete
   *       when the client started. Presumably, this is to allow the tracker to
   *       increment the "completed downloads" metric based soley on this event.
   */
  if (array_key_exists('event', $request)) {
    switch (strtolower(stripslashes($request['event']))) {
      case 'started':
      case 'stopped':
      case 'completed':
      case '':
        break;
      default:
        $request['event'] = '';
    }
    
    $request['event'] = strtolower(stripslashes($request['event']));
  }
  else {
    $request['event'] = '';
  }
  
  /**
   * numwant
   *   Optional. Number of peers that the client would like to receive
   *   from the tracker. This value is permitted to be zero. If omitted,
   *   typically defaults to 50 peers.
   */
  if (!array_key_exists('numwant', $request)) {
    foreach (array('num want', 'num_want') as $key) {
      if (array_key_exists($key, $request)) {
        $request['numwant'] = 0 + $request[$key];
        unset($request[$key]);
        break;
      }
    }
  }  
  if (array_key_exists('numwant', $request)) {
    if ($request['numwant'] == 0) {
      $request['numwant'] = 50;
    }
  }
  else {
    $request['numwant'] = 50;
  }
    
  /**
   * trackerid
   *   Optional. If a previous announce contained a tracker id, it should be
   *   set here. Default value is: BitTorrent Tracker - [version]
   */
  if (array_key_exists('trackerid', $request)) {
    $request['trackerid'] = strtolower(stripslashes($request['trackerid']));
  }
  else {
    $request['trackerid'] = $_tracker_name .'-'. $_tracker_version;
  }

  /* ************************************************************************ */
  /* ******************** DONE EVALUATING GET PARAMETERS ******************** */
  /* ************************************************************************ */
  
  // Set default values
  $uid = 0;
  
  // Lookup the torrent
  $torrent = db_fetch_object(db_query("SELECT info_hash FROM {bt_torrents} bt WHERE bt.info_hash = %b", $request['info_hash']));
  if(!$torrent) {
    bt_message('Torrent does not exist, please consider uploading it.', BITTORRENT_MESSAGE_ERROR);
  }
  
  // Start the generating the response.
  if ($_tracker_scope == 2) {
    // Lookup the user and validate permission to use the tracker.
    $uid = db_result(db_query("SELECT uid FROM {bt_tracker_users} WHERE passkey = %b", $request['passkey']));
    
    if (!$uid) {
      bt_message('Passkey does not exist please re-download torrent.', BITTORRENT_MESSAGE_ERROR);
    }
    
    // Determine if the user has permission to use the tracker.
    $role = db_result(db_query("SELECT r.name FROM {role} r INNER JOIN {users_role} ur ON ur.rid = r.rid INNER JOIN {permission} p ON p.rid = ur.rid WHERE ur.uid = %d AND p.perm LIKE '%download torrent%'"));
    
    if (!$role) {
      bt_message('User does not have permission to user the tracker.', BITTORRENT_MESSAGE_ERROR);
    }
    
    // Check to make sure the user meets minimum requirements      
    if ($_maximum_simultaneous_downloads != 0) {
      $count = db_result(db_query("SELECT COUNT(uid) FROM {bt_tracker_active_users} WHERE uid = %d", $uid));
      
      if ($count > $_maximum_simultaneous_downloads) {
        bt_message('Too many simultaneous downloads, the limit is '. $_maximum_simultaneous_downloads, BITTORRENT_MESSAGE_ERROR);
      }
    }
    
    if ($_minimum_ratio) {
      $ratio = db_result(db_query("SELECT bytes_downloaded / bytes_uploaded FROM {bt_tracker_users} WHERE uid = %d"));
      
      if (!$ratio) {
        $ratio = 0;
      }
    }
    
    if ($_minimum_ratio && $ratio < $_minimum_ratio) {
      bt_message('Minimum ratio has not been met, you have '. $ratio .', and you need '. $_minimum_ratio, BITTORRENT_MESSAGE_ERROR);
    }
    
    $user_count = db_result(db_query("SELECT DISTINCT COUNT(btau.ip) FROM {bt_tracker_active_users} btau INNER JOIN {bt_tracker_users} btu on btu.uid = btau.uid WHERE btu.passkey = %b", $request['passkey']));
        $passkey_status = db_result(db_query("SELECT passkey_status FROM {bt_tracker_users} WHERE passkey = %b", $request['passkey']));
    
    // Evaluates to true if there are two peers downloading with the same passkey
    if ($user_count && $user_count > 1) {
      bt_message("Passkey is already in use by another peer. The passkey has been marked to be reset, please redownload your torrents.", BITTORRENT_MESSAGE_ERROR);
      db_query("UPDATE {bt_tracker_users} SET passkey_status = 1 WHERE uid = %d", $uid);
    }
        else if ($passkey_status == 1) {
            bt_message("Passkey is marked for reset, please re-download your torrents.", BITTORRENT_MESSAGE_WARNING);
        }
  }
  
  if ($_tracker_scope == 1) {
    if (array_key_exists('passkey', $request)) {
      // Retrieve the information for the users previous request
      $last = db_fetch_array(db_query("SELECT bytes_uploaded, bytes_downloaded FROM {bt_tracker_active_users} btau WHERE (peer_id = %b AND peer_key = %b) AND info_hash = %b", $request['peer_id'], $request['key'], $request['info_hash']));
      
      if (!$last) {
        $last['bytes_uploaded'] = 0;
        $last['bytes_downloaded'] = 0;
      }
      
      // Set the statistics for the appropriate user.
      db_query("UPDATE {bt_tracker_users} SET bytes_uploaded = bytes_uploaded + %d, bytes_downloaded = bytes_downloaded + %d WHERE passkey = %b", ($request['uploaded'] - $last['bytes_uploaded']), ($request['downloaded'] - $last['bytes_downloaded']), $request['passkey']);
    }
  }
  
  
  if ($request['event'] != 'stopped' && $request['event'] != 'started') {
    // Update the user statistics in the active users table
    db_query("UPDATE {bt_tracker_active_users} btau SET uid = %d, ip = '%s', port = %d, bytes_uploaded = %d, bytes_downloaded = %d, bytes_left = %d, last_announce = %d WHERE (peer_id = %b AND peer_key = %b) AND info_hash = %b", $uid, $request['ip'], $request['port'], $request['uploaded'], $request['downloaded'], $request['left'], time(), $request['peer_id'], $request['key'], $request['info_hash']);
    
    if ($request['event'] == 'completed') {
      // Keep the user in the active users table and increment the downloaded field in torrents
      db_query("UPDATE {bt_torrents} bt SET downloaded = downloaded + 1 WHERE info_hash = %b", $request['info_hash']);
    }
  }
  else if ($request['event'] == 'started') {
    // Add the user to the active users table
    db_query("INSERT INTO {bt_tracker_active_users} (uid, peer_id, peer_key, ip, port, info_hash, bytes_uploaded, bytes_downloaded, bytes_left, last_announce) VALUES (%d, %b, %b, '%s', %d, %b, %d, %d, %d, %d)", 0, $request['peer_id'], $request['key'], $request['ip'], $request['port'], $request['info_hash'], $request['uploaded'], $request['downloaded'], $request['left'], time());
  }
  else if ($request['event'] == 'stopped') {
    // Remove the user from the active users table
    db_query("DELETE FROM {bt_tracker_active_users} WHERE (peer_id = %b AND peer_key = %b) AND info_hash = %b", $request['peer_id'], $request['key'], $request['info_hash']);
  }
  
  // The response object
  $response = array();
  $response['interval'] = variable_get('bt_announce_interval', 1800);
  $response['peers'] = array();
  
  // Retrieve the list of currently active users
  $result = db_query("SELECT btau.peer_id, btau.ip, btau.port FROM {bt_tracker_active_users} btau WHERE btau.info_hash = %b AND NOT (btau.peer_id = %b) LIMIT %d", $request['info_hash'], $request['peer_id'], $request['numwant']);
  while($row = db_fetch_array($result)) {
    $response['peers'][] = $row;
  }
  
  // Return the response
  bencode_response_raw(bencode($response));
}
else {
  // Let the client know that the tracker is offline.
  bt_message('Tracker is Offline', BITTORRENT_MESSAGE_ERROR);
}