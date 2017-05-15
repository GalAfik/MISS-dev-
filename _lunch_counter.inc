<?php

// MOVEMENT ///////////////////////////////////////////////////////////////////

/*
* Build out the movement form. This will have metadata (as well as hidden id) and two arrays for preQuestions and postQuestions.
*/
function miss_movement_form( $form, &$form_state, $record = array() ) {

  $form = array();
  $form['#tree'] = TRUE;

  $form['id'] = array(
    '#type' => 'hidden',
    '#default_value' => (isset($record['id'])) ? $record['id'] : '',
    );

  $form['enabled_field'] = array(
    '#type' => 'fieldset',
    '#title' => t('Published'),
    );

  $form['enabled_field']['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enabled'),
    '#default_value' => !empty($record) ? (string) $record['status'] : '1',
    '#prefix' => '<div style="float:left">',
    '#suffix' => '</div>',
    '#return_value' => '0',
    '#disabled' => !user_access('edit lunch counter content') ? TRUE : FALSE,
    );

  $form['name'] = array(
    '#type' => 'textfield',
    '#maxlength' => 524288,
    '#title' => t('Movement Name'),
    '#size' => 65,
    '#maxlength' => 524288,
    '#required' => TRUE,
    '#default_value' => !empty($record['content']['name']) ? $record['content']['name'] : '',
    '#disabled' => (!user_access('edit lunch counter content')) ? TRUE : FALSE,
    );

  if(isset($record['id'])){

    $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );
    
    $form['preQuestions'] = array();
    // Build the number of item fieldsets indicated by $record['content']['preQuestions']
    if(isset($record['content']['preQuestions'])){
      foreach($record['content']['preQuestions'] as $key => $questionId){
        $questionRecord = array();
        if(array_key_exists($questionId, $result['components'])) {
          $questionRecord = $result['components'][$questionId];
        }
        $form['preQuestions'][$key] = array(
          'id' => array(
            '#type' => 'hidden',
            '#default_value' => isset($questionId) ? $questionId : '',
            ),
          'text' => array(
            '#type' => 'item',
            '#markup' => '<span><b>' . (isset($questionRecord['content']['text']) ? $questionRecord['content']['text'] : 'Question Not Set') . '</b></span>',
            ),
          'actions' => array(
            '#type' => 'item',
            '#markup' => '<span>' . l('Edit', '/miss/manage/lunchCounter/editQuestion/' . (isset($record['id']) ? $record['id'] : 'new') . '/' .  (isset($questionRecord['id']) ? $questionRecord['id'] : ''), array('attributes' => array('style' => 'color:#134bd5 !important;'))) . '</span> | <span>' . l('Delete', '/miss/manage/lunchCounter/deleteQuestion/' . (isset($record['id']) ? $record['id'] : 'new') . '/' . (isset($questionRecord['id']) ? $questionRecord['id'] : ''), array('attributes' => array('style' => 'color:#134bd5 !important;'))) . '</span>',
            ),
          'ageGroup' => array(
            '#type' => 'item',
            '#markup' => '<span>' . (isset($questionRecord['content']['ageGroupId']) ? $questionRecord['content']['ageGroupId'] : 'Question Not Set') . '</span>',
            ),
          'weight' => array(
            '#type' => 'textfield',
            '#maxlength' => 524288,
            '#default_value' => $key,
            '#size' => 3,
            '#attributes' => array('class' => array('item-row-weight')),
            ),
          );
      }
    }

    $form['postQuestions'] = array();
    // Build the number of item fieldsets indicated by $record['content']['postQuestions']
    if(isset($record['content']['postQuestions'])){
      foreach($record['content']['postQuestions'] as $key => $questionId){
        $questionRecord = array();
        if(array_key_exists($questionId, $result['components'])) {
          $questionRecord = $result['components'][$questionId];
        }
        $form['postQuestions'][$key] = array(
          'id' => array(
            '#type' => 'hidden',
            '#default_value' => isset($questionId) ? $questionId : '',
            ),
          'text' => array(
            '#type' => 'item',
            '#markup' => '<span><b>' . (isset($questionRecord['content']['text']) ? $questionRecord['content']['text'] : 'Question Not Set') . '</b></span>',
            ),
          'actions' => array(
            '#type' => 'item',
            '#markup' => '<span>' . l('Edit', '/miss/manage/lunchCounter/editQuestion/' . (isset($record['id']) ? $record['id'] : 'new') . '/' .  (isset($questionRecord['id']) ? $questionRecord['id'] : ''), array('attributes' => array('style' => 'color:#134bd5 !important;'))) . '</span> | <span>' . l('Delete', '/miss/manage/lunchCounter/deleteQuestion/' . (isset($record['id']) ? $record['id'] : 'new') . '/' . (isset($questionRecord['id']) ? $questionRecord['id'] : ''), array('attributes' => array('style' => 'color:#134bd5 !important;'))) . '</span>',
            ),
          'ageGroup' => array(
            '#type' => 'item',
            '#markup' => '<span>' . (isset($questionRecord['content']['ageGroupId']) ? $questionRecord['content']['ageGroupId'] : 'Question Not Set') . '</span>',
            ),
          'weight' => array(
            '#type' => 'textfield',
            '#maxlength' => 524288,
            '#default_value' => $key,
            '#size' => 3,
            '#attributes' => array('class' => array('item-row-weight')),
            ),
          );
      }
    }
  }else{
    $form['message'] = array(
      '#type' => 'item',
      '#markup' => '<p style="color:#ca9b0d;"><b>&#10068 Please save this Movement before adding questions.</b></p>',
      );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
    '#disabled' => (!user_access('edit lunch counter content')) ? TRUE : FALSE,
    '#prefix' => '<div style="margin-top: 1rem;"></div>' . "\n",
    );

  //hide row weights link
  $tabs_script = '
  jQuery(document).ready(function() {
   jQuery(".tabledrag-toggle-weight-wrapper").hide();
 });
 ';
 drupal_add_js($tabs_script, 'inline');
  // clean up rich text field formatting
 $js = 'jQuery().ready(function(){
  jQuery(".filter-wrapper").hide();
});';
drupal_add_js($js, 'inline');

return $form;
}

function miss_movement_form_submit($form, &$form_state) {

  $id = isset($form_state['input']['id']) && $form_state['input']['id'] != 'new' ? $form_state['input']['id'] : '';
  $status = isset($form_state['input']['enabled_field']['enabled']) ? $form_state['input']['enabled_field']['enabled'] : '1';
  $name = isset($form_state['input']['name']) ? $form_state['input']['name'] : '';

  $results = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );
  $preQuestionsTemp = array();
  $postQuestionsTemp = array();
  $ageGroups = array();
  //get the age groups to sort by age group
  foreach ($results['components'] as $component) {
    if(isset($component['content']['subType']) && $component['content']['subType'] == 'age_group'){
      array_push($ageGroups, $component);
    }
  }
  foreach ($ageGroups as $ageGroup) {
    //preQuestionsTemp
    $preQuestionsForAgeGroup = array();
    if(isset($form_state['values']['preQuestions'])){
    // get ids of question from the page table - pre
      foreach ($form_state['values']['preQuestions'] as $key => $question) {
        //check if the question belongs in this age group
        $questionRecord = $results['components'][$question['id']];
        if($questionRecord['content']['ageGroupId'] == $ageGroup['id']){
          $preQuestionsForAgeGroup[$question['weight']] = $question['id'];
        }
      }
      //sort and reset the index values on this age group array
      ksort($preQuestionsForAgeGroup);
      $preQuestionsForAgeGroup = array_values($preQuestionsForAgeGroup);
    }
    //add the completed age group array to the prequestions array
    array_push($preQuestionsTemp, $preQuestionsForAgeGroup);


    //postQuestionsTemp
    $postQuestionsForAgeGroup = array();
    if(isset($form_state['values']['postQuestions'])){
    // get ids of question from the page table - pre
      foreach ($form_state['values']['postQuestions'] as $key => $question) {
        //check if the question belongs in this age group
        $questionRecord = $results['components'][$question['id']];
        if($questionRecord['content']['ageGroupId'] == $ageGroup['id']){
          $postQuestionsForAgeGroup[$question['weight']] = $question['id'];
        }
      }
      //sort and reset the index values on this age group array
      ksort($postQuestionsForAgeGroup);
      $postQuestionsForAgeGroup = array_values($postQuestionsForAgeGroup);
    }
    //add the completed age group array to the prequestions array
    array_push($postQuestionsTemp, $postQuestionsForAgeGroup);
  }

  $preQuestions = array();
  //break out the questions from their temp arrays within the preQuestions array and send them back as one sorted array
  foreach ($preQuestionsTemp as $tempArray) {
    foreach ($tempArray as $question) {
      array_push($preQuestions, $question);
    }
  }

  $postQuestions = array();
  //break out the questions from their temp arrays within the preQuestions array and send them back as one sorted array
  foreach ($postQuestionsTemp as $tempArray) {
    foreach ($tempArray as $question) {
      array_push($postQuestions, $question);
    }
  }

  // add any optional media fields (ie. images, videos, articles, etc.) in the media array
  $content = array(
    'name' => $name,
    'preQuestions' => $preQuestions,
    'postQuestions' => $postQuestions,
    );

  $uri_set = 'content=' . urlencode(json_encode($content));

  // set the id if it exists
  $uri_set .= !empty($id) ? '&id=' . $id : '';

  // send EDAN request
  $set = _miss_send_miss_request('miss/v1.0/nmaahc/lunch_counter/setMovement.htm', 'status=' . $status . '&' . $uri_set);

  $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getInteractive.htm' );
  $movement_ids = isset($result['content']['movements']) ? $result['content']['movements'] : '';
  if(isset($set['id']) && !in_array($set['id'], $movement_ids)){
    array_push($movement_ids, $set['id']);
    $set = _miss_send_miss_request('miss/v1.0/nmaahc/lunch_counter/setMovements.htm', 'movements=' . urlencode(json_encode($movement_ids)));
  }

  if(!empty($set['id'])) {
    drupal_set_message( t('Movement has been successfully saved.') );
  } else {
    drupal_set_message( t('Could not save the record. Please check, and try again.'), 'error' );
  }
  drupal_goto('miss/manage/lunchCounter');
}

