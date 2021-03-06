<?php
/**
 * @file
 * Functions to generate the RSS feed.
 */

/**
 * Callback for the RSS page menu item.
 */
function fb_instant_articles_rss_page() {
  module_load_include('inc', 'fb_instant_articles', 'includes/FacebookInstantArticlesSimpleXMLElement');

  header('Content-Type: text/xml; charset=utf-8', TRUE);

  $rss = new FacebookInstantArticlesSimpleXMLElement('<rss xmlns:content="http://purl.org/rss/1.0/modules/content/"></rss>');

  $rss->addAttribute('version', '2.0');
  $channel = $rss->addChild('channel');

  // Add Channel information;
  $channel->addChild('title', variable_get('fb_instant_articles_channel_title', variable_get('site_name', 'Default site name')));
  $channel->addChild('description', variable_get('fb_instant_articles_channel_description', variable_get('site_slogan', t('Default site slogan.'))));
  global $base_url;
  $channel->addChild('link', $base_url);
  $channel->addChild('language', variable_get('fb_instant_articles_channel_language', 'en-gb'));

  // Create ISO 8601 formatted date.
  $date_time = new DateTime('NOW');
  $channel->addChild('lastBuildDate', $date_time->format(DateTime::ATOM));

  // Create RSS items.
  $nodes = fb_instant_articles_get_article_nodes();
  foreach ($nodes as $node) {
    $item = $channel->addChild('item');
    $item->addChild('title', $node->title);
    $item->addChild('link', url(drupal_get_path_alias("node/" . $node->nid), array('absolute' => TRUE)));

    $node_view = node_view($node, 'fb_instant_article');
    $item->addChildCdata('content', drupal_render($node_view));

    // Add published date.
    $ts = $node->created;
    $date = new DateTime("@$ts");
    $item->addChild('pubDate', $date->format(DateTime::ATOM));
  }

  echo $rss->asXML();
}
