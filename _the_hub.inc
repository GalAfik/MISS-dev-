<?php
/**
 * Implements hook_form().
 */
function miss_hub_form( $form, &$form_state, $record = array() ) {

  $featured = 0;
  $default_images = !empty($record['defaultImages']) ? $record['defaultImages'] : array();
  $this_image_url = !empty($record['imageUrl']) ? $record['imageUrl'] : '';

  foreach($default_images as $image_key => $image_value) {
    if($image_value['imageUrl'] === $this_image_url) {
      $featured = 1;
    }
  }

  $form = array();

  global $base_url;

  //hide upload/insert buttons for non editors
  $button_script = '
  jQuery(document).ready(function() {
   jQuery("a:contains(' . "'Upload / Insert'" . ')").hide();
 });
 ';
 if(!user_access('upload files')){
  drupal_add_js($button_script, 'inline');
}

$form['featuredStory'] = array(
  '#type' => 'fieldset',
  '#title' => t('Published'),
  '#prefix' => l('Style Guide', 'sites/default/files/StyleGuide.pdf', array('attributes' => array('target' => array('_blank')))),
  );


$result = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/getComponents.htm' );
if(array_key_exists($record['id'], $result['components'])) {
  $record_status = $result['components'][$record['id']]['status'];
}

$form['featuredStory']['enabled'] = array(
  '#type' => 'checkbox',
  '#title' => t('Enabled'),
  '#default_value' => !empty($record) ? ($record_status == 1 ? '0' : '1') : '0',
  '#return_value' => '1',
  '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
  );

$form['featuredStory']['featured'] = array(
  '#type' => 'checkbox',
  '#title' => t('Featured Story'),
    // '#description' => t('Check to set this story as a featured story.'),
  '#default_value' => $featured,
  '#prefix' => '<div style="float:left">',
  '#suffix' => '</div>',
  '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
  );

$form['placeStoryTitle'] = array(
  '#type' => 'textfield',
  '#title' => t('Story Title'),
    // '#description' => t(''),
  '#size' => 65,
  '#maxlength' => 524288,
  '#required' => TRUE,
  '#default_value' => !empty($record['placeStoryTitle']) ? $record['placeStoryTitle'] : '',
    // '#prefix' => '',
  '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
  );

$form['text'] = array(
  '#type' => 'text_format',
  '#title' => t('Story Text'),
    // '#description' => t(''),
  '#rows' => 13,
  '#cols' => 10,
  '#required' => TRUE,
  '#resizable' => TRUE,
  '#format' => 'full_html',
  '#default_value' => !empty($record['text']) ? $record['text'] : '',
  '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
  );

$form['storySource'] = array(
  '#type' => 'text_format',
  '#title' => t('Story Source'),
  '#rows' => 6,
  '#cols' => 10,
  '#required' => FALSE,
  '#resizable' => TRUE,
  '#format' => 'full_html',
  '#default_value' => !empty($record['storySource']) ? $record['storySource'] : '',
  '#prefix' => '<div style="margin-top: 1rem;"></div>' . "\n",
  '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
  );

$form['imageUrl'] = array(
  '#type' => 'textfield',
  '#title' => t('Image URL'),
    // '#description' => t(''),
  '#size' => 65,
  '#maxlength' => 524288,
  '#required' => TRUE,
  '#default_value' => !empty($record['imageUrl']) ? $record['imageUrl'] : '',
  '#prefix' => '<hr style="box-sizing: content-box; margin-top: 20px; margin-bottom: 20px; border: 0; border-top: 1px solid #FFFFFF;">',
  '#suffix' => !empty($record['imageUrl']) ? '<div style="margin-bottom: 1.5rem;"><a href="' . $record['imageUrl'] . '" target="_blank">View Image</a> (opens in a new window/tab)</div>' : '',
  '#attributes' => array(
    'class' => array(
      'miss-file-manage',
      ),
    ),
  '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
  );

$form['imageCredit'] = array(
  '#type' => 'text_format',
  '#title' => t('Image Credit'),
  '#rows' => 6,
  '#cols' => 10,
  '#required' => TRUE,
  '#resizable' => TRUE,
  '#format' => 'full_html',
  '#default_value' => !empty($record['imageCredit']) ? $record['imageCredit'] : '',
  '#prefix' => '<div style="margin-top: 1rem;"></div>' . "\n",
  '#suffix' => '<hr style="box-sizing: content-box; margin-top: 20px; margin-bottom: 20px; border: 0; border-top: 1px solid #DEDEDE;">',
  '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
  );

$form['tags'] = array(
  '#type' => 'select',
  '#multiple' => TRUE,
  '#title' => t('Tag(s)'),
    // '#description' => t(''),
    // '#required' => TRUE,
  '#options' => _miss_hub_tags_form_field_options(),
  '#default_value' => !empty($record['tags']) ? $record['tags'] : '',
  '#attributes' => array('data-placeholder' => 'Choose tags...', 'style' => 'width: 545px;'),
  '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
  );

$form['region'] = array(
  '#type' => 'select',
  '#title' => t('Region'),
    // '#description' => t(''),
  '#required' => TRUE,
  '#options' => _miss_hub_region_form_field_options(),
  '#default_value' => !empty($record['region']) ? $record['region'] : '',
  '#suffix' => '<hr style="box-sizing: content-box; margin-top: 20px; margin-bottom: 20px; border: 0; border-top: 1px solid #DEDEDE;">',
  '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
  );

$form['time'] = array(
  '#type' => 'textfield',
  '#title' => t('Time'),
    // '#description' => t(''),
  '#size' => 65,
  '#maxlength' => 524288,
  '#required' => TRUE,
  '#default_value' => !empty($record['time']) ? $record['time'] : '',
  '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
  );

$form['place'] = array(
  '#type' => 'textfield',
  '#title' => t('Place'),
    // '#description' => t(''),
  '#size' => 65,
  '#maxlength' => 524288,
  '#required' => TRUE,
  '#default_value' => !empty($record['place']) ? $record['place'] : '',
  '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
  );

$form['respondentName'] = array(
  '#type' => 'textfield',
  '#title' => t('Respondent Name'),
  '#size' => 65,
  '#maxlength' => 524288,
  '#required' => FALSE,
  '#default_value' => !empty($record['respondentName']) ? $record['respondentName'] : '',
  '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
  );

$form['email'] = array(
  '#type' => 'textfield',
  '#title' => t('Respondent Email'),
  '#size' => 65,
  '#maxlength' => 524288,
  '#required' => FALSE,
  '#default_value' => !empty($record['email']) ? $record['email'] : '',
  '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
  );

  // This textarea contains all of the current defaultImages, in JSON format.
  // One may ask why not make a call to EDAN upon form submission?
  // This was tried, multiple times, within drupal's hook_ functions, but it failed for some unknown reason.
$form['defaultImages'] = array(
  '#type' => 'textarea',
  '#title' => t('Default Images'),
  '#attributes' => array('maxlength' => 10000),
  '#value' => !empty($record['defaultImages']) ? json_encode($record['defaultImages']) : '',
  '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
  );

$form['id'] = array(
  '#type' => 'hidden',
  '#default_value' => (isset($record['id'])) ? $record['id'] : '',
  );

$form['submit'] = array(
  '#type' => 'submit',
  '#default_value' => 'Submit',
  '#prefix' => '<div class="clearfix"></div>' . "\n",
  '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
  );

return $form;
}