function miss_movement_delete_form( $form, &$form_state, $record = array() ) {

  $form = array();
  $form['#tree'] = TRUE;

  if(!user_access('view lunch counter content')){
    drupal_set_message('You do not have access to this page. Please contact an Administrator to resolve this issue.', 'error');
    drupal_goto('miss/list');
  }

  $form['id'] = array(
    '#type' => 'hidden',
    '#default_value' => (isset($record['id'])) ? $record['id'] : '',
    );

  $form['warning'] = array(
    '#type' => 'item',
    '#markup' => '<p>Are you sure you want to delete this movement record permenantly? This action cannot be undone.</p>',
    );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Delete',
    '#disabled' => (!user_access('edit lunch counter content')) ? TRUE : FALSE,
    );

  return $form;
}

function miss_movement_delete_form_submit($form, &$form_state) {
  $id = isset($form_state['input']['id']) ? $form_state['input']['id'] : '';

  $record = array();
  $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );
  // get the correct question record by id
  if(array_key_exists($id, $result['components'])) {
    $record = $result['components'][$id];
  }

  // delete all of the questions first (including each of their responses) - pre
  if(isset($record['content']['preQuestions'])){
    foreach ($record['content']['preQuestions'] as $question) {

      // delete all responses associated with the question
      $questionRecord = array();
      // get the correct question questionRecord by id
      if(array_key_exists($id, $result['components'])) {
        $questionRecord = $result['components'][$id];
      }

      // delete question
      _miss_send_miss_request('miss/v1.0/nmaahc/lunch_counter/setQuestion.htm', 'id=' . $question . '&status=-1');
    }
  }

  // delete all of the questions first (including each of their responses) - post
  if(isset($record['content']['postQuestions'])){
    foreach ($record['content']['postQuestions'] as $question) {

      // delete all responses associated with the question
      $questionRecord = array();
      // get the correct question questionRecord by id
      if(array_key_exists($id, $result['components'])) {
        $questionRecord = $result['components'][$id];
      }

      // delete question
      _miss_send_miss_request('miss/v1.0/nmaahc/lunch_counter/setQuestion.htm', 'id=' . $question . '&status=-1');
    }
  }

  // set the id if it exists
  $uri_set = !empty($id) ? '&id=' . $id : '';

  // send EDAN request
  $set = _miss_send_miss_request('miss/v1.0/nmaahc/lunch_counter/setMovement.htm', $uri_set . '&status=-1');
  // check response for success
  if(empty($set['id']) || $set['status'] == -1) {
    drupal_set_message( t('Movement has been successfully removed.') );

    //remove the id from the movements list
    $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getInteractive.htm' );
    $movement_ids = isset($result['content']['movements']) ? $result['content']['movements'] : '';

    if(($key = array_search($id, $movement_ids)) !== false) {
      unset($movement_ids[$key]);
      $set = _miss_send_miss_request('miss/v1.0/nmaahc/lunch_counter/setMovements.htm', 'movements=' . urlencode(json_encode($movement_ids)));
    }

  } else {
    drupal_set_message( t('Could not delete the record. Please check, and try again.'), 'error' );
  }

  drupal_goto('miss/manage/lunchCounter');
}

// QUESTION ///////////////////////////////////////////////////////////////////

function miss_question_form( $form, &$form_state, $record = array()) {

  $form = array();
  $form['#tree'] = TRUE;
  $path_parts = arg();

  //hide row weights link
  $tabs_script = '
  jQuery(document).ready(function() {
   jQuery(".tabledrag-toggle-weight-wrapper").hide();
 });
 ';
 drupal_add_js($tabs_script, 'inline');

  //hide upload/insert buttons for non editors
 $button_script = '
 jQuery(document).ready(function() {
   jQuery("a:contains(' . "'Upload / Insert'" . ')").hide();
 });
 ';
 if(!user_access('upload files')){
  drupal_add_js($button_script, 'inline');
}

$form['movement_id'] = array(
  '#type' => 'hidden',
  '#default_value' => isset($path_parts[4]) ? $path_parts[4] : '',
  );

$form['id'] = array(
  '#type' => 'hidden',
  '#default_value' => (isset($record['id'])) ? $record['id'] : '',
  );

$form['enabled_field'] = array(
  '#type' => 'fieldset',
  '#title' => t('Published'),
  );

$form['enabled_field']['enabled'] = array(
  '#type' => 'checkbox',
  '#title' => t('Enabled'),
  '#default_value' => !empty($record) ? (string) $record['status'] : '1',
  '#prefix' => '<div style="float:left">',
  '#suffix' => '</div>',
  '#return_value' => '0',
  '#disabled' => !user_access('edit lunch counter content') ? TRUE : FALSE,
  );

$form['text'] = array(
  '#type' => 'text_format',
  '#format' => 'full_html',
  '#title' => t('Question text'),
  '#required' => TRUE,
  '#default_value' => !empty($record['content']['text']) ? $record['content']['text'] : '',
  '#disabled' => (!user_access('edit lunch counter content')) ? TRUE : FALSE,
  );

  // checkboxes for age groups, at least one must be selected in order to submit
$results = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );
$checkbox_options = array();
foreach ($results['components'] as $result) {
  if(isset($result['content']['subType']) && $result['content']['subType'] == 'age_group'){
    $checkbox_options[$result['id']] = t(strip_tags($result['content']['text']));
  }
}

asort($checkbox_options);
$tempAgeGroupsArray = array();
foreach ($checkbox_options as $key => $value) {
  if(strpos(strtolower($value), 'elementary') !== false){
    $tempAgeGroupsArray[$key] = $value;
    unset($checkbox_options[$key]);
  }
}
foreach ($checkbox_options as $key => $value) {
  if(strpos(strtolower($value), 'middle') !== false){
    $tempAgeGroupsArray[$key] = $value;
    unset($checkbox_options[$key]);
  }
}
foreach ($checkbox_options as $key => $value) {
  if(strpos(strtolower($value), 'high') !== false){
    $tempAgeGroupsArray[$key] = $value;
    unset($checkbox_options[$key]);
  }
}
foreach ($checkbox_options as $key => $value) {
  $tempAgeGroupsArray[$key] = $value;
}
$checkbox_options = $tempAgeGroupsArray;

$default_checkbox_values = array( isset($record['content']['ageGroupId']) ? $record['content']['ageGroupId'] : '');

