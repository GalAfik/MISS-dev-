<?php

/**
 * Implements hook_form().
 */
function miss_reflections_booth_daterange_form() {

  $form = array();

  $form['daterange'] = array(
    '#type' => 'fieldset',
    '#title' => 'Search By Date Range',
    '#attributes' => array('class' => array('container-inline')),
  );

  // Start date
  $form['daterange']['startDate'] = array(
    '#type' => 'date_popup',
    '#title' => t('Start Date'),
    '#size' => 10,
    '#required' => TRUE,
    '#date_format' => 'Y-m-d',
    '#attached' => array(
      'library' => array(
        array('system', 'ui.datepicker'),
      ),
    ),
  );

  // End date
  $form['daterange']['endDate'] = array(
    '#type' => 'date_popup',
    '#title' => t('End Date'),
    '#size' => 10,
    '#required' => TRUE,
    '#date_format' => 'Y-m-d',
    '#attached' => array(
      'library' => array(
        array('system', 'ui.datepicker'),
      ),
    ),
  );

  $form['daterange']['search_daterange'] = array(
    '#type' => 'submit',
    '#name' => 'search_daterange',
    '#value' => 'Filter by Date Range',
    '#attributes' => array('style' => 'background: #008cba; color: #fff; -webkit-border-radius: 0; border-radius: 0;'),
  );

  $add_js = '
    jQuery(document).ready(function() {
      jQuery("#edit-search-daterange").hover(
      function() {
        jQuery("#edit-search-daterange").css("background", "#0079a1")
      },
      function() {
        jQuery("#edit-search-daterange").css("background", "#008cba")
      }
      );
    });
 ';
  drupal_add_js($add_js, 'inline');

  $add_css = '
    .form-item-startDate-date label {
      display: none;
    }
    .form-item-endDate-date label {
      display: none;
    }
    .form-item-startDate label {
      float: left;
      margin-top: 6px;
      margin-right: 4px;
    }
    .form-item-endDate label {
      float: left;
      margin-top: 6px;
      margin-right: 4px;
    }
  ';
  drupal_add_css($add_css, 'inline');

  return $form;
}

/**
 * Implements hook_form().
 */
function miss_reflections_booth_search_form( $form, &$form_state ) {
  $form = array();
  $form['#tree'] = TRUE;

  $form['search'] = array(
    '#type' => 'fieldset',
    '#title' => 'Filter Video Submissions',
    '#attributes' => array('class' => array('container-inline')),
    );

  $form['search']['search_column'] = array(
    '#type' => 'select',
    '#title' => t('Field'),
    '#multiple' => FALSE,
    '#options' => array(
      'promptQuestion' => 'Prompt Question',
      'status' => 'Status',
      'reviewer' => 'Reviewer',
      'tags' => 'Tags',
      'age' => 'Age',
      'gender' => 'Gender',
      'race' => 'Race / Ethnicity',
      'groupType' => 'Type of Group',
      'location' => 'City',
      'region' => 'State',
      ),
    '#attributes' => array('placeholder' => t('Filter on ...'), 'style' => ''),
    );

  $form['search']['search_term'] = array(
    '#type' => 'textfield',
    '#title' => t('Value'),
    '#attributes' => array('style' => ''),
    );

  $form['search']['search_filter'] = array(
    '#type' => 'submit',
    '#name' => 'filter',
    '#value' => 'Filter',
    '#attributes' => array('style' => 'background: #008cba; color: #fff; -webkit-border-radius: 0; border-radius: 0;'),
    );

  $js_add_css = '
 jQuery(document).ready(function() {
  jQuery("#edit-search-search-filter").hover(
  function() {
    jQuery("#edit-search-search-filter").css("background", "#0079a1")
  },
  function() {
    jQuery("#edit-search-search-filter").css("background", "#008cba")
  }
  );
 });
 ';
  drupal_add_js($js_add_css, 'inline');

  $form['search']['search_clear'] = array(
    '#type' => 'submit',
    '#name' => 'clear',
    '#value' => 'Clear',
    '#attributes' => array('style' => '-webkit-border-radius: 0; border-radius: 0;'),
    );

  return $form;
}

/**
 * Implements hook_form().
 */
function miss_prompt_questions_form( $form, &$form_state, $record = array() ) {

  $form = array();
  $form['#tree'] = TRUE;

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

  // disable gallery tag field if the status is 0 and the user is not an admin
global $user;
$form['tag'] = array(
  '#type' => 'textfield',
  '#maxlength' => 524288,
  '#title' => t('Gallery Tag'),
  '#size' => 65,
  '#maxlength' => 524288,
  '#required' => TRUE,
  '#default_value' => !empty($record['galleryTag']) ? $record['galleryTag'] : '',
  '#prefix' => isset($record['id']) ? '<p><a href="/miss/manage/reflectionsBooth/videos/' . $record['id'] . '">Browse Video Submissions</a></p>' : '',
  '#suffix' => '<p>Note: The first 4 questions will appear in the kiosk interface.</p>',
  '#disabled' => (!user_access('edit reflections booth content') && isset($record['status']) && $record['status'] == 1) || (!in_array('administrator', array_values($user->roles)) && isset($record['status']) && $record['status'] == 0) ? TRUE : FALSE,
  );

$form['gallery_image'] = array(
  '#type' => 'textfield',
  '#title' => t('Gallery Image'),
  '#size' => 65,
  '#maxlength' => 524288,
  '#default_value' => !empty($record['content']['galleryImageUrl']) ? $record['content']['galleryImageUrl'] : '',
  '#suffix' => !empty($record['content']['galleryImageUrl']) ? '<div style="margin-bottom: 0rem;"><a href="' . $record['content']['galleryImageUrl'] . '" target="_blank">View Image</a> (opens in a new window/tab)</div>' : '',
  '#attributes' => array(
    'class' => array(
      'miss-file-manage',
      ),
    ),
  '#disabled' => (!user_access('edit reflections booth content')) ? TRUE : FALSE,
  );

$form['gallery_image_credit'] = array(
  '#type' => 'text_format',
  '#format' => 'full_html',
  '#maxlength' => 524288,
  '#title' => t('Gallery Image Credit'),
  '#size' => 65,
  '#maxlength' => 524288,
  '#default_value' => !empty($record['content']['galleryImageCredit']) ? $record['content']['galleryImageCredit'] : '',
  '#disabled' => (!user_access('edit reflections booth content')) ? TRUE : FALSE,
  );

$form['id'] = array(
  '#type' => 'hidden',
  '#default_value' => (isset($record['id'])) ? $record['id'] : '',
  );

if(empty($form_state['num_items']) && empty($record['promptQuestions'])) {
  $form_state['num_items'] = 1;
}

$form_state['num_items'] = !empty($form_state['num_items']) ? $form_state['num_items'] : 0;

if(!empty($record['promptQuestions'])) {
  if(!$form_state['num_items']) {
    $form_state['num_items'] = count($record['promptQuestions']);
  }
}

  // Build the number of item fieldsets indicated by $form_state['num_items']
for ($i = 1; $i <= $form_state['num_items']; $i++) {

  $id = !empty($record['promptQuestions'][$i-1]['id']) ? $record['promptQuestions'][$i-1]['id'] : 'new'.$i;

  $form['promptQuestions'][$id] = array(

    'text' => array(
      '#type' => 'text_format',
      '#format' => 'full_html',
      '#maxlength' => 524288,
      '#title' => t('Question #' . $i),
      '#size' => 97,
      '#maxlength' => 524288,
      '#required' => TRUE,
      '#default_value' => !empty($record['promptQuestions'][$i-1]['text']) ? $record['promptQuestions'][$i-1]['text'] : '',
      '#disabled' => (!user_access('edit reflections booth content')) ? TRUE : FALSE,
      ),

    'weight' => array(
      '#type' => 'textfield',
      '#maxlength' => 524288,
      '#default_value' => $i,
      '#size' => 3,
      '#attributes' => array('class' => array('item-row-weight')),
      ),

    );

}

$form['submit'] = array(
  '#type' => 'submit',
  '#value' => 'Submit',
  '#disabled' => (!user_access('edit reflections booth content')) ? TRUE : FALSE,
  );

  // Adds a button to add more items.
$form['add_item'] = array(
  '#type' => 'submit',
  '#value' => t('Add a question'),
  '#submit' => array('_miss_prompt_questions_form_add_item'),
  '#disabled' => (!user_access('edit reflections booth content')) ? TRUE : FALSE,
  );

  // If we have more than one item, this button allows removal of the last item.
if ($form_state['num_items'] > 1) {
  $form['remove_item'] = array(
    '#type' => 'submit',
    '#value' => t('Remove latest question'),
    '#submit' => array('_miss_prompt_questions_form_remove_item'),
      // Since we are removing an item, don't validate until later.
    '#limit_validation_errors' => array(),
    '#disabled' => (!user_access('edit reflections booth content')) ? TRUE : FALSE,
    );
}

$js = 'jQuery().ready(function(){
  jQuery(".filter-wrapper").hide();
});';
drupal_add_js($js, 'inline');

return $form;
}

function _miss_prompt_questions_form_add_item($form, &$form_state) {
  $form_state['num_items']++;
  $form_state['rebuild'] = TRUE;
}

function _miss_prompt_questions_form_remove_item($form, &$form_state) {
  if ($form_state['num_items'] > 1) {
    $form_state['num_items']--;
  }
  // Setting $form_state['rebuild'] = TRUE causes the form to be rebuilt again.
  $form_state['rebuild'] = TRUE;
}

/**
 * Implements hook_form_submit().
 */
function miss_prompt_questions_form_submit($form, &$form_state) {

  $ids_array = array();

  foreach ($form_state['input']['promptQuestions'] as $key => $value) {
    if(!empty($value['text'])) {

      // The 'value' comparison should check to see if $key equals 0 , and are of the same type (!==), which should be zero, not boolean.
      // See: http://php.net/manual/en/language.operators.comparison.php
      // $id = (strpos($key, 'new') == false) ? '&id=' . $key : '';
      $id = (strpos($key, 'new') !== 0) ? '&id=' . $key : '';

      // Remove p tags from prompt questions as requested by OCIO
      $tags_to_remove = array('<p>', '</p>');
      $uri = 'text=' . urlencode(str_replace($tags_to_remove, '', $value['text']['value']));

      $data = _miss_send_miss_request( variable_get('reflectionsSetPromptQuestion'), $uri . $id);
      // Gather all of the Art Profile ids.
      if(isset($data['id'])){
        array_push($ids_array, $data['id']);
      }
    }
  }

  //grab current attractMediaLoop
  $featured_ids = isset($form_state['build_info']['args'][0]['content']['attractMediaLoop']) ? $form_state['build_info']['args'][0]['content']['attractMediaLoop'] : array();

  if(!empty($ids_array)) {

    $id = !empty($form_state['input']['id']) ? '&id=' . $form_state['input']['id'] : '';
    // Set the gallery tag, with the array of question ids, and gallery id if it exists.
    $uri_set = 'tag=' . urlencode($form_state['input']['tag']);
    $uri_set .= '&galleryImageUrl=' . urlencode($form_state['input']['gallery_image']);
    $uri_set .= '&galleryImageCredit=' . urlencode($form_state['input']['gallery_image_credit']['value']);
    $uri_set .= '&promptQuestions=' . json_encode($ids_array);
    $set = _miss_send_miss_request( variable_get('reflectionsSetGalleryTag'), $uri_set . $id );

    //reset attractMediaLoop back to what it was
    $uri = 'galleryId=' . $form_state['input']['id'] . '&attractMediaLoop=' . json_encode($featured_ids);
    $result = _miss_send_miss_request( variable_get('reflectionsSetAttractMediaLoop'), $uri );

    if(!empty($set['id'])) {
      drupal_set_message( t('Prompt questions for the "' . $form_state['input']['tag'] . '" gallery have been successfully saved.') );
    } else {
      drupal_set_message( t('Could not save the record. Please check, and try again.'), 'error' );
    }

  }

  drupal_goto('miss/manage/reflectionsBooth');
}

