<?php

/**
* @file
* Defines a registration form controller for piano registrations
*/

class DsfEnsembleFormController extends SchedulingEnsembleDefaultController {
  
  /**
   * Override
   */
  public function defaultSettings() {
    $event = event_load(variable_get('tenuto_dsf_event')); 
    return array('event' => $event);
  }
  
  /**
   * Override
   */
  public function form_init($form, &$form_state, $pursuit) {
    $form_state['number_of_pieces'] = 2;
    $form_state['form_title'] = 'DSF ensemble form';
    $form_state['flags']['judged_accompanist'] = FALSE;
    $form_state['flags']['create_teacher'] = FALSE;
    $form_state['flags']['create_non_member_teacher'] = FALSE;
    $form_state['flags']['accompanist_needed'] = TRUE;
    $form_state['flags']['family_tag'] = TRUE;
    $form_state['agreement_fields'] = array();
    $form_state['redirect_when_done'] = 'dsf-forms';
    $form_state['grouping_fields'] = array(
      'field_piano_solo_level' => array(
        'field_name' => 'field_piano_solo_level',
      ),
      'pursuit_preferred_time' => array(
        'field_name'=>'pursuit_preferred_time',
      ),
    );
    $form_state['multiple_distributed_fields'] = array(
      'field_name'=>'field_theory_test_type',
      'field_name'=>'field_theory_test_level',
      'field_name'=>'field_instrument_options',
    );
    $form_state['maximum_number_of_students'] = 4;
    
    parent::form_init($form, $form_state, $pursuit);

  }
  
 
  
  
  /**
   * Override
   */
  protected function _get_minimum_required_pieces($form_state) {
    return 2;
  }
  
  /**
   * Override
   */
  protected function _get_sku($pursuit) {
    $pursuit_wrapper = entity_metadata_wrapper('pursuit', $pursuit);
    $number_of_students = count($pursuit_wrapper->pursuit_student_reference->raw());
    switch ($number_of_students) {
      case 2:
        return 'duet';
        break;
      case 3:
        return 'trio';
        break;
      case 4:
        return 'quartet';
        break; 
    }
  }
}