/**
 * Implements hook_form_validate().
 */
function miss_hub_form_validate($form, &$form_state) {

  // if(!$form_state['input']['tag']) {
  //   form_set_error( 'name', t('Please enter a gallery tag') );
  // }

}

/**
 * Implements hook_form_submit().
 */
function miss_hub_form_submit($form, &$form_state) {

  $id = !empty($form_state['input']['id']) ? '&id=' . $form_state['input']['id'] : '';
  $status = '&status=' . ($form_state['input']['enabled'] == 1 ? 0 : 1);

  $error_message = t('Could not modify the story. Please try again. If this persists, please contact the site administrator.');

  $content = array(
    'placeStoryTitle' => $form_state['input']['placeStoryTitle'],
    'text' => $form_state['input']['text']['value'],
    'storySource' => $form_state['input']['storySource']['value'],
    'imageUrl' => $form_state['input']['imageUrl'],
    'imageCredit' => $form_state['input']['imageCredit']['value'],
    'tags' => $form_state['input']['tags'],
    'time' => $form_state['input']['time'],
    'region' => $form_state['input']['region'],
    'place' => $form_state['input']['place'],
    'respondentName' => $form_state['input']['respondentName'],
    'email' => $form_state['input']['email'],
    );

  $uri = 'content=' . urlencode(json_encode($content)) . $id . $status;
  $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/setStory.htm', $uri );

  if(!empty($set['id'])) {

    // Set default images.
    $default_images = !empty($form_state['input']['defaultImages']) ? json_decode($form_state['input']['defaultImages'], true) : array();

    if(isset($form_state['input']['featured']) && ($form_state['input']['featured'] === '1')) {
      // Construct this record's array.
      $this_record = array(
        'tag' => $form_state['input']['placeStoryTitle'],
        'imageUrl' => $form_state['input']['imageUrl']
        );

      $all_default_image_urls = array();

      // Loop through the default images, build an array of all imageUrl.
      foreach($default_images as $image_key => $image_value) {
        $all_default_image_urls[] = $image_value['imageUrl'];
      }
      // If the image is not already in the array, push.
      if(!in_array($this_record['imageUrl'], $all_default_image_urls)) {
        array_push($default_images, $this_record);
      }

    } else {

      // Loop through the default images, and set the image key to be unset.
      foreach($default_images as $image_key => $image_value) {
        if($image_value['imageUrl'] === $form_state['input']['imageUrl']) {
          $unset_image_key = $image_key;
          // Unset the default image that's being taken away.
          unset($default_images[$unset_image_key]);
        }
      }

      $default_images = array_values($default_images);

    }

    // Send the request.
    $uri = 'defaultImages=' . json_encode($default_images);
    $set_default_images = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/setDefaultImages.htm', $uri );

    if(empty($set_default_images['id'])) {
      drupal_set_message( $error_message, 'error' );
    }

    drupal_set_message( t( '"' . $form_state['input']['placeStoryTitle'] . '" has been successfully modified.') );

    //If the id from the form is not set, then it was a newly added story
    //A newly added story should be appended to the current listing of stories
    if (empty($id)) {
      $curr_hub_records = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/render.htm' );
      $new_story_list = array();
      if(!empty($curr_hub_records['stories'])) {
        foreach($curr_hub_records['stories'] as $story) {
          $new_story_list[] = $story['id'];
        }
      }
      $new_story_list[] = $set['id'];
      $uri = 'stories=' . json_encode($new_story_list);
      $new_listing_output = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/setStories.htm', $uri );
    }
  }
  else {
    drupal_set_message( $error_message, 'error' );
  }

  drupal_goto('miss/manage/theHub');
}