/**
 * Table drag elements.
 */
function miss_reflections_booth_table_drag_elements($vars) {

  $form_element = $vars['page']['form']['promptQuestions'];

  drupal_add_tabledrag('reflections_booth_questions', 'order', 'sibling', 'item-row-weight');

  $data['header'] = array(
    'text' => t('Questions'),
    'weight' => t('Weight'),
    );

  $rows = array();
  foreach (element_children($form_element) as $key) {

    $row = array();
    $row['data'] = array();
    foreach ($data['header'] as $fieldname => $title) {
      $field = $form_element[$key][$fieldname];
      $row['data'][] = drupal_render($field);
      $row['class'] = array('draggable');
    }
    $data['rows'][] = $row;
  }

  return $data;
}

/**
 * Implements hook_form().
 */
function miss_video_submissions_form( $form, &$form_state, $record = array(), $all_records = array() ) {

  if(isset($record['userConsent']) && $record['userConsent']){

    $form = array();

    // Backwards, because EDAN's status flags are backwards.
    $featured = (isset($record['featured']) && $record['featured']) ? 1 : 0;
    $deletionRecommendation = (isset($record['deletionRecommendation']) && $record['deletionRecommendation']) ? 1 : 0;

    if($deletionRecommendation){
      $background_js = '
      jQuery(document).ready(function() {
        jQuery("#edit-moderationfield").css("background-color", "#FFCCCB");
      });';
      drupal_add_js($background_js, 'inline');
    }

    $markup = _miss_video_submissions_form_markup( $record );
    $poster_image_markup = _miss_video_submissions_form_poster_image_markup( $record );
    $set_start_time_markup =(user_access('edit reflections booth content')) ? '<button class="markerStartTime" data-current-time="">Transfer Current Time</button>' : '' . "\n";
    $set_end_time_markup =(user_access('edit reflections booth content')) ? '<button class="markerEndTime" data-current-time="">Transfer Current Time</button>' : ''  . "\n";
    $set_poster_image_markup =(user_access('edit reflections booth content')) ? '<h4 style="font-weight: bold;">Set the Poster and Thumbnail Images</h4>' : '<h4 style="font-weight: bold;">Poster and Thumbnail Image</h4>' . "\n";
    $set_poster_image_markup .=(user_access('edit reflections booth content')) ? '<p>To set the poster image and thumbnail, navigate to a frame and click the "Set Poster Image" button. Please allow several minutes for thumbnail submisstions to be uploaded and set.</p>' : '' . "\n";
    $set_poster_image_markup .=(user_access('edit reflections booth content')) ? '<a href="javascript:void(0);" class="button setPosterImage">Set Poster Image</a>' : ''. "\n";
    $clearfix_markup = '<div class="clearfix" style="margin-bottom: 1rem;"></div>' . "\n";

    $description = '';
    switch ($record['status']) {
      case 0:
      $description = 'This record is currently <span style="color:green;">APPROVED</span>. You can unapprove it by' . ( (isset($record['featured']) && $record['featured'] == true) ? ' unchecking the "Featured" box and then ' : ' ') . 'clicking "Pending Review" or delete it permanantly by clicking "Delete".';
      break;
      default:
      $description = 'This record is currently <span style="color:#FF9900;">PENDING REVIEW</span>. You can approve it by clicking "Approve" or delete it permanantly by clicking "Delete".';
      break;
    }
    if(isset($record['deletionRecommendation']) && $record['deletionRecommendation']){
      $description .= ' This record has also been recommended for <span style="color:red;">DELETION</span> by a reviewer. Deleted videos are deleted permanantly and cannot be recovered.';
    }
    if(user_access('review miss content')){
      $description = 'Records may only be moderated by Moderators and Admins. In order to Approve, Reject, or Delete this video, please contact a Moderator or Administrator.';
    }

    //hide upload/insert buttons for non editors
    $button_script = '
    jQuery(document).ready(function() {
      jQuery("a:contains(' . "'Upload / Insert'" . ')").hide();
    });
    ';
    drupal_add_js($button_script, 'inline');

    $form['moderationField'] = array(
      '#type' => 'fieldset',
      '#title' => t('Moderation Tools'),
      '#description' => $description,
      );

    $form['moderationField']['approve'] = array(
      '#name' => 'approve_record',
      '#type' => 'submit',
      '#value' => t('Approve'),
      '#submit' => array('_miss_reflections_booth_form_approve'),
      '#attributes' => array('style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 0rem;'),
      '#disabled' => !user_access('moderate miss content') ? TRUE : FALSE,
      );

    if($record['status'] != 1) $form['moderationField']['approve']['#type'] = 'hidden';

    $form['moderationField']['reject'] = array(
      '#name' => 'reject_record',
      '#type' => 'submit',
      '#value' => t('Pending Review'),
      '#submit' => array('_miss_reflections_booth_form_reject'),
      '#attributes' => array('style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 0rem;'),
      '#disabled' => !user_access('moderate miss content') ? TRUE : FALSE,
      );

    if($record['status'] != 0) $form['moderationField']['reject']['#type'] = 'hidden';

    $form['moderationField']['delete'] = array(
      '#name' => 'delete_record',
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('_miss_reflections_booth_form_delete'),
      '#attributes' => array(
        'style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 0rem; color: red;',
        'onclick' => 'if(!confirm("Are you sure you want to Delete this record? This action cannot be undone.")){return false;}',
        ),
      '#disabled' => !user_access('moderate miss content') ? TRUE : FALSE,
      );

    $form['moderationField']['deletionRecommendation'] = array(
      '#type' => 'checkbox',
      '#title' => t('Video Deletion Recommended'),
      '#prefix' => '<div style="float:right">',
      '#suffix' => '</div>',
      '#default_value' => $deletionRecommendation,
      '#disabled' => (user_access('edit reflections booth content') || user_access('review reflections booth content')) ? FALSE : TRUE,
      );

    $number_of_featured_videos = count(_miss_reflections_booth_get_featured_records( $record['galleryTagId'] ));

    $form['moderationField']['moderationHelptext'] = array(
      '#type' => 'fieldset',
      '#title' => '&#x2753 Deletion Recommendation Guidelines',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      );

    $form['moderationField']['moderationHelptext']['text'] = array(
      '#type' => 'item',
      '#markup' => '<i><p>Please recommend recordings with the following features for deletion:</p>
      <ol>
        <li>Less than 30 seconds long</li>
        <li>Poor sound or video quality</li>
        <li>Does not answer the question and/or does not address any of the priority areas listed above. In other words, is generally irrelevant</li>
        <li>Is offensive (profane, racist, sexist, etc...)</li>
      </ol>
    </i>',
    );
    $js = "jQuery(document).ready(function(){
      jQuery('#edit-moderationhelptext').find('.fieldset-legend').css('background-color', '#FFFCE5');
      jQuery('#edit-moderationhelptext').find('.fieldset-legend')
      .css('background-image', 'none')
      .css('background-repeat', 'no-repeat')
      .css('background-position', 'left center')
      .css('text-indent', '0px')
      .css('line-height', '1.95');
      jQuery('#edit-moderationhelptext').css('margin-bottom', '15px');
    });";
    drupal_add_js($js, 'inline');

    $form['featuredItem'] = array(
      '#type' => 'fieldset',
      '#title' => t('Featured Video'),
      '#description' => (!$featured && $number_of_featured_videos >= 9) ? '<span style="color:red;">The maximum amount of featured videos has been reached. This video may not be set to featured at this time.</span>' : '',
      );
    $form['featuredItem']['featured'] = array(
      '#type' => 'checkbox',
      '#title' => t('Set as a featured video'),
      '#default_value' => $featured,
      '#prefix' => '<div style="float:left">',
      '#suffix' => '</div>',
      '#disabled' => (!user_access('edit reflections booth content') || (!$featured && $number_of_featured_videos >= 9)) ? TRUE : FALSE,
      );

    $form['videoFileUrl'] = array(
      '#type' => 'hidden',
      '#title' => t('Video URL'),
      '#size' => 65,
      '#maxlength' => 524288,
      '#required' => TRUE,
      '#default_value' => !empty($record['videoFileUrl']) ? $record['videoFileUrl'] : '',
      '#attributes' => array(
        'class' => array(
          'miss-file-manage',
          ),
        ),
      '#disabled' => (!user_access('edit reflections booth content')) ? TRUE : FALSE,
      );

    $form['videoPosterImageCatcher'] = array(
      '#type' => 'hidden',
      '#prefix' => $markup,
      '#suffix' => $clearfix_markup,
      '#attributes' => array(
        'class' => array(
          'video-poster-image',
          ),
        ),
      );

    $form['videoThumbnailImageCatcher'] = array(
      '#type' => 'hidden',
      '#attributes' => array(
        'class' => array(
          'video-thumbnail-image',
          ),
        ),
      );

    $form['videoPosterImageUrl'] = array(
      '#type' => 'hidden',
      '#default_value' => !empty($record['videoPosterImageUrl']) ? $record['videoPosterImageUrl'] : '',
      );

    $form['videoThumbnailImageUrl'] = array(
      '#type' => 'hidden',
      '#default_value' => !empty($record['videoThumbnailImageUrl']) ? $record['videoThumbnailImageUrl'] : '',
      );

    $form['reviewerSection'] = array(
      '#type' => 'fieldset',
      '#title' => t('Reviewer Section'),
      '#collapsible' => TRUE,
      );

    $form['reviewerSection']['tags'] = array(
      '#type' => 'select',
      '#multiple' => TRUE,
      '#title' => t('Tags (select all that apply)'),
      '#options' => _miss_reflections_booth_tags_field_options(),
      // '#default_value' => !empty($record['tags']) ? _miss_reflections_booth_get_default_tags($record['tags']) : '',
      '#default_value' => !empty($record['tags']) ? $record['tags'] : '',
      '#attributes' => array('data-placeholder' => 'Choose tags...', 'style' => 'width: 545px;'),
      '#disabled' => (user_access('edit reflections booth content') || user_access('review reflections booth content')) ? FALSE : TRUE,
      );

    $form['reviewerSection']['commentsHelptext'] = array(
      '#type' => 'fieldset',
      '#title' => '&#x2753 Reviewer Comments Guidelines',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      );

    $form['reviewerSection']['commentsHelptext']['text'] = array(
      '#type' => 'item',
      '#markup' => '<i><p>In the box below, write a summary of the topics/themes covered in the recording. In your brief summary, please make note of the following features, if present:</p>
      <ol>
        <li>Visitor shares memory of a significant historical event, movement, or era (slavery, abolitionism, Civil Rights, Black Power, Black Lives Matter, etc...).</li>
        <li>Visitor describes a family tradition or detailed genealogy (Juneteenth, Homecoming, family tree).</li>
        <li>Visitors recalls personal experiencein a significant event of the modern era (Grand Opening of NMAAHC, Inauguration, Women’s march, Science March, etc...).</li>
        <li>Visitor describes a familial connection to a significant place of location-such as one of the many places featured in our galleries (Tulsa, Edisto, The Bronx).</li>
        <li>Visitor mentions artifacts, documents, or photographs potentially available to the Museum.</li>
        <li>Visitor makes a particularly insightful comment about the exhibition itself; things they like, dislike, are confused by, are sure to remember, etc.... not general or non-specific comments like “I like it.”</li>
        <li>Visitor shares a particularly poignant comment or memory (of course, this is your subjective judgement).</li>
      </ol>
    </i>',
    );
    $js = "jQuery(document).ready(function(){
      jQuery('#edit-commentshelptext').find('.fieldset-legend').css('background-color', '#FFFCE5');
      jQuery('#edit-commentshelptext').find('.fieldset-legend')
      .css('background-image', 'none')
      .css('background-repeat', 'no-repeat')
      .css('background-position', 'left center')
      .css('text-indent', '0px')
      .css('line-height', '1.95');
      jQuery('#edit-commentshelptext').css('margin-bottom', '20px');
    });";
    drupal_add_js($js, 'inline');

    $form['reviewerSection']['comments'] = array(
      '#type' => 'text_format',
      '#title' => t('Editor Comments'),
      '#resizable' => TRUE,
      '#default_value' => !empty($record['comments']) ? $record['comments'] : '',
      '#format' => 'full_html',
      '#disabled' => (user_access('edit reflections booth content') || user_access('review reflections booth content')) ? FALSE : TRUE,
      );

    $form['reviewerSection']['artifact'] = array(
      '#type' => 'text_format',
      '#title' => t('Artifact'),
      '#resizable' => TRUE,
      '#default_value' => !empty($record['artifact']) ? $record['artifact'] : '',
      '#format' => 'full_html',
      '#disabled' => (user_access('edit reflections booth content') || user_access('review reflections booth content')) ? FALSE : TRUE,
      );

    $form['reviewerSection']['age'] = array(
      '#type' => 'select',
      '#title' => t('Age'),
      '#default_value' => !empty($record['age']) ? $record['age'] : 'Unknown',
      '#options' => array(
        'Unknown' => 'Unknown',
        'Child' => 'Child',
        'Teenager' => 'Teenager',
        'Adult' => 'Adult',
        'senior' => 'Senior',
        'Mixed Group' => 'Mixed Group',
        ),
      '#disabled' => (user_access('edit reflections booth content') || user_access('review reflections booth content')) ? FALSE : TRUE,
      );

    $form['reviewerSection']['gender'] = array(
      '#type' => 'select',
      '#title' => t('Gender'),
      '#default_value' => !empty($record['gender']) ? $record['gender'] : 'Unknown',
      '#options' => array(
        'Unknown' => 'Unknown',
        'Male' => 'Male',
        'Female' => 'Female',
        'Mixed Group' => 'Mixed Group',
        ),
      '#disabled' => (user_access('edit reflections booth content') || user_access('review reflections booth content')) ? FALSE : TRUE,
      );

    $form['reviewerSection']['race'] = array(
      '#type' => 'textfield',
      '#title' => t('Race / Ethnicity'),
      '#default_value' => !empty($record['race']) ? $record['race'] : '',
      '#size' => 30,
      '#maxlength' => 30,
      '#disabled' => (user_access('edit reflections booth content') || user_access('review reflections booth content')) ? FALSE : TRUE,
      );

    $form['reviewerSection']['groupType'] = array(
      '#type' => 'select',
      '#multiple' => TRUE,
      '#title' => t('Type of Group (select all that apply)'),
      '#default_value' => !empty($record['groupType']) ? $record['groupType'] : '',
      '#options' => array(
        'Family Members' => 'Family Members',
        'Friends' => 'Friends',
        'Intergenerational' => 'Intergenerational',
        'School Group' => 'School Group',
        'Unknown' => 'Unknown',
        ),
      '#disabled' => (user_access('edit reflections booth content') || user_access('review reflections booth content')) ? FALSE : TRUE,
      '#attributes' => array('data-placeholder' => 'Choose Group Types...', 'style' => 'width: 545px;'),
      );

    $form['reviewerSection']['location']['city'] = array(
      '#type' => 'textfield',
      '#title' => t('Location'),
      '#default_value' => !empty($record['location']) ? $record['location'] : '',
      '#size' => 30,
      '#maxlength' => 30,
      '#disabled' => (user_access('edit reflections booth content') || user_access('review reflections booth content')) ? FALSE : TRUE,
      );

    $form['reviewerSection']['location']['region'] = array(
      '#type' => 'select',
      '#options' => _miss_reflections_booth_region_form_field_options(),
      '#default_value' => !empty($record['region']) ? $record['region'] : '',
      '#disabled' => (user_access('edit reflections booth content') || user_access('review reflections booth content')) ? FALSE : TRUE,
      );

    $form['reviewerSection']['reviewer'] = array(
      '#type' => 'textfield',
      '#title' => t('Reviewer'),
      '#default_value' => !empty($record['reviewer']) ? $record['reviewer'] : '',
      '#disabled' => TRUE,
      '#size' => 30,
      );

    $form['reviewerSection']['reviewDate'] = array(
      '#type' => 'textfield',
      '#title' => t('Review Date'),
      '#default_value' => !empty($record['last_time_updated']) ? $record['last_time_updated'] : '',
      '#disabled' => TRUE,
      '#size' => 30,
      );

    $form['videoMetadata'] = array(
      '#type' => 'fieldset',
      '#title' => t('Video Metadata'),
      '#collapsible' => TRUE,
      );

    $form['videoMetadata']['markerStartTime'] = array(
      '#type' => 'textfield',
      '#maxlength' => 524288,
      '#title' => t('Video Start Time'),
      '#description' =>( user_access('edit reflections booth content') || user_access('moderate miss content')) ? t('To set the video\'s start time, pause the video and transfer the current time using the button below.') : '',
      '#size' => 10,
      '#maxlength' => 524288,
      '#required' => FALSE,
      '#default_value' => !empty($record['markerStartTime']) ? $record['markerStartTime'] : '',
      '#suffix' => $set_start_time_markup . $clearfix_markup,
      '#disabled' => (!user_access('edit reflections booth content')) ? TRUE : FALSE,
      );

    $form['videoMetadata']['markerEndTime'] = array(
      '#type' => 'textfield',
      '#maxlength' => 524288,
      '#title' => t('Video End Time'),
      '#description' =>( user_access('edit reflections booth content') || user_access('moderate miss content')) ? t('To set the video\'s end time, pause the video and transfer the current time using the button below.') : '',
      '#size' => 10,
      '#maxlength' => 524288,
      '#required' => FALSE,
      '#default_value' => !empty($record['markerEndTime']) ? $record['markerEndTime'] : '',
      '#suffix' => $set_end_time_markup . $clearfix_markup . $set_poster_image_markup . $poster_image_markup,
      '#disabled' => (!user_access('edit reflections booth content')) ? TRUE : FALSE,
      );

    $form['userData'] = array(
      '#type' => 'fieldset',
      '#title' => t('Submitter\'s Information'),
      '#collapsible' => TRUE,
      '#attributes' => array(
        'style' => 'margin-top: 2.5rem;',
        'disabled' => 'disabled'
        ),
      );

    $form['userData']['userConsent'] = array(
      '#type' => 'checkbox',
      '#title' => t('The submitter of the video has agreed to the <a href="/miss/manage/reflectionsBooth/userReleaseForm">User Release Agreement</a>'),
      '#default_value' => !empty($record['userConsent']) ? $record['userConsent'] : false,
      '#prefix' => '<div class="clearfix"></div>' . "\n" . '<div style="float:left">',
      '#suffix' => '</div>' . "\n" . '<div class="clearfix"></div>' . "\n",
      );

    $form['userData']['name'] = array(
      '#type' => 'textfield',
      '#maxlength' => 524288,
      '#title' => t('Submitter\'s Name'),
      '#size' => 65,
      '#maxlength' => 524288,
      '#default_value' => !empty($record['name']) ? $record['name'] : 'Not specified',
      '#states' => array(
        'visible' => array(
          ':input[name="userConsent"]' => array('checked' => TRUE),
          ),
        ),
      );

    $form['userData']['emailAddress'] = array(
      '#type' => 'textfield',
      '#maxlength' => 524288,
      '#title' => t('Submitter\'s Email Address'),
      '#size' => 65,
      '#maxlength' => 524288,
      '#default_value' => !empty($record['emailAddress']) ? $record['emailAddress'] : 'Not specified',
      '#states' => array(
        'visible' => array(
          ':input[name="userConsent"]' => array('checked' => TRUE),
          ),
        ),
      );

    // This hidden field contains all of the current featuredIds, in JSON format.
    $form['featuredIds'] = array(
      '#type' => 'hidden',
      '#maxlength' => 1000,
      '#value' => !empty($record['featuredIds']) ? json_encode($record['featuredIds']) : '',
      );

    $form['id'] = array(
      '#type' => 'hidden',
      '#value' => (isset($record['id'])) ? $record['id'] : '',
      );

    $form['galleryTagId'] = array(
      '#type' => 'hidden',
      '#value' => (isset($record['galleryTagId'])) ? $record['galleryTagId'] : '',
      );

    $form['promptQuestionId'] = array(
      '#type' => 'hidden',
      '#value' => (isset($record['promptQuestionId'])) ? $record['promptQuestionId'] : $record['promptQuestId'],
      );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Save',
      '#disabled' => (!user_access('edit reflections booth content') && !user_access('review miss content')) ? TRUE : FALSE,
      );

    //if the status is set to pending approval, hide everything other than the moderation tools and video links

    if($record['status'] == 1){
      $form['featuredItem']['#type'] = 'hidden';
    }

    if(isset($record['featured']) && $record['featured'] == true){
      $form['moderationField']['reject']['#type'] = 'hidden';
    }
  }else{
    // global $user;
    drupal_set_title('Private Video');
    drupal_set_message('The owner of this video has not granted consent for the viewing, moderation, or distribution of this content. Please contact an administrator for further guidance.', 'error');
    // $form['deleteNonConsentVideo'] = array(
    //   '#type' => 'submit',
    //   '#value' => 'Delete This Record',
    //   '#name' => 'delete_non_consent_record',
    //   '#submit' => array('_miss_reflections_booth_form_delete'),
    //   '#attributes' => array(
    //     'style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 0rem; color: red;',
    //     'onclick' => 'if(!confirm("Are you sure you want to Delete this record? This action cannot be undone.")){return false;}',
    //     ),
    //   '#disabled' => (!in_array('administrator', array_values($user->roles)) ? TRUE : FALSE),
    //   '#prefix' => '<p>Clicking this button will permanantly delete this record. This video will not be recoverable and will be gone forever. This action may only be taken by Administrators.</p>',
    //   );
  }


  $js = "jQuery(document).ready(function(){
    jQuery('.filter-wrapper').hide();
  });";

  // jQuery('#screenShot').children('img').width(853).height(480);
  //  jQuery('#screenShot').width(853).height(480);

  drupal_add_js($js, 'inline');


  return $form;
}

