<?php

class StudentController extends EntityAPIController {

  public function create(array $values = array()) {
    global $user;
    $values += array(
      'name' => '',
      'description' => '',
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
      'uid' => $user->uid,
    );
    return parent::create($values);
  }

  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    //$wrapper = entity_metadata_wrapper('student', $entity);
    // $content['author'] = array('#markup' => t('Created by: !author', array('!author' => $wrapper->uid->name->value(array('sanitize' => TRUE)))));

    $extra_fields = field_extra_fields_get_display('student', $entity->type, $view_mode);
    
    // Make student id themed like default field
    if (!empty($extra_fields['student_id'] && $extra_fields['student_id']['visible'])) {
      $content['student_id'] = array(
        '#theme' => 'field',
        '#weight' => $extra_fields['student_id']['weight'],
        '#title' =>t('Student ID'),
        '#access' => TRUE,
        '#label_display' => 'inline',
        '#view_mode' => $view_mode,
        '#language' => LANGUAGE_NONE,
        '#field_name' => 'field_fake_student_id',
        '#field_type' => 'number_integer',
        '#entity_type' => 'student',
        '#bundle' => $entity->type,
        '#items' => array(array('value' => $entity->student_id)),
        '#formatter' => 'text_default',
        0 => array('#markup' => check_plain($entity->student_id))
      );        
    }
 
    // Make Description and Status themed like default fields.
    //$content['description'] = array(
    //  '#theme' => 'field',
    //  '#weight' => 0,
    //  '#title' =>t('Description'),
    //  '#access' => TRUE,
    //  '#label_display' => 'above',
    //  '#view_mode' => 'full',
    //  '#language' => LANGUAGE_NONE,
    //  '#field_name' => 'field_fake_description',
    //  '#field_type' => 'text',
    //  '#entity_type' => 'student',
    //  '#bundle' => $entity->type,
    //  '#items' => array(array('value' => $entity->description)),
    //  '#formatter' => 'text_default',
    //  0 => array('#markup' => check_plain($entity->description))
    //);

    return parent::buildContent($entity, $view_mode, $langcode, $content);
  }
  
  public function save($student, DatabaseTransaction $transaction = NULL){
      $student->name = isset($student->student_first_name[LANGUAGE_NONE][0]) ? $student->student_first_name[LANGUAGE_NONE][0]['value'] : 'unknown';
      $student->name .= isset($student->student_last_name[LANGUAGE_NONE][0]) ? ' ' .$student->student_last_name[LANGUAGE_NONE][0]['value'] : ' unknown';
      return parent::save($student);
  }
}

class StudentTypeController extends EntityAPIControllerExportable {
   public function create(array $values = array()) {
    $values += array(
      'label' => '',
      'description' => '',
    );
    return parent::create($values);
  }

  /**
   * Save Student Type.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    // Rebuild menu registry. We do not call menu_rebuild directly, but set
    // variable that indicates rebuild in the end.
    // @see http://drupal.org/node/1399618
    variable_set('menu_rebuild_needed', TRUE);
    return parent::save($entity, $transaction);
  }
}

/**
 * UI controller for Student Type.
 */
class StudentTypeUIController extends EntityDefaultUIController {
  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage Student types.';
    return $items;
  }
}

/**
 * Student class.
 */
class Student extends Entity {
  protected function defaultLabel() {
    return $this->name;
  }

  protected function defaultUri() {
    return array('path' => 'student/' . $this->identifier());
  }
}

/**
 * Student Type class.
 */
class StudentType extends Entity {
  public $type;
  public $label;
  public $weight = 0;

  public function __construct($values = array()) {
    parent::__construct($values, 'student_type');
  }

  function isLocked() {
    return isset($this->status) && empty($this->is_new) && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }
}
