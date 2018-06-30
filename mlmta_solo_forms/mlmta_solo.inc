<?php

function _form_mlmta_piano_solo_steps() {
  return array (
    1 => array (
      'form' => 'mlmta_piano_solo_student',
      'title' => 'DSF solo form. Student. Step 1 of 2.',
      'validation' => array(
        'mlmta_piano_solo_student_1_process'  // These are validation functions to call when any submit button is pressed (next, previous, etc)
      ),
      'submit hook' => 'mlmta_piano_solo_student_1_submit_hook',
    ),

    2 => array(
      'form' => 'mlmta_piano_solo_pieces',
      'title' => 'DSF solo form. Repertoire and level. Step 2 of 2.',
      'validation' => array(),
      'submit hook' => 'mlmta_piano_solo_pieces_submit_hook',
    ),
  );
}

/**
 * MLMTA Piano Solo Registration form
 */
function mlmta_piano_solo_form($form, &$form_state, $pursuit = NULL) {
  
  // This is necessary to reload the form when using AJAX update
  if (empty($form_state['build_info']['files'])) {
    form_load_include($form_state, 'inc', 'mlmta_solo_forms', 'mlmta_solo');
  }
  
  if (empty($form_state['step'])) {
    $form_state['step'] = 1;
    $form_state['step_information'] = _form_mlmta_piano_solo_steps();
  }
  
  if ($pursuit) {
    $form_state['pursuit'] = $pursuit;
    $form_state['original_pursuit'] = clone $pursuit; // Clone pursuit so that we can use its original values if necessary
  }
  
  $step = &$form_state['step'];
  drupal_set_title(t($form_state['step_information'][$step]['title']));
  
  $form = $form_state['step_information'][$step]['form']($form, $form_state);
  $deadline = variable_get('tenuto_solo_form_deadline', '1922425200');
  if (time()<=$deadline || user_access('edit pursuit after deadline')) {
    if ($step > 1) {
      $form['prev'] = array(
        '#type' => 'submit',
        '#value' => t('Previous'),
        '#name' => 'prev',
        '#submit' => array('mlmta_piano_solo_previous_submit'),
        '#weight' => 98,
        // '#limit_validation_errors' => array(),
      );
    }
    if ($step < count($form_state['step_information'])) {
      $form['next'] = array(
        '#type' => 'submit',
        '#value' => t('Next'),
        '#name' => 'next',
        '#submit' => array('mlmta_piano_solo_next_submit'),
        '#weight' => 99,
      );
    } else {
      $form['finish'] = array(
        '#type' => 'submit',
        '#value' => $pursuit ? t('Save') : t('Finish and add to cart'),
        '#weight' => 99,
      );
    }
  } else {
    $form['deadline_past'] = array(
      '#markup' => t('<strong style="color: red">The submission deadline has passed. This form is disabled and cannot be submitted.</strong>'),
      '#weight' => -99,
    );
  }
  return $form; 
}

/**
 * Submit handler for the next button
 */
function mlmta_piano_solo_next_submit($form, &$form_state) {
  $current_step = &$form_state['step'];
  $form_state['step_information'][$current_step]['stored_values'] = $form_state['values'];
  // Invoke the additional hook
  if (!empty($form_state['step_information'][$current_step]['submit hook'])) {
    $form_state['step_information'][$current_step]['submit hook']($form, $form_state);
  }
  if ($current_step < count($form_state['step_information'])) {
    $current_step++;
    if (!empty($form_state['step_information'][$current_step]['stored_values'])) {
      $form_state['values'] = $form_state['step_information'][$current_step]['stored_values'];
      $form_state['visited'] = TRUE;
    }
    else {
      $form_state['values'] = array();
    }
    // Force rebuild with next step.
    $form_state['rebuild'] = TRUE;
    return;
  }
}

/**
 * Submit handler for the previous button
 */
