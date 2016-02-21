<?php
function facebook_instant_articles_layout_form(&$form, &$form_state) {

  // Get the entity_type, bundle and view mode.
  $entity_type = $form['#entity_type'];
  $bundle_name = $form['#bundle'];
  $view_mode = $form['#view_mode'];
  $form['#export_id'] = $entity_type . '|' . $bundle_name . '|' . $view_mode;

  $layout_options = array();

  $layout = facebook_instant_articles_get_node_layout_settings($bundle_name);

  $form['#fbia_layout'] = $layout;

    // Attach js.
  $form['#attached']['js'][] = drupal_get_path('module', 'facebook_instant_articles') . '/js/admin.js';

  // Add process function to add the regions.
  $form['#process'][] = 'facebook_instant_articles_add_regions';

  // Add submit handler. Layout needs be first so
  // we can reset the type key for Field API fields.
  $submit = $form['#submit'];
  $form['#submit'] = array('facebook_instant_articles_field_ui_layouts_save');
  $form['#submit'] = array_merge($form['#submit'], $submit);
}

function facebook_instant_articles_add_regions(&$form, &$form_state) {

  // Get the entity_type, bundle and view mode.
  $entity_type = $form['#entity_type'];
  $bundle = $form['#bundle'];
  $view_mode = $form['#view_mode'];

  // Check layout.
  $layout = isset($form['#fbia_layout']) ? $form['#fbia_layout'] : FALSE;
  //exit(print_r($layout));
  $table = &$form['fields'];

  $table['#header'] = array(
    t('Field'),
    t('Weight'),
    t('Parent'),
    t('Region'),
    t('Label'),
    array('data' => t('Format'), 'colspan' => 3),
  );

  $regions = array(
    'header' => t('Header'),
    'body' => t('Body'),
    'footer' => t('Footer')
  );

  $table['#regions'] = array();
  foreach ($regions as $region_key => $region_title) {
    $region_options[$region_key] = $region_title;
    $table['#regions'][$region_key] = array(
      'title' => $region_title,
      'message' => t('No fields are displayed in this region'),
    );
  }

  // Let other modules alter the regions.
  $context = array(
    'entity_type' => $entity_type,
    'bundle' => $bundle,
    'view_mode' => $view_mode
  );
  $region_info = array(
    'region_options' => &$region_options,
    'table_regions' => &$table['#regions'],
  );
  drupal_alter('facebook_instant_articles_layout_region', $context, $region_info);

  $region_options['hidden'] = $view_mode != 'form' ? t('Disabled') : t('Hidden');
  $table['#regions']['hidden'] = array(
    'title' => $view_mode != 'form' ? t('Disabled') : t('Hidden'),
    'message' => t('No fields are hidden.')
  );

  $region = array(
    '#type' => 'select',
    '#options' => $region_options,
    '#default_value' => 'hidden',
    '#attributes' => array(
      'class' => array('ds-field-region'),
    )
  );

  $limit_items = array(
    '#type' => 'textfield',
    '#size' => 2,
    '#default_value' => '',
    '#weight' => 10,
    '#default_value' => '#',
    '#prefix' => '<div class="limit-float">',
    '#suffix' => '</div><div class="clearfix"></div>',
    '#attributes' => array(
      'alt' => t('Enter a number to limit the number of items or \'delta\' to print a specific delta (usually configured in views or found in entity->ds_delta). Leave empty to display them all. Note that depending on the formatter settings, this option might not always work.'),
      'title' => t('Enter a number to limit the number of items or \'delta\' to print a specific delta (usually configured in views or found in entity->ds_delta). Leave empty to display them all. Note that depending on the formatter settings, this option might not always work.'),
    ),
  );
  //exit(print_r($table));
   // Update existing rows by changing rowHandler and adding regions.
  foreach (element_children($table) as $name) {
    $row = &$table[$name];
    $row['#js_settings'] = array('rowHandler' => 'facebookInstantArticles');
    $row['#region_callback'] = 'facebook_instant_articles_field_ui_row_region';

    // Remove hidden format.
    if (isset($row['format']['type']['#options']['hidden'])) {
      unset($row['format']['type']['#options']['hidden']);
    }

    // Add label class.
    if (isset($row['label'])) {
      if (isset($form_state['formatter_settings']) && isset($form_state['formatter_settings'][$name]['ft'])) {
        if (!empty($form_state['formatter_settings'][$name]['ft']['lb'])) {
          $row['human_name']['#markup'] = check_plain($form_state['formatter_settings'][$name]['ft']['lb']) . ' ' . t('(Original: !orig)', array('!orig' => $row['human_name']['#markup']));
        }
      }
    }

    // Limit items.
      $field_info = field_info_field($name);
      if (isset($field_info['cardinality']) && $field_info['cardinality'] != 1 && $view_mode != 'form') {
        $row['format']['type']['#prefix'] = '<div class="limit-float">';
        $row['format']['type']['#suffix'] = '</div>';
        $row['format']['limit'] = $limit_items;
        $row['format']['limit']['#default_value'] = (isset($layout->settings['limit']) && isset($layout->settings['limit'][$name])) ? $layout->settings['limit'][$name] : '#';
      }

      // Add region.
      $split = ($view_mode != 'form') ? 7 : 6;
      if ($row['#row_type'] == 'group' && $view_mode == 'form') {
        $split = ($view_mode != 'form') ? 8 : 7;
      }

      $second = array_splice($row, $split);
      $row['region'] = $region;
      $row['region']['#default_value'] = (isset($layout->settings['fields'][$name]) && isset($region_options[$layout->settings['fields'][$name]])) ? $layout->settings['fields'][$name] : 'hidden';

      $row = array_merge($row, $second);
    }
  return $form;
}

