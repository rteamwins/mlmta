<?php

function mlmta_supersat_kits_form($form, &$form_state, $pursuit = NULL) {
  global $user;
  $current_user = user_load($user->uid);
  if (empty($form_state['build_info']['files'])) {
    form_load_include($form_state, 'inc', 'mlmta_supersat_forms', 'mlmta_supersat_kits');  
  }
  if (!$pursuit) {
    $pursuit = entity_create('pursuit', array('type'=>'kits'));
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
  
  $notice = <<<EOT
The KITS schedule is FIXED as follows:
<table>
<tr><th>Time</th><th>Reading</th><th>Listening</th><th>Playing</th></tr>
<tr><td>1:00 PM</td><td>Step 1</td><td>Step 3</td><td>Step 2</td></tr>
<tr><td>1:30 PM</td><td>Step 2</td><td>Step 1</td><td>Step 3</td></tr>
<tr><td>2:00 PM</td><td>Step 3</td><td>Step 2</td><td>Step 1</td></tr>
<tr><td>3:00 PM</td><td>Steps 4, 5, 6</td><td>&nbsp;</td><td>&nbsp;</td></tr>
</table>
<p><strong>Primary level students</strong> may enter the Reading Room at any time after 12:45pm.</p>

<p><strong>Step 4, Step 5, and Step 6 students:</strong> Due to a very low number of students, all students are expected to arrive at the reading room <u>on time by 2:30pm</u> and begin work on the written portion of the exam. Then students will receive instruction on where to go for the listening and playing room.</p>

<p>Teachers, please make sure that your students know which level of exam they are going to take.</p>

<p>Note: All of the exam rooms are on the third floor of Swope School of Music, West Chester University.</p>
  
EOT;
  $form['notice'] = array(
    '#markup' => $notice,
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
    '#weight' => -99,
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
    '#weight' => -98,
   );
  
  // Teacher
  
  if (!empty($pursuit_wrapper->pursuit_teacher_reference->raw())) {
    $default_value = $pursuit_wrapper->pursuit_teacher_reference->raw();
  } else {
    $default_value = scheduling_get_teacher_from_user() ?
        scheduling_get_teacher_from_user():
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
    '#weight' => 0,
  );
  
  $form['teacher_fieldset']['teacher']['#default_value'] = $default_value;

  
  
  
  $form_temp = array();
  
  // Attach element for grade level
  field_attach_form('pursuit', $pursuit, $form_temp, $form_state, NULL, array("field_name" => 'field_grade'));
  $form['field_grade'] = $form_temp['field_grade'];
  $form['field_grade']['#weight'] = 31;
  $form['field_grade']['und']['#chosen'] = FALSE;
  
  // Attach element for picking level
  $form_temp = array();
  field_attach_form('pursuit', $pursuit, $form_temp, $form_state, NULL, array("field_name" => 'field_kits_level'));
  $form['field_kits_level'] = $form_temp['field_kits_level'];
  $form['field_kits_level']['#weight'] = 32;

  // Attach element for picking years in honor roll
  $form_temp = array();
  field_attach_form('pursuit', $pursuit, $form_temp, $form_state, NULL, array("field_name" => 'field_years_in_honor_roll'));
  $form['field_years_in_honor_roll'] = $form_temp['field_years_in_honor_roll'];
  $form['field_years_in_honor_roll']['#weight'] = 33;
  
  // Attach element for picking Declaration
  $form_temp = array();
  field_attach_form('pursuit', $pursuit, $form_temp, $form_state, NULL, array("field_name" => 'field_declaration_of_elegibility'));
  $form['field_declaration_of_elegibility'] = $form_temp['field_declaration_of_elegibility'];
  $form['field_declaration_of_elegibility']['#weight'] = 34;
  
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
function mlmta_supersat_kits_form_validate($form, $form_state) {
  if ($form_state['triggering_element']['#type'] == 'submit') {
    if ($form_state['values']['field_declaration_of_elegibility']['und'][0]['value'] != 1 ) {
      form_set_error('field_declaration_of_elegibility[und]','You must agree to the Declaration of Elegibility');
    }
  }
}

/**
 * Submit handler
 */
function mlmta_supersat_kits_form_submit($form, $form_state) {
  $pursuit = $form_state['pursuit'];
  $is_new = isset($pursuit->is_new) ? $pursuit->is_new : FALSE; 
  $pursuit_wrapper = entity_metadata_wrapper('pursuit', $pursuit);

  $student_id = $form_state['values']['student'];
  $teacher_id = $form_state['values']['teacher'];
  $grade = $form_state['values']['field_grade']['und'][0]['value'];
  $kits_level = $form_state['values']['field_kits_level']['und'][0]['value'];
  $years_honor = $form_state['values']['field_years_in_honor_roll']['und'][0]['value'];
  
  $pursuit_wrapper->pursuit_student_reference->set(array($student_id));
  $pursuit_wrapper->pursuit_teacher_reference->set(array($teacher_id));
  $pursuit_wrapper->field_kits_level = $kits_level;
  $pursuit_wrapper->field_grade = $grade;
  $pursuit_wrapper->field_years_in_honor_roll = $years_honor;
  $pursuit_wrapper->field_declaration_of_elegibility  = 1;
  $pursuit_wrapper->field_event_reference->set(variable_get('tenuto_supersat_event', NULL));

  pursuit_update_name($pursuit);
  pursuit_save($pursuit);
  if ($is_new) {
    pursuit_add_to_shopping_cart(array($pursuit), 'kits'); 
  }
}