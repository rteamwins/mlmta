<?php

/**
 * @file
 * Checkout pane callback functions 
 */


/**
 *  pane: settings form callback.
 */
function mlmta_dsf_certificate_pickup_pane_settings_form($checkout_pane) {

}

/**
 * pane: form callback.
 */
function mlmta_dsf_certificate_pickup_pane_checkout_form($form, &$form_state, $checkout_pane, $order) {
  global $user;

  $pane_form = array();

  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $pane_form['pickup_options'] = array(
    '#type' => 'radios',
    '#title' => t('Receipt of certificates and medals'),
    '#required' => TRUE,
    '#options' => array(
      'pickup'=> 'I will arrange to pick them up from the chairperson James Anemone at his studio in Exton',
      'delivery' => 'Have them delivered by mail to my billing address'
    ),
    '#description' => t('Please choose your preference for the receipt of certificates and medals.'),
    '#default_value' =>  $order_wrapper->field_receipt_of_certificates_an->value(),
  );
  return $pane_form;
}

/**
 *  pane: validation callback.
 */
function mlmta_dsf_certificate_pickup_pane_checkout_form_validate($form, &$form_state, $checkout_pane, $order) {
  return TRUE;
}

/**
 *  pane: checkout form submission callback.
 */
function mlmta_dsf_certificate_pickup_pane_checkout_form_submit($form, &$form_state, $checkout_pane, $order) {
  if (!empty($form_state['values'][$checkout_pane['pane_id']])) {
    $pane_values = $form_state['values'][$checkout_pane['pane_id']];
    if (!empty($pane_values['pickup_options'])) {
      $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
      $order_wrapper->field_receipt_of_certificates_an = $pane_values['pickup_options'];
    }
  }
}

/**
 * Account pane: returns the username and e-mail for the Review checkout pane.
 */
function mlmta_dsf_certificate_pickup_pane_review($form, $form_state, $checkout_pane, $order) {
  $content = array();
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $thechoice = $order_wrapper->field_receipt_of_certificates_an->label();

  $content[] = array(
    '#type' => 'item',
    '#title' => '',
    '#markup' => check_plain($thechoice),
  );
  return drupal_render($content);
}
