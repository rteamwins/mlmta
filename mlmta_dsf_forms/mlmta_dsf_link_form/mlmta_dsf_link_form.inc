<?php

/**
 * Form to enter bla bla
 */
function mlmta_dsf_link_form_form($form, &$form_state) {

  //$form['user'] = array(
  //  '#type' => 'textfield',
  //  '#title' => t('Registration ID'),
  //  //'#description' => t('If you don\'t know the registration ID of your student, please contact the teacher'),
  //  '#autocomplete_path' => 'dsf/links/autocomplete_callback',
  //);
  
  $form['link_intro_text'] = array(
    '#markup' => "<strong>Directions:</strong><ol><li>Upload your 2 videos, 2 musical scores (PDF), and theory test PDF to a cloud service like Google Drive, Dropbox or others (videos may instead be uploaded to Youtube or Vimeo if you wish)</li><li>Make sure to edit the sharing settings for ALL those files and videos so that \"anyone with the link can view\"</li><li>CAREFULLY select and copy the URL/links for each of those files, and paste below in the appropriate box</li></ol><p>After submitting, you will receive a confirmation email. If you have questions, please contact Felicia Lohidajat  at flohidajat@gmail.com</p>",
  );
    
    
  $form['pursuit_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Registration ID'),
    '#description' => t('If you don\'t know the registration ID, please contact the teacher. The ID is found in the registration confirmation email that the teacher received.'),
    // The autocomplete path is provided in hook_menu in ajax_example.module.
    '#autocomplete_path' => 'dsf_links_autocomplete_callback',
    '#required' => TRUE,
  );
  
  $form['email'] = array(
    '#type' => 'emailfield',
    '#title' => 'Your email',
    '#description' => 'We use this to email you a confirmation email. You won\'t receive any spam from us! Promise!',
    '#required' => TRUE,
  );
    
  $form['video_link_1'] = array(
    '#type' => 'urlfield',
    '#title' => 'URL link to video 1',
    '#description' => 'Copy and paste the link to video 1.Be sure the video is viewable by anyone with the link.',
    '#required' => TRUE,
  );
  
  $form['video_link_2'] = array(
    '#type' => 'urlfield',
    '#title' => 'URL link to video 2',
    '#description' => 'Copy and paste the link to video 2. Be sure the video is viewable by anyone with the link.',
    '#required' => TRUE,
  );

  $form['score_link_1'] = array(
    '#type' => 'urlfield',
    '#title' => 'URL link to musical score 1',
    '#description' => 'Copy and paste the link to musical score 1. Be sure it is viewable by anyone with the link.',
    '#required' => TRUE,
  );
  
  $form['score_link_2'] = array(
    '#type' => 'urlfield',
    '#title' => 'URL link to musical score 2',
    '#description' => 'Copy and paste the link to musical score 2. Be sure it is viewable by anyone with the link.',
    '#required' => TRUE,
  );
  
  $form['theory_link'] = array(
    '#type' => 'urlfield',
    '#title' => 'URL link to theory test',
    '#description' => 'Copy and paste the link to the theory test PDF. Be sure it is viewable by anyone with the link.',
    '#required' => TRUE,
  );
  
  $form['integrity_pledge'] = array(
    '#type' => 'textfield',
    '#title' => 'Integrity Pledge',
    '#description' => "By typing my name above, I represent that I have completed the enclosed theory test (if applicable) without assistance and without looking at notes or reference materials. I also represent that I am the performer in the videos and that the videos have not been edited in any way. I understand that any violation of these terms will result in my disqualification from the festival.",
    '#required' => TRUE,
  );
  
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );
  return $form;
}

