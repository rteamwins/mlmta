<?php

function mlmta_supersat_playathon_form($form, &$form_state, $pursuit = NULL) {
  global $user;
  $current_user = user_load($user->uid);
  if (empty($form_state['build_info']['files'])) {
    form_load_include($form_state, 'inc', 'mlmta_supersat_forms', 'mlmta_supersat_playathon');  
  }
  if (!$pursuit) {
    $pursuit = entity_create('pursuit', array('type'=>'playathon'));
  }
  
  $pursuit_wrapper = entity_metadata_wrapper('pursuit', $pursuit);
  $form_state['pursuit'] = $pursuit;
  $form_state['original_pursuit'] = clone $pursuit;

  $form = array();

  $form['teacher'] = array(
    '#type' => 'select',
    '#title' => t('Teacher'),
    '#options' => _mlmta_teacher_build_list('basic_teacher'),
    '#required'=>TRUE,
    '#weight' => 0,
  );
  
  if (!empty($pursuit_wrapper->pursuit_teacher_reference->raw())) {
    $default_value = $pursuit_wrapper->pursuit_teacher_reference->raw();
  } else {
    $default_value = !empty ($current_user->field_user_teacher_reference['und'][0]['target_id']) ?
        $current_user->field_user_teacher_reference['und'][0]['target_id'] :
        NULL;
  }
  $form['teacher']['#default_value'] = $default_value;
  
  $student_ids = $pursuit_wrapper->pursuit_student_reference->raw();
  $form['student'] = array(
    '#type' => 'select',
    '#title' => t('Students'),
    '#required' => TRUE,
    '#description' => t('Click the field above or type the name to add your students one at a time. If you don\'t see the student you want, click on "My students" in the main menu bar to create them.'),
    '#options' => _mlmta_student_build_list('basic_student'),
    '#empty_option' => '- Select -',
    '#multiple' => TRUE,
    '#weight' => 30,
    '#chosen' => TRUE,
    '#default_value' => $student_ids,
  );

  
  $form['time_warning'] = array(
    '#markup' => '<b>ATTENTION:</b> If any of the students for this Play-a-Thon is also registered for KITS, you <span style="color:red;">MUST AVOID</span> time slots between 1pm and 4pm.',
    '#weight' => 31,
  );
  
  $form_temp = array();
  // Attach element for picking slot
  field_attach_form('pursuit', $pursuit, $form_temp, $form_state, NULL, array("field_name" => 'field_playathon_slots'));
  $form['field_playathon_slots'] = $form_temp['field_playathon_slots'];
  $form['field_playathon_slots']['#weight'] = 32;
  $form['field_playathon_slots']['und']['#required'] = TRUE;
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => isset($pursuit->is_new) ? 'Add to cart' : 'Save',
    '#weight' => 99,
  );

  return $form;
  
}

/**
 * Validation handler
 */
function mlmta_supersat_playathon_form_validate($form, $form_state) {
  
  $pursuit = $form_state['pursuit'];
  $is_new = isset($pursuit->is_new) ? $pursuit->is_new : FALSE;
  // Check if time slots are still available
  $slot_ids  = array();
  foreach ($form_state['values']['field_playathon_slots']['und'] as $item) {
    $slot_ids[] = $item['target_id'];
  }
  
  // Check if slots are taken
  $q = db_select('mlmta_playathon_slot', 'slot');
  $q->condition('slot.mlmta_playathon_slot_id', $slot_ids, 'IN');
  $q->fields('slot', array('mlmta_playathon_slot_id'));
  $q->join('field_data_playathon_pursuit_reference', 'slot_pursuit', 'slot_pursuit.entity_id = slot.mlmta_playathon_slot_id');
  $q->fields('slot_pursuit', array('playathon_pursuit_reference_target_id')); // Need to check if they are really taken
  $q->join('pursuit', 'pursuit', 'pursuit.pursuit_id = slot_pursuit.playathon_pursuit_reference_target_id');

  $r = $q->execute();
  $rows = $r->fetchAllAssoc('mlmta_playathon_slot_id');
  if (!empty($rows)) {
    $ids = array_keys($rows);
    $slots = mlmta_playathon_slot_load_multiple($ids);
    foreach ($slots as $slot) {
      drupal_set_message(t('Slot %slot is no longer available. Please choose a different slot.', array('%slot' => $slot->name)), 'warning');
    }
    form_set_error('field_playathon_slot', 'Slot error.');
  }
}

/**
 * Submit handler
 */
function mlmta_supersat_playathon_form_submit($form, &$form_state) {
  $pursuit = $form_state['pursuit'];
  $is_new = isset($pursuit->is_new) ? $pursuit->is_new : FALSE; 
  $pursuit_wrapper = entity_metadata_wrapper('pursuit', $pursuit);
  $slot_ids  = array();
  foreach ($form_state['values']['field_playathon_slots']['und'] as $item) {
    $slot_ids[] = $item['target_id'];
  }
  $student_ids = array_keys($form_state['values']['student']);
  $teacher_id = $form_state['values']['teacher'];
  
  $pursuit_wrapper->pursuit_student_reference->set($student_ids);
  $pursuit_wrapper->pursuit_teacher_reference->set(array($teacher_id));
  $pursuit_wrapper->field_event_reference->set(variable_get('tenuto_supersat_event', NULL));
  $pursuit_wrapper->field_playathon_slots->set($slot_ids);
  pursuit_update_name($pursuit);
  pursuit_save($pursuit);
  if ($is_new) {
    pursuit_add_to_shopping_cart(array($pursuit), 'playathon'); 
  }
  
  // Clean up any deselected slot when editing
  if (!$is_new) {
    $original_pursuit = $form_state['original_pursuit'];
    $original_pursuit_wrapper = entity_metadata_wrapper('pursuit', $original_pursuit);
    $original_slot_ids = $original_pursuit_wrapper->field_playathon_slots->raw();
    $deselected_slot_ids = array_diff($original_slot_ids, $slot_ids);
    if (!empty($deselected_slot_ids)) {
    $deselected_slots = mlmta_playathon_slot_load_multiple($deselected_slot_ids);
      foreach ($deselected_slots as $deselected_slot) {
        $slot_wrapper = entity_metadata_wrapper('mlmta_playathon_slot', $deselected_slot);
        $slot_wrapper->playathon_pursuit_reference = NULL;
        $slot_wrapper->save();
      }    
    }    
  }

  //
  // Update selected slots to point to  pursuit
  $pursuit_id = $pursuit->pursuit_id;
  $slots = mlmta_playathon_slot_load_multiple($slot_ids);
  foreach ($slots as $slot) {
    $slot_wrapper = entity_metadata_wrapper('mlmta_playathon_slot', $slot);
    $slot_wrapper->playathon_pursuit_reference = $pursuit->pursuit_id;
    $slot_wrapper->save();
  }
  $form_state['redirect'] = 'supersat-forms';
  
}