function _miss_reflections_booth_region_form_field_options() {
  return array(
    '' => t('Select State'),
    'Alabama' => 'Alabama',
    'Alaska' => 'Alaska',
    'Arizona' => 'Arizona',
    'Arkansas' => 'Arkansas',
    'California' => 'California',
    'Colorado' => 'Colorado',
    'Connecticut' => 'Connecticut',
    'Delaware' => 'Delaware',
    'District Of Columbia' => 'District Of Columbia',
    'Florida' => 'Florida',
    'Georgia' => 'Georgia',
    'Hawaii' => 'Hawaii',
    'Idaho' => 'Idaho',
    'Illinois' => 'Illinois',
    'Indiana' => 'Indiana',
    'Iowa' => 'Iowa',
    'Kansas' => 'Kansas',
    'Kentucky' => 'Kentucky',
    'Louisiana' => 'Louisiana',
    'Maine' => 'Maine',
    'Maryland' => 'Maryland',
    'Massachusetts' => 'Massachusetts',
    'Michigan' => 'Michigan',
    'Minnesota' => 'Minnesota',
    'Mississippi' => 'Mississippi',
    'Missouri' => 'Missouri',
    'Montana' => 'Montana',
    'Nebraska' => 'Nebraska',
    'Nevada' => 'Nevada',
    'New Hampshire' => 'New Hampshire',
    'New Jersey' => 'New Jersey',
    'New Mexico' => 'New Mexico',
    'New York' => 'New York',
    'North Carolina' => 'North Carolina',
    'North Dakota' => 'North Dakota',
    'Ohio' => 'Ohio',
    'Oklahoma' => 'Oklahoma',
    'Oregon' => 'Oregon',
    'Pennsylvania' => 'Pennsylvania',
    'Puerto Rico' => 'Puerto Rico',
    'Rhode Island' => 'Rhode Island',
    'South Carolina' => 'South Carolina',
    'South Dakota' => 'South Dakota',
    'Tennessee' => 'Tennessee',
    'Texas' => 'Texas',
    'Utah' => 'Utah',
    'Vermont' => 'Vermont',
    'Virginia' => 'Virginia',
    'Washington' => 'Washington',
    'West Virginia' => 'West Virginia',
    'Wisconsin' => 'Wisconsin',
    'Wyoming' => 'Wyoming',
    'International' => 'International',
    );
}

