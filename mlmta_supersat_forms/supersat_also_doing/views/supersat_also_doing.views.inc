<?php

/**
 * Implements hook_views_data_alter().
 */
function supersat_also_doing_views_data_alter(&$data) {
  $data['pursuit']['kits_also_doing'] = array(
    'title' => t('Also doing (KITS)'),
    'help' => t('Displays other KITS pursuits by the student(s) in this pursuit. Made for use in connection to Super Saturady Master Class pursuits.'),
    'real field' => 'pursuit_id',
    'field' => array(
      'handler' => 'supersat_kits_also_doing_handler_field_kits',
    ),
  );
  $data['pursuit']['playathon_also_doing'] = array(
    'title' => t('Also doing (Playathon)'),
    'help' => t('Displays other Playathon pursuits by the student(s) in this pursuit. Made for use in connection to Super Saturady Master Class pursuits.'),
    'real field' => 'pursuit_id',
    'field' => array(
      'handler' => 'supersat_playathon_also_doing_handler_field_playathon',
    ),
  );
}

