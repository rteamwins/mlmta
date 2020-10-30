<?php

function _form_mlmta_cmf_ensemble_steps() {
  return array (
    1 => array(
      'form' => 'mlmta_cmf_ensemble_student',
      'title' => "CMF ensemble form. Student 1. Step 1 of 3.",
      'delta' => 0, // Delta for multivalued reference fields
      'validation' => array('mlmta_cmf_ensemble_student_process'),
      'submit hook' => 'mlmta_cmf_ensemble_student_submit_hook',
    ),
    
    // This is a dummy step so that the Next button is activated.
    2 => array(
      'form' => 'mlmta_cmf_ensemble_student',
      'title' => "CMF ensemble form. Student 2. Step 2 of 3.",
      'delta' => 1, // Delta for multivalued reference fields
      'validation' => array('mlmta_cmf_ensemble_student_process'),
      'submit hook' => 'mlmta_cmf_ensemble_student_submit_hook',
    ),
    
    3 => array (
      'form' => 'mlmta_cmf_ensemble_pieces',
      'title' => "CMF ensemble form. Repertoire. Step 3 of 3.",
      'validation' => array(
        'mlmta_cmf_ensemble_pieces_process',
      ),
      'submit hook' => 'mlmta_cmf_ensemble_pieces_submit_hook',    
    )
  );
}

/**
 * DSMTA Piano Duet Registration form
 */
