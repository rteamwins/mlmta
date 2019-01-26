<?php

/**
 * @file
 * Checkout pane callback functions 
 */


/**
 *  pane: settings form callback.
 */
function mlmta_cmf_teacher_duties_pane_settings_form($checkout_pane) {

}

/**
 * pane: form callback.
 */
function mlmta_cmf_teacher_duties_pane_checkout_form($form, &$form_state, $checkout_pane, $order) {
  global $user;

  $pane_form = array();

  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $pane_form['teacher_duties_choice'] = array(
    '#type' => 'select',
    '#title' => t('Teacher duties time choice'),
    '#required' => TRUE,
    '#options' => drupal_map_assoc(array(
      'AM',
      'PM',
      'All day',
    )),
    '#description' => t('All teachers participating in CMF are required to help during the festival. Please choose your preference.'),
    '#default_value' =>  $order_wrapper->field_teacher_duties_choice->value(),
  );
  return $pane_form;
}

/**
 *  pane: validation callback.
 */
function mlmta_cmf_teacher_duties_pane_checkout_form_validate($form, &$form_state, $checkout_pane, $order) {
  return TRUE;
}

/**
 *  pane: checkout form submission callback.
 */
function mlmta_cmf_teacher_duties_pane_checkout_form_submit($form, &$form_state, $checkout_pane, $order) {
  if (!empty($form_state['values'][$checkout_pane['pane_id']])) {
    $pane_values = $form_state['values'][$checkout_pane['pane_id']];
    if (!empty($pane_values['teacher_duties_choice'])) {
      $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
      $order_wrapper->field_teacher_duties_choice = $pane_values['teacher_duties_choice'];
    }
  }
}

/**
 * Account pane: returns the username and e-mail for the Review checkout pane.
 */
function mlmta_cmf_teacher_duties_pane_review($form, $form_state, $checkout_pane, $order) {
  $content = array();
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $teacher_duties_choice = $order_wrapper->field_teacher_duties_choice->value();

  $content[] = array(
    '#type' => 'item',
    '#title' => t('Teacher duties time choice'),
    '#markup' => check_plain($teacher_duties_choice),
  );
  return drupal_render($content);
}