/**
 * Returns the region to which a row in the Field UI screen belongs.
 *
 * @param $row
 *   The current row that is being rendered in the Field UI screen.
 */
function facebook_instant_articles_field_ui_row_region($row) {
  return isset($row['region']['#value']) ? $row['region']['#value'] : 'hidden';
}

function facebook_instant_articles_field_ui_layouts_save($form, &$form_state) {
  $weight = 0;

  // Get default values.
  $entity_type = $form['#entity_type'];
  $bundle = $form['#bundle'];
  $view_mode = $form['#view_mode'];

  // Determine layout variables.
  $layout = 'facebook_instant_article';
  $old_layout = 'facebook_instant_article';
  $new_layout = ($layout != $old_layout) || empty($old_layout);

  // Save layout and add regions if necessary.
  $record = new stdClass;
  $record->id = $form['#export_id'];
  $record->entity_type = $entity_type;
  $record->bundle = $bundle;
  $record->view_mode = $view_mode;
  $record->layout = $layout;
  $record->settings = array();

  $form_state['layout_saved'] = FALSE;


  if (!empty($layout)) {

    $form_state['layout_saved'] = TRUE;

    $fields = facebook_instant_articles_sort_fields($form_state['values']['fields'], 'weight');

    foreach ($fields as $key => $field) {

      // Make sure we need to save anything for this field.
      if (facebook_instant_articles_field_valid($key, $field, $form_state, $view_mode)) {
        continue;
      }

      if (!isset($record->settings['regions'][$field['region']])) {
        $record->settings['regions'][$field['region']] = array();
      }
      $record->settings['regions'][$field['region']][$weight++] = $key;
      $record->settings['fields'][$key] = $field['region'];

      // Save limit.
      $limit = isset($field['format']['limit']) ? trim($field['format']['limit']) : '';
      if (is_numeric($limit) || $limit === 'delta') {
        $record->settings['limit'][$key] = $limit;
      }
    }

    // Let other modules alter the layout settings.
    drupal_alter('facebook_instant_articles_layout_settings', $record, $form_state);

    $layout = $form['#fbia_layout'];
    if (!isset($layout->export_type)) {
      drupal_write_record('facebook_instant_articles_layout_settings', $record);
    }
    else {
      drupal_write_record('facebook_instant_articles_layout_settings', $record, array('id'));
    }

    // Clear entity info cache.
    cache_clear_all('entity_info', 'cache', TRUE);
  }
}

/**
 * Get Display Suite layouts.
 */
function facebook_instant_articles_get_layout_info() {
  return array('facebook_instant_article' => array(
            'label' => t('Facebook Instant Article'),
            'path' => "",
            'regions' => array(
              'header' => t('Header'),
              'body' => t('Body'),
              'footer' => t('Footer')
            ),
            'image' => 1
          )
        );
}

/**
 * Utility function to sort a multidimensional array by a value in a sub-array.
 *
 * @param $a
 *   The array to sort.
 * @param $subkey
 *   The subkey to sort by.
 */
function facebook_instant_articles_sort_fields($a, $subkey) {
  foreach ($a as $k => $v) {
    if (isset($v[$subkey])) {
      $b[$k] = $v[$subkey];
    }
  }
  asort($b);
  foreach ($b as $key => $val) {
    $c[$key] = $a[$key];
  }
  return $c;
}

/**
 * Utility function to check if we need to save anything for this field.
 */
function facebook_instant_articles_field_valid($key, $field, &$form_state, $view_mode = 'default') {
  $continue = FALSE;

  // Ignore the Field group module and the region to block plugin.
  if ($key == '_add_new_group' || $key == '_add_new_field' || $key == '_add_new_block_region') {
    $continue = TRUE;
  }

  // If the field is in hidden region, do not save. Check if the
  // field has a type key which means it's from Field API and
  // we need to reset that type to 'hidden' so it doesn't get
  // fired by Field API in the frontend.
  if (isset($field['region']) && $field['region'] == 'hidden') {
    if (isset($field['type']) && $view_mode != 'form') {
      $form_state['values']['fields'][$key]['type'] = 'hidden';
    }

    // In case of a form, fields will be set with #access to FALSE.
    if ($view_mode != 'form') {
      $continue = TRUE;
    }
  }

  return $continue;
}