function mlmta_piano_solo_previous_submit($form, &$form_state) {
  $current_step = &$form_state['step'];
  $form_state['step_information'][$current_step]['stored_values'] = $form_state['values'];
  // Invoke the additional hook
  if (!empty($form_state['step_information'][$current_step]['submit hook'])) {
    $form_state['step_information'][$current_step]['submit hook']($form, $form_state);
  }
  if ($current_step > 1) {
    $current_step--;
    $form_state['values'] = $form_state['step_information'][$current_step]['stored_values'];
    $form_state['visited'] = TRUE;
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Form builder for student 1
 *
 * @param $form
 * @param &$form_state
 * 
 */
function mlmta_piano_solo_student($form, &$form_state) {
  $form = array();
  $form['#attached']['js'] = array(drupal_get_path('module', 'mlmta_solo_forms') . '/mlmta_solo.js');
  global $user;
  $current_user = user_load($user->uid);
  $current_step = &$form_state['step']; 
  $form['student_1']= array(
    '#tree' => TRUE,
    '#type' => 'fieldset',
    '#title' => t('Student'),
  );
  
  $form['#validate'] = $form_state['step_information'][$current_step]['validation'];
  $form['student_1']['previous_students'] = array(
    '#type' => 'select',
    '#title' => t('Student'),
    '#required' => TRUE,
    '#description' => t('Click -Select- and find your student. If you don\'t see your student here, click on "My students" in the main menu bar and create a student.'),
    '#options' => _mlmta_student_build_list('basic_student'),
    '#ajax' => array(
      'callback' => '_mlmta_student_1_prepopulate',
      'wrapper' => 'student-1-fieldset',
    ),
    '#weight' => -99,
  );
  $form['student_1']['instrument'] = array(
    '#type' => 'select',
    '#title' => t('Instrument'),
    '#required' => TRUE,
    '#options' => mlmta_solo_forms_instruments(),
    '#weight' => -97,
    '#default_value' => 'Piano',
  );
  
  $form['student_1']['theory_test'] = array(
    '#type' => 'select',
    '#title' => t('Theory test type'),
    '#required' => TRUE,
    '#options' => mlmta_solo_forms_theory_types(),
    '#attributes' => array('class' => array('theory-test')),
    '#weight' => -96,
  );
  
  $form['student_1']['theory_level'] = array(
    '#type' => 'select',
    '#title' => t('Theory test level'),
    '#required' => TRUE,
    '#options' => mlmta_solo_forms_theory_levels(),
    '#attributes' => array('class' => array('theory-level')),
    '#weight' => -95,
  );
    
  // Create an empty student object
  $student = new stdClass();
        
  if (!empty($form_state['triggering_element']['#name']) && $form_state['triggering_element']['#name'] == 'student_1[previous_students]') {
    // Form rebuilt triggered by ajax call
    // Triggered by the student select ajax
      // Selected an existing student
    $student = student_load($form_state['values']['student_1']['previous_students']);

  } else {
    // NOT triggered by ajax
    // Check if editing a pursuit
    if (!empty($form_state['pursuit'])) {
      $student = student_load($form_state['pursuit']->pursuit_student_reference['und'][0]['target_id']);
      $form['student_1']['previous_students']['#default_value'] = $form_state['pursuit']->pursuit_student_reference['und'][0]['target_id'];
      $form['student_1']['theory_test']['#default_value'] = $form_state['pursuit']->field_dsf_theory_test['und'][0]['value'];
      $form['student_1']['theory_level']['#default_value'] = $form_state['pursuit']->field_dsf_theory_test_level['und'][0]['value'];
      $form['student_1']['instrument']['#default_value'] = $form_state['pursuit']->field_instrument_list['und'][0]['value'];
    } 
  }
    // Let's check if we've been here before. If so, we need to reset the #default_values
  if (!empty($form_state['visited']) && $form_state['visited']) {
    // Form is being revisited
    $stored_values = &$form_state['step_information'][$current_step]['stored_values']['student_1'];
    $form['student_1']['previous_students']['#default_value'] = $stored_values['previous_students'];
    $form['student_1']['theory_test']['#default_value'] = $stored_values['theory_test'];
    $form['student_1']['theory_level']['#default_value'] = $stored_values['theory_level'];
    $form['student_1']['instrument']['#default_value'] = $stored_values['instrument'];
    $student = student_load($stored_values['previous_students']);
  }
  

  $student_rendered = 'None yet. Click -Select- above to select a student.';
  if (!empty($student->name)) {
    $student_view_array = entity_view('student', array($student), 'for_pursuit');
    $student_rendered = drupal_render($student_view_array);
  }
  $form['student_1']['student_fieldset'] = array(
    '#markup' => $student_rendered,
    '#prefix' => '<fieldset id="student-1-fieldset"><legend>Selected student</legend><div class="fieldset-wrapper">',
    '#suffix' => '</div></fieldset>',
    //'#tree' => TRUE,
    '#weight' => -98,
   );
  
  
  //****  Teacher 1

  $form['student_1']['previous_teachers'] = array(
    '#type' => 'select',
    '#title' => t('Teacher'),
    '#options' => _mlmta_teacher_build_list('basic_teacher'),
    '#required'=>TRUE,
    //'#ajax' => array(
    //  'callback' => '_mlmta_teacher_1_prepopulate',
    //  'wrapper' => 'teacher-1-fieldset',
    //),
    '#weight' => 64,
    '#size' => '100',
  );
  

  if (!empty($form_state['pursuit'])) {
    $form['student_1']['previous_teachers']['#default_value'] = $form_state['pursuit']->pursuit_teacher_reference['und'][0]['target_id'];
  } elseif (!empty($current_user->field_user_teacher_reference['und'][0]['target_id'])) {
    // This is reached when the triggering element is not ajax and there is no pursuit attached, and the user references a teacher      
    $teacher_id_for_form =  $current_user->field_user_teacher_reference['und'][0]['target_id'];
    if ($teacher_id_for_form) {
      $teacher = teacher_load($teacher_id_for_form);
      $form['student_1']['previous_teachers']['#default_value'] = $teacher_id_for_form;
    }
  }
    
  // Turn off visited flag (next or previous buttons will turn it on if necessary)
  $form_state['visited'] = FALSE;
  return $form;
}

/**
 * Validation for student 1 form 
 */
function mlmta_piano_solo_student_1_process($form, &$form_state) {
  // Ajax elements will trigger this, but we only want to process if next_submit is clicked 
  if ($form_state['triggering_element']['#type'] == 'submit') {
  }
}

/**
 * Submit hook for student 1
 */
function mlmta_piano_solo_student_1_submit_hook($form, &$form_state) {
  
}


/**
 * Form builder for pieces
 */
function mlmta_piano_solo_pieces($form, &$form_state) {

  $current_step = &$form_state['step'];
  $form['#attached']['js'] = array(drupal_get_path('module', 'mlmta_solo_forms') . '/mlmta_solo.js');
  
  $form['piece1_fieldset'] = array(
    '#tree' => TRUE,
    '#title' => t('Piece 1'),
    '#type' => 'fieldset',
    'title' => array(
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => 'Title',
      '#description' => t('Enter the title, key, opus number and movement. E.g. "Sonatina in C major, Op. 36 No. 1. Movement 1."'),
      '#weight' => -68,
    ),
  );
  $form['piece2_fieldset'] = array(
    '#tree' => TRUE,
    '#title' => t('Piece 2'),
    '#type' => 'fieldset',
    'title' => array(
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => 'Title',
      '#description' => t('Enter the title, key, opus number and movement. E.g. "Sonatina in C major, Op. 36 No. 1. Movement 1."'),
      '#weight' => -67,
    ),
  );
  
  $form['rep_notice'] = array(
    '#markup' => 'Check guidelines in the MLMTA handbook. Total playing time must be precise. Students using copied music or who have "no music" will automatically be disqualified.',
    '#weight' => -66,
  );
  
  $pursuit_temp = empty($form_state['pursuit']) ? entity_create('pursuit', array('type'=>'piano_solo')) : $form_state['pursuit'];

  //$form_temp = array();
  //// Attach level to form
  //field_attach_form('pursuit', $pursuit_temp,$form_temp, $form_state, NULL, array('field_name'=>'field_piano_solo_level'));
  //$form['field_piano_solo_level'] = $form_temp['field_piano_solo_level'];
  //$form['field_piano_solo_level']['#weight'] = -69;

  
  $form_temp = array();
  // Attach preferred time to form
  field_attach_form('pursuit', $pursuit_temp,$form_temp, $form_state, NULL, array('field_name'=>'pursuit_preferred_time'));  
  $form['pursuit_preferred_time'] = $form_temp['pursuit_preferred_time'];
  $form['pursuit_preferred_time']['#weight'] = 50;
  
  $piece1 = entity_create('piece', array('type' => 'basic_piece'));
  $piece2 = entity_create('piece', array('type' => 'basic_piece'));
  if (!empty($form_state['pursuit'])) {
    // Editing an existing one!
    $piece1 = piece_load($form_state['pursuit']->pursuit_piece_reference['und'][0]['target_id']);
    $piece2 = piece_load($form_state['pursuit']->pursuit_piece_reference['und'][1]['target_id']);
    $form['piece1_fieldset']['title']['#default_value'] = $piece1->title;
    $form['piece2_fieldset']['title']['#default_value'] = $piece2->title;
  }
  $form_temp = array();
  field_attach_form('piece', $piece1, $form_temp, $form_state);
  unset($form_temp['#parents']);
  $form['piece1_fieldset'] = $form['piece1_fieldset'] + $form_temp;
  $form_temp = array();
  field_attach_form('piece', $piece2, $form_temp, $form_state);
  unset($form_temp['#parents']);
  $form['piece2_fieldset'] = $form['piece2_fieldset'] + $form_temp;

  
  if (isset($form_state['visited']) && $form_state['visited']) {
    // Need to retrieve saved values. We have visited this step before!
    $stored_values = &$form_state['step_information'][$current_step]['stored_values'];
    $form['piece1_fieldset']['title']['#default_value'] = $stored_values['piece1_fieldset']['title'];
    $form['piece2_fieldset']['title']['#default_value'] = $stored_values['piece2_fieldset']['title'];
    $form['piece1_fieldset']['piece_composer'][LANGUAGE_NONE]['#default_value'] = $stored_values['piece1_fieldset']['piece_composer']['und'][0]['name'];
    $form['piece2_fieldset']['piece_composer'][LANGUAGE_NONE]['#default_value'] = $stored_values['piece2_fieldset']['piece_composer']['und'][0]['name'];
    $form['piece1_fieldset']['piece_duration'][LANGUAGE_NONE][0]['value']['#default_value'] = $stored_values['piece1_fieldset']['piece_duration'][LANGUAGE_NONE][0]['value'];
    $form['piece2_fieldset']['piece_duration'][LANGUAGE_NONE][0]['value']['#default_value'] = $stored_values['piece2_fieldset']['piece_duration'][LANGUAGE_NONE][0]['value'];
        
    unset($form_state['input']['pursuit_preferred_time']);
    $form['pursuit_preferred_time'][LANGUAGE_NONE]['#default_value'] = $stored_values['pursuit_preferred_time'][LANGUAGE_NONE][0]['value'];
    
  }

    
  $form_state['visited'] = FALSE;
  return $form;
}

/**
 * Submit hook for pieces
 */
function mlmta_piano_solo_pieces_submit_hook($form, &$form_state) {

  // Creates taxonomy terms for new composers
  if ($form_state['values']['piece2_fieldset']['piece_composer']['und'][0]['tid'] == 'autocreate') {
    unset($form_state['values']['piece2_fieldset']['piece_composer']['und'][0]['tid']);
    $term = (object) $form_state['values']['piece2_fieldset']['piece_composer']['und'][0];
    taxonomy_term_save($term);
    $form_state['values']['piece2_fieldset']['piece_composer']['und'][0]['tid'] = $term->tid;
  }
  if ($form_state['values']['piece1_fieldset']['piece_composer']['und'][0]['tid'] == 'autocreate') {
    unset($form_state['values']['piece1_fieldset']['piece_composer']['und'][0]['tid']);
    $term = (object) $form_state['values']['piece1_fieldset']['piece_composer']['und'][0];
    taxonomy_term_save($term);
    $form_state['values']['piece1_fieldset']['piece_composer']['und'][0]['tid'] = $term->tid;
  }
}

/**
 * Submit handler
 */
function mlmta_piano_solo_form_submit($form, &$form_state) {
  $current_step = &$form_state['step'];
  if (!empty($form_state['step_information'][$current_step]['submit hook'])) {
    $form_state['step_information'][$current_step]['submit hook']($form, $form_state);
  }
  $form_state['step_information'][$current_step]['stored_values'] = $form_state['values'];
 
  $student_1_step = 1;
  $pieces_step = 2;
  
  // First, get the student_id 
  $stored_values_1 = $form_state['step_information'][$student_1_step]['stored_values']['student_1'];
  $student_1_id = $stored_values_1['previous_students'];  
  // Get teacher 1 id
  $teacher_1_id = $stored_values_1['previous_teachers'];  
  
  // Create pieces
  if(!empty($form_state['pursuit'])) {
    $piece_1 = piece_load($form_state['pursuit']->pursuit_piece_reference['und'][0]['target_id']);
    $piece_2 = piece_load($form_state['pursuit']->pursuit_piece_reference['und'][1]['target_id']);
  } else {
    $piece_1 = entity_create('piece', array('type'=>'basic_piece'));
    $piece_2 = entity_create('piece', array('type'=>'basic_piece'));
  }

  $piece_1_stored_values = $form_state['step_information'][$pieces_step]['stored_values']['piece1_fieldset'];
  $piece_1->title = $piece_1_stored_values['title'];
  $piece_1_wrapper = entity_metadata_wrapper('piece', $piece_1);
  $piece_1_wrapper->piece_composer = $piece_1_stored_values['piece_composer']['und'][0]['tid'];
  $piece_1_wrapper->piece_duration = $piece_1_stored_values['piece_duration']['und'][0]['value'];
  $piece_1_wrapper->save();
  $piece_1_id = $piece_1_wrapper->getIdentifier();
  
  // piece 2
  $piece_2_stored_values = $form_state['step_information'][$pieces_step]['stored_values']['piece2_fieldset'];
  $piece_2->title = $piece_2_stored_values['title'];
  $piece_2_wrapper = entity_metadata_wrapper('piece', $piece_2);
  $piece_2_wrapper->piece_composer = $piece_2_stored_values['piece_composer']['und'][0]['tid'];
  $piece_2_wrapper->piece_duration = $piece_2_stored_values['piece_duration']['und'][0]['value'];
  $piece_2_wrapper->save();
  $piece_2_id = $piece_2_wrapper->getIdentifier();
  
  // Get key for level and instrument
  $theory_key = $form_state['step_information'][$student_1_step]['stored_values']['student_1']['theory_test'];
  $theory_level_key = $form_state['step_information'][$student_1_step]['stored_values']['student_1']['theory_level'];
  $instrument_key = $form_state['step_information'][$student_1_step]['stored_values']['student_1']['instrument'];
  
  // Get value for preferred time
  $preferred_time = $form_state['step_information'][$pieces_step]['stored_values']['pursuit_preferred_time']['und'][0]['value'];

  // Get value for preferred day
  // $preferred_day = $form_state['step_information'][$pieces_step]['stored_values']['field_preferred_day']['und'][0]['value'];  

  // Create or update pursuit
  if(!empty($form_state['pursuit'])) {
    $pursuit = pursuit_load($form_state['pursuit']->pursuit_id);
  } else {
    $pursuit = entity_create('pursuit', array('type' => 'piano_solo'));
  }
  $pursuit_wrapper = entity_metadata_wrapper('pursuit', $pursuit);
  $pursuit_wrapper->pursuit_student_reference->set(array($student_1_id));
  $pursuit_wrapper->pursuit_piece_reference->set(array($piece_1_id, $piece_2_id));
  $pursuit_wrapper->field_instrument_list[0]->set($instrument_key);
  $pursuit_wrapper->pursuit_teacher_reference->set(array($teacher_1_id));
  $pursuit_wrapper->pursuit_preferred_time->set($preferred_time);
  //$pursuit_wrapper->field_preferred_day->set($preferred_day);
  $pursuit_wrapper->field_event_reference->set(variable_get('tenuto_solo_event', NULL));
  $pursuit_wrapper->field_dsf_theory_test[0]->set($theory_key);
  $pursuit_wrapper->field_dsf_theory_test_level[0]->set($theory_level_key);

  pursuit_update_name($pursuit);
  pursuit_update_duration($pursuit);
  
  // IF this pursuit is new then add to commerce cart
  pursuit_save($pursuit);
  if (empty($form_state['pursuit'])) {
    pursuit_add_to_shopping_cart(array($pursuit), 'piano-solo');
  }
}