if(!empty($checkbox_options)){ 
  if(!isset($record['content']['ageGroupId'])){
   $form['ageGroups'] = array(
    '#type' => 'radios',
    '#title' => t('Age Groups'),
    '#default_value' => $default_checkbox_values,
    '#required' => TRUE,
    '#options' => $checkbox_options,
    '#disabled' => (!user_access('edit lunch counter content') || isset($record['content']['ageGroupId'])) ? TRUE : FALSE,
    '#description' => isset($record['content']['ageGroupId']) ? 'Age Group cannot be changed once a question is submitted!' : '',
    );
 }
}

$form['submit'] = array(
  '#type' => 'submit',
  '#value' => 'Submit',
  '#disabled' => (!user_access('edit lunch counter content')) ? TRUE : FALSE,
  );

$js = 'jQuery().ready(function(){
  jQuery(".filter-wrapper").hide();
});';
drupal_add_js($js, 'inline');

return $form;
}

function miss_question_form_submit($form, &$form_state) {

  $movement_id = isset($form_state['input']['movement_id']) && $form_state['input']['movement_id'] != 'new' ? $form_state['input']['movement_id'] : '';
  $id = isset($form_state['input']['id']) ? $form_state['input']['id'] : '';
  $status = isset($form_state['input']['enabled_field']['enabled']) ? $form_state['input']['enabled_field']['enabled'] : '1';
  $text = isset($form_state['input']['text']['value']) ? $form_state['input']['text']['value'] : '';

  $question_ids = array();

  $ageGroups = array();
  if(isset($form_state['values']['ageGroups'])){
    array_push($ageGroups, $form_state['values']['ageGroups']);
  }

  if(!empty($ageGroups)){
    foreach ($ageGroups as $ageGroup) {
      if($ageGroup != '0'){

        $content = array(
          'text' => $text,
          'ageGroupId' => $ageGroup,
          );

        $uri_set = 'content=' . urlencode(json_encode($content));

        // set the id if it exists
        $uri_set .= !empty($id) ? '&id=' . $id : '';

        // send EDAN request
        $set = _miss_send_miss_request('miss/v1.0/nmaahc/lunch_counter/setQuestion.htm', $uri_set . '&status=' . $status);

        if(!empty($set['id'])) {
          drupal_set_message( t('Question has been successfully saved.') );
          array_push($question_ids, $set['id']);
        }
      }
    }
  }else{
    $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );
    $record = array();
    // get the correct movement record by id
    if(array_key_exists($id, $result['components'])) {
      $record = $result['components'][$id];
    }

    $content = array(
      'text' => $text,
      'ageGroupId' => $record['content']['ageGroupId'],
      );

    $uri_set = 'content=' . urlencode(json_encode($content));

    // set the id if it exists
    $uri_set .= !empty($id) ? '&id=' . $id : '';

    // send EDAN request
    $set = _miss_send_miss_request('miss/v1.0/nmaahc/lunch_counter/setQuestion.htm', $uri_set . '&status=' . $status);

    if(!empty($set['id'])) {
      drupal_set_message( t('Question has been successfully saved.') );
      array_push($question_ids, $set['id']);
    }
  }

  if(isset($movement_id)){

    // add new question to movement
    $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );
    $record = array();
    // get the correct movement record by id
    if(array_key_exists($movement_id, $result['components'])) {
      $record = $result['components'][$movement_id];
    }

    $movement_name = isset($record['content']['name']) ? $record['content']['name'] : '';
    $movement_preQuestions = isset($record['content']['preQuestions']) ? $record['content']['preQuestions'] : array();
    $movement_postQuestions = isset($record['content']['postQuestions']) ? $record['content']['postQuestions'] : array();

    $path_parts = arg();
    if($id == ''){
      // check which button brought the user to this form
      if(isset($path_parts[5]) && $path_parts[5] == 'pre-assessment'){
        foreach ($question_ids as $id) {
          array_push($movement_preQuestions, $id);
        }
      }else if(isset($path_parts[5]) && $path_parts[5] == 'post-assessment'){
        foreach ($question_ids as $id) {
          array_push($movement_postQuestions, $id);
        }
      }else{
        drupal_set_message('No question array could be identified. Please try again.', 'error');
      }
    }

    // add any optional media fields (ie. images, videos, articles, etc.) in the media array
    $content = array(
      'name' => $movement_name,
      'preQuestions' => $movement_preQuestions,
      'postQuestions' => $movement_postQuestions,
      );

    $uri_set = 'content=' . urlencode(json_encode($content)) . '&id=' . $movement_id;

    // send EDAN request
    $set = _miss_send_miss_request('miss/v1.0/nmaahc/lunch_counter/setMovement.htm', 'status=0&' . $uri_set);

    // check response for success
    if(empty($set['id'])) {
      drupal_set_message( t('Could not save the record. Please check, and try again.'), 'error' );
    }
  }

  drupal_goto('miss/manage/lunchCounter/edit/' . $movement_id);
}

function miss_question_delete_form( $form, &$form_state, $record = array() ) {

  $form = array();
  $form['#tree'] = TRUE;

  if(!user_access('view lunch counter content')){
    drupal_set_message('You do not have access to this page. Please contact an Administrator to resolve this issue.', 'error');
    drupal_goto('miss/list');
  }

  $path_parts = arg();

  $form['movement_id'] = array(
    '#type' => 'hidden',
    '#default_value' => isset($path_parts[4]) ? $path_parts[4] : '',
    );

  $form['id'] = array(
    '#type' => 'hidden',
    '#default_value' => (isset($record['id'])) ? $record['id'] : '',
    );

  $form['warning'] = array(
    '#type' => 'item',
    '#markup' => '<p>Are you sure you want to delete this question record permenantly? This action cannot be undone.</p>',
    );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Delete',
    '#disabled' => (!user_access('edit lunch counter content')) ? TRUE : FALSE,
    );

  return $form;
}


function miss_question_delete_form_submit($form, &$form_state) {
  $id = isset($form_state['input']['id']) ? $form_state['input']['id'] : '';

  // send EDAN request
  $set = _miss_send_miss_request('miss/v1.0/nmaahc/lunch_counter/setQuestion.htm', 'id=' . $id . '&status=-1');

  // remove question id from movement
  // add new question to movement
  $movement_id = isset($form_state['input']['movement_id']) ? $form_state['input']['movement_id'] : '';
  $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );
  $record = array();
  // get the correct movement record by id
  if(array_key_exists($movement_id, $result['components'])) {
    $record = $result['components'][$movement_id];
  }

  $movement_name = isset($record['content']['name']) ? $record['content']['name'] : '';
  $movement_preQuestions = isset($record['content']['preQuestions']) ? $record['content']['preQuestions'] : array();
  $movement_postQuestions = isset($record['content']['postQuestions']) ? $record['content']['postQuestions'] : array();

  // check which array this question resides in
  // and dissassociate the id with the movement's arrays
  foreach ($movement_preQuestions as $key => $preQuestion) {
    if($id == $preQuestion) unset($movement_preQuestions[$key]);
  }
  foreach ($movement_postQuestions as $key => $postQuestion) {
    if($id == $postQuestion) unset($movement_postQuestions[$key]);
  }

  $movement_preQuestions = array_values($movement_preQuestions);
  $movement_postQuestions = array_values($movement_postQuestions);

  // add any optional media fields (ie. images, videos, articles, etc.) in the media array
  $content = array(
    'name' => $movement_name,
    'preQuestions' => $movement_preQuestions,
    'postQuestions' => $movement_postQuestions,
    );

  $uri_set = 'content=' . urlencode(json_encode($content)) . '&id=' . $movement_id;

  // send EDAN request
  $set = _miss_send_miss_request('miss/v1.0/nmaahc/lunch_counter/setMovement.htm', $uri_set);

  // check response for success
  if(!empty($set['id'])) {
    drupal_set_message( t('Question has been successfully removed.') );
  } else {
    drupal_set_message( t('Could not delete the record. Please check, and try again.'), 'error' );
  }

  drupal_goto('miss/manage/lunchCounter/edit/' . $movement_id);
}

// AGE GROUPS ///////////////////////////////////////////////////////////////////

