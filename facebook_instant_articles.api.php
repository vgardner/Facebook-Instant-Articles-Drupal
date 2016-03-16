<?php

/**
 * @file
 * Hooks provided by the Taxonomy module.
 */

/**
 * Provide node ids for the feeds.
 *
 * @param $content_type
 *   The content type for query.
 */
function hook_fb_instant_articles_node_ids($content_type) {

    $node_ids = array();

    // Your own logic to get node ids.

    return $node_ids;
}