function mlmta_cmf_ensemble_form($form, &$form_state, $pursuit = NULL) {
  // needed content: piece, student, teacher, family, pursuit level
  
  if (empty($form_state['build_info']['files'])) {
    form_load_include($form_state, 'inc', 'mlmta_cmf_forms', 'mlmta_cmf_ensemble');  
  }

  if (empty($form_state['step'])) {
    $form_state['step'] = 1;
    $form_state['step_information'] = _form_mlmta_cmf_ensemble_steps();
  }
  
  if ($pursuit) {
    $form_state['pursuit'] = $pursuit;
    $form_state['original_pursuit'] = clone $pursuit; // Clone pursuit so that we can use its original values if necessary
  }
  
  $step = &$form_state['step'];
  drupal_set_title(t($form_state['step_information'][$step]['title']));
  
  $form = $form_state['step_information'][$step]['form']($form, $form_state);
  $deadline = variable_get('tenuto_cmf_form_deadline', '1922425200'); // Using from the main module
  if (time()<=$deadline || user_access('edit pursuit after deadline')) {
    if ($step > 1) {
      $form['prev'] = array(
        '#type' => 'submit',
        '#value' => t('Previous'),
        '#name' => 'prev',
        '#submit' => array('mlmta_cmf_ensemble_previous_submit'),
        '#weight' => 98,
        // '#limit_validation_errors' => array(),
      );
    }
    if ($step < count($form_state['step_information'])) {
      $form['next'] = array(
        '#type' => 'submit',
        '#value' => t('Next'),
        '#name' => 'next',
        '#submit' => array('mlmta_cmf_ensemble_next_submit'),
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
function mlmta_cmf_ensemble_next_submit($form, &$form_state) {
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
function mlmta_cmf_ensemble_previous_submit($form, &$form_state) {
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
 * Form builder for student
 *
 * @param $form
 * @param &$form_state
 * 
 */
function mlmta_cmf_ensemble_student ($form, &$form_state) {

  global $user;
  $current_user = user_load($user->uid);
  
  $form = array();
  //$form['#attached']['js'] = array(drupal_get_path('module', 'mlmta_cmf_forms') . '/mlmta_cmf_solo.js');
  $current_step = &$form_state['step'];
  
  $form['student'] = array('#tree' => TRUE, '#type' => 'container',);
  $delta =  $form_state['step_information'][$current_step]['delta'];
  $fstitle = 'Performer '.($delta+1);
  $form['student'][$delta]= array(
    '#tree' => TRUE,
    '#type' => 'fieldset',
    '#title' => $fstitle,
  );
  
  $form['#validate'] = $form_state['step_information'][$current_step]['validation'];
  $form['student'][$delta]['previous_students'] = array(
    '#type' => 'select',
    '#title' => t('Student'),
    '#description' => t('Click -Select- and find your student. If you don\'t see your student here, click on "My students" in the main menu bar and create a student.'),
    '#options' => _mlmta_student_build_list('basic_student'),
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => '_mlmta_student_generic_prepopulate',
      'wrapper' => 'student-fieldset',
    ),
    '#weight' => -99,
  );
  
  //$form['student'][$delta]['instrument'] = array(
  //  '#type' => 'select',
  //  '#title' => t('Instrument'),
  //  '#required' => TRUE,
  //  '#options' => mlmta_cmf_forms_instruments(),
  //  '#weight' => -97,
  //  '#default_value' => 'Piano',
  //);
  
  $student = new stdClass();
        
  if (!empty($form_state['triggering_element']['#name']) && $form_state['triggering_element']['#name'] == "student[{$delta}][previous_students]") {
    // Form rebuilt triggered by ajax call
    // Triggered by the student select ajax
    // Selected an existing student
    $student = student_load($form_state['values']['student'][$delta]['previous_students']);
  } else {
    // NOT triggered by ajax
    // Check if editing a pursuit
    if (!empty($form_state['pursuit'])) {
      $student = student_load($form_state['pursuit']->pursuit_student_reference['und'][$delta]['target_id']);
      $form['student'][$delta]['previous_students']['#default_value'] = $form_state['pursuit']->pursuit_student_reference['und'][$delta]['target_id'];
      $form['student'][$delta]['instrument']['#default_value'] = $form_state['pursuit']->field_instrument_list['und'][$delta]['value'];
    } 
  }
  
  
  // Let's check if we've been here before. If so, we need to reset the #default_values
  if (!empty($form_state['visited']) && $form_state['visited']) {
    // Form is being revisited
    $stored_values = &$form_state['step_information'][$current_step]['stored_values']['student'][$delta];
    $form['student'][$delta]['previous_students']['#default_value'] = $stored_values['previous_students'];
    $form['student'][$delta]['instrument']['#default_value'] = $stored_values['instrument'];
    $student = student_load($stored_values['previous_students']);
  }

  $student_rendered = 'None yet. Click -Select- above to select a student.';
  if (!empty($student->name)) {
    $student_view_array = entity_view('student', array($student), 'for_pursuit');
    $student_rendered = drupal_render($student_view_array);
  }
  $form['student'][$delta]['student_fieldset'] = array(
    '#markup' => $student_rendered,
    '#prefix' => '<fieldset id="student-fieldset"><legend>Selected student</legend><div class="fieldset-wrapper">',
    '#suffix' => '</div></fieldset>',
    //'#tree' => TRUE,
    '#weight' => -98,
   );
  

  //****  Teacher 1
  

  $form['student'][$delta]['previous_teachers'] = array(
    '#type' => 'select',
    '#title' => t('Teacher'),
    '#options' => _mlmta_teacher_build_list('basic_teacher'),
    '#required'=>TRUE,
    '#weight' => 64,
  );
  
  if (!empty($form_state['pursuit'])) {
    $form['student'][$delta]['previous_teachers']['#default_value'] = $form_state['pursuit']->pursuit_teacher_reference['und'][$delta]['target_id'];
  } elseif ($teacher_id_for_form = scheduling_get_teacher_from_user()) {
    // This is reached when the triggering element is not ajax and there is no pursuit attached, and the user references a teacher      
    if ($teacher_id_for_form) {
      $form['student'][$delta]['previous_teachers']['#default_value'] = $teacher_id_for_form;
    }
  }

  // Turn off visited flag (next or previous buttons will turn it on if necessary)
  $form_state['visited'] = FALSE;
  return $form;
}

/**
 * Validation for student form 
 */
function mlmta_cmf_ensemble_student_process($form, &$form_state) {
  $current_step = $form_state['step'];
  $delta =  $form_state['step_information'][$current_step]['delta'];
  // Ajax elements will trigger this, but we only want to process if next_submit is clicked 
  if ($form_state['triggering_element']['#type'] == 'submit') {
    
  }
}

/**
 * Submit hook for student 
 */
function mlmta_cmf_ensemble_student_submit_hook($form, &$form_state) {
  // This hook will create a family tag and a new teacher
  //$current_step = $form_state['step'];
  //$delta =  $form_state['step_information'][$current_step]['delta'];
}
  

/**
 * Form builder for pieces
 */
function mlmta_cmf_ensemble_pieces($form, &$form_state) {

  $current_step = &$form_state['step'];
  $form['#validate'] = $form_state['step_information'][$current_step]['validation'];
  
  $pursuit_temp = empty($form_state['pursuit']) ? entity_create('pursuit', array('type'=>'cmf_ensemble')) : $form_state['pursuit'];
  
  
  $form_temp = array();
  // Attach level to form
  field_attach_form('pursuit', $pursuit_temp,$form_temp, $form_state, NULL, array('field_name'=>'field_piano_solo_level')); // This is actually the DSF level
  $form['field_piano_solo_level'] = $form_temp['field_piano_solo_level'];
  $form['field_piano_solo_level']['#weight'] = -69;
  
  $form['pieces'] = array('#type' => 'container', '#tree' => TRUE);
  $form['pieces']['piece1_fieldset'] = array(
    '#tree' => TRUE,
    '#title' => t('Piece 1'),
    '#type' => 'fieldset',
    '#weight' => 1,
    'title' => array(
      '#type' => 'textfield',
      '#title' => 'Title',
      '#required' => TRUE,
      '#description' => t('Enter the title, key, opus number and movement. E.g. "Sonatina in C major, Op. 36 No. 1. Movement 1."'),
      '#weight' => -99,
    ),
  );
  $form['pieces']['piece2_fieldset'] = array(
    '#tree' => TRUE,
    '#title' => t('Piece 2'),
    '#type' => 'fieldset',
    '#weight' => 2,
    'title' => array(
      '#type' => 'textfield',
      '#title' => 'Title',
      '#description' => t('Enter the title, key, opus number and movement. E.g. "Sonatina in C major, Op. 36 No. 1. Movement 1."'),
      '#weight' => -99,
      '#required' => TRUE,
    ),
  );

  $piece_array = array();
  for ($i = 0; $i<2; $i++) {
    $piece_array[$i] = entity_create('piece', array('type' => 'basic_piece'));
  }
  if (!empty($form_state['pursuit'])) {
    // Editing an existing one!
    $pursuit_wrapper = entity_metadata_wrapper('pursuit', $form_state['pursuit']);

    $form['pursuit_preferred_time'][LANGUAGE_NONE]['#default_value'] = !empty($form_state['pursuit']->pursuit_preferred_time['und']) ?
    $form_state['pursuit']->pursuit_preferred_time['und'][0]['value'] : '_none' ;

    foreach ($pursuit_wrapper->pursuit_piece_reference->getIterator() as $delta => $piece_wrapper) {
      $piece_array[$delta] = $piece_wrapper->value();
      $i = $delta+1;
      $fs = "piece{$i}_fieldset";
      $form['pieces'][$fs]['title']['#default_value'] = $piece_array[$delta]->title;
    }
  }
  
  foreach($piece_array as $delta => $piece) {
    $form_temp = array();
    $i = $delta+1;
    $fs = "piece{$i}_fieldset";
    field_attach_form('piece', $piece, $form_temp, $form_state);
    unset($form_temp['#parents']);

    $form['pieces'][$fs] = $form['pieces'][$fs] + $form_temp;
  }

  $form_temp = array();
  
  if (isset($form_state['visited']) && $form_state['visited']) {
    // Need to retrieve saved values. We have visited this step before!
    $stored_values = &$form_state['step_information'][$current_step]['stored_values'];
    
    $form['pieces']['piece1_fieldset']['title']['#default_value'] = $stored_values['pieces']['piece1_fieldset']['title'];
    $form['pieces']['piece1_fieldset']['piece_composer'][LANGUAGE_NONE]['#default_value'] = $stored_values['pieces']['piece1_fieldset']['piece_composer']['und'][0]['name'];
    $form['pieces']['piece1_fieldset']['piece_duration'][LANGUAGE_NONE][0]['value']['#default_value'] = $stored_values['pieces']['piece1_fieldset']['piece_duration'][LANGUAGE_NONE][0]['value'];
        
    $form['pieces']['piece2_fieldset']['title']['#default_value'] = $stored_values['pieces']['piece2_fieldset']['title'];
    $form['pieces']['piece2_fieldset']['piece_composer'][LANGUAGE_NONE]['#default_value'] = $stored_values['pieces']['piece2_fieldset']['piece_composer']['und'][0]['name'];
    $form['pieces']['piece2_fieldset']['piece_duration'][LANGUAGE_NONE][0]['value']['#default_value'] = $stored_values['pieces']['piece2_fieldset']['piece_duration'][LANGUAGE_NONE][0]['value'];
    
    unset($form_state['input']['field_piano_solo_level']);
    $form['field_piano_solo_level'][LANGUAGE_NONE]['#default_value'] = $stored_values['field_piano_solo_level'][LANGUAGE_NONE][0]['value'];
    
    unset($form_state['input']['pursuit_preferred_time']);
    $form['pursuit_preferred_time'][LANGUAGE_NONE]['#default_value'] = $stored_values['pursuit_preferred_time'][LANGUAGE_NONE][0]['value'];
  }

  $form_state['visited'] = FALSE;
  return $form;
}


/**
 *  Validation for pieces
 */
function mlmta_cmf_ensemble_pieces_process($form, &$form_state) {
  //if(!empty($form_state['values']['pieces']['piece3_fieldset']['title'])) {
  //  if(empty($form_state['values']['pieces']['piece3_fieldset']['piece_composer']['und'])) {
  //    form_set_error('piece][piece3_fieldset][piece_composer', t('Enter a composer for piece 3'));
  //  }
  //  if(empty($form_state['values']['pieces']['piece3_fieldset']['piece_duration']['und'][0]['value'])) {
  //    form_set_error('piece][piece3_fieldset][piece_duration', t('Enter a duration for piece 3'));
  //  }
  //}
  //
  //if(!empty($form_state['values']['pieces']['piece4_fieldset']['title'])) {
  //  if(empty($form_state['values']['pieces']['piece4_fieldset']['piece_composer']['und'][0]['name'])) {
  //    form_set_error('piece][piece4_fieldset][piece_composer', t('Enter a composer for piece 4'));
  //  }
  //  if(empty($form_state['values']['pieces']['piece4_fieldset']['piece_duration']['und'][0]['value'])) {
  //    form_set_error('piece][piece4_fieldset][piece_duration', t('Enter a duration for piece 4'));
  //  }
  //}
}

/**
 * Submit hook for pieces
 */
function mlmta_cmf_ensemble_pieces_submit_hook($form, &$form_state) {
  // Creates taxonomy terms for new composers
  if ($form_state['values']['pieces']['piece1_fieldset']['piece_composer']['und'][0]['tid'] == 'autocreate') {
    unset($form_state['values']['pieces']['piece1_fieldset']['piece_composer']['und'][0]['tid']);
    $term = (object) $form_state['values']['pieces']['piece1_fieldset']['piece_composer']['und'][0];
    taxonomy_term_save($term);
    $form_state['values']['pieces']['piece1_fieldset']['piece_composer']['und'][0]['tid'] = $term->tid;
  }
  if ($form_state['values']['pieces']['piece2_fieldset']['piece_composer']['und'][0]['tid'] == 'autocreate') {
    unset($form_state['values']['pieces']['piece2_fieldset']['piece_composer']['und'][0]['tid']);
    $term = (object) $form_state['values']['pieces']['piece2_fieldset']['piece_composer']['und'][0];
    taxonomy_term_save($term);
    $form_state['values']['pieces']['piece2_fieldset']['piece_composer']['und'][0]['tid'] = $term->tid;
  }
}

/**
 * Submit handler
 */
function mlmta_cmf_ensemble_form_submit($form, &$form_state) {
  $current_step = &$form_state['step'];
  if (!empty($form_state['step_information'][$current_step]['submit hook'])) {
    $form_state['step_information'][$current_step]['submit hook']($form, $form_state);
  }
  $form_state['step_information'][$current_step]['stored_values'] = $form_state['values'];
 
  $student_count = 2;
  $pieces_step = 3;

  $student_ids = array();
  $teacher_ids = array();
  
  //  Loop through students first. First student is form step #2
  for ($step = 1; $step <= $student_count; $step++) {
    $delta = $form_state['step_information'][$step]['delta'];
    $stored_values = $form_state['step_information'][$step]['stored_values']['student'][$delta];
    $student_ids[$delta] = $stored_values['previous_students'];
   // Get teacher  id
    $teacher_ids[$delta]= $stored_values['previous_teachers'];
  }
   
  // Create pieces
  $stored_values = $form_state['step_information'][$pieces_step]['stored_values'];
  $delta = 0;
  $piece_array = array();

  foreach($stored_values['pieces'] as $key => $piece_fields) {
    if (!empty($piece_fields['title'])) {
      $piece_array[$delta] = entity_create('piece', array('type'=>'basic_piece'));
      if(!empty($form_state['pursuit'])) {
        $pursuit_wrapper = entity_metadata_wrapper('pursuit', $form_state['pursuit']);
        $piece_array[$delta] = $pursuit_wrapper->pursuit_piece_reference[$delta]->value();  
      } 
      $piece_array[$delta]->title = $piece_fields['title'];
      $wrapper = entity_metadata_wrapper('piece', $piece_array[$delta]);
      $wrapper->piece_composer =  $piece_fields['piece_composer']['und'][0]['tid'];
      $wrapper->piece_duration =  $piece_fields['piece_duration']['und'][0]['value'];
      $wrapper->save(); 
      $delta++;      
    }
  }
  
  // Get value for preferred time
  $preferred_time = $form_state['step_information'][$pieces_step]['stored_values']['pursuit_preferred_time']['und'][0]['value'];

  // Get value for preferred time
  $level = $form_state['step_information'][$pieces_step]['stored_values']['field_piano_solo_level']['und'][0]['value'];
  
  // Create or update pursuit
  if(!empty($form_state['pursuit'])) {
    $pursuit = pursuit_load($form_state['pursuit']->pursuit_id);
  } else {
    $pursuit = entity_create('pursuit', array('type' => 'recital_ensemble'));
  }
  $pursuit_wrapper = entity_metadata_wrapper('pursuit', $pursuit);
  $pursuit_wrapper->pursuit_student_reference = NULL;
  $pursuit_wrapper->field_piano_solo_level = '';
  $pursuit_wrapper->pursuit_teacher_reference = NULL;
  
  foreach($student_ids as $delta => $student_id) {
    $pursuit_wrapper->pursuit_student_reference[$delta] = $student_id;
    $pursuit_wrapper->pursuit_teacher_reference[$delta] = $teacher_ids[$delta];
//    $pursuit_wrapper->field_instrument_list[$delta] = $instruments[$delta];
  }
  
  $pursuit_wrapper->pursuit_piece_reference->set(array());
  foreach($piece_array as $delta => $piece) {
    $pursuit_wrapper->pursuit_piece_reference[$delta]->set($piece->piece_id);
  }

  //$pursuit_wrapper->pursuit_preferred_time->set($preferred_time);
  $pursuit_wrapper->field_piano_solo_level->set($level);
  //$pursuit_wrapper->field_judged_accompanist = $form_state['step_information'][$accompanist_step]['stored_values']['accompanist']['judged_accompanist'];
  $pursuit_wrapper->field_event_reference->set(variable_get('tenuto_cmf_event', NULL)); 
  
  pursuit_update_name($pursuit);
  pursuit_update_duration($pursuit);
  pursuit_save($pursuit);
  
  
  // IF this pursuit is new then add to commerce cart
  
  if (empty($form_state['pursuit'])) {
    pursuit_add_to_shopping_cart(array($pursuit), 'cmf_ensemble');
    mlmta_cmf_check_teacher_limits($pursuit);
  } 
}
