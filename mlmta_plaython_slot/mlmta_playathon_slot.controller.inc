<?php

class PlayathonSlotController extends EntityAPIController {

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
    //$wrapper = entity_metadata_wrapper('mlmta_playathon_slot', $entity);
    // $content['author'] = array('#markup' => t('Created by: !author', array('!author' => $wrapper->uid->name->value(array('sanitize' => TRUE)))));

    $extra_fields = field_extra_fields_get_display('mlmta_playathon_slot', $entity->type, $view_mode);
    
    // Make mlmta_playathon_slot id themed like default field
    if (!empty($extra_fields['mlmta_playathon_slot_id'] && $extra_fields['mlmta_playathon_slot_id']['visible'])) {
      $content['mlmta_playathon_slot_id'] = array(
        '#theme' => 'field',
        '#weight' => $extra_fields['mlmta_playathon_slot_id']['weight'],
        '#title' =>t('PlayathonSlot ID'),
        '#access' => TRUE,
        '#label_display' => 'inline',
        '#view_mode' => $view_mode,
        '#language' => LANGUAGE_NONE,
        '#field_name' => 'field_fake_mlmta_playathon_slot_id',
        '#field_type' => 'number_integer',
        '#entity_type' => 'mlmta_playathon_slot',
        '#bundle' => $entity->type,
        '#items' => array(array('value' => $entity->mlmta_playathon_slot_id)),
        '#formatter' => 'text_default',
        0 => array('#markup' => check_plain($entity->mlmta_playathon_slot_id))
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
    //  '#entity_type' => 'mlmta_playathon_slot',
    //  '#bundle' => $entity->type,
    //  '#items' => array(array('value' => $entity->description)),
    //  '#formatter' => 'text_default',
    //  0 => array('#markup' => check_plain($entity->description))
    //);

    return parent::buildContent($entity, $view_mode, $langcode, $content);
  }
  
  //public function save($mlmta_playathon_slot, DatabaseTransaction $transaction = NULL){
  //    $mlmta_playathon_slot->name = isset($mlmta_playathon_slot->mlmta_playathon_slot_first_name[LANGUAGE_NONE][0]) ? $mlmta_playathon_slot->mlmta_playathon_slot_first_name[LANGUAGE_NONE][0]['value'] : 'unknown';
  //    $mlmta_playathon_slot->name .= isset($mlmta_playathon_slot->mlmta_playathon_slot_last_name[LANGUAGE_NONE][0]) ? ' ' .$mlmta_playathon_slot->mlmta_playathon_slot_last_name[LANGUAGE_NONE][0]['value'] : ' unknown';
  //    return parent::save($mlmta_playathon_slot);
  //}
}

class PlayathonSlotTypeController extends EntityAPIControllerExportable {
   public function create(array $values = array()) {
    $values += array(
      'label' => '',
      'description' => '',
    );
    return parent::create($values);
  }

  /**
   * Save PlayathonSlot Type.
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
 * UI controller for PlayathonSlot Type.
 */
class PlayathonSlotTypeUIController extends EntityDefaultUIController {
  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage PlayathonSlot types.';
    return $items;
  }
}

/**
 * PlayathonSlot class.
 */
class PlayathonSlot extends Entity {
  protected function defaultLabel() {
    return $this->name;
  }

  protected function defaultUri() {
    return array('path' => 'mlmta-playathon-slot/' . $this->identifier());
  }
}

/**
 * PlayathonSlot Type class.
 */
class PlayathonSlotType extends Entity {
  public $type;
  public $label;
  public $weight = 0;

  public function __construct($values = array()) {
    parent::__construct($values, 'mlmta_playathon_slot_type');
  }

  function isLocked() {
    return isset($this->status) && empty($this->is_new) && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }
}