function _miss_reflections_booth_tags_field_options() {
  // $json = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/render.htm' );
  // $sanitized = array();
  // $unwanted_tags = array('amp;', '\n');
  // if(!empty($json['stories'])){
  //   foreach ($json['stories'] as $item){
  //     array_push($sanitized, str_replace($unwanted_tags, '', strip_tags($item['name'])));
  //   }
  // }
  // return $sanitized;
  return array(
    'Activism' => 'Activism',
    'Africa & The Diaspora' => 'Africa & The Diaspora',
    'Artifact' => 'Artifact',
    'Black Power' => 'Black Power',
    'Civil Rights' => 'Civil Rights',
    'Communities' => 'Communities',
    'Culture/Traditions' => 'Culture/Traditions',
    'Education' => 'Education',
    'Families/Genealogy' => 'Families/Genealogy',
    'Gender' => 'Gender',
    'Historic Moments' => 'Historic Moments',
    'Immigration & Migration' => 'Immigration & Migration',
    'Military' => 'Military',
    'Museum/Exhibitions' => 'Museum/Exhibitions',
    'Organizations' => 'Organizations',
    'Politics' => 'Politics',
    'Race Relations' => 'Race Relations',
    'Religion/Faith' => 'Religion/Faith',
    'Segregation' => 'Segregation',
    'Slavery' => 'Slavery',
    'The Arts' => 'The Arts',
    );
}

function _miss_reflections_booth_get_default_tags($record){
  $results = array();
  $json = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/render.htm' );
  foreach ($record as $value) {
    for ($i=0; $i < count($json['stories']); $i++) {
      if($json['stories'][$i]['id'] == $value){
        array_push($results, $i);
      }
    }
  }
  return $results;
}

function _miss_reflections_booth_form_approve($form, &$form_state) {
  //set status to 0 to be approved and moved to EDAN
  $id = !empty($form_state['input']['id']) ? $form_state['input']['id'] : '';
  $uri = 'id=' . $id . '&status=0';
  $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/reflections_booth/setVideoStatus.htm', $uri );


  if(!empty($set['id']) && $set['status'] == 0) {
    drupal_set_message( t('The record has been successfully approved.') );
  } else {
    drupal_set_message( t('Could not approve the record. Please check, and try again.'), 'error' );
  }

  drupal_goto(current_path());
}

function _miss_reflections_booth_form_reject($form, &$form_state) {
  //set status to 1 to be put back in isilon
  $id = !empty($form_state['input']['id']) ? $form_state['input']['id'] : '';
  $uri = 'id=' . $id . '&status=1';
  $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/reflections_booth/setVideoStatus.htm', $uri );


  if(!empty($set['id']) && $set['status'] == 1) {
    drupal_set_message( t('The record has been successfully set to "Pending Review".') );
  } else {
    drupal_set_message( t('Could not update the record. Please check, and try again.'), 'error' );
  }

  drupal_goto(current_path());
}

function _miss_reflections_booth_form_delete($form, &$form_state) {
  //set status to -1 to be deleted
  $id = !empty($form_state['input']['id']) ? $form_state['input']['id'] : '';
  $uri = 'id=' . $id . '&status=-1';
  $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/reflections_booth/setVideoStatus.htm', $uri );

  if(empty($set['id'])) {
    drupal_set_message( t('The record has been successfully deleted.') );
  } else {
    drupal_set_message( t('Could not delete the record. Please check, and try again.'), 'error' );
  }

  drupal_goto('miss/manage/reflectionsBooth');
}

/**
 * Implements hook_form_validate().
 */
function miss_video_submissions_form_validate($form, &$form_state) {

  $featured = (isset($form_state['input']['featured']) && $form_state['input']['featured']) ? true : false;
  $featured_ids = !empty($form_state['input']['featuredIds']) ? json_decode($form_state['input']['featuredIds']) : array();

  if((count($featured_ids) >= 9) && $featured && !in_array($form_state['input']['id'], $featured_ids)) {
    form_set_error('featured', t('Unable to set status of the video to \'featured\'. The maximium number of videos (9) has been reached.'));
  }

}

// function _miss_reflections_booth_get_tags_id($choice){
//   $json = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/render.htm' );
//   return $json['stories'][$choice]['id'];
// }

// function _miss_reflections_booth_get_tags_name($id){
//   $json = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/render.htm' );
//   foreach ($json['stories'] as $value) {
//     if($value['id'] == $id) return $value['name'];
//   }
//   return;
// }

/**
 * Implements hook_form_submit().
 */
function miss_video_submissions_form_submit($form, &$form_state) {

  // FOR MANUAL SUBMISSIONS (meant to be used during the development phase).
  // $manual_submission = _miss_manual_video_submissions();
  // dumper($manual_submission);
  // die('dump function dies, but just to be safe...');

  $id = !empty($form_state['input']['id']) ? '&id=' . $form_state['input']['id'] : '';

  // Backwards, because EDAN's status flags are backwards.
  // Featured status: 0 = published, 1 = not published
  $featured = (isset($form_state['values']['featured']) && $form_state['values']['featured']) ? true : false;
  $deletionRecommendation = (isset($form_state['values']['deletionRecommendation']) && $form_state['values']['deletionRecommendation']) ? true : false;

  include('_file.inc');
  $token = _miss_get_token(FALSE);

  // Write the poster image to disk, and upload to EDAN.
  if(!empty($form_state['values']['videoPosterImageCatcher'])) {
    // Process the base64 encoded string to a file.
    $file_result_poster = _miss_process_base64_file( $form_state['values']['videoFileUrl'], $form_state['values']['videoPosterImageCatcher'], 'poster' );
    // Upload the file to EDAN.
    if(isset($file_result_poster['file_name'])) {
      $video_poster_image_url = _miss_upload_file_to_edan( $file_result_poster['file_name'], $token );
    } else {
      drupal_set_message( $file_result_poster['error_message'], 'error' );
    }
  }

  // Write the video thumbnail image to disk, and upload to EDAN.
  if(!empty($form_state['values']['videoThumbnailImageCatcher'])) {
    // Process the base64 encoded string to a file.
    $file_result_thumb = _miss_process_base64_file( $form_state['values']['videoFileUrl'], $form_state['values']['videoThumbnailImageCatcher'], 'thumb' );
    // Upload the file to EDAN.
    if(isset($file_result_thumb['file_name'])) {
      $video_thumbnail_image_url = _miss_upload_file_to_edan( $file_result_thumb['file_name'], $token );
    } else {
      drupal_set_message( $file_result_thumb['error_message'], 'error' );
    }
  }

  // get tags by id
  // $tags = array();
  // if(!empty($form_state['input']['tags'])){
  //   foreach($form_state['input']['tags'] as $value){
  //     array_push($tags, _miss_reflections_booth_get_tags_id($value));
  //   }
  // }

  $tags = isset($form_state['input']['tags']) ? $form_state['input']['tags'] : array();

  global $user;
  $params = array(
    'comments' => $form_state['values']['comments']['value'],
    'artifact' => $form_state['values']['artifact']['value'],
    'age' => $form_state['values']['age'],
    'gender' => $form_state['values']['gender'],
    'race' => $form_state['values']['race'],
    'location' => $form_state['input']['city'],
    'region' => $form_state['values']['region'],
    // reviewer is always set to the current user
    'reviewer' => $user->name,
    'galleryTagId' => $form_state['values']['galleryTagId'],
    'promptQuestionId' => $form_state['values']['promptQuestionId'],
    'videoFileUrl' => $form_state['values']['videoFileUrl'],
    'videoPosterImageUrl' => isset($video_poster_image_url['url'])
    ? $video_poster_image_url['url']
    : $form_state['values']['videoPosterImageUrl'],
    'videoThumbnailImageUrl' => isset($video_thumbnail_image_url['url'])
    ? $video_thumbnail_image_url['url']
    : $form_state['values']['videoThumbnailImageUrl'],
    'featured' => $featured,
    'deletionRecommendation' => $deletionRecommendation ? 1 : 0,
    'email' => $form_state['values']['emailAddress'],
    'userConsent' => $form_state['values']['userConsent'] ? true : false,
    'markerStartTime' => $form_state['values']['markerStartTime'],
    'markerEndTime' => $form_state['values']['markerEndTime'],
    );


  $uri = drupal_http_build_query($params);
  // Set the gallery tag, with the array of question ids, and gallery id if it exists.
  $uri_set = 'tags=' . urlencode(json_encode($tags)) . '&groupType=' . urlencode(json_encode($form_state['input']['groupType'])) . '&' . $uri . $id;

  $set = _miss_send_miss_request( variable_get('reflectionsSetAttractVideo'), $uri_set );
  $result = _miss_video_submissions_manage_attract_media_loop($set, $form_state, $featured);

  // Set an error message if the setting of the attract media loop fails.
  if(isset($result['error_message'])) {
    drupal_set_message( $result['error_message'], 'error' );
  }

  if(!empty($result)) {
    drupal_set_message( t('<a href="/miss/manage/reflectionsBooth/videos/' . $form_state['values']['galleryTagId'] . '/' . str_replace('&id=', '', $id) . '">Video submission</a> successfully modified.') );
  } else {
    drupal_set_message( t('Could not modify the video submission. Please try again. If this persists, please contact the site administrator.'), 'error' );
  }

  // dumper($form_state['values']['galleryTagId']);

  drupal_goto('miss/manage/reflectionsBooth/videos/' . $form_state['values']['galleryTagId']);
}

