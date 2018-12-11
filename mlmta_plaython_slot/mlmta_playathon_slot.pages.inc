<?php

/**
 * PlayathonSlot view callback.
 */
function mlmta_playathon_slot_view($mlmta_playathon_slot) {
  drupal_set_title(entity_label('mlmta_playathon_slot', $mlmta_playathon_slot));
  return entity_view('mlmta_playathon_slot', array(entity_id('mlmta_playathon_slot', $mlmta_playathon_slot) => $mlmta_playathon_slot), 'full');
}
