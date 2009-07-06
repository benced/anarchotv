<?php
// $Id: bt_common.inc,v 1.4.2.8 2008/02/04 18:51:49 bradfordcp Exp $

/**
 * @file
 * The bencoding and bencdecoding functions for the bittorrent module.
 */
 
// Define the global values
define('BITTORRENT_MESSAGE_WARNING', 0, true);
define('BITTORRENT_MESSAGE_ERROR', 1, true);

/**
 * Sends a message to the bittorrent client
 * 
 * Depending on the type of message processing of the GET variables may be stopped
 * @param $msg
 *   The message to be displayed to the client
 * @param $type
 *   The type of message being sent vaild options are: BITTORRENT_MESSAGE_WARNING and BITTORRENT_MESSAGE_ERROR
 */
function bt_message($msg, $type) {
  switch($type) {
    case BITTORRENT_MESSAGE_WARNING:
      $msg = bencode(array('warning message' => $msg));
      $exit = false;
      break;
      
    case BITTORRENT_MESSAGE_ERROR:
      $msg = bencode(array('failure reason' => $msg));
      $exit = true;
      break;
    
    default:
      return;
  }
  
  bencode_response_raw($msg);
  
  if ($exit) {
    exit();
  }
}

/**
 * Converts a specified structure into its bencoded form. Acceptable value include: integers, strings, arrays, and assoc. arrays.
 * Note that associative arrays are encoded into dictionaries while non-associative arrays are encoded into lists
 *
 * @param $struct
 *    This is the structure to bencode example passing the value 'hi' would return '2:hi'
 * @return The bencoded form of $struct
 */
function bencode($struct) {
  if (is_array($struct)) {
    $numeric = true;
    foreach(array_keys($struct) as $key) {
      if (!is_numeric($key))
        $numeric = false;
    }
    
    if (!$numeric) {
      $out = 'd';
      ksort($struct);
      foreach($struct as $key => $value) {
        $out .= bencode($key);
        $out .= bencode($value);
      }
      $out .= 'e';
    }
    else {
      $out = 'l';
      ksort($struct);
      foreach($struct as $key => $value) {
        $out .= bencode($value);
      }
      $out .= 'e';
    }
    
    return $out;
  }
  elseif (preg_match('/^(\+|\-)?\d+$/',$struct)) {
    return 'i'.$struct.'e';
  }
  else {
    return strlen($struct).':'.$struct;
  }
  return;
}

/**
 * This funtion decodes bencoded data. It automatically determines type and create the structure based off of the type.
 * 
 * @param $metadata
 *    The data to be decoded
 * @return The decoded data in the following structure:
 *    array(
 *      ['type'] => type,              // This can be integer, string, list, dictionary
 *      ['value'] => value,            // This is the decoded value
 *      ['strlen'] => strlen,          // This is the length of the bencoded string
 *      ['string'] => bstring,         // This is the bencoded string
 *    )
 */
function bdecode($metadata) {
  if (preg_match('/^(\d+):/', $metadata, $matches)) {
    $length = $matches[1];
    $string_start = strlen($length) + 1;
    $value = substr($metadata, $string_start, $length);
    $bencoded = substr($metadata, 0, $string_start + $length);
    if (strlen($value) != $length) {
      return;
    }
    return array('type' => "string", 'value' => $value, 'strlen' => strlen($bencoded), 'string' => $bencoded);
  }
  
  if (preg_match('/^i(\d+)e/', $metadata, $matches)) {
    $value = $matches[1];
    $bencoded = "i" . $value . "e";
    if ($value === "-0") {
      return;
    }
    if ($v[0] == "0" && strlen($v) != 1) {
      return;
    }
    return array('type' => "integer", 'value' => $value, 'strlen' => strlen($bencoded), 'string' => $bencoded);
  }
  
  switch ($metadata[0]) {
    case "l":
      return bdecode_list($metadata);
    
    case "d":
      return bdecode_dictionary($metadata);
    
    default:
      return;
  }
}

/**
 * Used to decode bencoded lists
 *
 * @param $metadata
 *    The bencoded list
 * @return An array containing the contents of the list
 */
function bdecode_list($metadata) {
  if ($metadata[0] != "l") {
    return;
  }
  
  $length = strlen($metadata);
  $position = 1;
  $value = array();
  $bencoded = "l";
  
  for (;;) {
    if ($position >= $length) {
      return;
    }
    if ($metadata[$position] == "e") {
      break;
    }
    $return = bdecode(substr($metadata, $position));
    if (!isset($return) || !is_array($return)) {
      return;
    }
    $value[] = $return;
    $position += $return["strlen"];
    $bencoded .= $return["string"];
  }
  
  $bencoded .= "e";
  return array('type' => "list", 'value' => $value, 'strlen' => strlen($bencoded), 'string' => $bencoded);
}

/**
 * Used to decode bencoded dictionaries
 *
 * @param $metadata
 *    The bencoded dctionary
 * @return An associative array containing the keys and values of the dictionary.
 */