/**
 * Process base64 encoded file.
 */
function _miss_process_base64_file($file_name_url = false, $base64_string = false, $image_type = 'unknown') {

  $data = array();

  if($file_name_url && $base64_string) {

    // First, get the file name.
    $url_parsed = parse_url($file_name_url);
    $url_parts = explode('/', $url_parsed['path']);
    $file_name = array_pop($url_parts);
    $file_name_parts = explode('.', $file_name);
    $final_file_name = $file_name_parts[0] . '_' . mt_rand( 0, 0xffff ) . '_' . $image_type . '.png';

    // Now, decode the base64 encoded string.
    list($type, $base64_string) = explode(';', $base64_string);
    list(, $base64_string)      = explode(',', $base64_string);
    $image_data = base64_decode($base64_string);

    // Write the data to a temporary file.
    if (file_put_contents('temporary://' . $final_file_name, $image_data) === FALSE) {
      $data['error_message'] = 'The ' . $image_type . ' image could not be created.';
    } else {
      $data['file_name'] = $final_file_name;
    }

  }

  return $data;
}

/**
 * Upload a file to EDAN.
 */
function _miss_upload_file_to_edan( $file = false, $token = false ) {

  $data = false;

  if($file && $token) {
    // Initialise the curl request.
    $request = curl_init('http://review:600Maryland@edan.si.edu/miss/nmaahc/upload.htm');
    // Send the file.
    curl_setopt($request, CURLOPT_POST, true);
    curl_setopt($request, CURLOPT_POSTFIELDS,
      array(
        $file => class_exists('CurlFile') ? new CurlFile( realpath( '/tmp/' . $file ), 'image/png', $file ) : '@' . '/tmp/' . $file,
        'token' => $token,
        )
      );
    // Set the response.
    curl_setopt($request, CURLOPT_RETURNTRANSFER, true);
    $data = curl_exec($request);
    // Close the session
    curl_close($request);
  } else {
    $data['error_message'] = 'The image could not be uploaded to EDAN.';
  }

  return json_decode($data, true);
}

/**
 * Add videos to the attract media loop.
 */
function _miss_video_submissions_manage_attract_media_loop( $set = false, $form_state, $featured ) {

  $featured_ids = array();
  $featured_records = _miss_reflections_booth_get_featured_records( $set['content']['galleryTagId'] );
  $error_message = t('Could not modify the attract media loop. Please try again. If this persists, please contact the site administrator.');

  if(!empty($featured_records)) {
    foreach($featured_records as $value) {
      array_push($featured_ids, $value['id']);
    }
  }

  if($set['content']['featured']){
    // If the media item is not already in the array, push.
    if(!in_array($set['id'], $featured_ids)) {
      if(count($featured_ids) < 9){
        array_push($featured_ids, $set['id']);
      }else{
        $error_message = t('The maximum of 9 featured videos has been reached for this gallery. No more videos may be set to featured at this time.');
      }
    }
  }else{
    // take the id out if it was unset as featured
    if(!empty($featured_ids)) {
      foreach($featured_ids as $featured_id_key => $featured_id_value) {
        if($featured_id_value == $set['id']) {
          // Unset the default image that's being taken away.
          unset($featured_ids[$featured_id_key]);
        }
      }
      // reset and reorder array keys
      $featured_ids = array_values($featured_ids);
    }
  }

  $uri = 'galleryId=' . $set['content']['galleryTagId'] . '&attractMediaLoop=' . json_encode($featured_ids);
  $result = _miss_send_miss_request( variable_get('reflectionsSetAttractMediaLoop'), $uri );
  if(!empty($result)){
    if(($set['content']['featured'] && !in_array($set['id'], $result['content']['attractMediaLoop'])) || (!$set['content']['featured'] && in_array($set['id'], $result['content']['attractMediaLoop']))) {
      $result['error_message'] = $error_message;
    }
  }

  return $result;
}

/**
 * HTML markup for the miss_video_submissions_form().
 */
function _miss_video_submissions_form_markup($record) {

  $question = !empty($record['gallery_data']['gallery_questions'][$record['promptQuestionId']])
  ? $record['gallery_data']['gallery_questions'][$record['promptQuestionId']]
  : 'Question Not Set';
  $markerStartTime = (isset($record['markerStartTime']) && !empty($record['markerEndTime'])) ? $record['markerStartTime'] : '0.0';
  $markerEndTime = (isset($record['markerEndTime']) && !empty($record['markerEndTime'])) ? $record['markerEndTime'] : '0.0';
  $poster_image = isset($record['videoPosterImageUrl']) ? ' poster="' . $record['videoPosterImageUrl'] . '"': '';

  $markup = '<div style="margin: 1rem 0;">' . "\n";
  $markup .= '<div style="border: 1px solid #D8D8D8; background-color: #F8F8F8; margin: 1rem 0; padding: 1rem; width: 820px;">' . "\n";
  $markup .= '<h2 style="margin-top: 0; font-size: 1.1rem; font-weight: 500;">' . $question . '</h2>' . "\n";
  $markup .= '<p style="margin: 1rem 0 0 0; font-size: 0.85rem;">';
  $markup .= '<strong>Submitter\'s email address:</strong> <a href="mailto:' . $record['emailAddress'] . '">' . $record['emailAddress'] . '</a><br>' . "\n";
  $markup .= '<strong>Video URL:</strong> ' . $record['videoFileUrl'];
  $markup .= '</p>' . "\n";
  $markup .= '</div>' . "\n";
  $markup .= '<video width="853" height="480" id="video_element" crossOrigin="anonymous"' . $poster_image . ' controls>' . "\n";
  $markup .= '<source src="' . $record['videoFileUrl'] . '#t=' .
  $markerStartTime . ',' . $markerEndTime . '" type="video/mp4">Your browser does not support the video tag.' . "\n";
  $markup .= '</video>' . "\n";
  $markup .= '</div>' . "\n";

  return $markup;
}

/**
 * HTML markup for the miss_video_submissions_form().
 */
function _miss_video_submissions_form_poster_image_markup( $record ) {
  $display = (!isset($record['videoPosterImageUrl']) || empty($record['videoPosterImageUrl'])) ? ' display: none;' : '';
  $markup = '<canvas id="canvas" width="853" height="480" style="display:none;"></canvas>' . "\n";
  $markup .= '<canvas id="canvasThumb" width="853" height="480" style="display:none;"></canvas>' . "\n";
  $markup .= '<div id="screenShot" style="margin-top: 1rem;' . $display . '">' . "\n";
  $markup .= (isset($record['videoPosterImageUrl']) && !empty($record['videoPosterImageUrl']))
  ? '<img src="' . $record['videoPosterImageUrl'] . '">' . "\n" : '';
  $markup .= '</div>' . "\n";
  return $markup;
}

/**
 * Get Gallery Data
 */
function _miss_get_gallery_data($gallery_tags = false, $path_part = false) {

  $data = false;

  $prompt_questions = _miss_rb_video_submissions_callback(FALSE, FALSE, 'prompt_question');

  if($gallery_tags && $path_part) {
    // Get the gallery id and tag (title).
    foreach ($gallery_tags as $key => $value) {
      if($value['id'] === $path_part) {
        $data['gallery_id'] = $value['id'];
        $data['gallery_title'] = isset($value['content']['galleryTag']) ? $value['content']['galleryTag'] : $value['galleryTag'];
        // Index questions by id.
        foreach($prompt_questions['data']['components'] as $pkey => $pvalue) {
          $data['gallery_questions'][$pvalue['id']] = $pvalue['content']['text'];
        }
      }
    }
  }

  return $data;
}

function _miss_reflections_booth_get_prompt_question_id( $text = '', $gallery_tag_id = 0 ){

  $matching_question = '';

  $results = _miss_send_miss_request( variable_get('reflectionsRender') );

  if(isset($results['galleryTags'])) {
    foreach ($results['galleryTags'] as $value) {
      foreach ($value['promptQuestions'] as $val) {
        if (strpos($val['text'], $text) !== false) $matching_question = $val['id'];
      }
    }
  }

  return $matching_question;
}

/**
 * Implements hook_form().
 */