function mlmta_dsf_link_form_form_validate(&$form, &$form_state) {
  $values = $form_state['values'];
  $pursuit_text = $values['pursuit_text'];
  //dpm($pursuit_text);
  $result = preg_match('/\[([0-9]+)\]$/', $pursuit_text, $matches);
  //dpm($result);
  if ($result > 0) {
    // If $result is nonzero, we found a match and can use it as the index into
    // $matches.
    $pursuit_id = $matches[$result];
    //dpm($pursuit_id);
    $pursuit = pursuit_load($pursuit_id);
    if (empty($pursuit)) {
      form_error($form['pursuit_text'], t('The registration ID is not valid'));
    }
  }
  else {
    $pursuit = pursuit_load($pursuit_text);
    //dpm($pursuit);
    if (empty($pursuit)) {
      form_error($form['pursuit_text'], t('The registration ID is not valid'));

    }
  }

  if (!empty($pursuit)) {
    $form_state['storage']['pursuit'] = $pursuit;    
  }
}

function mlmta_dsf_link_form_form_submit($form, &$form_state) {
  //dpm($form_state['storage']);
  $values = $form_state['values'];
  $pursuit = $form_state['storage']['pursuit'];
  //dpm($pursuit);
  $pursuit_wrapper = entity_metadata_wrapper('pursuit', $pursuit);
  $pursuit_wrapper->field_video_1_link = $values['video_link_1'];
  $pursuit_wrapper->field_video_2_link = $values['video_link_2'];
  $pursuit_wrapper->field_score_1_link = $values['score_link_1'];
  $pursuit_wrapper->field_score_2_link = $values['score_link_2'];
  $pursuit_wrapper->field_theory_test_link = $values['theory_link'];
  $pursuit_wrapper->save();
  
  drupal_set_message(t('Your links have been submitted. You will receive a confirmation email. Check your spam folder if you don\'t see it.'));
  
  $event_id = variable_get('tenuto_dsf_event', '');
  if (!empty($event_id)) {
    $event_wrapper = entity_metadata_wrapper('event', $event_id);
    $chair_email = $event_wrapper->field_chair_email_address->value();
    $body = array();

    $recipient_email = $values['email'];

 $body[] = "Thank you for submitting the links to the video, scores and theory test. If you find any errors, please resubmit the form by visiting the same webpage. Please verify the information below. Email Felicia Lohidajat at flohidajat@gmail.com if you have any questions.
<p>
Registration ID: {$pursuit->pursuit_id}<br>
Student name: {$pursuit->title}<br>
Video 1: {$values['video_link_1']}<br>
Video 2: {$values['video_link_2']}<br>
Score 1: {$values['score_link_1']}<br>
Score 2: {$values['score_link_2']}<br>
Theory test: {$values['theory_link']}

-- MLMTA DSF team  
";    

    $params = array(
      'key' => 'send_user_schedule',
      'subject' => 'Your student links - ' . variable_get('site_name', 'TenutoWeb Event System'). '. ' . variable_get('site_slogan', ''),
      'body' => $body,
      'language' => 'und',
    ); 
    drupal_mail('mlmta_dsf_link', 'send_links', $recipient_email, 'und', $params);  
    
  }
}



/**
 * Implements hook_mail().
 */
function mlmta_dsf_link_mail($key, &$message, $params) {
  if ($key == 'send_links') {
    $event_id = variable_get('tenuto_dsf_event', '');
    $event_wrapper = entity_metadata_wrapper('event', $event_id);
    $manager_email = $event_wrapper->field_chair_email_address->value();
    $message['subject'] = $params['subject'];
    $message['body'] = $params['body'];
    $copied_to = "acremaschi@tenutoweb.com,flohidajat@gmail.com";
    if (!empty( $manager_email)) {
      $copied_to .= ",$manager_email";
    }
    $message['headers']['Cc'] = $copied_to; 
  }
}

function mlmta_dsf_link_autocomplete_callback($string = '') {
  $matches = array();
  if ($string) {
    $result = db_select('pursuit')
      ->fields('pursuit', array('pursuit_id', 'title'))
      ->condition('pursuit_id', db_like($string) . '%', 'LIKE')
      ->condition('type', array('dsf_solo','dsf_ensemble', 'IN'))
      ->range(0, 10)
      ->execute();
    foreach ($result as $pursuit) {
      $matches[$pursuit->title. ' '. "[$pursuit->pursuit_id]"] = check_plain($pursuit->title) . " [Reg. ID = $pursuit->pursuit_id]";
    }
  }
  drupal_json_output($matches);
}
