<?php

/**
* @file
* Defines a registration form controller for piano registrations
*/

class CmfSoloFormController extends SchedulingDefaultFormController {
  
  /**
   * Override
   */
  public function defaultSettings() {
    $event = event_load(variable_get('tenuto_cmf_event')); 
    return array('event' => $event);
  }
  
  /**
   * Override
   */
  public function form_init($form, &$form_state, $pursuit) {
    $form_state['number_of_pieces'] = 2;
    $form_state['form_title'] = 'CMF form';
    $form_state['flags']['judged_accompanist'] = FALSE;
    $form_state['flags']['create_teacher'] = FALSE;
    $form_state['flags']['create_non_member_teacher'] = FALSE;
    $form_state['flags']['accompanist_needed'] = FALSE;
    $form_state['flags']['family_tag'] = TRUE;
    $form_state['agreement_fields'] = array();
    $form_state['redirect_when_done'] = 'cmf-forms';
    $form_state['grouping_fields'] = array(
      'field_grade' => array(
        'field_name' => 'field_grade',
      ),
      'field_piano_solo_level' => array(
        'field_name' => 'field_piano_solo_level',
      ),
      'pursuit_preferred_time' => array(
        'field_name'=>'pursuit_preferred_time',
      ),
    );
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
  protected function _pre_save_hook(&$pursuit) {
    $pursuit_wrapper = entity_metadata_wrapper('pursuit', $pursuit);
    $pursuit_wrapper->pursuit_instrument->set(array('Piano')); 
  }
  /**
   * Override
   */
  protected function _get_sku($pursuit) {
    return 'cmf';
  }
  
}

