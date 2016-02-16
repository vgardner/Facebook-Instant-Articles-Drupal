<?php
/**
 * @file
 * Provides the Bazaarvoice administrative interface.
 */

/**
 * Admin settings form.
 */
function facebook_instant_articles_admin_settings_form() {
  $form['account'] = array(
    '#type' => 'fieldset',
    '#title' => t('Account Settings'),
    '#description' => t('Bazaarvoice account settings'),
    '#collapsible' => TRUE,
  );

  $form['account']['bazaarvoice_mode'] = array(
    '#type' => 'select',
    '#title' => t('Account Mode'),
    '#description' => t('Mode to use for connecting to Bazaarvoice. Use staging for pre-production development and testing.'),
    '#options' => array(
      'stg' => t('Staging'),
      'prod' => t('Production'),
    ),
    '#default_value' => variable_get('bazaarvoice_mode', 'stg'),
  );

  $form = system_settings_form($form);

  return $form;
}
