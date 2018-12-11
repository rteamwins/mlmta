<?php

/**
 * Student view callback.
 */
function student_view($student) {
  drupal_set_title(entity_label('student', $student));
  return entity_view('student', array(entity_id('student', $student) => $student), 'full');
}

/**
 * Generates a detailed view of the student, including points if module is available
 * @param Student $student
 */
function student_detailed_view($student) {
  drupal_set_title(entity_label('student', $student));
  $output = entity_view('student', array(entity_id('student', $student) => $student), 'with_points');
  if (module_exists('point')) {
    // load the points view here
    // Add to $output
  }
  
  return $output;
  
}
