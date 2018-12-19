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
    '#markup' => '<p><strong>Please notice:</strong> A maximum of 4 students per teacher (80 minutes) can be registered through this form. Any attempt to register additional more than 4 will be rejected by the system. Please contact the chair if you wish to get permission to register more than four students.</p>'
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
    $default_value = !empty ($current_user->field_user_teacher_reference['und'][0]['target_id']) ?
        $current_user->field_user_teacher_reference['und'][0]['target_id'] :
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
  
  // Piece
  if ($is_new) {
    $piece = entity_create('piece', array('type' => 'basic_piece'));
  } else {
    $piece = $pursuit_wrapper->pursuit_piece_reference[0]->value();
  }
  $form_state['piece'] = $piece;
  $piece_wrapper = entity_metadata_wrapper('piece', $piece);
  
  $form['piece_fieldset'] = array(
    '#title' => t('Piece'),
    '#type' => 'fieldset',
    'title' => array(
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => 'Title',
      '#description' => t('Enter the title, key, opus number and movement. E.g. "Sonatina in C major, Op. 36 No. 1. Movement 1."'),
      '#weight' => -68,
      '#default_value' => $piece_wrapper->label(),
    ),
  );

  $form_temp = array();
  field_attach_form('piece', $piece, $form_temp, $form_state);
  unset($form_temp['#parents']);
  $form['piece_fieldset'] = $form['piece_fieldset'] + $form_temp;
  
   $form_temp = array();
  // Attach preferred time to form
  field_attach_form('pursuit', $pursuit, $form_temp, $form_state, NULL, array('field_name'=>'pursuit_preferred_time'));  
  $form['pursuit_preferred_time'] = $form_temp['pursuit_preferred_time'];
  $form['pursuit_preferred_time']['#weight'] = 50;
  
  
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
  if ($form_state['triggering_element']['#type'] == 'submit') {
    // Check if there are already 4 registered MC students in the system
    if (!user_access('register more than four students in master class')) {
      global $user;
      $current_user = user_load($user->uid);
      $teacher_id = $current_user->field_user_teacher_reference['und'][0]['target_id'];
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
  }
}

/**
 * Submit handler
 */
function mlmta_supersat_mc_form_submit($form, $form_state) {
  $pursuit = $form_state['pursuit'];
  $piece = $form_state['piece'];
  $piece_wrapper = entity_metadata_wrapper('piece', $piece);
  
  $piece->title = $form_state['values']['title'];
  $piece_wrapper->piece_composer = $form_state['values']['piece_composer']['und'][0]['tid'];
  $piece_wrapper->piece_duration = $form_state['values']['piece_duration']['und'][0]['value'];
  $piece_wrapper->save();
  $piece_id = $piece_wrapper->getIdentifier();
    
  $is_new = isset($pursuit->is_new) ? $pursuit->is_new : FALSE; 
  $pursuit_wrapper = entity_metadata_wrapper('pursuit', $pursuit);

  $student_id = $form_state['values']['student'];
  $teacher_id = $form_state['values']['teacher'];
  
  $pursuit_wrapper->pursuit_student_reference->set(array($student_id));
  $pursuit_wrapper->pursuit_teacher_reference->set(array($teacher_id));
  $pursuit_wrapper->pursuit_preferred_time = $form_state['values']['pursuit_preferred_time']['und'][0]['value'];
  $pursuit_wrapper->pursuit_piece_reference = array($piece_id);
  $pursuit_wrapper->field_event_reference = variable_get('tenuto_supersat_event', NULL);
  
  pursuit_update_name($pursuit);
  pursuit_update_duration($pursuit);
  pursuit_save($pursuit);
  if ($is_new) {
    pursuit_add_to_shopping_cart(array($pursuit), 'mc'); 
  }
  
  // Creates taxonomy terms for new composers
  if ($form_state['values']['piece_composer']['und'][0]['tid'] == 'autocreate') {
    unset($form_state['values']['piece_composer']['und'][0]['tid']);
    $term = (object) $form_state['values']['piece_composer']['und'][0];
    taxonomy_term_save($term);
    $form_state['values']['piece_composer']['und'][0]['tid'] = $term->tid;
  }
  $form_state['rebuild'] = TRUE;
}