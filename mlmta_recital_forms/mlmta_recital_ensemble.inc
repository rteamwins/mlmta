<?php

function _form_mlmta_recital_ensemble_steps() {
  return array (
    1 => array(
      'form' => 'mlmta_recital_ensemble_numbers',
      'title' => 'Recital ensemble form. Number of performers',
      'validation' => array(
      ),
      'submit hook' => 'mlmta_recital_ensemble_numbers_submit_hook',
    ),
    
    // This is a dummy step so that the Next button is activated.
    2 => array(
      NULL
    )

  );
}

/**
 * DSMTA Piano Duet Registration form
 */
function mlmta_recital_ensemble_form($form, &$form_state, $pursuit = NULL) {
  // needed content: piece, student, teacher, family, pursuit level
  
  if (empty($form_state['build_info']['files'])) {
    form_load_include($form_state, 'inc', 'mlmta_recital_forms', 'mlmta_recital_ensemble');  
  }

  if (empty($form_state['step'])) {
    $form_state['step'] = 1;
    $form_state['step_information'] = _form_mlmta_recital_ensemble_steps();
  }
  
  if ($pursuit) {
    $form_state['pursuit'] = $pursuit;
    $form_state['original_pursuit'] = clone $pursuit; // Clone pursuit so that we can use its original values if necessary
  }
  
  $step = &$form_state['step'];
  drupal_set_title(t($form_state['step_information'][$step]['title']));
  
  $form = $form_state['step_information'][$step]['form']($form, $form_state);
  $deadline = variable_get('tenuto_solo_form_deadline', '1922425200'); // Using from the main module
  if (time()<=$deadline || user_access('edit pursuit after deadline')) {
    if ($step > 1) {
      $form['prev'] = array(
        '#type' => 'submit',
        '#value' => t('Previous'),
        '#name' => 'prev',
        '#submit' => array('mlmta_recital_ensemble_previous_submit'),
        '#weight' => 98,
        // '#limit_validation_errors' => array(),
      );
    }
    if ($step < count($form_state['step_information'])) {
      $form['next'] = array(
        '#type' => 'submit',
        '#value' => t('Next'),
        '#name' => 'next',
        '#submit' => array('mlmta_recital_ensemble_next_submit'),
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
function mlmta_recital_ensemble_next_submit($form, &$form_state) {
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
function mlmta_recital_ensemble_previous_submit($form, &$form_state) {
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
 * Form builder for number of performers
 *
 * @param $form
 * @param &$form_state
 * 
 */
function mlmta_recital_ensemble_numbers($form, &$form_state) {
  $form = array();
  $current_step = &$form_state['step'];
  $form['student_count'] = array (
    '#title' => 'Number of performers',
    '#description' => 'Enter the number of performers in this ensemble (minimum 2)',
    '#type' => 'select',
    '#options' => array(
      1 =>'One student and one accompanist (not judged)',
      2 => 'Two students and an optional accompanist',
      3 => 'Three students and an optional accompanist',
      4 => 'Four students and an optional accompanist',
    ),
    '#default_value' => 1,
    '#required' => TRUE,
  );
  
  $form['note'] = array(
    '#markup'=>t('<strong>IMPORTANT:</strong> Before starting this registration, be sure that the students (and accompanist if needed) you intend to register now are already IN THE SYSTEM. To add students and accompanists to the system, click "My students" in the main menu above. Accompanists must be entered into the system as regular students.'),
    '#prefix' => '<div>',
    '#suffix' => '<div>',
  );
  
  if (!empty($form_state['pursuit'])) {
    $wrapper = entity_metadata_wrapper('pursuit', $form_state['pursuit']);
    $form['student_count']['#default_value'] = $wrapper->pursuit_student_reference->count() - 1;
  }
  
  // Let's check if we've been here before. If so, we need to reset the #default_value
  if (!empty($form_state['visited']) && $form_state['visited']) {
    // Form is being revisited
    $stored_values = &$form_state['step_information'][$current_step]['stored_values'];
    $form['student_count']['#default_value'] = $stored_values['student_count'];
  }
  $form['visited'] = FALSE;
  return $form;
  
}

/**
 * Submit hook for mlmta_recital_ensemble_numbers
 */
function mlmta_recital_ensemble_numbers_submit_hook($form, &$form_state) {
  // Modify $form_state['step_information'] according to the number of students to be entered. Add accompanist and pieces at the very end
  // Modifying the number of students by going back to the first step will effectively get rid of stored_values. Deal with it!
  $student_count = $form_state['values']['student_count'];
  $steps = $student_count + 3;
  for ($i = 2; $i < $student_count+2; $i++) {
    $j = $i-1;
    $form_state['step_information'][$i] = array(
      'form' => 'mlmta_recital_ensemble_student',
      'title' => "Recital ensemble form. Student {$j}. Step {$i} of {$steps}.",
      'delta' => $i-2, // Delta for multivalued reference fields
      'validation' => array('mlmta_recital_ensemble_student_process'),
      'submit hook' => 'mlmta_recital_ensemble_student_submit_hook',
    );
  }
  
  for ($j=$i; $j<11; $j++) {
    // Unset previous remaining steps. Thsi is used in case the user decides to have fewer students than originally started
    unset($form_state['step_information'][$i]);
  }
  
  $j = $i;
  $form_state['step_information'][$i++] = array (
      'form' => 'mlmta_recital_ensemble_accompanist',
      'title' => "Recital ensemble form. Accompanist. Step {$j} of {$steps}.",
      'delta' => $i-3, // Delta for multivalued reference fields
      'validation' => array(
        'mlmta_recital_ensemble_accompanist_process'  // These are functions to call when any submit button is pressed (next, previous, etc)
      ),
      'submit hook' => 'mlmta_recital_ensemble_accompanist_submit_hook',    
  );
  
  $j = $i;
  $form_state['step_information'][$i++] = array (
    'form' => 'mlmta_recital_ensemble_pieces',
    'title' => "Recital ensemble form. Repertoire. Step {$j} of {$steps}.",
    'validation' => array(
      'mlmta_recital_ensemble_pieces_process',
    ),
    'submit hook' => 'mlmta_recital_ensemble_pieces_submit_hook',    
  );
}

/**
 * Form builder for student
 *
 * @param $form
 * @param &$form_state
 * 
 */
function mlmta_recital_ensemble_student ($form, &$form_state) {

  global $user;
  $current_user = user_load($user->uid);
  
  $form = array();
  //$form['#attached']['js'] = array(drupal_get_path('module', 'mlmta_recital_forms') . '/mlmta_recital_solo.js');
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
  
  $form['student'][$delta]['instrument'] = array(
    '#type' => 'select',
    '#title' => t('Instrument'),
    '#required' => TRUE,
    '#options' => mlmta_recital_forms_instruments(),
    '#weight' => -97,
    '#default_value' => 'Piano',
  );
  
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
  } elseif (!empty($current_user->field_user_teacher_reference['und'][0]['target_id'])) {
    // This is reached when the triggering element is not ajax and there is no pursuit attached, and the user references a teacher      
    $teacher_id_for_form =  $current_user->field_user_teacher_reference['und'][0]['target_id'];
    if ($teacher_id_for_form) {
      $teacher = teacher_load($teacher_id_for_form);
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
function mlmta_recital_ensemble_student_process($form, &$form_state) {
  $current_step = $form_state['step'];
  $delta =  $form_state['step_information'][$current_step]['delta'];
  // Ajax elements will trigger this, but we only want to process if next_submit is clicked 
  if ($form_state['triggering_element']['#type'] == 'submit') {
    
  }
}

/**
 * Submit hook for student 
 */
function mlmta_recital_ensemble_student_submit_hook($form, &$form_state) {
  // This hook will create a family tag and a new teacher
  $current_step = $form_state['step'];
  $delta =  $form_state['step_information'][$current_step]['delta'];
}

/**
 * Form builder for accompanist
 */
function mlmta_recital_ensemble_accompanist($form, &$form_state) {

  global $user;

  $form = array();
  $current_step = &$form_state['step'];
  $current_user = user_load($user->uid);
  $delta = $form_state['step_information'][$current_step]['delta'];
  
  $form['accompanist']= array(
    '#tree' => TRUE,
    '#type' => 'fieldset',
    '#title' => t('Accompanist'),
  );
  $form['note'] = array(
    '#markup' => t('Please note: accompanists must be first created within the "My students" option in the menu above.'),
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );
  
  $form['#validate'] = $form_state['step_information'][$current_step]['validation'];
  $form['accompanist']['previous_students'] = array(
    '#type' => 'select',
    '#title' => t('Accompanist'),
    '#description' => t('Optional. Click -Select- and find the accompanist. If you don\'t see your accompanist here, click on "My students" in the main menu bar and create an accompanist. Accompanists are created as "students".'),
    '#options' => _mlmta_student_build_list('basic_student'),
    '#empty_option' => '- None -',
    '#empty_value' =>'_none',
    '#ajax' => array(
      'callback' => '_mlmta_accompanist_prepopulate',
      'wrapper' => 'student-2-fieldset',
    ),
  );
  
  $student = new stdClass();
        
  if (!empty($form_state['triggering_element']['#name']) && $form_state['triggering_element']['#name'] == 'accompanist[previous_students]') {
    // Form rebuilt triggered by ajax call
    // Triggered by the student select ajax
    // Selected an existing student
    if ($form_state['values']['accompanist']['previous_students'] != '_none') {
      $student = student_load($form_state['values']['accompanist']['previous_students']);      
    } else {
      $student = new stdClass();
    }

  } else {
    // NOT triggered by ajax
    // Check if editing a pursuit
    if (!empty($form_state['pursuit'])) {
      if (!empty($form_state['pursuit']->field_accompanist['und'][0]['target_id'])) {
        $student = student_load($form_state['pursuit']->field_accompanist['und'][0]['target_id']);
        $form['accompanist']['previous_students']['#default_value'] = $form_state['pursuit']->field_accompanist['und'][0]['target_id'];
        //$form['accompanist']['judged_accompanist']['#default_value'] = empty($form_state['pursuit']->field_judged_accompanist['und'][0]['value']) ? 0 : 1; 
      }
    } 
  }
  
  
  // Let's check if we've been here before. If so, we need to reset the #default_values
  if (!empty($form_state['visited']) && $form_state['visited']) {
    // Form is being revisited
    $stored_values = &$form_state['step_information'][$current_step]['stored_values']['accompanist'];
    $form['accompanist']['previous_students']['#default_value'] = $stored_values['previous_students'];
//    $form['accompanist']['judged_accompanist']['#default_value'] = $stored_values['judged_accompanist'];
    $student = student_load($stored_values['previous_students']);
  }

  $student_rendered = 'None yet. Click -Select- above to select an accompanist.';
  if (!empty($student->name)) {
    $student_view_array = entity_view('student', array($student), 'for_pursuit');
    $student_rendered = drupal_render($student_view_array);
  }
  $form['accompanist']['student_fieldset'] = array(
    '#markup' => $student_rendered,
    '#prefix' => '<fieldset id="student-2-fieldset"><legend>Selected accompanist</legend><div class="fieldset-wrapper">',
    '#suffix' => '</div></fieldset>',
    //'#tree' => TRUE,
   );
  
  //$form['accompanist']['judged_accompanist'] = array(
  //  '#type' => 'checkbox',
  //  '#title' => 'Judged accompanist',
  //  '#description' => t('Indicate whether this accompanist will be judged.'),
  //  '#default_value' => 0,
  //  '#weight' => 62,
  //);

  //****  Teacher 2
  
  // We are not providing the option of judged accompanist for this system, therefore the teacher step is not necessary
  
  //
  //$form['accompanist']['previous_teachers'] = array(
  //  '#type' => 'select',
  //  '#title' => t('Teacher'),
  //  '#options' => _mlmta_teacher_build_list('basic_teacher'),
  //  '#weight' => 64,
  //  '#size' => '100',
  //  '#states'=> array(
  //    'visible' => array(
  //      ':input[name="accompanist[judged_accompanist]"]' => array('checked' => TRUE),
  //    ),
  //  ),
  //);
  
  //if (!empty($form_state['pursuit'])) {
  //  if (!empty($form_state['pursuit']->field_judged_accompanist['und'][0]['value'])) {
  //    $form['accompanist']['previous_teachers']['#default_value'] = $form_state['pursuit']->pursuit_teacher_reference['und'][$delta]['target_id'];
  //  }
  //} elseif (!empty($current_user->field_user_teacher_reference['und'][0]['target_id'])) {
  //  // This is reached when the triggering element is not ajax and there is no pursuit attached, and the user references a teacher      
  //  $teacher_id_for_form =  $current_user->field_user_teacher_reference['und'][0]['target_id'];
  //  if ($teacher_id_for_form) {
  //    $teacher = teacher_load($teacher_id_for_form);
  //    $form['accompanist']['previous_teachers']['#default_value'] = $teacher_id_for_form;
  //  }
  //}

  // Turn off visited flag (next or previous buttons will turn it on if necessary)
  $form_state['visited'] = FALSE;
  return $form;
}

/**
 * Validation for accompanist form 
 */
function mlmta_recital_ensemble_accompanist_process($form, &$form_state) {
  // Ajax elements will trigger this, but we only want to process if next_submit is clicked 
  if ($form_state['triggering_element']['#type'] == 'submit' && !empty($form_state['values']['accompanist']['student_fieldset']['student_first_name']['und'][0]['value'])) {
    
  }
}

/**
 * Submit hook for accompanist
 */
function mlmta_recital_ensemble_accompanist_submit_hook($form, &$form_state) {
  

}
/**
 * Form builder for pieces
 */
function mlmta_recital_ensemble_pieces($form, &$form_state) {

  $current_step = &$form_state['step'];
  $form['#validate'] = $form_state['step_information'][$current_step]['validation'];
  
  $pursuit_temp = empty($form_state['pursuit']) ? entity_create('pursuit', array('type'=>'recital_ensemble')) : $form_state['pursuit'];
  
  
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

  $piece_array = array();
  for ($i = 0; $i<1; $i++) {
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
    
    
    unset($form_state['input']['field_piano_solo_level']);
    $form['field_piano_solo_level'][LANGUAGE_NONE]['#default_value'] = $stored_values['field_piano_solo_level'][LANGUAGE_NONE][0]['value'];
    
    //unset($form_state['input']['pursuit_preferred_time']);
    //$form['pursuit_preferred_time'][LANGUAGE_NONE]['#default_value'] = $stored_values['pursuit_preferred_time'][LANGUAGE_NONE][0]['value'];
  }

  $form_state['visited'] = FALSE;
  return $form;
}


/**
 *  Validation for pieces
 */
function mlmta_recital_ensemble_pieces_process($form, &$form_state) {
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
function mlmta_recital_ensemble_pieces_submit_hook($form, &$form_state) {
  // Creates taxonomy terms for new composers
  if ($form_state['values']['pieces']['piece1_fieldset']['piece_composer']['und'][0]['tid'] == 'autocreate') {
    unset($form_state['values']['pieces']['piece1_fieldset']['piece_composer']['und'][0]['tid']);
    $term = (object) $form_state['values']['pieces']['piece1_fieldset']['piece_composer']['und'][0];
    taxonomy_term_save($term);
    $form_state['values']['pieces']['piece1_fieldset']['piece_composer']['und'][0]['tid'] = $term->tid;
  }
}

/**
 * Submit handler
 */
function mlmta_recital_ensemble_form_submit($form, &$form_state) {
  $current_step = &$form_state['step'];
  if (!empty($form_state['step_information'][$current_step]['submit hook'])) {
    $form_state['step_information'][$current_step]['submit hook']($form, $form_state);
  }
  $form_state['step_information'][$current_step]['stored_values'] = $form_state['values'];
 
  $student_count_step = 1;
  $student_count = $form_state['step_information'][$student_count_step]['stored_values']['student_count'];
  $accompanist_step = $student_count + 2;
  $pieces_step = $student_count + 3;

  $student_ids = array();
  $teacher_ids = array();
  $theory_levels = array();
  $theory_tests = array();
  $instruments = array();
  
  //  Loop through students first. First student is form step #2
  for ($step = 2; $step < $student_count + 2; $step++) {
    $delta = $form_state['step_information'][$step]['delta'];
    $stored_values = $form_state['step_information'][$step]['stored_values']['student'][$delta];
    $student_ids[$delta] = $stored_values['previous_students'];
   // Get teacher  id
    $teacher_ids[$delta]= $stored_values['previous_teachers'];
    $instruments[$delta] = $stored_values['instrument'];

  }
 
  // Create and save accompanist if needed
  $stored_values = $form_state['step_information'][$accompanist_step]['stored_values']['accompanist'];
  
  if ($stored_values['previous_students'] != '_none') {
    $accompanist_id = $stored_values['previous_students'];
    // $accompanist_teacher_id = $stored_values['previous_teachers'];    
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
  
  //// Get value for preferred time
  //$preferred_time = $form_state['step_information'][$pieces_step]['stored_values']['pursuit_preferred_time']['und'][0]['value'];

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
  $pursuit_wrapper->field_instrument_list = array();
  $pursuit_wrapper->field_piano_solo_level = '';
  $pursuit_wrapper->pursuit_teacher_reference = NULL;
  
  foreach($student_ids as $delta => $student_id) {
    $pursuit_wrapper->pursuit_student_reference[$delta] = $student_id;
    $pursuit_wrapper->pursuit_teacher_reference[$delta] = $teacher_ids[$delta];
    $pursuit_wrapper->field_instrument_list[$delta] = $instruments[$delta];
  }
  
  if(!empty($accompanist_id)) {
    $pursuit_wrapper->field_accompanist = $accompanist_id;
    //if ($form_state['step_information'][$accompanist_step]['stored_values']['accompanist']['judged_accompanist'] == 1) {
    //  $pursuit_wrapper->pursuit_teacher_reference[] = $accompanist_teacher_id;
    //} 
  } else {
    $pursuit_wrapper->field_accompanist = NULL;
  }

  $pursuit_wrapper->pursuit_piece_reference->set(array());
  foreach($piece_array as $delta => $piece) {
    $pursuit_wrapper->pursuit_piece_reference[$delta]->set($piece->piece_id);
  }

  //$pursuit_wrapper->pursuit_preferred_time->set($preferred_time);
  $pursuit_wrapper->field_piano_solo_level->set($level);
  //$pursuit_wrapper->field_judged_accompanist = $form_state['step_information'][$accompanist_step]['stored_values']['accompanist']['judged_accompanist'];
  $pursuit_wrapper->field_event_reference->set(variable_get('tenuto_recital_event', NULL)); // solo because this is part of the DSF event, which has the same info
  
  pursuit_update_name($pursuit);
  pursuit_update_duration($pursuit);
  pursuit_save($pursuit);
  
  
  // IF this pursuit is new then add to commerce cart
  
  if (empty($form_state['pursuit'])) {
    switch ($student_count) {
      case 1:
        $sku = 'recital';
        break;
      case 2:
        $sku = 'recital-duo';
        break;
      case 3:
        $sku = 'recital-trio';
        break;
      case 4:
        $sku = 'recital-quartet';
        break;
    }
    pursuit_add_to_shopping_cart(array($pursuit), $sku);
    mlmta_recital_check_teacher_limits($pursuit);
  } 
}

