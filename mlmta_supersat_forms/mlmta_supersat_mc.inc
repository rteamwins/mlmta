<?php

function mlmta_supersat_mc_form($form, &$form_state, $pursuit = NULL) {
  global $user;
  $current_user = user_load($user->uid);
  if (empty($form_state['build_info']['files'])) {
    form_load_include($form_state, 'inc', 'mlmta_supersat_forms', 'mlmta_supersat_mc');  
  }
  $is_new = FALSE;
  if (!$pursuit) {
    $pursuit = entity_create('pursuit', array('type'=>'mc'));
    $is_new = TRUE;
  }
  
  $pursuit_wrapper = entity_metadata_wrapper('pursuit', $pursuit);
  $form_state['pursuit'] = $pursuit;
  $form_state['original_pursuit'] = clone $pursuit;

  $form = array();

  $student_id = $pursuit_wrapper->pursuit_student_reference->raw();
  if (!empty($form_state['triggering_element']['#name']) &&
    $form_state['triggering_element']['#name'] == 'student') {
    // Form rebuilt triggered by ajax call
    // Triggered by the student select ajax
    // Selected an existing student
    $student_id = (array) $form_state['values']['student'];
  }
  
  $form['notice'] = array(
    '#markup' => '<p><strong>Please notice:</strong> A maximum of 4 students per teacher can be registered through this form. Any attempt to register additional more than 4 will be rejected by the system. Please contact the chair if you wish to get permission to register more than four students.</p>'
  );
  
  $form['student_1']= array(
    '#type' => 'fieldset',
    '#title' => t('Student'),
  );

  $form['student_1']['student'] = array(
    '#type' => 'select',
    '#title' => t('Student'),
    '#required' => TRUE,
    '#description' => t('Click -Select- and find your student. If you don\'t see your student here, click on "My students" in the main menu bar and create a student.'),
    '#options' => _mlmta_student_build_list('basic_student'),
    '#empty_option' => '- Select -',
    '#ajax' => array(
      'callback' => '_mlmta_student_1_prepopulate',
      'wrapper' => 'student-1-fieldset',
    ),
    '#weight' => -9,
    '#default_value' => $student_id,
  );

  $student_rendered = 'None yet. Click -Select- above to select a student.';
  if (!empty($student_id[0])) {
    $student = student_load($student_id[0]);
    $student_view_array = entity_view('student', array($student), 'for_pursuit');
    $student_rendered = drupal_render($student_view_array);
  }
  $form['student_1']['student_fieldset'] = array(
    '#markup' => $student_rendered,
    '#prefix' => '<fieldset id="student-1-fieldset"><legend>Selected student</legend><div class="fieldset-wrapper">',
    '#suffix' => '</div></fieldset>',
    //'#tree' => TRUE,
    '#weight' => -8,
   );
  
  // Teacher
  
  if (!empty($pursuit_wrapper->pursuit_teacher_reference->raw())) {
    $default_value = $pursuit_wrapper->pursuit_teacher_reference->raw();
  } else {
    $default_value = scheduling_get_teacher_from_user() ?
        scheduling_get_teacher_from_user() :
        NULL;
  }
  $form['teacher_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => 'Teacher',
  );
  $form['teacher_fieldset']['teacher'] = array(
    '#type' => 'select',
    '#title' => t('Teacher'),
    '#options' => _mlmta_teacher_build_list('basic_teacher'),
    '#required'=>TRUE,
    '#default_value' => $default_value,
  );
  
  // Length of slot - hard coded to 20, 40, 60 or 90 minutes
  $form_temp = array();
  $form['length_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => 'Slot length and preferred time',
  ); 
  field_attach_form('pursuit', $pursuit, $form_temp, $form_state);
  unset($form_temp['#parents']);
  $form['length_fieldset']['field_mc_slot_length'] = $form_temp['field_mc_slot_length'];
  $form_temp = array();
  // Attach preferred time to form
  field_attach_form('pursuit', $pursuit, $form_temp, $form_state, NULL, array('field_name'=>'field_preferred_mc_time'));  
  $form['length_fieldset']['field_preferred_mc_time'] = $form_temp['field_preferred_mc_time'];
  
  // Pieces for registration. Create 5 pieces
  $piece = array();
  $total_pieces  = 5;
  for ($i = 0; $i<$total_pieces; $i++) {
    if ($is_new || empty($pursuit_wrapper->pursuit_piece_reference[$i]->raw())) {
      $piece[$i] = entity_create('piece', array('type' => 'basic_piece'));
    } else {
      $piece[$i] = $pursuit_wrapper->pursuit_piece_reference[$i]->value();
    }
    $form_state['piece'][$i] = $piece[$i];
    $piece_wrapper = entity_metadata_wrapper('piece', $piece[$i]);
    //dpm(!empty($pursuit_wrapper->pursuit_piece_reference[$i]->raw()));
    $piece_no = $i+1;
    $form["piece_fieldset_$i"] = array(
      '#title' => t("Piece $piece_no" . (($i >0) ? ' (Optional. Click to open.)' : '')),
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#weight' => 68 + $i,
      '#collapsible' => TRUE,
      'title' => array(
        '#type' => 'textfield',
        '#required' => $i >0 ? FALSE : TRUE,
        '#title' => 'Title',
        '#description' => t('Enter the title, key, opus number and movement. E.g. "Sonatina in C major, Op. 36 No. 1. Movement 1."'),
        '#default_value' => $piece_wrapper->label(),
        '#weight' => -10,
      ),
    );
    if(empty($pursuit_wrapper->pursuit_piece_reference[$i]->raw()) && $i != 0) {
      $form["piece_fieldset_$i"]['#collapsed'] = TRUE;
    }
    
    $form_temp = array();
    field_attach_form('piece', $piece[$i], $form_temp, $form_state);
    unset($form_temp['#parents']);
    $form["piece_fieldset_$i"] = $form["piece_fieldset_$i"] + $form_temp;
    if ($i >0) {
      $form["piece_fieldset_$i"]['piece_composer'][LANGUAGE_NONE]['#required'] = FALSE;      
      $form["piece_fieldset_$i"]['piece_duration'][LANGUAGE_NONE][0]['value']['#required'] = FALSE;
    }
  }
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
function mlmta_supersat_mc_form_validate($form, $form_state) {
  // dpm($form_state['values']);
  if ($form_state['triggering_element']['#type'] == 'submit') {
    // Check if there are already 4 registered MC students in the system
    if (!user_access('register more than four students in master class')) {
      global $user;
      $current_user = user_load($user->uid);
      $teacher_id = scheduling_get_teacher_from_user();
      $q = new EntityFieldQuery();
      $q->entityCondition('entity_type', 'pursuit')
        ->entityCondition('bundle', 'mc')
        ->fieldCondition('pursuit_teacher_reference', 'target_id', $teacher_id);
      $r = $q->execute();
      if (!empty($r['pursuit'])) {
        if (count($r['pursuit']) >= 4) {
          $pursuit_ids = array_keys($r['pursuit']);
          $pursuits = pursuit_load_multiple($pursuit_ids);
          $names = array();
          foreach ($pursuits as $pursuit) {
            $names[] = $pursuit->title;
          }
          $name_comma = implode(', ', $names);
          form_set_error('form', t('You have already registered at least 4 students in the master class and are not allowed to register more. Please contact the chair. Your registered master class students are: %students', array('%students' => $name_comma)));
        }
      }      
    }
    $piece_no = 5;
    for ($i = 1; $i<$piece_no; $i++) { // Start on piece 1 since piece 0 has required true
      if (!empty($form_state['values']["piece_fieldset_$i"]['title'])) {
        $current_piece = $i+1;
        if (empty($form_state['values']["piece_fieldset_$i"]['piece_composer']['und'][0])) {
          form_set_error("piece_fieldset_$i[piece_composer][und", "Composer $current_piece field is required.");
        }
        if (empty($form_state['values']["piece_fieldset_$i"]['piece_duration']['und'][0]['value'])) {
          form_set_error("piece_fieldset_$i[piece_duration][und][0][value", "Duration $current_piece field is required.");
        }
      }
    }
  }
}

/**
 * Submit handler
 */
function mlmta_supersat_mc_form_submit($form, $form_state) {
  $pursuit = $form_state['pursuit'];

  $piece_ids = array();  
  $piece_no = 5;
  for ($i=0; $i<$piece_no; $i++) {
    $piece = $form_state['piece'][$i];
    $piece_wrapper = entity_metadata_wrapper('piece', $piece);
    if (!empty($form_state['values']["piece_fieldset_$i"]['title'])) {
      // Creates taxonomy terms for new composers
      $term = (object) $form_state['values']["piece_fieldset_$i"]['piece_composer']['und'][0];
      if ($term->tid == 'autocreate') {
        if (empty($old_term = taxonomy_get_term_by_name($term->name))) {
          unset($term->tid);
          taxonomy_term_save($term);
          $form_state['values']["piece_fieldset_$i"]['piece_composer']['und'][0]['tid'] = $term->tid;          
        } else {
          $form_state['values']["piece_fieldset_$i"]['piece_composer']['und'][0]['tid'] = $old_term->tid;
        }
      }
      $piece->title = $form_state['values']["piece_fieldset_$i"]['title'];
      $piece_wrapper->piece_composer = $form_state['values']["piece_fieldset_$i"]['piece_composer']['und'][0]['tid'];
      $piece_wrapper->piece_duration = $form_state['values']["piece_fieldset_$i"]['piece_duration']['und'][0]['value'];
      $piece_wrapper->save();
      $piece_ids[] = $piece_wrapper->getIdentifier();      
    }
  }

  $is_new = isset($pursuit->is_new) ? $pursuit->is_new : FALSE; 
  $pursuit_wrapper = entity_metadata_wrapper('pursuit', $pursuit);

  $student_id = $form_state['values']['student'];
  $teacher_id = $form_state['values']['teacher'];
  
  $pursuit_wrapper->pursuit_student_reference->set(array($student_id));
  $pursuit_wrapper->pursuit_teacher_reference->set(array($teacher_id));
  $pursuit_wrapper->field_preferred_mc_time = $form_state['values']['field_preferred_mc_time']['und'][0]['value'];
  $pursuit_wrapper->field_mc_slot_length = $form_state['values']['field_mc_slot_length']['und'][0]['value'];
  $pursuit_wrapper->pursuit_piece_reference = $piece_ids;
  $pursuit_wrapper->field_event_reference = variable_get('tenuto_supersat_event', NULL);
  switch ($pursuit_wrapper->field_mc_slot_length->value()) {
    case 'mc20':
      $overridden = 20*60;
      break;
    case 'mc40':
      $overridden = 40*60;
      break;
    case 'mc60':
      $overridden = 60*60;
      break;
    case 'mc90':
      $overridden = 90*60;
      break;
  }
  $pursuit_wrapper->pursuit_overriden_duration = $overridden;
  
  pursuit_update_name($pursuit);
  pursuit_update_duration($pursuit);
  pursuit_save($pursuit);
  if ($is_new) {
    pursuit_add_to_shopping_cart(array($pursuit), $pursuit_wrapper->field_mc_slot_length->value()); 
  } 
  
  $form_state['rebuild'] = TRUE;
}