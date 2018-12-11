<?php
/**
 * @file
 * Hooks provided by this module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Acts on student being loaded from the database.
 *
 * This hook is invoked during $student loading, which is handled by
 * entity_load(), via the EntityCRUDController.
 *
 * @param array $entities
 *   An array of $student entities being loaded, keyed by id.
 *
 * @see hook_entity_load()
 */
function hook_student_load(array $entities) {
  $result = db_query('SELECT pid, foo FROM {mytable} WHERE pid IN(:ids)', array(':ids' => array_keys($entities)));
  foreach ($result as $record) {
    $entities[$record->pid]->foo = $record->foo;
  }
}

/**
 * Responds when a $student is inserted.
 *
 * This hook is invoked after the $student is inserted into the database.
 *
 * @param Student $student
 *   The $student that is being inserted.
 *
 * @see hook_entity_insert()
 */
function hook_student_insert(Student $student) {
  db_insert('mytable')
    ->fields(array(
      'id' => entity_id('student', $student),
      'extra' => print_r($student, TRUE),
    ))
    ->execute();
}

/**
 * Acts on a $student being inserted or updated.
 *
 * This hook is invoked before the $student is saved to the database.
 *
 * @param Student $student
 *   The $student that is being inserted or updated.
 *
 * @see hook_entity_presave()
 */
function hook_student_presave(Student $student) {
  $student->name = 'foo';
}

/**
 * Responds to a $student being updated.
 *
 * This hook is invoked after the $student has been updated in the database.
 *
 * @param Student $student
 *   The $student that is being updated.
 *
 * @see hook_entity_update()
 */
function hook_student_update(Student $student) {
  db_update('mytable')
    ->fields(array('extra' => print_r($student, TRUE)))
    ->condition('id', entity_id('student', $student))
    ->execute();
}

/**
 * Responds to $student deletion.
 *
 * This hook is invoked after the $student has been removed from the database.
 *
 * @param Student $student
 *   The $student that is being deleted.
 *
 * @see hook_entity_delete()
 */
function hook_student_delete(Student $student) {
  db_delete('mytable')
    ->condition('pid', entity_id('student', $student))
    ->execute();
}

/**
 * Act on a student that is being assembled before rendering.
 *
 * @param $student
 *   The student entity.
 * @param $view_mode
 *   The view mode the student is rendered in.
 * @param $langcode
 *   The language code used for rendering.
 *
 * The module may add elements to $student->content prior to rendering. The
 * structure of $student->content is a renderable array as expected by
 * drupal_render().
 *
 * @see hook_entity_prepare_view()
 * @see hook_entity_view()
 */
function hook_student_view($student, $view_mode, $langcode) {
  $student->content['my_additional_field'] = array(
    '#markup' => $additional_field,
    '#weight' => 10,
    '#theme' => 'mymodule_my_additional_field',
  );
}

/**
 * Alter the results of entity_view() for students.
 *
 * @param $build
 *   A renderable array representing the student content.
 *
 * This hook is called after the content has been assembled in a structured
 * array and may be used for doing processing which requires that the complete
 * student content structure has been built.
 *
 * If the module wishes to act on the rendered HTML of the student rather than
 * the structured content array, it may use this hook to add a #post_render
 * callback. Alternatively, it could also implement hook_preprocess_student().
 * See drupal_render() and theme() documentation respectively for details.
 *
 * @see hook_entity_view_alter()
 */
function hook_student_view_alter($build) {
  if ($build['#view_mode'] == 'full' && isset($build['an_additional_field'])) {
    // Change its weight.
    $build['an_additional_field']['#weight'] = -10;

    // Add a #post_render callback to act on the rendered HTML of the entity.
    $build['#post_render'][] = 'my_module_post_render';
  }
}

/**
 * Acts on student_type being loaded from the database.
 *
 * This hook is invoked during student_type loading, which is handled by
 * entity_load(), via the EntityCRUDController.
 *
 * @param array $entities
 *   An array of student_type entities being loaded, keyed by id.
 *
 * @see hook_entity_load()
 */
function hook_student_type_load(array $entities) {
  $result = db_query('SELECT pid, foo FROM {mytable} WHERE pid IN(:ids)', array(':ids' => array_keys($entities)));
  foreach ($result as $record) {
    $entities[$record->pid]->foo = $record->foo;
  }
}

/**
 * Responds when a student_type is inserted.
 *
 * This hook is invoked after the student_type is inserted into the database.
 *
 * @param StudentType $student_type
 *   The student_type that is being inserted.
 *
 * @see hook_entity_insert()
 */
function hook_student_type_insert(StudentType $student_type) {
  db_insert('mytable')
    ->fields(array(
      'id' => entity_id('student_type', $student_type),
      'extra' => print_r($student_type, TRUE),
    ))
    ->execute();
}

/**
 * Acts on a student_type being inserted or updated.
 *
 * This hook is invoked before the student_type is saved to the database.
 *
 * @param StudentType $student_type
 *   The student_type that is being inserted or updated.
 *
 * @see hook_entity_presave()
 */
function hook_student_type_presave(StudentType $student_type) {
  $student_type->name = 'foo';
}

/**
 * Responds to a student_type being updated.
 *
 * This hook is invoked after the student_type has been updated in the database.
 *
 * @param StudentType $student_type
 *   The student_type that is being updated.
 *
 * @see hook_entity_update()
 */
function hook_student_type_update(StudentType $student_type) {
  db_update('mytable')
    ->fields(array('extra' => print_r($student_type, TRUE)))
    ->condition('id', entity_id('student_type', $student_type))
    ->execute();
}

/**
 * Responds to student_type deletion.
 *
 * This hook is invoked after the student_type has been removed from the database.
 *
 * @param StudentType $student_type
 *   The student_type that is being deleted.
 *
 * @see hook_entity_delete()
 */
function hook_student_type_delete(StudentType $student_type) {
  db_delete('mytable')
    ->condition('pid', entity_id('student_type', $student_type))
    ->execute();
}

/**
 * Define default student_type configurations.
 *
 * @return
 *   An array of default student_type, keyed by machine names.
 *
 * @see hook_default_student_type_alter()
 */
function hook_default_student_type() {
  $defaults['main'] = entity_create('student_type', array(
    // …
  ));
  return $defaults;
}

/**
 * Alter default student_type configurations.
 *
 * @param array $defaults
 *   An array of default student_type, keyed by machine names.
 *
 * @see hook_default_student_type()
 */
function hook_default_student_type_alter(array &$defaults) {
  $defaults['main']->name = 'custom name';
}
