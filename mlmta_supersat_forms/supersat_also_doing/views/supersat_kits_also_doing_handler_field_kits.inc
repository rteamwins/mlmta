<?php

class supersat_kits_also_doing_handler_field_kits extends views_handler_field {
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
          if ($targetted_pursuit_id != $my_pursuit_id && $targetted_pursuit_wrapper->getBundle() == 'kits' ) {
            if (empty($targetted_pursuit_wrapper->value()) ||
                empty($targetted_pursuit_wrapper->pursuit_order_paid->value()) ||
                !$targetted_pursuit_wrapper->pursuit_order_paid->value() ||
                !$targetted_pursuit_wrapper->pursuit_active->value() ||
                $targetted_pursuit_wrapper->field_event_reference->raw() != $active_event // Only include current events
                ) continue;
            $time = '1-4PM';
            $targetted_pursuit_id = $targetted_pursuit_wrapper->getIdentifier();
            $fieldset_content[$targetted_pursuit_id] =
              substr(scheduling_bundle_label('pursuit', $targetted_pursuit_wrapper->type->value()), 0, 10) .
              '.. (Id '. $targetted_pursuit_id . ') ' . $time;
            $pursuit_dates[$targetted_pursuit_id] = $targetted_pursuit_id;
          }
        }
      }
    }
    
    asort($pursuit_dates);
    $properly_sorted_content = '';
    $properly_sorted_content = implode('<br>', array_replace(array_flip(array_keys($pursuit_dates)), $fieldset_content));

    if (!empty($properly_sorted_content) && count($fieldset_content) > 1) {
      $fieldset = array(
        '#type'=>'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#parents' => array(),
        'content' => array('#markup' => $properly_sorted_content),
        '#title'=> 'view',
        '#attributes' => array(
          'class' => array(
            'scheduling-table-fieldset',
          ),
        ),
      );
      $dummy_form_state = array();
      form_process_fieldset($fieldset, $dummy_form_state);
      $output = drupal_render($fieldset);    
    } else {
      $output = $properly_sorted_content;
    }    
    return $output;
  }
}