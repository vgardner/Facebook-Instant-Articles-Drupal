<?php
function facebook_instant_articles_rss_page() {
  module_load_include('inc', 'facebook_instant_articles', 'includes/FacebookInstantArticlesSimpleXMLElement');

  header('Content-Type: text/xml; charset=utf-8', true);

  $rss = new FacebookInstantArticlesSimpleXMLElement('<rss xmlns:content="http://purl.org/rss/1.0/modules/content/"></rss>');

  $rss->addAttribute('version', '2.0');
  $channel = $rss->addChild('channel');

  // Add Channel information;
  $title = $channel->addChild('title','Your title here...');
  $description = $channel->addChild('description','Your description here...');
  global $base_url;
  $link = $channel->addChild('link', $base_url);
  $language = $channel->addChild('language','en-gb');

  // Create ISO 8601 formatted date.
  $date_time = new DateTime('NOW');
  $lastBuildDate = $channel->addChild('lastBuildDate', $date_time->format(DateTime::ATOM));

  // Create RSS items.
  $nodes = facebook_instant_articles_get_article_nodes();
  foreach ($nodes as $node) {
    $item = $channel->addChild('item');
    $item->addChild('title', $node->title);
    $item->addChild('link', url(drupal_get_path_alias("node/" . $node->nid), array('absolute' => TRUE)));
    //$new_node = node_build_content($node);
    //exit($new_node);
    //exit(print_r(node_view($node, 'rss')));
    $item->addChildCData('content', drupal_render(node_view($node, 'facebook_instant_article')));
    // Add published date.
    $date_time = new DateTime();
    $date_time->setTimestamp($node->created);
    $item->addChild('pubDate', $date_time->format(DateTime::ATOM));
  }

  echo $rss->asXML();
}