function miss_reflections_booth_user_release_form( $form, &$form_state, $record = array() ) {

  $form['formTextPartOne'] = array(
    '#type' => 'text_format',
    '#title' => t('User Release Form Part I'),
    '#rows' => 13,
    '#cols' => 10,
    '#required' => TRUE,
    '#resizable' => TRUE,
    '#format' => 'full_html',
    '#maxlength' => 524288,
    '#default_value' => !empty($record['userReleaseForm']) ? $record['userReleaseForm'] : '',
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
function miss_reflections_booth_user_release_form_submit($form, &$form_state) {

  $uri = 'formText=' . urlencode($form_state['input']['formTextPartOne']['value']);
  $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/reflections_booth/setUserReleaseForm.htm', $uri );

  if(!empty($set['id'])) {
    drupal_set_message( t( 'User release content has been successfully modified.') );
  }
  else {
    drupal_set_message( 'Could not modify the user release content. Please try again. If this persists, please contact the site administrator.', 'error' );
  }

  // drupal_goto('miss/manage/theHub');
  drupal_goto(current_path());
}

/**
 * Video Submissions callback returns JSON
 *
 * @return string
 */
function _miss_rb_video_submissions_callback($json = TRUE, $passed_gallery_tagId = '', $passed_component_type = 'attract_video', $passed_status = '', $dump = FALSE, $gallery_tags = array(), $prompt_questions = array()) {

  $default_start = 'start=0';
  $default_length = '&rows=10';

  if($passed_component_type === 'attract_video') {
    $default_start = 'start=0';
    $default_length = '&rows=10';
  }

  $results = $gallery_data = $data = $result = $featured = array();

  $get_start = filter_input(INPUT_GET, 'start', FILTER_SANITIZE_STRING);
  $start = !empty($get_start) ? 'start=' . $get_start : $default_start;

  // Custom start for gallery tags
  if($passed_component_type === 'gallery_tag') $start = '';

  $get_length = filter_input(INPUT_GET, 'length', FILTER_SANITIZE_STRING);
  $length = !empty($get_length) ? '&rows=' . $get_length : $default_length;

  // Custom length for prompt questions
  if($passed_component_type === 'prompt_question') $length = '&rows=50';

  // $get_status = $_GET['status'];
  $get_status = isset($_GET['status']) ? $_GET['status'] : '';
  $status = '&status=' . (!empty($passed_status) ? $passed_status : $get_status);

  // Custom status for gallery tags
  if($passed_component_type === 'gallery_tag') $status = 'status=1';

  $get_gallery_tag_id = filter_input(INPUT_GET, 'gallery_tag_id', FILTER_SANITIZE_STRING);
  $gallery_tag_id = !empty($get_gallery_tag_id) ? '&galleryTagId=' . $get_gallery_tag_id : '';
  $gallery_tag_id = !empty($passed_gallery_tagId) ? '&galleryTagId=' . $passed_gallery_tagId : $gallery_tag_id;

  $component_type = filter_input(INPUT_GET, 'component_type', FILTER_SANITIZE_STRING);
  $component_type = !empty($passed_component_type) ? $passed_component_type : $component_type;

  $get_start_date = filter_input(INPUT_GET, 'startDate', FILTER_SANITIZE_STRING);
  $get_end_date = filter_input(INPUT_GET, 'endDate', FILTER_SANITIZE_STRING);

  // Date range
  $date_range = '';
  if(!empty($get_start_date) && !empty($get_end_date)) {
    $date_range = '&dateRange=' . $get_start_date . ',' . $get_end_date . ' 23:59:59';
  }

  $sort = 'lastTimeUpdated';

  if(isset($_GET['order']) && !empty($_GET['order'])) {

    switch((int)$_GET['order'][0]['column']) {
      case 3:
      $sort = 'lastTimeUpdated';
      break;
      default:
      $sort = 'lastTimeUpdated';
    }

  }

  $sortDir = (isset($_GET['order']) && !empty($_GET['order'])) ? (string)$_GET['order'][0]['dir'] : 'asc';

  $get_search_field = filter_input(INPUT_GET, 'search_field', FILTER_SANITIZE_STRING);
  $get_search_value = filter_input(INPUT_GET, 'search_value', FILTER_SANITIZE_STRING);

  $fq = array();
  if(isset($get_search_field) && !empty($get_search_field) && !empty($get_search_value)) {
    switch ($get_search_field) {
      case 'promptQuestion':
      $matching_questions_id = _miss_reflections_booth_get_prompt_question_id( $get_search_value, $gallery_tag_id );
      array_push( $fq, 'promptQuestionId:' . $matching_questions_id );
      break;
      case 'status':
        if (strpos('Featured', $get_search_value) !== false) {
          array_push($fq, 'featured:true');
        }
        if (strpos('Deletion is Recommended', $get_search_value) !== false) {
          array_push($fq, 'deletionrecommendation:true');
        }
        if (strpos('Reviewed', $get_search_value) !== false) {
          array_push($fq, 'reviewer:*');
        }
        if (strpos('Pending Review', $get_search_value) !== false) {
          array_push($fq, 'reviewer:\"\"');
        }
      break;
      // must match exactly
      case 'reviewer':
      array_push($fq, 'reviewer:' . $get_search_value);
      break;
      case 'tags':
      $fq = array('tags:' . $get_search_value . '');
      break;
      case 'age':
      array_push($fq, 'age:' . $get_search_value);
      break;
      case 'gender':
      array_push($fq, 'gender:' . $get_search_value);
      break;
      case 'race':
      array_push($fq, 'race:' . $get_search_value);
      break;
      case 'groupType':
      $fq = array('groupType:' . $get_search_value);
      break;
      case 'location':
      array_push($fq, 'location:' . $get_search_value);
      break;
      case 'region':
      array_push($fq, 'region:' . $get_search_value);
      break;
    }
  }

  // Parameters:
  // start, rows, sort, sortDir...
  // Acceptable sort parameter arguments:
  // title, url, id, last_time_updated, timestamp, unit_code, status
  // Example:
  // start=0&rows=50&sort=timestamp&sortDir=desc
  // TODO: Need to add search, if possible... example:
  // start=0&rows=50&sort=timestamp&sortDir=desc&q=place_search_query_here

  // use getComponents if we don't need to use getVideoSubmissions
  switch($component_type) {
    case 'gallery_tag':
    $endpoint = 'miss/v1.0/nmaahc/reflections_booth/getComponents.htm';
    break;
    case 'prompt_question':
    $endpoint = 'miss/v1.0/nmaahc/reflections_booth/getComponents.htm';
    break;
    case 'attract_video':
    $endpoint = 'miss/v1.0/nmaahc/reflections_booth/getVideoSubmissions.htm';
    break;
    default:
    $endpoint = 'miss/v1.0/nmaahc/reflections_booth/getVideoSubmissions.htm';
    break;
  }

  // $uri = $start . $length . $status . $gallery_tag_id . '&componentType=' . $component_type;
  $uri = $start . $length . $status . $gallery_tag_id . '&componentType=' . $component_type . '&sort=' . $sort . '&sortDir=' . $sortDir . $date_range . '&renderAsArray=true';

  // only add fq onto the query uri if it is not empty
  if (!empty($fq)){
    $uri .= '&fq=';
    foreach ($fq as $value) {
      $uri .= $value;
    }
  }

  // Get the components.
  $items = _miss_send_miss_request( $endpoint, $uri );

  // if($endpoint === 'miss/v1.0/nmaahc/reflections_booth/getVideoSubmissions.htm') {
  //   dumper($endpoint,0);
  //   dumper($uri,0);
  //   dumper(json_encode($items));
  // }

  switch($component_type) {

    case 'gallery_tag':

    $data['data'] = $items;
    return $data;
    break;

    case 'prompt_question':

    $endpoint = 'miss/v1.0/nmaahc/reflections_booth/getComponents.htm';
    $uri = 'start=0&rows=100&status=0&componentType=prompt_question';
    $prompt_questions = _miss_send_miss_request( $endpoint, $uri );

    $data['data'] = $prompt_questions;
    return $data;

    break;

    case 'attract_video':

    if(empty($prompt_questions)) {
      $endpoint = 'miss/v1.0/nmaahc/reflections_booth/getComponents.htm';
      $uri = 'start=0&rows=100&status=0&componentType=prompt_question';
      $prompt_questions = _miss_send_miss_request( $endpoint, $uri );
    }

    $i = 0;
    // All videos.
    foreach($items['rows'] as $record_value) {

      $result[$i] = $record_value;

          // Set the status of the item.
      $result[$i]['content_processed']['status'] = '';

      if ($record_value['status'] == 0) {
        $result[$i]['content_processed']['status'] = 'Approved';
        if (isset($record_value['content']['featured']) && $record_value['content']['featured']) {
          $result[$i]['content_processed']['status'] = '<span style="color: #069E2D;font-weight:bold;">Featured</span>';
        }
      }else{
        if (isset($record_value['content']['reviewer'])) {
          $result[$i]['content_processed']['status'] = 'Reviewed';
        }else{
          $result[$i]['content_processed']['status'] = 'Pending Review';
        }
      }

      $result[$i]['content_processed']['status'] .= (isset($record_value['content']['deletionRecommendation']) && $record_value['content']['deletionRecommendation']) ? ' <span style="color: red;">(Deletion is Recommended)</span>' : '';

      // Get timestamp for the table.
      $time = (isset($record_value['timestamp']) && $record_value['timestamp']) ? $record_value['timestamp'] : 'N/A';
      $datetime = DateTime::createFromFormat( 'Y-m-d H:i:s.u', $time, new DateTimeZone('America/New_York'));
      $result[$i]['content_processed']['timestamp'] = $datetime->format('m/d/Y g:i A');

      // Get lastTimeUpdated for the table.
      $time = (isset($record_value['lastTimeUpdated']) && $record_value['lastTimeUpdated']) ? $record_value['lastTimeUpdated'] : 'N/A';
      $datetime = DateTime::createFromFormat( 'Y-m-d H:i:s.u', $time, new DateTimeZone('America/New_York'));
      $result[$i]['content_processed']['lastTimeUpdated'] = $datetime->format('m/d/Y g:i A');

      // $result[$i]['content_processed']['timestamp'] = $time;

      $result[$i]['content_processed']['thumb'] = (isset($record_value['content']['videoThumbnailImageUrl']) && !empty($record_value['content']['videoThumbnailImageUrl']))
      ? '<img src="' . $record_value['content']['videoThumbnailImageUrl'] . '" alt="Thumbnail image file">'
      : '';
      $result[$i]['content_processed']['question'] = (!empty($prompt_questions) && isset($prompt_questions['components'][$record_value['content']['promptQuestionId']]))
      ? $prompt_questions['components'][$record_value['content']['promptQuestionId']]['content']['text']
      : 'Question Not Set';

      $result[$i]['content_processed']['reviewer'] = (isset($record_value['content']['reviewer']) && !empty($record_value['content']['reviewer']))
      ? $record_value['content']['reviewer'] : 'Not Reviewed';

        // $tag_names_array = array();
        // if(isset($record_value['content']['tags']) && !empty($record_value['content']['tags'])){
        //   foreach ($record_value['content']['tags'] as $value) {
        //     array_push($tag_names_array, strip_tags(_miss_reflections_booth_get_tags_name($value)));
        //   }
        //   $result[$i]['content_processed']['tags'] = implode(', ', $tag_names_array);
        // }else{
        //   $result[$i]['content_processed']['tags'] = 'None';
        // }

      if(isset($record_value['content']['tags']) && !empty($record_value['content']['tags'])){
        $result[$i]['content_processed']['tags'] = implode(', ', $record_value['content']['tags']);
      }else{
        $result[$i]['content_processed']['tags'] = 'None';
      }

      $result[$i]['content_processed']['age'] = (isset($record_value['content']['age']) && !empty($record_value['content']['age']))
      ? $record_value['content']['age'] : 'Not Set';

      $result[$i]['content_processed']['gender'] = (isset($record_value['content']['gender']) && !empty($record_value['content']['gender']))
      ? $record_value['content']['gender'] : 'Not Set';

      $result[$i]['content_processed']['race'] = (isset($record_value['content']['race']) && !empty($record_value['content']['race']))
      ? $record_value['content']['race'] : 'Not Set';

      $result[$i]['content_processed']['groupType'] = (isset($record_value['content']['groupType']) && !empty($record_value['content']['groupType']))
      ? $record_value['content']['groupType'] : 'Not Set';

      $result[$i]['content_processed']['location'] = (isset($record_value['content']['location']) && !empty($record_value['content']['location'])) ? $record_value['content']['location'] : '';
      $result[$i]['content_processed']['location'] .= (isset($record_value['content']['location']) && !empty($record_value['content']['location']) && isset($record_value['content']['region']) && !empty($record_value['content']['region'])) ? ', ' : '';
      $result[$i]['content_processed']['location'] .= (isset($record_value['content']['region']) && !empty($record_value['content']['region'])) ? $record_value['content']['region'] : '';
      if(empty($result[$i]['content_processed']['location'])){
        $result[$i]['content_processed']['location'] = 'Not Set';
      }

      $result[$i]['content_processed']['deletionRecommendation'] = (isset($record_value['content']['deletionRecommendation']) && !empty($record_value['content']['deletionRecommendation']))
      ? $record_value['content']['deletionRecommendation'] : false;


      $result[$i]['content_processed']['recordId'] = $record_value['id'];

      $gallery_tag_id_parts = explode('=', $gallery_tag_id);
      $result[$i]['content_processed']['galleryTagId'] = $gallery_tag_id_parts[1];
          // Featured counter.
      if(isset($record_value['content']['featured']) && $record_value['content']['featured']) $featured[] = '';

      $i++;

    }

    $data['data'] = $result;
    // total number of videos returned to by the request
    $data['recordsTotal'] = count($items['rows']);
    // total number of videos for the entire gallery (of the appropriate status)
    $data['recordsFiltered'] = $items['rowCount'];


    if(!$json || $dump) {
      return $data;
    } else {
      echo json_encode($data);
    }

    break;
  }

}

function _miss_reflections_booth_get_featured_records( $gallery_id = null) {
  // grab the number of featured videos from render
  $render = _miss_send_miss_request( variable_get('reflectionsRender') );
  $featured_items = array();
  foreach ($render['galleryTags'] as $value) {
    if($value['id'] == $gallery_id){
      $featured_items = $value['attractMediaLoop'];
    }
  }
  return $featured_items;
}

/**
 * Builds the Browse and Manage pages.
 */
function _miss_reflections_booth_page_build($interactive = '') {

  if(!user_access('view reflections booth content')){
    drupal_set_message('You do not have access to this page. Please contact an Administrator to resolve this issue.', 'error');
    drupal_goto('miss/list');
  }

  // Variables
  $data = array();
  $path_parts = arg();
  $types = _miss_types();
  $title = $types[$interactive];

  /////////////////////////////////////////////
  // VIDEO SUBMISSION PAGE //////////////////////
  /////////////////////////////////////////////
  if(!empty($path_parts[5])) {

    // Get the record.
    // miss/v1.0/nmaahc/miss_content/getContent.htm
    $endpoint = 'miss/v1.0/nmaahc/miss_content/getContent.htm';
    $uri = 'id=' . $path_parts[5];

    $item = _miss_send_miss_request( $endpoint, $uri );

    if(!isset($item['content'])) {
      return MENU_NOT_FOUND;
    }

    $data['page']['record'] = $item['content'];
    $data['page']['record']['id'] = $item['id'];
    $data['page']['record']['status'] = $item['status'];
    $data['page']['record']['last_time_updated'] = $item['lastTimeUpdated'];

    // EDAN record dump
    if(!empty($data['page']['record']) && isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
      $dump = '<pre>' . var_export($data['page']['record'], TRUE) . '</pre>';
      drupal_set_message( $dump, 'status' );
    }

    // Get all of the gallery tags.
    $gallery_tags = _miss_rb_video_submissions_callback(FALSE, FALSE, 'gallery_tag');
    // Get the gallery id, tag (title), and questions.
    $data['page']['record']['gallery_data'] = _miss_get_gallery_data($gallery_tags['data']['components'], $path_parts[4]);

    // Set the title.
    drupal_set_title((isset($data['page']['record']['gallery_data']['gallery_title']) ? '"' . $data['page']['record']['gallery_data']['gallery_title'] . '"' : '') . ' Video Submission');

    // Build-out the breadcrumbs.
    $current_breadcrumbs = drupal_get_breadcrumb();
    $crumbs = array();
    foreach($current_breadcrumbs as $key => $value) {
      $crumbs[] = $value;
    }
    $crumbs[] = l(t($data['page']['record']['gallery_data']['gallery_title']), 'miss/manage/reflectionsBooth/edit/' . $data['page']['record']['gallery_data']['gallery_id']);
    $crumbs[] = l(t('Browse Video Submissions'), 'miss/manage/reflectionsBooth/videos/' . $data['page']['record']['gallery_data']['gallery_id']);
    $crumbs[] = t(drupal_get_title());
    drupal_set_breadcrumb($crumbs);

    // CSS
    drupal_add_css(
      drupal_get_path('module', 'miss') . '/css/_reflections_booth.css',
      array('group' => CSS_THEME, 'type' => 'file', 'weight' => 1)
      );

    // Javascript
    drupal_add_js(
      drupal_get_path('module', 'miss') . '/js/_reflections_booth.js',
      array('scope' => 'footer', 'weight' => 52)
      );

     // Javascript
    drupal_add_js(
      '//cdnjs.cloudflare.com/ajax/libs/chosen/1.4.2/chosen.jquery.min.js',
      array('scope' => 'footer', 'type' => 'external', 'weight' => 50)
      );
      // CSS
    drupal_add_css(
      '//cdnjs.cloudflare.com/ajax/libs/chosen/1.4.2/chosen.min.css',
      array('group' => CSS_THEME, 'type' => 'external', 'weight' => 50)
      );

    $chosen_js = "
    jQuery(document).ready(function($) {
      $('#edit-tags').chosen();
      $('#edit-grouptype').chosen();
    });
    ";
    drupal_add_js($chosen_js, 'inline');

    // Get the form.
    $form = drupal_get_form('miss_video_submissions_form', $data['page']['record']);
    $data['page']['form'] = drupal_render($form);

    return $data['page']['form'];

  }

  /////////////////////////////////////
  // BROWSE PAGE //////////////////////
  /////////////////////////////////////
  if(empty($path_parts[3])) {

    drupal_set_title($title . ': Browse');

    // Get all of the gallery tags.
    $gallery_tags = _miss_send_miss_request('miss/v1.0/nmaahc/reflections_booth/getGalleries.htm');

    // Add a new set of prompt questions link.
    $data['page'] = '<p>' . l(t('Add a New Set of Prompt Questions'), 'miss/manage/reflectionsBooth/add') . '<br>' . "\n";
    // Manage user release form content link.
    $data['page'] .= l(t('Edit the User Release Form'), 'miss/manage/reflectionsBooth/userReleaseForm') . '<br>' . "\n";
    // $data['page'] .= l(t('View Reflections Booth Statistics'), 'miss/manage/reflectionsBooth/stats') . '</p>' . "\n";

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
      $dump = '<pre>' . var_export($data['record'], TRUE) . '</pre>';
      drupal_set_message( $dump, 'status' );
    }

    // DIV
    $data['page'] .=' <div class="datatables-body">' . "\n";
    // Table top
    $data['page'] .= '<table id="browse_reflections_booth" class="display" cellspacing="0" width="100%">' . "\n";
    $data['page'] .= '<thead>' . "\n";
    $data['page'] .= '<tr>' . "\n";
    $data['page'] .= '<th width="75%">Title</th>' . "\n";
    $data['page'] .= '<th class="no-sort" style="text-align: center;">Actions</th>' . "\n";
    $data['page'] .= '</tr>' . "\n";
    $data['page'] .= '</thead>' . "\n";
    $data['page'] .= '<tbody>' . "\n";

    // Loop through all of the records to build the table rows.
    foreach($gallery_tags['rows'] as $key => $value) {

      $approved_record_count = isset($value['content']['counts'][0]['count']) ? $value['content']['counts'][0]['count'] : 0;
      $unapproved_record_count = isset($value['content']['counts'][1]['count']) ? $value['content']['counts'][1]['count'] : 0;

      $data['page'] .= '<tr>' . "\n";
      $data['page'] .= '<td><strong>' . $value['content']['galleryTag'] . '</strong></td>' . "\n";
      $data['page'] .= '<td style="text-align: center;">' . "\n";
      $data['page'] .= '<a href="/' . current_path() . '/edit/' . $value['id'] . '" style="margin-right: 4px;">' . ((!user_access('edit reflections booth content')) ? 'View' : 'Edit') . '</a> | ' . "\n";
      $data['page'] .= '<a href="/' . current_path() . '/videos/' . $value['id'] . '" style="margin-left: 4px;">Video Submissions</a><br>(Approved: ' . $approved_record_count . ', Pending Review: ' . $unapproved_record_count . ')' . "\n";
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

      // Add a new record.
      case 'add':

        // CSS
      drupal_add_css(
        drupal_get_path('module', 'miss') . '/css/_reflections_booth.css',
        array('group' => CSS_THEME, 'type' => 'file', 'weight' => 1)
        );

        // Get the form.
      $data['page']['form'] = drupal_get_form('miss_prompt_questions_form');
        // Intregrate table drag.
      $table_elements = miss_reflections_booth_table_drag_elements($data);
      $output = '<form id="' . $data['page']['form']['#id'] . '" accept-charset="UTF-8" method="' . $data['page']['form']['#method'] . '" action="' . $data['page']['form']['#action'] . '">';
      $output .= drupal_render($data['page']['form']['tag']);
      $output .= drupal_render($data['page']['form']['gallery_image']);
      $output .= drupal_render($data['page']['form']['gallery_image_credit']);
      $output .= theme('table', array('header' => $table_elements['header'], 'rows' => $table_elements['rows'], 'attributes' => array('id' => 'reflections_booth_questions')));
      unset($data['page']['form']['promptQuestions']);
      $output .= drupal_render_children($data['page']['form']);

      drupal_set_title($title . ': Add a New Set of Prompt Questions');
      return $output;
      break;

      // Edit an existing record.
      case 'edit':

        // CSS
      drupal_add_css(
        drupal_get_path('module', 'miss') . '/css/_reflections_booth.css',
        array('group' => CSS_THEME, 'type' => 'file', 'weight' => 1)
        );

      $data['page']['record'] = array();
        // Get the record from EDAN.
      if(!empty($path_parts[4])) {



          // Get all of the components.
        $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/reflections_booth/getComponents.htm' );

        if(array_key_exists($path_parts[4], $result['components'])) {
          $data['page']['record'] = $result['components'][$path_parts[4]];
        }

        if(!empty($data['page']['record'])) {

            // EDAN record dump
          if(!empty($data['page']['record']) && isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
            $dump = '<pre>' . var_export($data['page']['record'], TRUE) . '</pre>';
            drupal_set_message( $dump, 'status' );
          }

            // Gallery tag.
          $data['page']['record']['galleryTag'] = $data['page']['record']['content']['galleryTag'];

            // Prompt questions.
          foreach($data['page']['record']['content']['promptQuestions'] as $question_id) {
            $data['page']['record']['promptQuestions'][] = array(
              'text' => $result['components'][$question_id]['content']['text'],
              'id' => $result['components'][$question_id]['id'],
              );
          }

            // Set the page title.
          drupal_set_title('Manage Prompt Questions for "' . $data['page']['record']['content']['galleryTag'] . '"');
            // Build-out the breadcrumbs.
          $current_breadcrumbs = drupal_get_breadcrumb();
          $crumbs = array();
          foreach($current_breadcrumbs as $key => $value) {
            $crumbs[] = $value;
          }
          $crumbs[] = t($data['page']['record']['content']['galleryTag']);
          drupal_set_breadcrumb($crumbs);

            // If a record doesn't exist, set a message.
          if(empty($data['page']['record'])) {
            drupal_set_message( t('The gallery record could not be found. (ID = ' . $path_parts[4]) . ')', 'error' );
            drupal_goto('miss/manage/reflectionsBooth');
          }

            // Get the form.
          $data['page']['form'] = drupal_get_form('miss_prompt_questions_form', $data['page']['record']);
            // Intregrate table drag.
          $table_elements = miss_reflections_booth_table_drag_elements($data);
          $output = '<form id="' . $data['page']['form']['#id'] . '" accept-charset="UTF-8" method="' . $data['page']['form']['#method'] . '" action="' . $data['page']['form']['#action'] . '">';
          $output .= drupal_render($data['page']['form']['tag']);
          $output .= drupal_render($data['page']['form']['gallery_image']);
          $output .= drupal_render($data['page']['form']['gallery_image_credit']);
          $output .= theme('table', array('header' => $table_elements['header'], 'rows' => $table_elements['rows'], 'attributes' => array('id' => 'reflections_booth_questions')));
          unset($data['page']['form']['promptQuestions']);
          $output .= drupal_render_children($data['page']['form']);

        } else {
            // If a record doesn't exist, set a message.
          drupal_set_message( t('The gallery record could not be found. (ID = ' . $path_parts[4]) . ')', 'error' );
          drupal_goto('miss/manage/reflectionsBooth');
        }

      }

      return $output;
      break;

      // BROWSE video submissions
      case 'videos':

      $data = array();
      $out = '';
      $data['page']['record'] = array();

        // Get the record from EDAN.
      if(!empty($path_parts[4])) {

          // Get all of the records.
        $result = _miss_send_miss_request( variable_get('reflectionsRender') );
          // Get the gallery id, tag (title), and questions.
        $gallery_data = _miss_get_gallery_data($result['galleryTags'], $path_parts[4]);

        // Display a message if there are 9 featured videos.
        $featured_items = _miss_reflections_booth_get_featured_records( $path_parts[4] );
        if(count($featured_items) >= 9) {
          drupal_set_message( t('The maximum number of 9 featured videos has been reached. No more videos may be set to featured at this time.' ), 'warning' );
        }

          // EDAN Record Dump.
        if(isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
            // Get all of the components.
          $record_dump = _miss_rb_video_submissions_callback(FALSE, $path_parts[4], 'attract_video', FALSE, TRUE);
          $dump = '<h2>EDAN Data and Processed Data</h2>';
          $dump .= '<pre>' . var_export($record_dump, TRUE) . '</pre>';
          drupal_set_message( $dump, 'message' );
        }

          // Process Page...
        if(!isset($_GET['dump'])) {

            // Set the title.
          drupal_set_title((isset($gallery_data['gallery_title']) ? '"' . $gallery_data['gallery_title'] . '" ' : '') . 'Video Submissions');

            // Build-out the breadcrumbs.
          $current_breadcrumbs = drupal_get_breadcrumb();
          $crumbs = array();
          foreach($current_breadcrumbs as $key => $value) {
            $crumbs[] = $value;
          }
          $crumbs[] = l(t($gallery_data['gallery_title']), 'miss/manage/reflectionsBooth/edit/' . $gallery_data['gallery_id']);
          $crumbs[] = t('Browse Video Submissions');
          drupal_set_breadcrumb($crumbs);

            // Add DataTables CSS and Javascript.
          drupal_add_css(
            '//cdnjs.cloudflare.com/ajax/libs/datatables/1.10.11/css/jquery.dataTables.min.css',
            array('group' => CSS_THEME, 'type' => 'external', 'weight' => 50)
            );
          drupal_add_css(
            drupal_get_path('module', 'miss') . '/css/_visual_arts_tour_browse.css',
            array('group' => CSS_THEME, 'type' => 'file', 'weight' => 51)
            );
          // CSS
          drupal_add_css(
            drupal_get_path('module', 'miss') . '/css/_reflections_booth_videos.css',
            array('group' => CSS_THEME, 'type' => 'file', 'weight' => 52)
            );
          drupal_add_js(
            '//cdnjs.cloudflare.com/ajax/libs/datatables/1.10.11/js/jquery.dataTables.min.js',
            array('scope' => 'footer', 'type' => 'external', 'weight' => 50)
            );
          drupal_add_js(
            drupal_get_path('module', 'miss') . '/js/datatables_directives.js',
            array('scope' => 'footer', 'weight' => 51)
            );

            // DIV
          drupal_add_library('system', 'ui.tabs');
          drupal_add_library('system', 'ui.datepicker');

          $tabs_script = '
          jQuery(document).ready(function() {
              // jQuery("#tabsElement").responsiveTabs({
              //   startCollapsed: "accordion"
              // });
            jQuery("#tabsElement").tabs();
          });
          ';
          drupal_add_js($tabs_script, 'inline');

          $date_range_form = drupal_get_form('miss_reflections_booth_daterange_form');
          $out .= drupal_render($date_range_form);

          $search_form = drupal_get_form('miss_reflections_booth_search_form');
          $out .= drupal_render($search_form);

            // Setting up responsive tabs.
          $out .= '<div id="tabsElement">' . "\n";

          $out .= '<ul>' . "\n";
          $out .= '<li><a href="#tab-1"> Approved </a></li>' . "\n";
          $out .= '<li><a href="#tab-2"> Pending Review </a></li>' . "\n";
          $out .= '</ul>' . "\n";

            // The table.
          $out .= ' <div class="datatables-body" id="tab-1">' . "\n";
          $out .= '<table id="browse_reflections_booth_videos" class="display" cellspacing="0" width="100%">' . "\n";
          $out .= '<thead>' . "\n";
          $out .= '<tr>' . "\n";
          $out .= '<th width="12%" class="no-sort" style="text-align: center;">Thumbnail Image</th>' . "\n";
          $out .= '<th width="50%">Prompt Question</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Status</th>' . "\n";
          $out .= '<th width="20%" style="text-align: center;">Created</th>' . "\n";
          $out .= '<th width="20%" style="text-align: center;">Last Updated</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Reviewer</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Tags</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Age</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Gender</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Race / Ethnicity</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Type of Group</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Location</th>' . "\n";
          $out .= '<th width="8%" class="no-sort">Actions</th>' . "\n";
          $out .= '</tr>' . "\n";
          $out .= '</thead>' . "\n";
          $out .= '</table>' . "\n";
          $out .= '</div>' . "\n";
            // second table here
          $out .= ' <div class="datatables-body" id="tab-2">' . "\n";
            // Table top
          $out .= '<table id="browse_reflections_booth_videos_unapproved" class="display" cellspacing="0" width="100%">' . "\n";
          $out .= '<thead>' . "\n";
          $out .= '<tr>' . "\n";
          $out .= '<th width="12%" class="no-sort" style="text-align: center;">Thumbnail Image</th>' . "\n";
          $out .= '<th width="50%">Prompt Question</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Status</th>' . "\n";
          $out .= '<th width="20%" style="text-align: center;">Created</th>' . "\n";
          $out .= '<th width="20%" style="text-align: center;">Last Updated</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Reviewer</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Tags</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Age</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Gender</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Race / Ethnicity</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Type of Group</th>' . "\n";
          $out .= '<th width="10%" style="text-align: center;">Location</th>' . "\n";
          $out .= '<th width="8%" class="no-sort">Actions</th>' . "\n";
          $out .= '</tr>' . "\n";
          $out .= '</thead>' . "\n";
          $out .= '</table>' . "\n";
          $out .= '</div>' . "\n";
          $out .= '</div>' . "\n";

        }

      }

      return $out;
      break;

      // Edit user release form.
      case 'userReleaseForm':

        // Get all of the records.
      $record = _miss_send_miss_request( variable_get('reflectionsRender') );

        // Set the title.
      drupal_set_title('Edit User Release Form');
        // Build-out the breadcrumbs.
      $current_breadcrumbs = drupal_get_breadcrumb();
      $crumbs = array();
      foreach($current_breadcrumbs as $key => $value) {
        $crumbs[] = $value;
      }
      $crumbs[] = t(drupal_get_title());
      drupal_set_breadcrumb($crumbs);

        // Get the form.
      $form = drupal_get_form('miss_reflections_booth_user_release_form', $record);
      return drupal_render($form);

      break;


      case 'stats':

      drupal_add_js(drupal_get_path('module', 'miss') . '/plugins/highcharts/highcharts.js');
      if(isset($_GET["theme"])){
        drupal_add_js(drupal_get_path('module', 'miss') . '/plugins/highcharts/themes/monokai.js');
      }
      drupal_add_js(drupal_get_path('module', 'miss') . '/chart.js');
      $content = '<div id="total" style="width:100%; height:400px;"></div>
      <div id="booths" style="width:70%; height:400px; float:left;"></div>
    </div><div id="pieChart" style="width:30%; height:400px; float:left;"></div>';


        // Set the title.
    drupal_set_title('Reflections Booth: File Statistics');
        // Build-out the breadcrumbs.
    $current_breadcrumbs = drupal_get_breadcrumb();
    $crumbs = array();
    foreach($current_breadcrumbs as $key => $value) {
      $crumbs[] = $value;
    }
    $crumbs[] = t(drupal_get_title());
    drupal_set_breadcrumb($crumbs);

    return $content;

    break;

      // Send a 404.
    default:
    return MENU_NOT_FOUND;

  }

}

}

if (!empty($interactive)) {
  $output = _miss_reflections_booth_page_build($interactive);
}
