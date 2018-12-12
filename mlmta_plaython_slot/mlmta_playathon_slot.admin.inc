<?php

/**
 * Generates the mlmta_playathon_slot type editing form.
 */
function mlmta_playathon_slot_type_form($form, &$form_state, $mlmta_playathon_slot_type, $op = 'edit') {

  if ($op == 'clone') {
    $mlmta_playathon_slot_type->label .= ' (cloned)';
    $mlmta_playathon_slot_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $mlmta_playathon_slot_type->label,
    '#description' => t('The human-readable name of this mlmta_playathon_slot type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($mlmta_playathon_slot_type->type) ? $mlmta_playathon_slot_type->type : '',
    '#maxlength' => 32,
    '#disabled' => $mlmta_playathon_slot_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'mlmta_playathon_slot_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this mlmta_playathon_slot type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#default_value' => isset($mlmta_playathon_slot_type->description) ? $mlmta_playathon_slot_type->description : '',
    '#description' => t('Description about the mlmta_playathon_slot type.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save mlmta_playathon_slot type'),
    '#weight' => 40,
  );

  if (!$mlmta_playathon_slot_type->isLocked() && $op != 'add' && $op != 'clone') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete mlmta_playathon_slot type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('mlmta_playathon_slot_type_form_submit_delete')
    );
  }
  return $form;
}

/**
 * Submit handler for creating/editing mlmta_playathon_slot_type.
 */
function mlmta_playathon_slot_type_form_submit(&$form, &$form_state) {
  $mlmta_playathon_slot_type = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  $op = mlmta_playathon_slot_type_save($mlmta_playathon_slot_type);
  if ($op == SAVED_NEW) {
    // Notify Field API of new bundle
    field_attach_create_bundle('mlmta_playathon_slot', $mlmta_playathon_slot_type->type);
    
    // Add default fields to piece bundle
    mlmta_playathon_slot_add_default_fields($mlmta_playathon_slot_type);
  }

  // Redirect user back to list of mlmta_playathon_slot types.
  $form_state['redirect'] = 'admin/structure/scheduling/mlmta-playathon-slot-types';
}

function mlmta_playathon_slot_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/scheduling/mlmta-playathon-slot-types/' . $form_state['mlmta_playathon_slot_type']->type . '/delete';
}

/**
 * Student type delete form.
 */
function mlmta_playathon_slot_type_form_delete_confirm($form, &$form_state, $mlmta_playathon_slot_type) {
  $form_state['mlmta_playathon_slot_type'] = $mlmta_playathon_slot_type;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['mlmta_playathon_slot_type_id'] = array('#type' => 'value', '#value' => entity_id('mlmta_playathon_slot_type' ,$mlmta_playathon_slot_type));
  return confirm_form($form,
    t('Are you sure you want to delete mlmta_playathon_slot type %title?', array('%title' => entity_label('mlmta_playathon_slot_type', $mlmta_playathon_slot_type))),
    'mlmta-playathon-slot/' . entity_id('mlmta_playathon_slot_type' ,$mlmta_playathon_slot_type),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Student type delete form submit handler.
 */
function mlmta_playathon_slot_type_form_delete_confirm_submit($form, &$form_state) {
  $mlmta_playathon_slot_type = $form_state['mlmta_playathon_slot_type'];
  mlmta_playathon_slot_type_delete($mlmta_playathon_slot_type);

  watchdog('mlmta_playathon_slot_type', '@type: deleted %title.', array('@type' => $mlmta_playathon_slot_type->type, '%title' => $mlmta_playathon_slot_type->label));
  drupal_set_message(t('@type %title has been deleted.', array('@type' => $mlmta_playathon_slot_type->type, '%title' => $mlmta_playathon_slot_type->label)));

  $form_state['redirect'] = 'admin/structure/scheduling/mlmta-playathon-slot-types';
}

/**
 * Page to select mlmta_playathon_slot Type to add new mlmta_playathon_slot.
 */
function mlmta_playathon_slot_admin_add_page() {
  $items = array();
  foreach (mlmta_playathon_slot_types() as $mlmta_playathon_slot_type_key => $mlmta_playathon_slot_type) {
    $items[] = l(entity_label('mlmta_playathon_slot_type', $mlmta_playathon_slot_type), 'mlmta-playathon-slot/add/' . $mlmta_playathon_slot_type_key);
  }
  return array('list' => array('#theme' => 'item_list', '#items' => $items, '#title' => t('Select type of mlmta_playathon_slot to create.')));
}

/**
 * Add new mlmta_playathon_slot page callback.
 */
function mlmta_playathon_slot_add($type) {
  $mlmta_playathon_slot_type = mlmta_playathon_slot_types($type);

  $mlmta_playathon_slot = entity_create('mlmta_playathon_slot', array('type' => $type));
  drupal_set_title(t('Create @name', array('@name' => entity_label('mlmta_playathon_slot_type', $mlmta_playathon_slot_type))));

  $output = drupal_get_form('mlmta_playathon_slot_form', $mlmta_playathon_slot);

  return $output;
}

/**
 * Student Form.
 *
 */
function mlmta_playathon_slot_form($form, &$form_state, $mlmta_playathon_slot) {
  $form_state['mlmta_playathon_slot'] = $mlmta_playathon_slot;
  // Don't need this as this is filled in with first and last name on presave
  //$form['name'] = array(
  //  '#type' => 'textfield',
  //  '#required' => TRUE,
  //  '#title' => t('Name of the mlmta_playathon_slot'),
  //  '#default_value' => $mlmta_playathon_slot->name,
  //);

  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $mlmta_playathon_slot->uid,
  );
  
  field_attach_form('mlmta_playathon_slot', $mlmta_playathon_slot, $form, $form_state);
  
  
  $submit = array();
  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions'] = array(
    '#weight' => 100,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save mlmta_playathon_slot'),
    '#submit' => $submit + array('mlmta_playathon_slot_form_submit'),
  );

  // Show Delete button if we edit mlmta_playathon_slot.
  $mlmta_playathon_slot_id = entity_id('mlmta_playathon_slot' ,$mlmta_playathon_slot);
  if (!empty($mlmta_playathon_slot_id) && mlmta_playathon_slot_access('edit', $mlmta_playathon_slot)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('mlmta_playathon_slot_form_submit_delete'),
    );
  }

  $form['#validate'][] = 'mlmta_playathon_slot_form_validate';

  return $form;
}

