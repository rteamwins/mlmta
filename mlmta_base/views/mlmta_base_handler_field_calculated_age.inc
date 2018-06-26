<?php

// THIS   ended up not being needed, as I defined the calculated age through a computed entity property

class mlmta_base_handler_field_calculated_age extends views_handler_field {
  
  function construct() {
    parent::construct();
    $this->additional_fields['calculated_age'] = array(
      'table' => 'field_data_student_birthday',
      'field' => 'student_birthday_value',
    );
  }
  
  /**
   * Loads additional fields.
   */
  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }
  
  /**
  * Render the name field.
  */
  public function render($values) {
    $value = $this->get_value($values, 'calculated_age');
    $diff = 'Unknown';
    if (!empty($value)) {
      $active_event_id = event_switch_get_active_event(); 
      $bday = new DateTime($value);
      $event = event_load($active_event_id);
      $cutoff = date('Y-m-d' , $event->cutoff);
      $cutoffdt = new DateTime($cutoff);
      $age = $cutoffdt->diff($bday);
      $diff = $age->y;
    }

    return $diff;

  }
}