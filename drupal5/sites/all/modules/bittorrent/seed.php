<?php

// $Id: seed.php,v 1.1.2.2 2007/08/20 18:22:51 bradfordcp Exp $

/**
 * @file
 * The web-seeding portion of the tracker.
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

$_web_seeding_enabled = variable_get('bt_tracker_web_seeding', 0);

if ($_web_seeding_enabled == 0 || $_web_seeding_enabled == 1) {
  seed_message(403, 'BitTornado Web-Seeding implementation is disabled on this tracker.', TRUE);
}

// Process the variables...
$_required_keys = array('info_hash', 'piece');
$_optional_keys = array('ranges');
$request = array();

foreach($_required_keys as $key) {
  if(!array_key_exists($key, $_GET)) {
    seed_message(400, 'Missing Key: '. $key, TRUE);
  }
  else {
    $request[$key] = $_GET[$key];
  }
}

foreach($_optional_keys as $key) {
  if(array_key_exists($key, $_GET)) {
    $request[$key] = $_GET[$key];
  }
}

/**
 * Lookup the info_hash and verify that we are indeed tracking it and it supports web seeding. Retrieve the torrent file should all criteria be met.
 */
$result = db_result(db_query("SELECT web_seed FROM {bt_torrents} btt WHERE btt.info_hash = %b", $request['info_hash']));
if (isset($result) || $result == 0) {
  seed_message(400, 'Tracker is currently not tracking that torrent or the torrent does not support web seeding.', TRUE);
}

// TODO: Add server control settings

$torrent = strip_excess(bdecode(db_result(db_query("SELECT metadata FROM {bt_torrents} btt WHERE info_hash = %b", $request['info_hash']))));
  
if (array_key_exists('length', $torrent['info'])) {
  // Single file torrent
  $torrent_length = $torrent['info']['length'];
}
else {
  // Multiple file torrent
  $torrent_length = 0;
  foreach($torrent['info']['files'] as $file) {
    $torrent_length += $file['length'];
  }
}

/**
 * Verify that the piece exists within the torrent
 */
if ($request['piece'] != 0) {  
  $piece_count = $torrent_length / $torrent['info']['piece length'];
  
  if (!is_numeric($request['piece']) || $request['piece'] > round($piece_count, 0)) {
    seed_message(400, 'Invalid Piece requested', TRUE);
  }
  else {
    $range = array();
    $range[0] = $piece * $torrent['info']['piece length'];
    $range[1] = $range[0] + $torrent['info']['piece length'];
    $request['range'] = $range[0] .'-'.$range[1];
  }
}

/**
 * Translate the byte ranges and verify them (if they are provided)
 */
if (array_key_exists('range', $request)) {
  $request['range'] = explode(',', $request['range']);
  $request['range'] = asort($request['range']);
  
  foreach($request['range'] as &$range) {
    $range = explode('-', $range, 2);
  }
  
  foreach($request['range'] as $range) {
    if ($range[0] > $range[1] || $range[1] > $torrent_length) {
      seed_message(400, 'Invalid range requested', TRUE);
    }
  }
}

// Retrieve the data
if (array_key_exists('length', $torrent['info'])) {
  // Single file torrent
  
  $bytes = '';
  
  // Build the server path
  $base_path = variable_get('bt_web_seed_dir', 'files/'. md5(drupal_get_private_key()) . '-web-seeding/');
  $path = $base_path . $torrent['info']['name'];
  
  // Open the file
  $handle = fopen($path, 'rb');
  
  foreach($request['range'] as $range) {
    fseek($handle, $range[0]);
    $bytes .= fread($handle, $range[1]-$range[0]);
  }
  
  fclose($handle);
  
}
else {
  // Multiple file torrent
  
  $bytes = '';
  
  //Build the root server path
  $base_path = variable_get('bt_web_seed_dir', 'files/'. md5(drupal_get_private_key()) . '-web-seeding/');
  $path = $base_path . $torrent['info']['name'];
  
  // Create an array of files in the torrent
  $files = array();
  
  $byte_offset = 0;
  foreach($torrent['info']['files'] as $file){
    $file_path = '';
    foreach($file['path'] as $torrent_file_path) {
      $file_path .= '/'. $torrent_file_path;
    }
    
    $files[] = array(
      'path' => $file_path,
      'byte_start' => $byte_offset,
      'byte_end' => $byte_offset + $file['length'],
    );
    $byte_offset += $file['length'];
  }
  
  // Process the ranges
  foreach($request['range'] as $range) {
    $byte_offset = 0;
    foreach($files as $file) {
      if($range[0] < $file['byte_end'] && $range[1] < $file['byte_end']) {
        // In this instance the byte range lies within the entire file
        $handle = fopen($path . $file['path'], 'rb');
        
        fseek($handle, $range[0]-$byte_offset);
        $bytes .= fread($handle, $range[1]-$range[0]);
        
        fclose($handle);
      }
      else if($range[0] < $file['byte_end'] && $range[1] > $file['byte_end']) {
        // The range begins in this file, but continues on into the next we trick the system by reseting the range value for the next file
        $handle = fopen($path . $file['path'], 'rb');
        
        fseek($handle, $range[0]-$byte_offset);
        $bytes .= fread($handle, $range[1]-$range[0]); // This reads to the end of the file
        
        // Now let's adjust the range values to pull data out of the next file to cover the rest of the byte range
        $range[0] = $file['byte_end'];
      }
      $byte_offset = $file['byte_end'];
    }
  }
}

seed_message(200, $bytes, TRUE);

/**
 * Returns a message to teh torrent client trying to request the piece
 * 
 * @param $status_code
 *   The numerical status code to return.
 * @param $message
 *   The message to return along with the status code.
 * @param $exit
 *   Determines whether or not processing should stop.
 */
function seed_message($status_code = 503, $message = '' , $exit = false) {
  ob_clean();
  if ($status_code == 503) {
    $header = '503 Service Temporarily Unavailable';
  }
  else if ($status_code == 400) {
    $header = '400 Bad Request';
  }
  else if ($status_code == 500) {
    $header = '500 Internal Server Error';
  }
  else if ($status_code == 403) {
    $header = '403 Forbidden';
  }
  else if ($status_code == 200) {
    $header = '200 Ok';
  }
  
  header('HTTP/1.0 '. $header);
  print($message);
  
  if ($exit) {
    exit();
  }
}