/**
 * MISS Hub tags form field options.
 */
function _miss_hub_tags_form_field_options() {
  return array(
    'Community' => t('Community'),
    'Displacement' => t('Displacement'),
    'Home' => t('Home'),
    'Loss' => t('Loss'),
    'Migration' => t('Migration'),
    'Neighborhood' => t('Neighborhood'),
    'Travel' => t('Travel'),
    );
}

/**
 * MISS Hub region form field options.
 */
function _miss_hub_region_form_field_options() {
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

/**
 * Implements hook_form().
 */
function miss_hub_call_to_action_form( $form, &$form_state, $records = array() ) {

  $form = array();
  $form['#tree'] = TRUE;

  if(empty($form_state['num_items']) && empty($records)) {
    $form_state['num_items'] = 1;
  }

  $form_state['num_items'] = !empty($form_state['num_items']) ? $form_state['num_items'] : 0;

  if(!empty($records)) {
    if(!$form_state['num_items']) {
      $form_state['num_items'] = count($records);
    }
  }

  // Build the number of item fieldsets indicated by $form_state['num_items']
  for ($i = 1; $i <= $form_state['num_items']; $i++) {

    $form['callToAction'][$i] = array(

      'callToActionContent' => array(
        '#type' => 'text_format',
        '#base_type' => 'textarea',
        '#title' => t('Call to Action #' . $i),
        '#rows' => 3,
        '#cols' => 10,
        '#required' => FALSE,
        '#resizable' => TRUE,
        '#format' => 'full_html',
        '#default_value' => !empty($records[$i-1]) ? $records[$i-1] : '',
        '#prefix' => '<div style="margin-top: 1rem;"></div>' . "\n",
        '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
        ),
      'weight' => array(
        '#type' => 'textfield',
        '#maxlength' => 524288,
        '#default_value' => ($i),
        '#size' => 3,
        '#attributes' => array('class' => array('item-row-weight')),
        ),

      );

  }

  // Adds a button to add more items.
  $form['add_item'] = array(
    '#type' => 'submit',
    '#value' => t('Add a new Call to Action'),
    '#submit' => array('_miss_hub_call_to_action_form_add_item'),
    '#attributes' => array('style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 2rem;'),
    '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
    );

  // If we have more than one item, this button allows removal of the last item.
  if ($form_state['num_items'] > 1) {
    $form['remove_item'] = array(
      '#type' => 'submit',
      '#value' => t('Remove the latest Call to Action'),
      '#submit' => array('_miss_hub_call_to_action_form_remove_item'),
      // Since we are removing an item, don't validate until later.
      '#limit_validation_errors' => array(),
      '#attributes' => array('style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 2rem;'),
      '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
      );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#default_value' => 'Submit',
    '#prefix' => '<div class="clearfix"></div>' . "\n",
    '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
    );

  return $form;

}

function _miss_hub_call_to_action_form_add_item($form, &$form_state) {
  $form_state['num_items']++;
  $form_state['rebuild'] = TRUE;
}

function _miss_hub_call_to_action_form_remove_item($form, &$form_state) {
  if ($form_state['num_items'] > 1) {
    $form_state['num_items']--;
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Table drag elements.
 */
function miss_hub_table_drag_elements($vars) {

  drupal_add_tabledrag('hub_call_to_action', 'order', 'sibling', 'item-row-weight');

  $data['header'] = array(
    'callToActionContent' => t('Call to Action Entries'),
    'weight' => t('Weight'),
    );

  $rows = array();
  foreach (element_children($vars['callToAction']) as $key) {
    $row = array();
    $row['data'] = array();
    foreach ($data['header'] as $fieldname => $title) {
      $field = $vars['callToAction'][$key][$fieldname];
      $row['data'][] = drupal_render($field);
      $row['class'] = array('draggable');
    }
    $data['rows'][] = $row;
  }

  return $data;
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
// function miss_form_miss_hub_form_alter(&$form, &$form_state, $form_id) {
function miss_form_miss_hub_call_to_action_form_alter(&$form, &$form_state, $form_id) {
  $js = 'jQuery().ready(function(){
    jQuery(".filter-wrapper").hide();
  });';
  drupal_add_js($js, 'inline');
  $css = '.form-item label { clear: both; } .text-format-wrapper { margin-top: 2rem; }';
  drupal_add_css($css, 'inline');
  return $form;
}

/**
 * Implements hook_form_submit().
 */
function miss_hub_call_to_action_form_submit($form, &$form_state) {

  // Sort Call to Action items by the 'weight' field.
  $items = array();
  foreach($form_state['values']['callToAction'] as $akey => $avalue) {
    if(!empty($avalue['callToActionContent']['value'])) {
      $items[(int)$avalue['weight']] = $avalue['callToActionContent']['value'];
    }
  }
  ksort($items);

  $call_to_action_items = array_values($items);

  $uri = 'actionCalls=' . urlencode(json_encode($call_to_action_items));
  $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/setActionCalls.htm', $uri );

  if(isset($set['content']['actionCalls'])) {
    drupal_set_message( t( 'Calls to Action have been successfully modified.') );
  }
  else {
    drupal_set_message( 'Could not modify the Calls to Action. Please try again. If this persists, please contact the site administrator.', 'error' );
  }

  drupal_goto(current_path());
}


/**
 * Implements hook_form().
 */
function miss_hub_instructional_text_form( $form, &$form_state, $record = array() ) {

  $form = array();

  $form['instructionalText'] = array(
    '#type' => 'text_format',
    '#title' => t('Instructional Text'),
    // '#description' => t(''),
    '#rows' => 13,
    '#cols' => 10,
    '#required' => TRUE,
    '#resizable' => TRUE,
    '#format' => 'full_html',
    '#default_value' => !empty($record['instructionalText']) ? $record['instructionalText'] : '',
    '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
    );

  $form['submit'] = array(
    '#type' => 'submit',
    '#default_value' => 'Submit',
    '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
    );

  return $form;
}

/**
 * Implements hook_form_submit().
 */
function miss_hub_instructional_text_form_submit($form, &$form_state) {

  $uri = 'instructionalText=' . urlencode($form_state['input']['instructionalText']['value']);
  $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/setInstructionalText.htm', $uri );

  if(!empty($set['id'])) {
    drupal_set_message( t( 'Instructional text has been successfully modified.') );
  }
  else {
    drupal_set_message( 'Could not modify the instructional text. Please try again. If this persists, please contact the site administrator.', 'error' );
  }

  // drupal_goto('miss/manage/theHub');
  drupal_goto(current_path());
}

/**
 * Implements hook_form()
 */
function miss_hub_remove_story_form( $form, &$form_state, $obj = NULL) {
  $form = array();
  if (!empty($obj['id'])) {
    $form['header'] = array(
      '#markup' => '<h2>' . $obj['placeStoryTitle'] . '</h2>',
      );

    $form['id'] = array(
      '#type' => 'hidden',
      '#default_value' => $obj['id'],
      );

    $form['confirm'] = array(
      '#type' => 'select',
      '#title' => 'Are you sure you want to remove this story?',
      '#options' => array(
        1 => 'Yes',
        0 => 'No'
        ),
      '#default_value' => 0,
      '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
      );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
      );
  }

  return $form;
}

function miss_hub_remove_story_form_submit($form, &$form_state) {
  if (!isset($form_state['values']['id'])) {
    drupal_set_message('missing id', 'error');
    return;
  }

  if ($form_state['values']['confirm'] == 1) {
    //Go through the current listing and create a new listing that does not include the stated item
    $id = $form_state['values']['id'];
    $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/render.htm' );
    $to_be_removed_story = NULL;
    if(!empty($result['stories'])) {
      $new_story_list = array();
      foreach($result['stories'] as $story_key => $story_value) {
        if ($story_value['id'] === $id) {
          $to_be_removed_story = $story_value;
        }
        else {
          $new_story_list[] = $story_value['id'];
        }
      }
      if ($to_be_removed_story !== NULL) {
        $uri = 'stories=' . json_encode($new_story_list);
        $new_listing_output = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/setStories.htm', $uri );
        drupal_set_message(sprintf('The record %s has been removed.',$to_be_removed_story['placeStoryTitle']));
      }
    } else {
      drupal_set_message(sprintf('There was an error processing your request: %s', $result['data']), 'error');
    }

  }
  else {
    drupal_set_message('No changes were made.');
  }

  drupal_goto('miss/manage/theHub');

}
/**
 * Implements hook_form().
 */
function miss_hub_website_directionals_form( $form, &$form_state, $record = array() ) {

  $form = array();

  $form['primaryTitle'] = array(
    '#type' => 'textfield',
    '#title' => t('Primary Title'),
    // '#description' => t(''),
    '#size' => 65,
    '#maxlength' => 524288,
    '#required' => TRUE,
    '#default_value' => !empty($record['primaryTitle']) ? $record['primaryTitle'] : '',
    '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
    );

  $form['primaryCaption'] = array(
    '#type' => 'text_format',
    '#title' => t('Primary Caption'),
    // '#description' => t(''),
    '#rows' => 13,
    '#cols' => 10,
    '#required' => TRUE,
    '#resizable' => TRUE,
    '#format' => 'full_html',
    '#default_value' => !empty($record['primaryCaption']) ? $record['primaryCaption'] : '',
    '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
    );

  $form['secondaryTitle'] = array(
    '#type' => 'textfield',
    '#title' => t('Secondary Title'),
    // '#description' => t(''),
    '#size' => 65,
    '#maxlength' => 524288,
    '#required' => TRUE,
    '#default_value' => !empty($record['secondaryTitle']) ? $record['secondaryTitle'] : '',
    '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
    );

  $form['secondaryCaption'] = array(
    '#type' => 'text_format',
    '#title' => t('Secondary Caption'),
    // '#description' => t(''),
    '#rows' => 13,
    '#cols' => 10,
    '#required' => TRUE,
    '#resizable' => TRUE,
    '#format' => 'full_html',
    '#default_value' => !empty($record['secondaryCaption']) ? $record['secondaryCaption'] : '',
    '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
    );

  $form['submit'] = array(
    '#type' => 'submit',
    '#default_value' => 'Submit',
    '#disabled' => !user_access('edit hub content') ? TRUE : FALSE,
    );

  return $form;
}

/**
 * Implements hook_form_submit().
 */
function miss_hub_website_directionals_form_submit($form, &$form_state) {

  $content = array(
    array(
      'primaryTitle' => $form_state['input']['primaryTitle'],
      'primaryCaption' => $form_state['input']['primaryCaption']['value'],
      'secondaryTitle' => $form_state['input']['secondaryTitle'],
      'secondaryCaption' => $form_state['input']['secondaryCaption']['value'],
      )
    );

  $uri = 'websiteDirectionals=' . urlencode(json_encode($content));
  $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/setWebsiteDirectionals.htm', $uri );

  if(!empty($set['id'])) {
    drupal_set_message( t( 'Website directionals has been successfully modified.') );
  }
  else {
    drupal_set_message( 'Could not modify the website directionals. Please try again. If this persists, please contact the site administrator.', 'error' );
  }

  // drupal_goto('miss/manage/theHub');
  drupal_goto(current_path());
}

/**
 * Builds the Browse and Manage pages.
 */
function _miss_hub_page_build($interactive = '') {

  if(!user_access('view hub content')){
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

    drupal_set_title($title . ': Browse Stories');

    // Get the all of the records.
    $data['record'] = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/render.htm' );

    $default_images = !empty($data['record']['defaultImages']) ? $data['record']['defaultImages'] : array();
    $all_default_image_urls = array();

    // Loop through the default images, build an array of all imageUrls.
    foreach($default_images as $image_key => $image_value) {
      $all_default_image_urls[] = $image_value['imageUrl'];
    }

    // view links
    $data['page'] = '<p><strong>View:</strong> ' . l(t('Statistics'), 'miss/stats/home/theHub') . ' | ' . l(t('Backups'), 'miss/backup/browse') . "<br>";
    // Manage links
    $data['page'] .= '<strong>Manage:</strong> ';
    $data['page'] .= l(t('Calls to Action'), 'miss/manage/theHub/callToAction') . ' | ' . "\n";
    $data['page'] .= l(t('Instructional Text'), 'miss/manage/theHub/instructionalText') . ' | ' . "\n";
    $data['page'] .= l(t('Website Directionals'), 'miss/manage/theHub/websiteDirectionals') . ' | ' . "\n";
    $data['page'] .= (user_access('edit hub content') ? l(t('Add a new story'), 'miss/manage/theHub/add') : '<span style="color:gray;">Add a new Story</span>') . '</p>' . "\n";

    if(!empty($data['record']['stories'])) {

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
        $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/getComponents.htm' );
        if(array_key_exists($data['page']['record']['id'], $result['components'])) {
          $record = $result['components'][$data['page']['record']['id']];
        }
        $dump = '<pre>' . var_export($record, TRUE) . '</pre>';
        drupal_set_message( $dump, 'status' );
      }

      // DIV
      $data['page'] .=' <div class="datatables-body">' . "\n";
      // Table top
      $data['page'] .= '<table id="browse_the_hub" class="display" cellspacing="0" width="100%">' . "\n";
      $data['page'] .= '<thead>' . "\n";
      $data['page'] .= '<tr>' . "\n";
      $data['page'] .= '<th width="38%">Story Title</th>' . "\n";
      $data['page'] .= '<th width="38%">Tags</th>' . "\n";
      $data['page'] .= '<th width="12%">Featured</th>' . "\n";
      $data['page'] .= '<th width="12%" class="no-sort" style="text-align: center;">Actions</th>' . "\n";
      $data['page'] .= '</tr>' . "\n";
      $data['page'] .= '</thead>' . "\n";
      $data['page'] .= '<tbody>' . "\n";

      // Loop through all of the records to build the table rows.
      foreach($data['record']['stories'] as $key => $value) {

        if(!empty($value)) {
          // Set the featured record flag.
          $featured_flag = '';

          if(in_array($value['imageUrl'], $all_default_image_urls)) {
            $featured_flag = '<span style="color: red;">featured</span>' . "\n";
          }

          //tags not mandatory. if no tags on story then index doesn't get set but we want to prep for tags implode
          if (empty($value['tags'])) $value['tags'] = array();
          $data['page'] .= '<tr>' . "\n";
          $data['page'] .= '<td><strong>' . $value['placeStoryTitle'] . '</strong></td>' . "\n";
          $data['page'] .= '<td>' . implode(', ', $value['tags']) . '</td>' . "\n";
          $data['page'] .= '<td style="text-align: center;">' . $featured_flag . '</td>' . "\n";
          $data['page'] .= '<td style="text-align: center;">' . "\n";
          $data['page'] .= '<a href="/' . current_path() . '/edit/' . $value['id'] . '" style="margin-right: 4px;">'. (user_access('edit hub content') ? t('Edit') : t('View')) .'</a>' . "\n";
          $data['page'] .= ' | ';
          $data['page'] .= (user_access('edit hub content') ? ('<a href="/' . current_path() . '/remove/' . $value['id'] . '" style="margin-right: 4px;">'.t('Delete').'</a>') : ('<span style="color:gray;">Remove</span>')) . "\n";
          $data['page'] .= '</td>' . "\n";
          $data['page'] .= '</tr>' . "\n";
        }

      }

      // Table bottom
      $data['page'] .= '</tbody>' . "\n";
      $data['page'] .= '</table>' . "\n";
      // DIV end
      $data['page'] .= '</div>' . "\n";

    } else {
      drupal_set_message( t('No records found.'), 'error' );
    }

    return $data['page'];
  }
  /////////////////////////////////////////////
  // MANAGE PAGES //////////////////////
  /////////////////////////////////////////////
  else {

    switch($path_parts[3]) {

      // Add a new record.
      case 'add':

      if(!user_access('edit hub content')){
        drupal_set_message( t('You do not have permission to view this page. Please contact a site administrator for further instructions.'), 'error' );
        drupal_goto('miss/manage/theHub');
      }

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
      drupal_add_css(
        drupal_get_path('module', 'miss') . '/css/_the_hub_manage.css',
        array('group' => CSS_THEME, 'type' => 'file', 'weight' => 51)
        );
        // Chosen.js
      $chosen_js = "
      jQuery(document).ready(function($) {
        $('#edit-tags').chosen();
      });
      ";
      drupal_add_js($chosen_js, 'inline');

      drupal_set_title($title . ': Add a New Story');

        // Get the form.
      $form = drupal_get_form('miss_hub_form');
      $out = drupal_render($form);

      return $out;
      break;
      case 'remove':

      if(!user_access('edit hub content')){
        drupal_set_message( t('You do not have permission to view this page. Please contact a site administrator for further instructions.'), 'error' );
        drupal_goto('miss/manage/theHub');
      }

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
      drupal_add_css(
        drupal_get_path('module', 'miss') . '/css/_the_hub_manage.css',
        array('group' => CSS_THEME, 'type' => 'file', 'weight' => 51)
        );
        // Chosen.js
      $chosen_js = "
      jQuery(document).ready(function($) {
        $('#edit-tags').chosen();
      });
      ";
      drupal_add_js($chosen_js, 'inline');

      $data['page']['form'] = array();
      if (!empty($path_parts[4])) {
          // Get all of the records.
        $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/render.htm' );
          // Loop through to find the desired record.
        if(!empty($result['stories'])) {
          foreach($result['stories'] as $story_key => $story_value) {

            if($story_value['id'] === $path_parts[4]) {
              $data['page']['record'] = $result['stories'][$story_key];

                // Set the title.
              drupal_set_title('Remove "' . $story_value['placeStoryTitle'] . '"');
                // Build-out the breadcrumbs.
              $current_breadcrumbs = drupal_get_breadcrumb();
              $crumbs = array();
              foreach($current_breadcrumbs as $key => $value) {
                $crumbs[] = $value;
              }
              $crumbs[] = t(drupal_get_title());
              drupal_set_breadcrumb($crumbs);
            }

          }

            // EDAN record dump
          if(!empty($data) && isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
            $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/getComponents.htm' );
            if(array_key_exists($data['page']['record']['id'], $result['components'])) {
              $record = $result['components'][$data['page']['record']['id']];
            }
            $dump = '<pre>' . var_export($record, TRUE) . '</pre>';
            drupal_set_message( $dump, 'status' );
          }

            // If a record doesn't exist, set a message.
          if(empty($data['page']['record'])) {
            drupal_set_message( t('The story record could not be found. (ID = ' . $path_parts[4]) . ')', 'error' );
            drupal_goto('miss/manage/theHub');
          }

            // Return the default images array.
          $data['page']['record']['defaultImages'] = $result['defaultImages'];

            // Get the form.
          $form = drupal_get_form('miss_hub_remove_story_form', $data['page']['record']);
          $data['page']['form'] = drupal_render($form);
        } else {
            // If a record doesn't exist, set a message.
          drupal_set_message( t('The story record could not be found. (ID = ' . $path_parts[4]) . ')', 'error' );
          drupal_goto('miss/manage/theHub');
        }

        drupal_set_title($title . ': Remove an Existing Story');
      }
      return $data['page']['form'];
      break;
      // Edit an existing record.
      case 'edit':

      $out = '';
      $error_message = t('The story record could not be found. (ID = ' . $path_parts[4] . ')');
      $data['page']['record'] = array();

        // Get the record from EDAN.
      if(!empty($path_parts[4])) {
          // Get all of the records.
        $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/render.htm' );

          // Loop through to find the desired record.
        if(!empty($result['stories'])) {
          foreach($result['stories'] as $story_key => $story_value) {

            if($story_value['id'] === $path_parts[4]) {

              $data['page']['record'] = $result['stories'][$story_key];

                // Set the title.
              drupal_set_title('Manage "' . $story_value['placeStoryTitle'] . '"');
                // Build-out the breadcrumbs.
              $current_breadcrumbs = drupal_get_breadcrumb();
              $crumbs = array();
              foreach($current_breadcrumbs as $key => $value) {
                $crumbs[] = $value;
              }
              $crumbs[] = t(drupal_get_title());
              drupal_set_breadcrumb($crumbs);
            }

          }

            // If a record doesn't exist, set a message.
          if(empty($data['page']['record'])) {
            drupal_set_message( $error_message, 'error' );
            drupal_goto('miss/manage/theHub');
          }

            // EDAN record dump
          if(!empty($data) && isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
            $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/getComponents.htm' );
            if(array_key_exists($data['page']['record']['id'], $result['components'])) {
              $record = $result['components'][$data['page']['record']['id']];
            }
            $dump = '<pre>' . var_export($record, TRUE) . '</pre>';
            drupal_set_message( $dump, 'status' );
          }

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
          drupal_add_css(
            drupal_get_path('module', 'miss') . '/css/_the_hub_manage.css',
            array('group' => CSS_THEME, 'type' => 'file', 'weight' => 51)
            );
            // Chosen.js
          $chosen_js = "
          jQuery(document).ready(function($) {
            $('#edit-tags').chosen();
          });
          ";
          drupal_add_js($chosen_js, 'inline');

            // Return the default images array.
          $data['page']['record']['defaultImages'] = $result['defaultImages'];

            // Get the form.
          $form = drupal_get_form('miss_hub_form', $data['page']['record']);
          $out .= drupal_render($form);

        } else {
            // If a record doesn't exist, set a message.
          drupal_set_message( $error_message, 'error' );
          drupal_goto('miss/manage/theHub');
        }

      }
      return $out;
      break;

      // Edit instructional text.
      case 'instructionalText':

        // Get all of the records.
      $record = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/render.htm' );

        // Set the title.
      drupal_set_title('Manage Instructional Text');
        // Build-out the breadcrumbs.
      $current_breadcrumbs = drupal_get_breadcrumb();
      $crumbs = array();
      foreach($current_breadcrumbs as $key => $value) {
        $crumbs[] = $value;
      }
      $crumbs[] = t(drupal_get_title());
      drupal_set_breadcrumb($crumbs);

        // Get the form.
      $form = drupal_get_form('miss_hub_instructional_text_form', $record);
      return drupal_render($form);

      break;

      // Edit website directionals.
      case 'websiteDirectionals':

        // Get all of the records.
      $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/render.htm' );
        // Not sure why it's indexed (0)...
      $record = $result['websiteDirectionals'][0];

        // Set the title.
      drupal_set_title('Manage Website Directionals');
        // Build-out the breadcrumbs.
      $current_breadcrumbs = drupal_get_breadcrumb();
      $crumbs = array();
      foreach($current_breadcrumbs as $key => $value) {
        $crumbs[] = $value;
      }
      $crumbs[] = t(drupal_get_title());
      drupal_set_breadcrumb($crumbs);

        // Get the form.
      $form = drupal_get_form('miss_hub_website_directionals_form', $record);
      return drupal_render($form);

      break;

      // Edit call to action items.
      case 'callToAction':

        // Get all of the records.
      $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/render.htm' );

      $record = isset($result['actionCalls']) ? $result['actionCalls'] : array();

        // Set the title.
      drupal_set_title('Manage Calls to Action');
        // Build-out the breadcrumbs.
      $current_breadcrumbs = drupal_get_breadcrumb();
      $crumbs = array();
      foreach($current_breadcrumbs as $key => $value) {
        $crumbs[] = $value;
      }
      $crumbs[] = t(drupal_get_title());
      drupal_set_breadcrumb($crumbs);

        // Get the form.
      $form = drupal_get_form('miss_hub_call_to_action_form', $record);
        // Intregrate table drag.
      $table_elements = miss_hub_table_drag_elements($form);
        // Begin building the output.
      $out = '<form id="' . $form['#id'] . '" accept-charset="UTF-8" method="' . $form['#method'] . '" action="' . $form['#action'] . '">';
        // Unset the callToAction field.
      unset($form['callToAction']);
      $out .= theme('table', array('header' => $table_elements['header'], 'rows' => $table_elements['rows'], 'attributes' => array('id' => 'hub_call_to_action')));
        // Add buttons and the rest of the form.
      $out .= drupal_render($form['add_item']);
      $out .= drupal_render($form['remove_item']);
      $out .= drupal_render_children($form);
      return $out;

      break;

      // Send a 404.
      default:
      return MENU_NOT_FOUND;

    }

  }

}

$output = _miss_hub_page_build($interactive);
