<?php
// $Id: feedapi_mapper_taxonomy.inc,v 1.1.2.1 2009/01/05 19:06:46 alexb Exp $
/**
 * On behalf implementation of feed element mapper for taxonomy.module.
 */

/**
 * Implementation of hook_feedapi_mapper().
 * Maps feed elements to taxonomy values.
 * @param $op
 *   'list' or 'map'
 * @param $node
 *   Feed item node to map on.
 * @param $field_name
 *   Name of field to map to.
 * @param $feed_element
 *   Feed item element to map from. Parameter only present on $op = 'map'
 * @param @sub_field
 *   Subfield on field to map to. Parameter only present on $op = 'map'.
 *   This parameter will depend on if the hook implementation returns a subfield on 
 *   $op = 'list'.
 * 
 * Return key => name array of sub fields on 'list'.
 * If only one field and no subfields are available for this content type return TRUE.
 * If no fields available for this content type return FALSE.
 * Options are necessary because a field like 
 * e. g. "taxonomy" might have more than one slot for information - e. g. vocabularies.
 * 
 * Todo: $node could be passed by reference - implementers wouldn't need to return node 
 * (PHP5 passes in by reference by default).
 */
function taxonomy_feedapi_mapper($op, $node, $field_name, $feed_element = array(), $sub_field = '') {
  if ($field_name == 'taxonomy') {
    if ($op == 'describe') {
      return t('Maps a string or an array of strings to taxonomy terms. Chose a vocabulary from sub options.');
    }
    else if ($op == 'list') {
      if ($vocabularies = taxonomy_get_vocabularies($node->type)) {
        foreach ($vocabularies as $v) {
          $sub_fields[$v->vid] = $v->name;
        }
        return $sub_fields;
      }
      return FALSE;
    }
    else if ($op == 'map') {
      // Todo: some plausibility check of $feed_element
      // Todo: security check of $feed_element
      if (is_string($feed_element)) {
        $feed_element = array($feed_element);
      }
      if (is_array($feed_element)) {
        if (!is_array($node->taxonomy)) {
          $node->taxonomy = array();
        }
        $node->taxonomy = array_merge($node->taxonomy, _feedapi_mapper_create_terms($feed_element, $sub_field));
      }
      return $node;
    }
  }
}

/**
 * This helper function creates terms from keywords.
 */
function _feedapi_mapper_create_terms($keywords, $vid) {
  $terms = array();
  if (is_array($keywords) && !empty($vid)) {
    foreach ($keywords as $term) {
      $curr_terms = taxonomy_get_term_by_name($term);
      $found = FALSE;
      if (is_array($curr_terms)) {
        foreach ($curr_terms as $curr_term) {
          if ($curr_term->vid == $vid) {
            $terms[$curr_term->tid] = $curr_term;
            $found = TRUE;
          }
        }
      }
      // Create a new term if there isn't a matching existing one.
      if (!$found) {
        $new_term = array();
        $new_term['name'] = $term;
        $new_term['vid'] = $vid;
        taxonomy_save_term($new_term);
        $terms[$new_term['tid']] = taxonomy_get_term($new_term['tid']);
      }
    }
  }
  return $terms;
}
