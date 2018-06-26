<?php
// THIS   ended up not being needed, as I defined the calculated age through a computed entity property

/**
 * Implements hook_views_data_alter().
 */
function mlmta_base_views_data_alter(&$data) {
  $data['student']['calculated_age'] = array(
    'title' => t('Calculated age'),
    'help' => t('Calculates the age of the student based on the active event cutoff date.'),
    'group' => 'Student',
    'field' => array(
      'handler' => 'mlmta_base_handler_field_calculated_age',
    ),
  );
}