function bdecode_dictionary($metadata) {
  if ($metadata[0] != "d") {
    return;
  }
  $length = strlen($metadata);
  $position1 = 1;
  $value = array();
  $bencoded = "d";
  for (;;) {
    if ($position1 >= $length) {
      return;
    }
    if ($metadata[$position1] == "e") {
      break;
    }
    $return = bdecode(substr($metadata, $position1));
    if (!isset($return) || !is_array($return) || $return["type"] != "string") {
      return;
    }
    $position2 = $return["value"];
    $position1 += $return["strlen"];
    $bencoded .= $return["string"];
    if ($position1 >= $length) {
      return;
    }
    $return = bdecode(substr($metadata, $position1));
    if (!isset($return) || !is_array($return)) {
      return;
    }
    $value[$position2] = $return;
    $position1 += $return["strlen"];
    $bencoded .= $return["string"];
  }
  $bencoded .= "e";
  return array('type' => "dictionary", 'value' => $value, 'strlen' => strlen($bencoded), 'string' => $bencoded);
}

/**
 * A frinedlier interface for bdecode(). This function returns a cleaner structure modelled after the torrent specification.
 * 
 * @param $file
 *    The path to the .torrent file to decode
 * @return The structure containing all of the bencoded data. Example:
 *    array(
 *      'announce' => 'announce url',
 *      'creation date' => 'creation date',
 *      'info' => array(
 *        'files' => 'files list',
 *        'name' => 'torrent name',
 *        'piece length' => 'length of each piece',
 *        'pieces' => 'The piece hashes',
 *      ),
 *    )
 *
 * The advantage of this structure is that should it be run through bencode() the complete contents of the .torrent will be returned
 */    
function bdecode_torrent($file) {
  $handle = fopen($file, 'rb');
  if (!$handle) {
    return;
  }
  
  $metadata = fread($handle, filesize($file));
  fclose($handle);
  
  $bparsed = bdecode($metadata);
  
  // Clean up the output from bdecode()
  $bparsed = strip_excess($bparsed);
  
  return $bparsed;
}

/**
 * Strips the unnescessary values from the bdecode() result.
 * 
 * @param $bparsed
 *   The array of data returned by bdecode()
 * @return The modified structure without the 'value' field. Example:
 *    array(
 *      'announce' => 'announce url',
 *      'creation date' => 'creation date',
 *      'info' => array(
 *        'files' => 'files list',
 *      'name' => 'torrent name',
 *        'piece length' => 'length of each piece',
 *        'pieces' => 'The piece hashes',
 *      ),
 *    )
 */
function strip_excess($bparsed) {
  if (is_array($bparsed)) {
    foreach($bparsed as $key => $value) {
      if (is_array($value)) {
        if (array_key_exists('value', $value)) {
          $bparsed[$key] = strip_excess($value['value']);
        }
        else {
          $bparsed[$key] = strip_excess($value);
        }
      }
    }
  }
  
  if (is_array($bparsed) && array_key_exists('value', $bparsed)) {
    return $bparsed['value'];
  }
  else {
    return $bparsed;
  }
}

/**
 * Returns the information hash for the spcified structure.
 * 
 * @param $struct
 *   The structure to generate the hash from
 * @return
 *    The SHA1 of the information section
 */
function info_hash($struct) {
  return sha1(bencode($struct['info']), TRUE);
}

/**
 * Returns whether or not the specified structure is a valid torrent.
 *
 * @param $struct
 *   The structure that may or not be a torrent
 * @return TRUE if the specified structure meets minimum requirements to be a torrent
 */
function is_valid_torrent($struct) {
  if (is_array($struct)) {
    $base_required_keys = array('announce', 'creation date', 'info');
    foreach($base_required_keys as $key) {
      if (!array_key_exists($key, $struct)) {
        return FALSE;
      }
    }
    
    $info_required_keys = array('pieces', 'piece length');
    foreach($info_required_keys as $key) {
      if (!array_key_exists($key, $struct['info'])) {
        return FALSE;
      }
    }
    
    if (array_key_exists('files', $struct['info'])) {
      // Multi-file torrent
      $file_required_keys = array('path', 'length');
      foreach($struct['info']['files'] as $file) {
        foreach($file_required_keys as $key) {
          if (!array_key_exists($key, $file)) {
            return FALSE;
          }
        }
      }
    }
    else {
      // Single file torrent
      $single_required_keys = array('name', 'length');
      foreach($single_required_keys as $key) {
        if (!array_key_exists($key, $struct['info'])) {
          return FALSE;
        }
      }
    }
  }
  else {
    return FALSE;
  }
  
  return TRUE;
}

/**
 * Returns a specified value within the HTTP response
 *
 * @param $msg
 *   The message to be returned within the HTTP response
 */
function bencode_response_raw ($msg) {
  ob_end_clean();
  header('Content-Type: text/plain');
  header('Pragma: no-cache');
  print($msg);
}