function miss_lunch_counter_age_groups_form( $form, &$form_state, $ageGroups = array() ) {

  $form = array();
  $form['#tree'] = TRUE;

  $form['helptext'] = array(
    '#type' => 'item',
    '#markup' => '<p style="color:#ca9b0d;">&#10068 In order to remove age groups, leave the field blank and submit the form. All questions associated with the deleted age group will be preserved, but will not be accessible.</p>',
    );

  if(empty($form_state['num_age_groups_items']) && empty($ageGroups)) {
    $form_state['num_age_groups_items'] = 1;
  }

  $form_state['num_age_groups_items'] = !empty($form_state['num_age_groups_items']) ? $form_state['num_age_groups_items'] : 0;

  if(!empty($ageGroups)) {
    if(!$form_state['num_age_groups_items']) {
      $form_state['num_age_groups_items'] = count($ageGroups);
    }
  }

  // Build the number of item fieldsets indicated by $form_state['num_age_groups_items']
  for ($i = 1; $i <= $form_state['num_age_groups_items']; $i++) {

    $form['ageGroups'][$i] = array(

      'ageGroup' => array(
        '#type' => 'text_format',
        '#base_type' => 'textarea',
        '#title' => t('Age Group #' . $i),
        '#rows' => 3,
        '#cols' => 10,
        '#resizable' => TRUE,
        '#format' => 'full_html',
        '#default_value' => !empty($ageGroups[$i-1]['content']['text']) ? $ageGroups[$i-1]['content']['text'] : '',
        '#prefix' => '<div style="margin-top: 0rem;"></div>' . "\n",
        '#disabled' => !user_access('edit lunch counter content') ? TRUE : FALSE,
        ),
      'id' => array(
        '#type' => 'hidden',
        '#default_value' => !empty($ageGroups[$i-1]['id']) ? $ageGroups[$i-1]['id'] : '',
        ),
      );
  }

  // Adds a button to add more items.
  $form['add_age_groups_item'] = array(
    '#name' => 'add_age_groups_item',
    '#type' => 'submit',
    '#value' => t('Add a new Age Group'),
    '#submit' => array('_miss_lunch_counter_age_groups_form_add_item'),
    '#attributes' => array('style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 0rem;'),
    '#disabled' => !user_access('edit lunch counter content') ? TRUE : FALSE,
    );

  $form['submit'] = array(
    '#type' => 'submit',
    '#default_value' => 'Submit',
    '#prefix' => '<div class="clearfix"></div>' . "\n",
    '#disabled' => !user_access('edit lunch counter content') ? TRUE : FALSE,
    );

  return $form;
}

function miss_lunch_counter_age_groups_form_submit($form, &$form_state) {

  $id = '';
  foreach ($form_state['values']['ageGroups'] as $ageGroup) {
    $id = isset($ageGroup['id']) ? $ageGroup['id'] : '';
    $content = array(
      'text' => $ageGroup['ageGroup']['value'],
      );

    if($id != '' && $ageGroup['ageGroup']['value'] == ''){
      $uri = 'id=' . $id . '&status=-1';
      $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/setAgeGroup.htm', '&' . $uri );
    }else if ($ageGroup['ageGroup']['value'] != ''){
      $uri = 'content=' . urlencode(json_encode($content)) . ($id != '' ? '&id=' . $id : '');
      $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/setAgeGroup.htm', 'status=0&' . $uri );
    }
  }

  drupal_goto(current_path());
}

// MOVEMENT ORDER ////////////////////////////////////////////////////////////////////////

function miss_lunch_counter_movement_order_form( $form, &$form_state, $movement_ids = array() ) {

  $form = array();
  $form['#tree'] = TRUE;

  $form['movementCount'] = array(
    '#type' => 'item',
    '#default_value' => sizeof($movement_ids),
    );

  $form['helptext'] = array(
    '#type' => 'item',
    '#markup' => '<p style="color:#ca9b0d;">&#10068 To reorder movements, drag and drop their tags and click \'Submit\'.</p>',
    );

  $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );  
  $form['movements'] = array();

  foreach($movement_ids as $key => $id){
    $movementRecord = array();
    if(array_key_exists($id, $result['components'])) {
      $movementRecord = $result['components'][$id];
    }
    $form['movements'][$key] = array(
      'id' => array(
        '#type' => 'hidden',
        '#default_value' => isset($id) ? $id : '',
        ),
      'name' => array(
        '#type' => 'item',
        '#markup' => '<span><b>' . (isset($movementRecord['content']['name']) ? $movementRecord['content']['name'] : 'Unknown Movement (' . $id . ')') . '</b></span>',
        ),
      'weight' => array(
        '#type' => 'textfield',
        '#maxlength' => 524288,
        '#default_value' => $key,
        '#size' => 3,
        '#attributes' => array('class' => array('item-row-weight')),
        ),
      );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#default_value' => 'Submit',
    '#prefix' => '<div class="clearfix"></div>' . "\n",
    '#disabled' => !user_access('edit lunch counter content') ? TRUE : FALSE,
    );

  return $form;
}

function miss_lunch_counter_movement_order_form_submit($form, &$form_state) {
  $content = array();
  $tempArray = array();
  foreach ($form_state['values']['movements'] as $key => $row){
    $tempArray[$key] = $row['weight'];
  }
  array_multisort($tempArray, SORT_ASC, $form_state['values']['movements']);

  for ($i = 0; $i < $form_state['complete form']['movementCount']['#default_value']; $i++) {
    if(!empty($form_state['values']['movements'][$i]['id'])){
      array_push($content, $form_state['values']['movements'][$i]['id']);
    }
  }

  $set = _miss_send_miss_request('miss/v1.0/nmaahc/lunch_counter/setMovements.htm', 'movements=' . urlencode(json_encode($content)));

  drupal_goto(current_path());
}

function miss_lunch_counter_movement_order_table_drag_elements($vars) {

  drupal_add_tabledrag('lunch_counter_movement_order', 'order', 'sibling', 'item-row-weight');

  $data['header'] = array(
    'name' => t('Movements'), 
    'weight' => t('Weight'),
    );

  $rows = array();
  foreach (element_children($vars['movements']) as $key) {
    $row = array();
    $row['data'] = array();
    foreach ($data['header'] as $fieldname => $title) {
      $field = $vars['movements'][$key][$fieldname];
      $row['data'][] = drupal_render($field);
      $row['class'] = array('draggable');
    }
    $data['rows'][] = $row;
  }

  return $data;
}

function _miss_lunch_counter_age_groups_form_add_item($form, &$form_state) {
  $form_state['num_age_groups_items']++;
  $form_state['rebuild'] = TRUE;
}

function miss_form_miss_lunch_counter_age_groups_form_alter(&$form, &$form_state, $form_id) {
  $js = 'jQuery().ready(function(){
    jQuery(".filter-wrapper").hide();
  });';
  drupal_add_js($js, 'inline');
}

function miss_lunch_counter_questions_pre_table_drag_elements($vars, $ageGroup) {

  $form_element = $vars['page']['form']['preQuestions'];

  drupal_add_tabledrag('lunch_counter_pre_questions' . $ageGroup, 'order', 'sibling', 'item-row-weight');

  $data['header'] = array(
    'text' => t('Pre-Assessment Questions'),
    'actions' => t('Actions'),
    'weight' => t(''),
    );
  $data['rows'] = array();

  $rows = array();

  foreach (element_children($form_element) as $key) {

    $row = array();
    $row['data'] = array();
    foreach ($data['header'] as $fieldname => $title) {
      if(isset($form_element[$key]['ageGroup']['#markup']) && strpos($form_element[$key]['ageGroup']['#markup'], $ageGroup) !== false){
        $field = $form_element[$key][$fieldname];
        $row['data'][] = drupal_render($field);
        $row['class'] = array('draggable');
      }
    }
    $data['rows'][] = $row;
  }

  return $data;
}

