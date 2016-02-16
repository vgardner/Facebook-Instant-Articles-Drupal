<?php
function facebook_instant_articles_rss_page() {
  module_load_include('inc', 'facebook_instant_articles', 'includes/FacebookInstantArticlesSimpleXMLElement');

  header('Content-Type: text/xml; charset=utf-8', true);

  $rss = new FacebookInstantArticlesSimpleXMLElement('<rss xmlns:content="http://purl.org/rss/1.0/modules/content/"></rss>');

  $rss->addAttribute('version', '2.0');
  $channel = $rss->addChild('channel');

  $title = $channel->addChild('title','Your title here...');
  $description = $channel->addChild('description','Your description here...');
  global $base_url;
  $link = $channel->addChild('link', $base_url);
  $language = $channel->addChild('language','en-gb');
  //Create RFC822 Date format to comply with RFC822
  $date_f = date("D, d M Y H:i:s T", time());
  $build_date = gmdate(DATE_RFC2822, strtotime($date_f));
  $lastBuildDate = $channel->addChild('lastBuildDate',$date_f);

  $nodes = node_load_multiple(array(), array('type' => 'article'));
  //exit(print_r($nodes));

  foreach ($nodes as $node) {
    $item = $channel->addChild('item');
    $item->addChild('title', $node->title);
    $item->addChild('link', url(drupal_get_path_alias("node/" . $node->nid), array('absolute' => TRUE)));
    $item->addChildCData('content', drupal_render(node_view($node)));
  }

  echo $rss->asXML();
}