function mlmta_playathon_slot_form_validate($form, &$form_state) {

}

/**
 * Student submit handler.
 */
function mlmta_playathon_slot_form_submit($form, &$form_state) {

  $mlmta_playathon_slot = $form_state['mlmta_playathon_slot'];
  
  entity_form_submit_build_entity('mlmta_playathon_slot', $mlmta_playathon_slot, $form, $form_state);
  
  mlmta_playathon_slot_save($mlmta_playathon_slot);

  $mlmta_playathon_slot_uri = entity_uri('mlmta_playathon_slot', $mlmta_playathon_slot);

  $form_state['redirect'] = $mlmta_playathon_slot_uri['path'];

  drupal_set_message(t('PlayathonSlot %title saved.', array('%title' => entity_label('mlmta_playathon_slot', $mlmta_playathon_slot))));
  
}

function mlmta_playathon_slot_form_submit_delete($form, &$form_state) {
  $mlmta_playathon_slot = $form_state['mlmta_playathon_slot'];
  $mlmta_playathon_slot_uri = entity_uri('mlmta_playathon_slot', $mlmta_playathon_slot);
  $form_state['redirect'] = array($mlmta_playathon_slot_uri['path'] . '/delete', array('query'=>drupal_get_destination()));
  
  // See https://drupal.stackexchange.com/questions/5440/form-redirect-not-working-if-destination-is-in-url
  unset($_GET['destination']);
  drupal_static_reset('drupal_get_destination');
  drupal_get_destination();
}

/**
 * Delete confirmation form.
 */
function mlmta_playathon_slot_delete_form($form, &$form_state, $mlmta_playathon_slot) {
  $form_state['mlmta_playathon_slot'] = $mlmta_playathon_slot;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['mlmta_playathon_slot_type_id'] = array('#type' => 'value', '#value' => entity_id('mlmta_playathon_slot' ,$mlmta_playathon_slot));
  $mlmta_playathon_slot_uri = entity_uri('mlmta_playathon_slot', $mlmta_playathon_slot);
  return confirm_form($form,
    t('Are you sure you want to delete PlayathonSlot %title?', array('%title' => entity_label('mlmta_playathon_slot', $mlmta_playathon_slot))),
    $mlmta_playathon_slot_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete form submit handler.
 */
function mlmta_playathon_slot_delete_form_submit($form, &$form_state) {
  $mlmta_playathon_slot = $form_state['mlmta_playathon_slot'];
  mlmta_playathon_slot_delete($mlmta_playathon_slot);

  drupal_set_message(t('PlayathonSlot %title deleted.', array('%title' => entity_label('mlmta_playathon_slot', $mlmta_playathon_slot))));

  $form_state['redirect'] = '<front>';
}