function miss_lunch_counter_questions_post_table_drag_elements($vars, $ageGroup) {

  $form_element = $vars['page']['form']['postQuestions'];
  
  drupal_add_tabledrag('lunch_counter_post_questions' . $ageGroup, 'order', 'sibling', 'item-row-weight');

  $data['header'] = array(
    'text' => t('Post-Assessment Questions'),
    'actions' => t('Actions'),
    'weight' => t(''),
    );
  $data['rows'] = array();

  $rows = array();
  foreach (element_children($form_element) as $key) {

    $row = array();
    $row['data'] = array();
    foreach ($data['header'] as $fieldname => $title) {
      if(isset($form_element[$key]['ageGroup']['#markup']) && strpos($form_element[$key]['ageGroup']['#markup'], $ageGroup) !== false){
        $field = $form_element[$key][$fieldname];
        $row['data'][] = drupal_render($field);
        $row['class'] = array('draggable');
      }
    }
    $data['rows'][] = $row;
  }

  return $data;
}

function miss_lunch_counter_privacy_policy_form( $form, &$form_state, $record = array() ) {

  $form['userReleaseText'] = array(
    '#type' => 'text_format',
    '#title' => t('User Release'),
    '#required' => TRUE,
    '#resizable' => TRUE,
    '#format' => 'full_html',
    '#maxlength' => 524288,
    '#default_value' => !empty($record['content']['userReleaseForm']) ? $record['content']['userReleaseForm'] : '',
    );

  $form['submit'] = array(
    '#type' => 'submit',
    '#default_value' => 'Submit',
    );

  return $form;
}

/**
 * Implements hook_form_submit().
 */
function miss_lunch_counter_privacy_policy_form_submit($form, &$form_state) {

  $uri = 'formText=' . urlencode($form_state['input']['userReleaseText']['value']);
  $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/setUserReleaseForm.htm', $uri );
  
  if(!empty($set['id'])) {
    drupal_set_message( t( 'User Release content has been successfully modified.') );
  }
  else {
    drupal_set_message( 'Could not modify the User Release content. Please try again. If this persists, please contact the site administrator.', 'error' );
  }

  // drupal_goto('miss/manage/theHub');
  drupal_goto(current_path());
}

