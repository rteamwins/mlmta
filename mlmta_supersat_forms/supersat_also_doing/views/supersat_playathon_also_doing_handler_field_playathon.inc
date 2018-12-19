<?php

class supersat_playathon_also_doing_handler_field_playathon extends views_handler_field {
  /**
  * Render the name field.
  */
  public function render($values) {
    $my_pursuit_id = $this->get_value($values);
    $w = entity_metadata_wrapper('pursuit', $my_pursuit_id);
    $fieldset_content = array();
    $pursuit_dates = array();
    $active_event = event_switch_get_active_event();
    // First iterate through all the pursuit_student_reference references (this is an unlimited cardinality field)
    foreach ($w->pursuit_student_reference->getIterator() as $source_student_wrapper) {
      if (isset($source_student_wrapper->student_pursuit_reference)) {
        foreach ($source_student_wrapper->student_pursuit_reference->getIterator() as $targetted_pursuit_wrapper) {
          $targetted_pursuit_id = $targetted_pursuit_wrapper->getIdentifier();
          if ($targetted_pursuit_id != $my_pursuit_id && $targetted_pursuit_wrapper->getBundle() == 'playathon') {
            if (empty($targetted_pursuit_wrapper->value()) ||
                empty($targetted_pursuit_wrapper->pursuit_order_paid->value()) ||
                !$targetted_pursuit_wrapper->pursuit_order_paid->value() ||
                !$targetted_pursuit_wrapper->pursuit_active->value() ||
                $targetted_pursuit_wrapper->field_event_reference->raw() != $active_event // Only include current events
                ) continue;
            $time = array();
            $timestamp1 = $timestamp2 = '' ;
            if(!(empty($targetted_pursuit_wrapper->field_playathon_slots->value()))) {
              foreach ($targetted_pursuit_wrapper->field_playathon_slots->getIterator() as $playathon_slot) {
                $timestamp1 = $playathon_slot->mlmta_playathon_slot_time->value->value();
                $timestamp2 = $playathon_slot->mlmta_playathon_slot_time->value2->value();
                $time[] = format_date($timestamp1, 'time') . ' - ' . format_date($timestamp2, 'time');
              }
            }
            $time_string = implode('<br>', $time);
            $targetted_pursuit_id = $targetted_pursuit_wrapper->getIdentifier();
            $fieldset_content[$targetted_pursuit_id] =
              substr(scheduling_bundle_label('pursuit', $targetted_pursuit_wrapper->type->value()), 0, 10) .
              '.. (Id '. $targetted_pursuit_id . ') ' . '<br>'.$time_string;
            $pursuit_dates[$targetted_pursuit_id] = $targetted_pursuit_id;
          }
        }
      }
    }

    asort($pursuit_dates);
    $properly_sorted_content = '';
    $properly_sorted_content = implode('<br>', array_replace(array_flip(array_keys($pursuit_dates)), $fieldset_content));
    $output = '' ;
    if (!empty($properly_sorted_content)) {
      $fieldset = array(
        '#type'=>'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#parents' => array(),
        'content' => array('#markup' => $properly_sorted_content),
        '#title'=> 'Playathon',
        '#attributes' => array(
          'class' => array(
            'scheduling-table-fieldset',
          ),
        ),
      );
      $dummy_form_state = array();
      form_process_fieldset($fieldset, $dummy_form_state);
      $output = drupal_render($fieldset);    
    }   
    return $output;
  }
}