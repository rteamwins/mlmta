<?php

/**
 * Generates the student type editing form.
 */
function student_type_form($form, &$form_state, $student_type, $op = 'edit') {

  if ($op == 'clone') {
    $student_type->label .= ' (cloned)';
    $student_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $student_type->label,
    '#description' => t('The human-readable name of this student type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($student_type->type) ? $student_type->type : '',
    '#maxlength' => 32,
    '#disabled' => $student_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'student_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this student type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#default_value' => isset($student_type->description) ? $student_type->description : '',
    '#description' => t('Description about the student type.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save student type'),
    '#weight' => 40,
  );

  if (!$student_type->isLocked() && $op != 'add' && $op != 'clone') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete student type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('student_type_form_submit_delete')
    );
  }
  return $form;
}

/**
 * Submit handler for creating/editing student_type.
 */
function student_type_form_submit(&$form, &$form_state) {
  $student_type = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  $op = student_type_save($student_type);
  if ($op == SAVED_NEW) {
    // Notify Field API of new bundle
    field_attach_create_bundle('student', $student_type->type);
    
    // Add default fields to piece bundle
    student_add_default_fields($student_type);
  }

  // Redirect user back to list of student types.
  $form_state['redirect'] = 'admin/structure/scheduling/student-types';
}

function student_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/scheduling/student-types/' . $form_state['student_type']->type . '/delete';
}

/**
 * Student type delete form.
 */
function student_type_form_delete_confirm($form, &$form_state, $student_type) {
  $form_state['student_type'] = $student_type;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['student_type_id'] = array('#type' => 'value', '#value' => entity_id('student_type' ,$student_type));
  return confirm_form($form,
    t('Are you sure you want to delete student type %title?', array('%title' => entity_label('student_type', $student_type))),
    'student/' . entity_id('student_type' ,$student_type),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Student type delete form submit handler.
 */
function student_type_form_delete_confirm_submit($form, &$form_state) {
  $student_type = $form_state['student_type'];
  student_type_delete($student_type);

  watchdog('student_type', '@type: deleted %title.', array('@type' => $student_type->type, '%title' => $student_type->label));
  drupal_set_message(t('@type %title has been deleted.', array('@type' => $student_type->type, '%title' => $student_type->label)));

  $form_state['redirect'] = 'admin/structure/scheduling/student-types';
}

/**
 * Page to select student Type to add new student.
 */
function student_admin_add_page() {
  $items = array();
  foreach (student_types() as $student_type_key => $student_type) {
    $items[] = l(entity_label('student_type', $student_type), 'student/add/' . $student_type_key);
  }
  return array('list' => array('#theme' => 'item_list', '#items' => $items, '#title' => t('Select type of student to create.')));
}

/**
 * Add new student page callback.
 */
function student_add($type) {
  $student_type = student_types($type);

  $student = entity_create('student', array('type' => $type));
  drupal_set_title(t('Create @name', array('@name' => entity_label('student_type', $student_type))));

  $output = drupal_get_form('student_form', $student);

  return $output;
}

/**
 * Student Form.
 *
 */
function student_form($form, &$form_state, $student) {
  $form_state['student'] = $student;
  // Don't need this as this is filled in with first and last name on presave
  //$form['name'] = array(
  //  '#type' => 'textfield',
  //  '#required' => TRUE,
  //  '#title' => t('Name of the student'),
  //  '#default_value' => $student->name,
  //);

  if (isset($form_state['same_name_confirm_flag']) && $form_state['same_name_confirm_flag'] == TRUE) {
    $form['same_name_confirm'] = array(
      '#type' => 'checkbox',
      '#title' => t('Yes, I would like to create this student.'),
      '#description' => t('The system has found a student with the same first and last names. Please notice that having duplicate records for the same student WILL cause issues for future registrations and point tracking.Click <i>Yes</i> at your own risk!'),
      '#default_value' => FALSE,
    );
  }
  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $student->uid,
  );
  
  field_attach_form('student', $student, $form, $form_state);
  
  // Hide fields if they are set to be hidden (except for admin roles)
  $admin_role = variable_get('user_admin_role', 0);
  foreach (element_children($form) as $field_name) {
    $info = field_info_instance('student', $field_name, $form['#bundle']);
    if (!user_has_role($admin_role) && !empty($info['tenuto_settings']['hide_in_form']) && $info['tenuto_settings']['hide_in_form']) {
      $form[$field_name]['#access'] = FALSE;
    }
  }
  
  // Always hide the teacher reference field for normal users, regardless of hide in form flag
  if (isset($form['student_teacher_references']) && !user_access('edit any student entities')) {
    $form['student_teacher_references']['#access'] = FALSE;
  }
  $submit = array();
  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions'] = array(
    '#weight' => 100,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save student'),
    '#submit' => $submit + array('student_form_submit'),
  );

  // Show Delete button if we edit student.
  $student_id = entity_id('student' ,$student);
  if (!empty($student_id) && student_access('edit', $student)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('student_form_submit_delete'),
    );
  }

  $form['#validate'][] = 'student_form_validate';

  return $form;
}