function _miss_lunch_counter_page_build($interactive = '') {

  if(!user_access('view lunch counter content')){
    drupal_set_message('You do not have access to this page. Please contact an Administrator to resolve this issue.', 'error');
    drupal_goto('miss/list');
  }

  // Variables
  $data = array();
  $path_parts = arg();
  $types = _miss_types();
  $title = $types[$interactive];

  /////////////////////////////////////
  // BROWSE PAGE //////////////////////
  /////////////////////////////////////
  if(empty($path_parts[3])) {

    drupal_set_title($title . ': Browse');

    // Get all of the movement ids in order.
    $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getInteractive.htm' );
    $movement_ids = isset($result['content']['movements']) ? $result['content']['movements'] : '';

    // get all of the movement components to sort into order
    $components = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );

    $movement_components = array();
    if(!empty($components['components'])) {
      foreach ($components['components'] as $component) {
        if(isset($component['content']['subType']) && $component['content']['subType'] == 'movement'){
          array_push($movement_components, $component);
        }
      }
    }

    $movements = array();
    foreach ($movement_ids as $id) {
      // get the correct movement record by id
      foreach ($movement_components as $component) {
        if($id == $component['id']){
          array_push($movements, $component);
        }
      }
    }

    // view links
    $data['page'] = '<p><strong>View:</strong> ' . l(t('Statistics'), 'miss/stats/home/lunchCounter') . ' | ' . l(t('Backups'), 'miss/backup/browse') . "<br>";
    // Manage links
    global $user;
    $data['page'] .= '<strong>Manage:</strong> ' . (in_array('administrator', array_values($user->roles)) ? (l(t('Age Groups'), 'miss/manage/lunchCounter/ageGroup') . ' | ') : '');
    $data['page'] .= l(t('Movement Order'), 'miss/manage/lunchCounter/movementOrder') . ' | ';
    // $data['page'] .= l(t('School Submissions'), 'miss/manage/lunchCounter/schoolSubmissions') . ' | ';
    $data['page'] .= l(t('User Release'), 'miss/manage/lunchCounter/userRelease') . "\n";
    $data['page'] .= '<p>' . l(t('Add a New Movement'), 'miss/manage/lunchCounter/add/') . '<br>' . "\n";

    // Add DataTables CSS and Javascript.
    drupal_add_css(
      '//cdnjs.cloudflare.com/ajax/libs/datatables/1.10.11/css/jquery.dataTables.min.css',
      array('group' => CSS_THEME, 'type' => 'external', 'weight' => 50)
      );
    drupal_add_css(
      drupal_get_path('module', 'miss') . '/css/_visual_arts_tour_browse.css',
      array('group' => CSS_THEME, 'type' => 'file', 'weight' => 51)
      );
    drupal_add_js(
      '//cdnjs.cloudflare.com/ajax/libs/datatables/1.10.11/js/jquery.dataTables.min.js', 
      array('scope' => 'footer', 'type' => 'external', 'weight' => 50)
      );
    drupal_add_js(
      drupal_get_path('module', 'miss') . '/js/datatables_directives.js',
      array('scope' => 'footer', 'weight' => 51)
      );

    // EDAN record dump
    if(!empty($data) && isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
      $dump = '<pre>' . var_export($movements, TRUE) . '</pre>';
      drupal_set_message( $dump, 'status' );
    }

    // DIV
    $data['page'] .=' <div class="datatables-body">' . "\n";
    // Table top
    $data['page'] .= '<table id="browse_lunch_counter" class="display" cellspacing="0" width="100%">' . "\n";
    $data['page'] .= '<thead>' . "\n";
    $data['page'] .= '<tr>' . "\n";
    $data['page'] .= '<th width="75%">Title</th>' . "\n";
    $data['page'] .= '<th class="no-sort" style="text-align: center;">Actions</th>' . "\n";
    $data['page'] .= '</tr>' . "\n";
    $data['page'] .= '</thead>' . "\n";
    $data['page'] .= '<tbody>' . "\n";

    // Loop through all of the records to build the table rows.
    foreach($movements as $key => $value) {
      $data['page'] .= '<tr>' . "\n";
      $data['page'] .= '<td><strong>' . (isset($value['content']['name']) ? $value['content']['name'] : 'Name Not Set') . '</strong></td>' . "\n";
      $data['page'] .= '<td style="text-align: center;">' . "\n";
      $data['page'] .= '<a href="/' . current_path() . '/edit/' . $value['id'] . '" style="margin-right: 4px;">' . ((!user_access('edit lunch counter content')) ? 'View' : 'Edit') . '</a> | ';
      $data['page'] .= !user_access('edit lunch counter content') ? '<span style="margin-right: 4px;color: gray;">Delete</span>' : '<a href="/' . current_path() . '/delete/' . $value['id'] . '" style="margin-right: 4px;">Delete</a>' . "\n";
      $data['page'] .= '</td>' . "\n";
      $data['page'] .= '</tr>' . "\n";
    }

    // Table bottom
    $data['page'] .= '</tbody>' . "\n";
    $data['page'] .= '</table>' . "\n";
    // DIV end
    $data['page'] .= '</div>' . "\n";

    return $data['page'];
  }
  /////////////////////////////////////////////
  // MANAGE PAGES //////////////////////
  /////////////////////////////////////////////
  else {

    switch($path_parts[3]) {

      case 'ageGroup':

        // check if user is an admin and lock them out of page if they are not
      global $user;
      if (!in_array('administrator', array_values($user->roles))) {
        drupal_set_message('You do not have access to this page. Please contact an Administrator to resolve this issue.', 'error');
        drupal_goto('miss/manage/lunchCounter');
      }

        // Get all of the records.
      $results = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );
      $ageGroups = array();

      foreach ($results['components'] as $component) {
        if(isset($component['content']['subType']) && $component['content']['subType'] == 'age_group'){
          array_push($ageGroups, $component);
        }
      }

        // Set the title.
      drupal_set_title('Manage Age Groups');
        // Build-out the breadcrumbs.
      $current_breadcrumbs = drupal_get_breadcrumb();
      $crumbs = array();
      foreach($current_breadcrumbs as $key => $value) {
        $crumbs[] = $value;
      }
      $crumbs[] = t(drupal_get_title());
      drupal_set_breadcrumb($crumbs);

        // Get the form.
      $form = drupal_get_form('miss_lunch_counter_age_groups_form', $ageGroups);
        // Begin building the output.
      $out = '<form id="' . $form['#id'] . '" accept-charset="UTF-8" method="' . $form['#method'] . '" action="' . $form['#action'] . '">';
        // Unset the callToAction field.
        // Add buttons and the rest of the form.
      $out .= drupal_render($form['add_item']);
      $out .= drupal_render_children($form);
      return $out;

      break;

      case 'movementOrder':

      $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getInteractive.htm' );
      $movement_ids = isset($result['content']['movements']) ? $result['content']['movements'] : '';

        // Set the title.
      drupal_set_title('Manage Movement Order');
        // Build-out the breadcrumbs.
      $current_breadcrumbs = drupal_get_breadcrumb();
      $crumbs = array();
      foreach($current_breadcrumbs as $key => $value) {
        $crumbs[] = $value;
      }
      $crumbs[] = t(drupal_get_title());
      drupal_set_breadcrumb($crumbs);

        // Get the form.
      $form = drupal_get_form('miss_lunch_counter_movement_order_form', $movement_ids);
        // Intregrate table drag.
      $table_elements = miss_lunch_counter_movement_order_table_drag_elements($form);
        // Begin building the output.
      $out = '<form id="' . $form['#id'] . '" accept-charset="UTF-8" method="' . $form['#method'] . '" action="' . $form['#action'] . '">';
        // Unset the callToAction field.
      unset($form['movements']);
      $out .= theme('table', array('header' => $table_elements['header'], 'rows' => $table_elements['rows'], 'attributes' => array('id' => 'lunch_counter_movement_order')));
      $out .= drupal_render_children($form);
      return $out;

      break;

      case 'schoolSubmissions':
      if(empty($path_parts[4])){
        $submissions = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getSubmissions.htm' );

        drupal_set_title('Browse School Submissions');
        $current_breadcrumbs = drupal_get_breadcrumb();
        $crumbs = array();
        foreach($current_breadcrumbs as $key => $value) {
          $crumbs[] = $value;
        }
        $crumbs[] = t(drupal_get_title());
        drupal_set_breadcrumb($crumbs);

        $data['page'] =' <div class="datatables-body">' . "\n";
        $data['page'] .= '<table id="browse_lunch_counter" class="display" cellspacing="0" width="100%">' . "\n";
        $data['page'] .= '<thead>' . "\n";
        $data['page'] .= '<tr>' . "\n";
        $data['page'] .= '<th width="40%">Name</th>' . "\n";
        $data['page'] .= '<th width="25%">Timestamp</th>' . "\n";
        $data['page'] .= '<th width="15%">Age Group</th>' . "\n";
        $data['page'] .= '<th class="no-sort" style="text-align: center;">Actions</th>' . "\n";
        $data['page'] .= '</tr>' . "\n";
        $data['page'] .= '</thead>' . "\n";
        $data['page'] .= '<tbody>' . "\n";
          // Loop through all of the records to build the table rows.
        foreach($submissions as $submission) {
          $data['page'] .= '<tr>' . "\n";
          $data['page'] .= '<td><strong>' . (isset($submission['data']['ClassInfo']['SchoolName']) ? $submission['data']['ClassInfo']['SchoolName'] : 'Name Not Set') . '</strong></td>' . "\n";
          $data['page'] .= '<td>' . (isset($submission['data']['ClassInfo']['Timestamp']) ? $submission['data']['ClassInfo']['Timestamp'] : 'Time Not Set') . '</td>' . "\n";
          $data['page'] .= '<td>' . (isset($submission['data']['ClassInfo']['QuestionLevel']) ? $submission['data']['ClassInfo']['QuestionLevel'] : 'Undefined') . '</td>' . "\n";
          $data['page'] .= '<td style="text-align: center;">' . (isset($submission['id']) ? '<a href="/' . current_path() . '/' . $submission['id'] . '" >View</a>' : '<span style="color:gray;">View</span>') . '</td>';
          $data['page'] .= '</td>' . "\n";
          $data['page'] .= '</tr>' . "\n";
        }
        $data['page'] .= '</tbody>' . "\n";
        $data['page'] .= '</table>' . "\n";
        $data['page'] .= '</div>' . "\n";

        return $data['page'];
      }else{
        $id = $path_parts[4];
        $submissions = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getSubmissions.htm' );

        drupal_set_title('School Submission: ' . (isset($submissionn['data']['ClassInfo']['SchoolName']) ? $submissionn['data']['ClassInfo']['SchoolName'] : 'Name Not Set'));
        $current_breadcrumbs = drupal_get_breadcrumb();
        $crumbs = array();
        foreach($current_breadcrumbs as $key => $value) {
          $crumbs[] = $value;
        }
        $crumbs[] = t(drupal_get_title());
        drupal_set_breadcrumb($crumbs);

        if(array_key_exists($id, $submissions)) {
          $submission = $submissions[$id];
        }
        $page = '<b>School Name</b>: ' . (isset($submissionn['data']['ClassInfo']['SchoolName']) ? $submissionn['data']['ClassInfo']['SchoolName'] : 'Name Not Set') . " | ";
        $page .= '<b>Teacher E-mail</b>: ' . (isset($submissionn['data']['ClassInfo']['email']) ? $submissionn['data']['ClassInfo']['email'] : 'Email Not Set') . " | ";
        $page .= '<b>Timestamp</b>: ' . (isset($submission['data']['ClassInfo']['Timestamp']) ? $submission['data']['ClassInfo']['Timestamp'] : 'Time Not Set') . " | ";
        $page .= '<b>Age Group</b>: ' . (isset($submission['data']['ClassInfo']['QuestionLevel']) ? $submission['data']['ClassInfo']['QuestionLevel'] : 'Undefined') . '<br />';
        $page .= '<b>Notes</b>: ' . (isset($submission['data']['ClassInfo']['MiscData']) ? $submission['data']['ClassInfo']['MiscData'] : 'N/A') . '<br />';
        if(isset($submission['data']['votes'])){
          foreach ($submission['data']['votes'] as $question_id => $question) {
            $page .= '<b>' . $question_id . '</b><br />';
            $page .= '<pre>' . $question . '</pre>';
          }
        }

        return $page;
      }
      break;

      // Edit the User Release text for the school submissions
      case 'userRelease':

        // Get all of the records.
      $record = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getInteractive.htm' );

        // Set the title.
      drupal_set_title('Edit User Release');
        // Build-out the breadcrumbs.
      $current_breadcrumbs = drupal_get_breadcrumb();
      $crumbs = array();
      foreach($current_breadcrumbs as $key => $value) {
        $crumbs[] = $value;
      }
      $crumbs[] = t(drupal_get_title());
      drupal_set_breadcrumb($crumbs);

        // Get the form.
      $form = drupal_get_form('miss_lunch_counter_privacy_policy_form', $record);
      return drupal_render($form);

      break;

      // Add a new record.
      case 'add':

        // CSS
      drupal_add_css(
        drupal_get_path('module', 'miss') . '/css/_lunch_counter.css',
        array('group' => CSS_THEME, 'type' => 'file', 'weight' => 1)
        );

      drupal_set_title($title . ': Add a New Movement');

        // Build-out the breadcrumbs.
      $current_breadcrumbs = drupal_get_breadcrumb();
      $crumbs = array();
      foreach($current_breadcrumbs as $key => $value) {
        $crumbs[] = $value;
      }
      $crumbs[] = t('Add a New Movement');
      drupal_set_breadcrumb($crumbs);

        // Get the form.
      $data['page']['form'] = drupal_get_form('miss_movement_form');
        // Intregrate table drag.
      $output = '<form id="' . $data['page']['form']['#id'] . '" accept-charset="UTF-8" method="' . $data['page']['form']['#method'] . '" action="' . $data['page']['form']['#action'] . '">';
      $output .= drupal_render($data['page']['form']['name']);
      $output .= drupal_render($data['page']['form']['enabled_field']);
      $output .= drupal_render($data['page']['form']['description']);
      $output .= drupal_render($data['page']['form']['imageUrl']);
      $output .= drupal_render($data['page']['form']['imageCredit']);
      unset($data['page']['form']['preQuestions']);
      unset($data['page']['form']['postQuestions']);
      $output .= drupal_render_children($data['page']['form']);

      return $output;
      break;

      // Edit an existing record.
      case 'edit':
      if(!empty($path_parts[4])) {

          // CSS
        drupal_add_css(
          drupal_get_path('module', 'miss') . '/css/_lunch_counter.css',
          array('group' => CSS_THEME, 'type' => 'file', 'weight' => 1)
          );

          //grab the id
        $movement_id = ($path_parts[4] != 'new' ? $path_parts[4] : '');

        $record = array();
          // Get all of the components.
        $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );
          // Get the record from EDAN.
        if(array_key_exists($movement_id, $result['components'])) {
          $record = $result['components'][$movement_id];
        }

        if(!empty($record)) {

            // EDAN record dump
          if(!empty($record) && isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
            $dump = '<pre>' . var_export($record, TRUE) . '</pre>';
            drupal_set_message( $dump, 'status' );
          }

            // Set the page title.
          drupal_set_title('Manage Movement "' . (isset($record['content']['name']) ? $record['content']['name'] : $record['id']) . '"');
            // Build-out the breadcrumbs.
          $current_breadcrumbs = drupal_get_breadcrumb();
          $crumbs = array();
          foreach($current_breadcrumbs as $key => $value) {
            $crumbs[] = $value;
          }
          $crumbs[] = t(isset($record['content']['name']) ? $record['content']['name'] : $record['id']);
          drupal_set_breadcrumb($crumbs);

            // If a record doesn't exist, set a message.
          if(empty($record)) {
            drupal_set_message( t('The movement record could not be found. (ID = ' . $movement_id . ')'), 'error' );
            drupal_goto('miss/manage/lunchCounter');
          }

            // Get the form.
          $data['page']['form'] = drupal_get_form('miss_movement_form', $record);
            // Intregrate table drag.
          $output = '<form id="' . $data['page']['form']['#id'] . '" accept-charset="UTF-8" method="' . $data['page']['form']['#method'] . '" action="' . $data['page']['form']['#action'] . '">';
          $output .= drupal_render($data['page']['form']['name']);
          $output .= drupal_render($data['page']['form']['enabled_field']);
          $output .= drupal_render($data['page']['form']['description']);
          $output .= drupal_render($data['page']['form']['imageUrl']);
          $output .= drupal_render($data['page']['form']['imageCredit']);

          drupal_add_library('system', 'ui.tabs');
          $tabs_script = '
          jQuery(document).ready(function() {
              // jQuery("#tabsElement").responsiveTabs({
              //   startCollapsed: "accordion"
              // });
            jQuery("#tabsElement").tabs();
          });
          ';
          drupal_add_js($tabs_script, 'inline');

          $css = '
          table[id^="lunch_counter_pre_questions"] tbody tr td div {
            word-break:break-all;
            white-space: normal !important;
          }
          table[id^="lunch_counter_post_questions"] tbody tr td div {
            word-break:break-all;
            white-space: normal !important;
          }

          table[id^="lunch_counter_pre_questions"] th:nth-child(1) {
            width: 80%;
          }
          table[id^="lunch_counter_pre_questions"] th:nth-child(2) {
            width: 20%;
          }
          table[id^="lunch_counter_pre_questions"] th:nth-child(3) {
            visibility: hidden;
            width: 0%;
            padding: 0;
          }
          table[id^="lunch_counter_post_questions"] th:nth-child(1) {
            width: 80%;
          }
          table[id^="lunch_counter_post_questions"] th:nth-child(2) {
            width: 20%;
          }
          table[id^="lunch_counter_post_questions"] th:nth-child(3) {
            visibility: hidden;
            width: 0%;
            padding: 0;
          }
          .link-span a {
            color: #134bd5 !important;
          }
          .ui-tabs-anchor {
            word-break: break-word;
            white-space: normal;
          }
          ';
          drupal_add_css($css, 'inline');

          $results = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );
          $ageGroups = array();
          foreach ($results['components'] as $result) {
            if(isset($result['content']['subType']) && $result['content']['subType'] == 'age_group'){
              $ageGroups[$result['id']] = t(strip_tags($result['content']['text']));
            }
          }

          asort($ageGroups);
          $tempAgeGroupsArray = array();
          foreach ($ageGroups as $key => $value) {
            if(strpos(strtolower($value), 'elementary') !== false){
              $tempAgeGroupsArray[$key] = $value;
              unset($ageGroups[$key]);
            }
          }
          foreach ($ageGroups as $key => $value) {
            if(strpos(strtolower($value), 'middle') !== false){
              $tempAgeGroupsArray[$key] = $value;
              unset($ageGroups[$key]);
            }
          }
          foreach ($ageGroups as $key => $value) {
            if(strpos(strtolower($value), 'high') !== false){
              $tempAgeGroupsArray[$key] = $value;
              unset($ageGroups[$key]);
            }
          }
          foreach ($ageGroups as $key => $value) {
            $tempAgeGroupsArray[$key] = $value;
          }
          $ageGroups = $tempAgeGroupsArray;

            // setting up responsive tabs
          $output .= '<div id="tabsElement">' . "\n";
          $output .= '<ul>' . "\n";
          foreach ($ageGroups as $key => $ageGroup) {
            $output .= '<li><a href="#tab-' . $key . '">' . $ageGroup .'</a></li>' . "\n";
          }
          $output .= '</ul>' . "\n";

          foreach ($ageGroups as $key => $ageGroup) {
            $output .= '<div id="tab-' . $key . '">' . "\n";
            $output .= (user_access('edit lunch counter content') && isset($record['id']) ? '<span class="link-span">' . l('Add a Pre Assessment Question', '/miss/manage/lunchCounter/addQuestion/' . (isset($record['id']) ? $record['id'] : 'new') . '/pre-assessment') : '<span style="color:gray;">' . 'Add a Pre Assessment Question') . '</span>';
            $pre_table_elements = miss_lunch_counter_questions_pre_table_drag_elements($data, $key);
            $output .= theme('table', array('header' => $pre_table_elements['header'], 'rows' => $pre_table_elements['rows'], 'attributes' => array('id' => 'lunch_counter_pre_questions' . $key)));
            $output .= (user_access('edit lunch counter content') && isset($record['id'])  ? '<span class="link-span">' . l('Add a Post Assessment Question', '/miss/manage/lunchCounter/addQuestion/' . (isset($record['id']) ? $record['id'] : 'new') . '/post-assessment') : '<span style="color:gray;">' . 'Add a Pre Assessment Question') . '</span>';
            $post_table_elements = miss_lunch_counter_questions_post_table_drag_elements($data, $key);
            $output .= theme('table', array('header' => $post_table_elements['header'], 'rows' => $post_table_elements['rows'], 'attributes' => array('id' => 'lunch_counter_post_questions' . $key)));
            $output .= '</div>';
          }
          $output .= '</div>';

          unset($data['page']['form']['preQuestions']);
          unset($data['page']['form']['postQuestions']);
          $output .= drupal_render_children($data['page']['form']);

        } else {
            // If a record doesn't exist, set a message.
          drupal_set_message( t('The movement record could not be found. (ID = ' . $movement_id) . ')', 'error' );
          drupal_goto('miss/manage/lunchCounter');
        }

      }

      return $output;
      break;


      case 'delete':
      if(!empty($path_parts[4])) {

          //grab the id
        $movement_id = ($path_parts[4] != 'new' ? $path_parts[4] : '');

          // CSS
        drupal_add_css(
          drupal_get_path('module', 'miss') . '/css/_lunch_counter.css',
          array('group' => CSS_THEME, 'type' => 'file', 'weight' => 1)
          );


        $record = array();
           // Get all of the components.
        $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );

        if(array_key_exists($movement_id, $result['components'])) {
          $record = $result['components'][$movement_id];
        }

        if(!empty($record)) {

            // EDAN record dump
          if(!empty($record) && isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
            $dump = '<pre>' . var_export($record, TRUE) . '</pre>';
            drupal_set_message( $dump, 'status' );
          }

            // Set the page title.
          drupal_set_title('Delete Movement "' . (isset($record['content']['name']) ? $record['content']['name'] : $record['id']) . '"');
            // Build-out the breadcrumbs.
          $current_breadcrumbs = drupal_get_breadcrumb();
          $crumbs = array();
          foreach($current_breadcrumbs as $key => $value) {
            $crumbs[] = $value;
          }
          $crumbs[] = t(isset($record['content']['name']) ? $record['content']['name'] : $record['id']);
          drupal_set_breadcrumb($crumbs);

            // If a record doesn't exist, set a message.
          if(empty($record)) {
            drupal_set_message( t('The movement record could not be found. (ID = ' . $movement_id) . ')', 'error' );
            drupal_goto('miss/manage/lunchCounter');
          }

            // Get the form.
          $data['page']['form'] = drupal_get_form('miss_movement_delete_form', $record);
          $output = '<form id="' . $data['page']['form']['#id'] . '" accept-charset="UTF-8" method="' . $data['page']['form']['#method'] . '" action="' . $data['page']['form']['#action'] . '">';
          $output .= drupal_render_children($data['page']['form']);

        } else {
            // If a record doesn't exist, set a message.
          drupal_set_message( t('The movement record could not be found. (ID = ' . $movement_id) . ')', 'error' );
          drupal_goto('miss/manage/lunchCounter');
        }
      }

      return $output;
      break;

      case 'addQuestion':
      if(!empty($path_parts[4])) {

          //grab the id
        $movement_id = ($path_parts[4] != 'new' ? $path_parts[4] : '');

          // CSS
        drupal_add_css(
          drupal_get_path('module', 'miss') . '/css/_lunch_counter.css',
          array('group' => CSS_THEME, 'type' => 'file', 'weight' => 1)
          );

        $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );

          // Build-out the breadcrumbs.
        $current_breadcrumbs = drupal_get_breadcrumb();
        $crumbs = array();
        foreach($current_breadcrumbs as $key => $value) {
          $crumbs[] = $value;
        }
        $movement_name = 'new';
        if(array_key_exists($movement_id, $result['components'])) {
          $movement_name = isset($result['components'][$movement_id]['content']['name']) ? $result['components'][$movement_id]['content']['name'] : 'new';
        }
        $crumbs[] = $movement_name != 'new' ? l($movement_name, 'miss/manage/lunchCounter/edit/' . $movement_id) : t('new');
        $crumbs[] = t('Add a New Question');
        drupal_set_breadcrumb($crumbs);

          // Get the form.
        $data['page']['form'] = drupal_get_form('miss_question_form');

        drupal_set_title($title . ': Add a New Question');
        $output = '<form id="' . $data['page']['form']['#id'] . '" accept-charset="UTF-8" method="' . $data['page']['form']['#method'] . '" action="' . $data['page']['form']['#action'] . '">';
        $output .= drupal_render($data['page']['form']['enabled_field']);
        $output .= drupal_render($data['page']['form']['text']);
        $output .= drupal_render($data['page']['form']['description']);
        $output .= drupal_render($data['page']['form']['imageUrl']);
        $output .= drupal_render($data['page']['form']['imageCredit']);
        $output .= drupal_render_children($data['page']['form']);
      } else {

        drupal_set_message('No movement was found to attach the question to. Please try again.');
        drupal_goto('miss/manage/lunchCounter');
      }

      return $output;
      break;

      // Edit an existing record.
      case 'editQuestion':

      if(!empty($path_parts[4])) {

          //grab the id
        $movement_id = ($path_parts[4] != 'new' ? $path_parts[4] : '');

        if(!empty($path_parts[5])) {

            //grab the id
          $question_id = $path_parts[5];

            // CSS
          drupal_add_css(
            drupal_get_path('module', 'miss') . '/css/_lunch_counter.css',
            array('group' => CSS_THEME, 'type' => 'file', 'weight' => 1)
            );

          $record = array();

            // Get all of the components.
          $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );

          if(array_key_exists($question_id, $result['components'])) {
            $record = $result['components'][$question_id];
          }

          if(!empty($record)) {

              // EDAN record dump
            if(!empty($record) && isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
              $dump = '<pre>' . var_export($record, TRUE) . '</pre>';
              drupal_set_message( $dump, 'status' );
            }

              // Set the page title.
            drupal_set_title('Manage Question "' . (isset($record['content']['text']) ? strip_tags($record['content']['text']) : $record['id']) . '"');
              // Build-out the breadcrumbs.
            $current_breadcrumbs = drupal_get_breadcrumb();
            $crumbs = array();
            foreach($current_breadcrumbs as $key => $value) {
              $crumbs[] = $value;
            }
            if(array_key_exists($movement_id, $result['components'])) {
              $movement_name = $result['components'][$movement_id]['content']['name'];
            }
            $crumbs[] = l($movement_name, 'miss/manage/lunchCounter/edit/' . $movement_id);
            $crumbs[] = t(isset($record['content']['text']) ? strip_tags($record['content']['text']) : $record['id']);
            drupal_set_breadcrumb($crumbs);

              // If a record doesn't exist, set a message.
            if(empty($record)) {
              drupal_set_message( t('The question record could not be found. (ID = ' . $question_id) . ')', 'error' );
              drupal_goto('miss/manage/lunchCounter');
            }

              // Get the form.
            $data['page']['form'] = drupal_get_form('miss_question_form', $record);
            $output = '<form id="' . $data['page']['form']['#id'] . '" accept-charset="UTF-8" method="' . $data['page']['form']['#method'] . '" action="' . $data['page']['form']['#action'] . '">';
            $output .= drupal_render($data['page']['form']['enabled_field']);
            $output .= drupal_render($data['page']['form']['text']);
            $output .= drupal_render($data['page']['form']['description']);
            $output .= drupal_render($data['page']['form']['imageUrl']);
            $output .= drupal_render($data['page']['form']['imageCredit']);
            $output .= drupal_render_children($data['page']['form']);

          } else {
              // If a record doesn't exist, set a message.
            drupal_set_message( t('The question record could not be found. (ID = ' . $question_id) . ')', 'error' );
            drupal_goto('miss/manage/lunchCounter');
          }

        }
      }

      return $output;
      break;

      case 'deleteQuestion':
      if(!empty($path_parts[4])) {

          //grab the id
        $movement_id = ($path_parts[4] != 'new' ? $path_parts[4] : '');

        if(!empty($path_parts[5])) {

            //grab the id
          $question_id = $path_parts[5];

          // CSS
          drupal_add_css(
            drupal_get_path('module', 'miss') . '/css/_lunch_counter.css',
            array('group' => CSS_THEME, 'type' => 'file', 'weight' => 1)
            );


          $record = array();
           // Get all of the components.
          $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/lunch_counter/getComponents.htm' );

          if(array_key_exists($question_id, $result['components'])) {
            $record = $result['components'][$question_id];
          }

          if(!empty($record)) {

            // EDAN record dump
            if(!empty($record) && isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
              $dump = '<pre>' . var_export($record, TRUE) . '</pre>';
              drupal_set_message( $dump, 'status' );
            }

            // Set the page title.
            drupal_set_title('Delete Question "' . (isset($record['content']['text']) ? strip_tags($record['content']['text']) : $record['id']) . '"');
            // Build-out the breadcrumbs.
            $current_breadcrumbs = drupal_get_breadcrumb();
            $crumbs = array();
            foreach($current_breadcrumbs as $key => $value) {
              $crumbs[] = $value;
            }
            if(array_key_exists($question_id, $result['components'])) {
              $movement_name = $result['components'][$movement_id]['content']['name'];
            }
            $crumbs[] = l($movement_name, 'miss/manage/lunchCounter/edit/' . $question_id);
            $crumbs[] = t(isset($record['content']['text']) ? strip_tags($record['content']['text']) : $record['id']);
            drupal_set_breadcrumb($crumbs);

            // If a record doesn't exist, set a message.
            if(empty($record)) {
              drupal_set_message( t('The question record could not be found. (ID = ' . $question_id) . ')', 'error' );
              drupal_goto('miss/manage/lunchCounter');
            }

            // Get the form.
            $data['page']['form'] = drupal_get_form('miss_question_delete_form', $record);
            $output = '<form id="' . $data['page']['form']['#id'] . '" accept-charset="UTF-8" method="' . $data['page']['form']['#method'] . '" action="' . $data['page']['form']['#action'] . '">';
            $output .= drupal_render_children($data['page']['form']);

          } else {
            // If a record doesn't exist, set a message.
            drupal_set_message( t('The question record could not be found. (ID = ' . $question_id) . ')', 'error' );
            drupal_goto('miss/manage/lunchCounter');
          }
        }
      }

      return $output;
      break;

      // Send a 404.
      default:
      return MENU_NOT_FOUND;

    }

  }

}

$output = _miss_lunch_counter_page_build($interactive);