function student_form_validate($form, &$form_state) {

}

/**
 * Student submit handler.
 */
function student_form_submit($form, &$form_state) {
    // Check if a student with the same name exists and warn the user. This is here rather than in validate function because I need to pass values back to the form
  $values = &$form_state['values'];
  $first_name = $values['student_first_name']['und'][0]['value'];
  $last_name = $values['student_last_name']['und'][0]['value'];
  if (!isset($values['same_name_confirm'])) {
    if (isset($form_state['student']->is_new) && $form_state['student']->is_new) {
      $q = new EntityFieldQuery();
      $q->entityCondition('entity_type', 'student')
        ->fieldCondition('student_first_name', 'value', ucwords(trim($first_name)))
        ->fieldCondition('student_last_name', 'value', ucwords(trim($last_name)));
      $r = $q->execute();
      if(!empty($r['student'])) {
        $student_ids = array_keys($r['student']);
        $w = entity_metadata_wrapper('student', $student_ids[0]);
        $teachers_array = array();
        foreach ($w->student_teacher_references->getIterator() as $teacher) {
          $teachers_array[] = $teacher->label();
        }
        $teachers = empty($teachers_array) ? 'None' : implode(', ', $teachers_array);
        $form_state['rebuild'] = TRUE;
        $form_state['same_name_confirm_flag'] = TRUE;
        drupal_set_message(t('A student with the same name as the one you are trying to create (%name) already exists in the system, linked to the teacher(s) %teacher. Click <a href="@cancel">cancel</a> to cancel and go back. If you still want to create this student, click "Yes" below.',array('%cancel' => '/student-front', '%name' => $w->label(), '%teacher' => $teachers)), 'error'); // ADd name etc
        return;
      } 
    }
  } elseif (!$values['same_name_confirm']) {
    drupal_set_message('Student not created.', 'warning');
    drupal_goto('student-front');
  }
  
  global $user;
  $user_wrapper = entity_metadata_wrapper('user', $user->uid);
  $teacher_id = $user_wrapper->field_user_teacher_reference->getIdentifier();

  // Automatically link to current teacher account
  $student = $form_state['student'];
  if (empty($form_state['values']['student_teacher_references']['und'][0]['target_id']) && !empty($user_wrapper->field_user_teacher_reference->raw())) {
    $form_state['values']['student_teacher_references']['und'][0]['target_id'] = $user_wrapper->field_user_teacher_reference->raw();
  }
  $values['student_first_name']['und'][0]['value'] = ucwords(trim($values['student_first_name']['und'][0]['value']));
  $values['student_last_name']['und'][0]['value'] = ucwords(trim($values['student_last_name']['und'][0]['value'])); 

  entity_form_submit_build_entity('student', $student, $form, $form_state);
  
  student_save($student);

  $student_uri = entity_uri('student', $student);

  $form_state['redirect'] = $student_uri['path'];

  drupal_set_message(t('Student %title saved.', array('%title' => entity_label('student', $student))));
  
}

function student_form_submit_delete($form, &$form_state) {
  $student = $form_state['student'];
  $student_uri = entity_uri('student', $student);
  $form_state['redirect'] = array($student_uri['path'] . '/delete', array('query'=>drupal_get_destination()));
  
  // See https://drupal.stackexchange.com/questions/5440/form-redirect-not-working-if-destination-is-in-url
  unset($_GET['destination']);
  drupal_static_reset('drupal_get_destination');
  drupal_get_destination();
}

/**
 * Delete confirmation form.
 */
function student_delete_form($form, &$form_state, $student) {
  $form_state['student'] = $student;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['student_type_id'] = array('#type' => 'value', '#value' => entity_id('student' ,$student));
  $student_uri = entity_uri('student', $student);
  return confirm_form($form,
    t('Are you sure you want to delete student %title?', array('%title' => entity_label('student', $student))),
    $student_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete form submit handler.
 */
function student_delete_form_submit($form, &$form_state) {
  $student = $form_state['student'];
  student_delete($student);

  drupal_set_message(t('Student %title deleted.', array('%title' => entity_label('student', $student))));

  $form_state['redirect'] = '<front>';
}
