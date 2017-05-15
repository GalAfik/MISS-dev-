<?php

require '_in_the_studio.class.php';
define('MISS_SOUND_ENABLED_STATUS', 0);
define('MISS_SOUND_DISABLED_STATUS', 1);

function _miss_in_the_studio_non_audio_categories() {
  return array(
    'pop-ups',
  );
}

function _miss_in_the_studio_reorder_form($element, &$form_state) {
  $form = array();

  $form['order'] = array(
    '#type' => 'hidden',
    '#default_value' => filter_input(INPUT_POST, 'order', FILTER_SANITIZE_STRING),
    '#attributes' => array('id' => 'miss-reorder-holder'),
    '#disabled' => !user_access('edit in the studio content') ? TRUE : FALSE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Reorder'),
    '#prefix' => '<div id="studio-instructions"><p>Drag sounds into the desired order (muted sounds will be excluded) &nbsp; ',
    '#suffix' => '</p></div>',
    '#disabled' => !user_access('edit in the studio content') ? TRUE : FALSE,
  );

  return $form;
}

function _miss_in_the_studio_reorder_form_submit($element, &$form_state) {

  // Get EDAN object with this site's key
  $edan = edan();

  $obj = new InTheStudio();

  $order_array = explode("\n", trim($form_state['values']['order']));
  array_walk($order_array, create_function('&$val', '$val = trim($val);'));
  $result = $obj->reorder($order_array, $edan);

  #echo '<pre>' . var_export($result, TRUE) . '</pre>';
  if ($result['info']['http_code'] == 200) {
    drupal_set_message('The order has been updated.');
  } else {
    drupal_set_message(sprintf('There was an error processing your request: %s', $result['data']), 'error');
  }
}

function _miss_in_the_studio_mute_form($component, &$form_state, $obj = NULL) {
  $form = array();

  if (isset($obj->id) && !empty($obj->id)) {
    // TODO: Review this
    // Track the object through form processing
    $form_state['_miss']['mute_object'] = $obj;

    $form['header'] = array(
      '#markup' => '<h2>' . $obj->title . '</h2>',
    );

    $form['id'] = array(
      '#type' => 'hidden',
      '#default_value' => $obj->id,
    );

    $confirm_title = ($obj->status == MISS_SOUND_ENABLED_STATUS) ? 'mute (disable)' : 'unmute (enable)';
    $form['confirm'] = array(
      '#type' => 'select',
      '#title' => 'Are you sure you want to ' . $confirm_title . ' this sound?',
      '#options' => array(
        1 => 'Yes',
        0 => 'No'
      ),
      '#default_value' => 0,
    );

    $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  }

  return $form;
}
/**
 * Helper function to reorder the sounds with an item that is newly muted or unmuted
 *
 * @param $id
 *   The sound id (something like li2-9999999999324-1459026634403-0 ) 
 * @param $status
 *   The status of the sound: 0 - Unmuted (Enabled), 1 - Muted (Disabled)
 *   IGNORED if $toggle is TRUE
 * @param $toggle
 *   TRUE / FALSE - toggle status between Unmuted / Muted
 *
 * @returns
 *   Error message if any, NULL otherwise
 */
function _miss_in_the_studio_reorder_with_mute($id = NULL, $status = NULL, $toggle = FALSE) {

  if ($status !== MISS_SOUND_ENABLED_STATUS && $status !== MISS_SOUND_DISABLED_STATUS && $toggle === FALSE) {
    // Coming directly from a form submission may have a string status instead of the integer status
    drupal_set_message('The status entered does not match the known statuses available', 'error');
  }
  $to_return = NULL;
  $edan = edan();

  //Send the reorder with the muted or unmuted item
  $its_obj = new InTheStudio();

  //Get the current ordering
  $service = 'miss/v1.0/nmaahc/in_the_studio/render.htm';
  $info = array();
  $current_ordering_results = $edan->sendRequest('', $service, FALSE, $info);
  $cor_obj = json_decode($current_ordering_results, TRUE);

  if ($toggle) {
    $status = ($its_obj->components[$id]->status === MISS_SOUND_ENABLED_STATUS) ? MISS_SOUND_DISABLED_STATUS : MISS_SOUND_ENABLED_STATUS;
  }
  if (!empty($cor_obj)) {

    // The InTheStudio object is not yet updated with the enabled status, so manually set the unmute status
    $its_obj->components[$id] = new stdClass();
    $its_obj->components[$id]->status = $status;

    $order_array = array();
    $found_in_current_order = FALSE;
    foreach($cor_obj['soundAssets'] as $sound_asset){
      // Make sure $sound_asset is not empty.
      if(!empty($sound_asset)) {
        $order_array[] = $sound_asset['id'];
      }
    }

    if($status == MISS_SOUND_ENABLED_STATUS) {
      array_push($order_array, $id);
    }

    // Make sure there are no duplicates.
    $order_array = array_unique($order_array);
    
    // Send to EDAN to reorder.
    $ordering_result = $its_obj->reorder($order_array, $edan);

    if ($ordering_result['info']['http_code'] != 200) {
      $to_return = sprintf('There was an error processing your request: %s', $ordering_result['data']);
    }
  }else{
    $to_return = 'There was an error processing your request';
  }
  return $to_return;
}

function _miss_in_the_studio_mute_form_submit($form, &$form_state) {
  if (!isset($form_state['values']['id'])) {
    drupal_set_message('missing id', 'error');
    return;
  }

  if ($form_state['values']['confirm'] == 1) {
    // Get EDAN object with this site's key
    $edan = edan();

    $result = $form_state['_miss']['mute_object']->mute($edan);

    if ($result['info']['http_code'] == 200) {
      $id = $form_state['values']['id'];
      //The form is a toggle, not a set to true or set to false
      $reordering_error_message = _miss_in_the_studio_reorder_with_mute($id, NULL, TRUE);
      if ($reordering_error_message !== NULL){
        drupal_set_message($reordering_error_message, 'error');
      }  
      drupal_set_message('Your record has been saved.');
    } else {
      drupal_set_message(sprintf('There was an error processing your request: %s', $result['data']), 'error');
    }

    drupal_goto();
  }

}

function _miss_in_the_studio_form($component, &$form_state, $record = array()) {

  $obj = new InTheStudio();
  $category = isset($record) ? $record['category'] : '';
  $non_audio_categories = _miss_in_the_studio_non_audio_categories();
  
  $form = array();
  $form['#tree'] = (!in_array($category, $non_audio_categories)) ? TRUE : FALSE;

  // Backwards, because EDAN's status flags are backwards.
  $status = (isset($record['status']) && $record['status']) ? 0 : 1;

  //hide upload/insert buttons for non editors
  $button_script = '
  jQuery(document).ready(function() {
     jQuery("a:contains(' . "'Upload / Insert'" . ')").hide();
  });
  ';
  if(!user_access('upload files')){
    drupal_add_js($button_script, 'inline');
  }

  $form['featuredItem'] = array(
    '#type' => 'fieldset',
    '#title' => t('Published'),
  );

  $form['featuredItem']['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enabled'),
    '#default_value' => $status,
    '#prefix' => '<div style="float:left">',
    '#suffix' => '</div>',
    '#disabled' => !user_access('edit in the studio content') ? TRUE : FALSE,
  );

  if (isset($record['id']) && !empty($record['id'])) {
    $form['id'] = array(
      '#type' => 'hidden',
      '#default_value' => $record['id']
    );
  }

  $form['title'] = array(
    '#type' => 'textfield',
    '#maxlength' => 524288,
    '#title' => 'Title',
    '#required' => TRUE,
    '#default_value' => !empty($record['title']) ? $record['title'] : '',
    '#disabled' => !user_access('edit in the studio content') ? TRUE : FALSE,
  );

  $form['popUpText'] = array(
    '#type' => 'text_format',
    '#format' => 'full_html',
    '#title' => 'Pop Up Text',
    '#required' => FALSE,
    '#default_value' => !empty($record['popUpText']) ? $record['popUpText'] : '',
    '#disabled' => !user_access('edit in the studio content') ? TRUE : FALSE,
  );

  $form['icon'] = array(
    '#type' => 'textfield',
    '#maxlength' => 524288,
    '#title' => 'Icon URL',
    '#required' => FALSE,
    '#default_value' => !empty($record['icon']) ? $record['icon'] : '',
    '#suffix' => !empty($record['icon']) ? '<div style="margin-bottom: 1rem;"><a href="' . $record['icon'] . '" target="_blank">View Icon</a> (opens in a new window/tab)</div>' : '',
    '#attributes' => array(
      'class' => array(
        'miss-file-manage',
      ),
    ),
    '#disabled' => !user_access('edit in the studio content') ? TRUE : FALSE,
  );

  $form['iconCredit'] = array(
    '#type' => 'text_format',
    '#format' => 'full_html',
    '#title' => 'Icon Credit(s)',
    '#required' => FALSE,
    '#default_value' => !empty($record['iconCredit']) ? $record['iconCredit'] : '',
    '#suffix' => '<div style="margin-bottom: 1rem;"></div>',
    '#disabled' => !user_access('edit in the studio content') ? TRUE : FALSE,
  );

  // disable the category select if the user is not an admin and this record's status is 0
  global $user;
  $form['category'] = array(
    '#type' => 'select',
    '#title' => 'Category',
    '#options' => drupal_map_assoc($obj->cats),
    '#required' => TRUE,
    '#default_value' => !empty($record['category']) ? $record['category'] : $category,
    '#disabled' => !user_access('edit in the studio content') || (!in_array('administrator', array_values($user->roles)) && isset($record['status']) && $record['status'] == 0) ? TRUE : FALSE,
  );

  if(!in_array($category, $non_audio_categories)) {

    $form['loop'] = array(
      '#type' => 'hidden',
      '#title' => 'Loop',
      '#options' => array(
        'true' => 'Yes',
        'false' => 'No'
      ),
      '#default_value' => !empty($record['loop']) ? 'true' : 'false',
      '#disabled' => !user_access('edit in the studio content') ? TRUE : FALSE,
    );

    if(empty($form_state['num_items']) && empty($record['audioClips'])) {
      $form_state['num_items'] = 1;
    }

    $form_state['num_items'] = !empty($form_state['num_items']) ? $form_state['num_items'] : 0;

    if(!empty($record['audioClips'])) {
      if(!$form_state['num_items']) {
        $form_state['num_items'] = count($record['audioClips']);
      }
    }

    // Build the number of item fieldsets indicated by $form_state['num_items']
    for ($i = 1; $i <= $form_state['num_items']; $i++) {

      $form['audioClips'][$i] = array(

        'tempo' => array(
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#title' => 'Tempo',
          '#size' => 20,
          '#required' => TRUE,
          '#default_value' => !empty($record['audioClips'][$i-1]['tempo']) ? $record['audioClips'][$i-1]['tempo'] : 'any',
          '#disabled' => !user_access('edit in the studio content') ? TRUE : FALSE,
        ),

        'key' => array(
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#title' => 'Key',
          '#size' => 20,
          '#required' => TRUE,
          '#default_value' => !empty($record['audioClips'][$i-1]['key']) ? $record['audioClips'][$i-1]['key'] : 'any',
          '#disabled' => !user_access('edit in the studio content') ? TRUE : FALSE,
        ),

        'url' => array(
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#title' => 'Audio Clip URL',
          '#size' => 40,
          '#maxlength' => 524288,
          '#required' => TRUE,
          '#default_value' => !empty($record['audioClips'][$i-1]['url']) ? $record['audioClips'][$i-1]['url'] : '',
          '#suffix' => !empty($record['audioClips'][$i-1]['url']) ? '<div style="margin: 0 0 0.5rem 0;"><a href="' . $record['audioClips'][$i-1]['url'] . '" target="_blank">Play audio file</a> (opens in a new window/tab)</div>' : '',
          '#attributes' => array(
            'class' => array(
              'miss-file-manage',
            ),
          ),
          '#disabled' => !user_access('edit in the studio content') ? TRUE : FALSE,
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
      '#value' => t('Add a new audio clip'),
      '#submit' => array('_miss_in_the_studio_audio_clip_form_add_item'),
      '#attributes' => array('style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 2rem;'),
      '#disabled' => !user_access('edit in the studio content') ? TRUE : FALSE,
    );

    // If we have more than one item, this button allows removal of the last item.
    if ($form_state['num_items'] > 1) {
      $form['remove_item'] = array(
        '#type' => 'submit',
        '#value' => t('Remove latest audio clip'),
        '#submit' => array('_miss_in_the_studio_audio_clip_form_remove_item'),
        // Since we are removing an item, don't validate until later.
        '#limit_validation_errors' => array(),
        '#attributes' => array('style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 2rem;'),
        '#disabled' => !user_access('edit in the studio content') ? TRUE : FALSE,
      );
    }

  }

  $form['submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Save'),
    '#prefix' => '<div class="clearfix" style="margin-bottom: 1rem;"></div>',
    '#disabled' => !user_access('edit in the studio content') ? TRUE : FALSE,
  );

  return $form;
}

function _miss_in_the_studio_audio_clip_form_add_item($form, &$form_state) {
  $form_state['num_items']++;
  $form_state['rebuild'] = TRUE;
}

function _miss_in_the_studio_audio_clip_form_remove_item($form, &$form_state) {
  if ($form_state['num_items'] > 1) {
    $form_state['num_items']--;
  }
  // Setting $form_state['rebuild'] = TRUE causes the form to be rebuilt again.
  $form_state['rebuild'] = TRUE;
}

/**
 * Table drag elements.
 */
function miss_in_the_studio_table_drag_elements($vars) {

  drupal_add_tabledrag('in_the_studio_audio_clips', 'order', 'sibling', 'item-row-weight');
 
  $data['header'] = array(
    'tempo' => t('Audio Clip Tempo'), 
    'key' => t('Audio Clip Key'), 
    'url' => t('Audio Clip URL'), 
    'weight' => t('Weight'),
  );
 
  $rows = array();
  foreach (element_children($vars['audioClips']) as $key) {

    $row = array();
    $row['data'] = array();
    foreach ($data['header'] as $fieldname => $title) {
      $field = $vars['audioClips'][$key][$fieldname];
      $row['data'][] = drupal_render($field);
      $row['class'] = array('draggable');
    }
    $data['rows'][] = $row;
  }

  return $data;
}

function _miss_in_the_studio_form_submit($form, &$form_state) {

  // Get EDAN object with this site's key
  $edan = edan();
  $component = new InTheStudio_Component();
  $non_audio_categories = _miss_in_the_studio_non_audio_categories();

  if(isset($form_state['values']['id'])) {
    $component->id = $form_state['values']['id'];
  }

  // Sort audio clips by the 'weight' field.
  $audio_clips = array();
  // Checking due to non-audio categories (e.g. pop-ups) not having audio clips.
  if(isset($form_state['values']['audioClips'])) {
    foreach($form_state['values']['audioClips'] as $akey => $avalue) {
      $audio_clips[(int)$avalue['weight']]['tempo'] = $avalue['tempo'];
      $audio_clips[(int)$avalue['weight']]['key'] = $avalue['key'];
      $audio_clips[(int)$avalue['weight']]['url'] = $avalue['url'];
    }
    ksort($audio_clips);
    $audio_clips = array_values($audio_clips);
  }

  // Backwards, because EDAN's status flags are backwards.
  // Featured status: 0 = published, 1 = not published
  // Need to differentiate between the status variable being passed by the pop-up category and all other categories.
  $item_status = (!in_array($form_state['values']['category'], $non_audio_categories)) 
    ? $form_state['input']['featuredItem']['status'] 
    : $form_state['input']['status'];
  $status = (isset($item_status) && $item_status) ? '0' : '1';

  // Need to supply unique titles for new records.
  // $title = !isset($form_state['values']['id']) ? $form_state['values']['title'] . '-' . mt_rand( 0, 0xffff ) : $form_state['values']['title'];

  $component->title = $form_state['values']['title'];
  $component->icon = $form_state['values']['icon'];
  $component->iconCredit = $form_state['values']['iconCredit']['value'];
  $component->popUpText = $form_state['values']['popUpText']['value'];
  $component->category = $form_state['values']['category'];
  // Checking due to non-audio categories (e.g. pop-ups) not having audio clips and loops.
  if(!empty($audio_clips)) {
    $component->loop = $form_state['values']['loop'];
    $component->audioClips = json_encode($audio_clips);
  }
  $component->status = $status;

  $result = $component->save($edan);
  $result_array = json_decode($result['data'], true);

  if ($result['info']['http_code'] == 200) {
    $reordering_error_message = '';
    if(!empty($result_array['id'])) {
      $reordering_error_message = _miss_in_the_studio_reorder_with_mute($result_array['id'], intval($status));
    }
    if(!empty($reordering_error_message)){
      drupal_set_message($reordering_error_message, 'error');
    }  
    drupal_set_message('"' . $form_state['values']['title'] . '" has been successfully saved.');
  } else {
    drupal_set_message(sprintf('There was an error processing your request: %s', $result['data']), 'error');
  }

  drupal_goto('miss/manage/inTheStudio');
}

function _miss_in_the_studio_delete_form( $form, &$form_state, $record = array() ) {
  
  $form = array();

  if(isset($record['id']) && !empty($record['id'])) {
    $form['id'] = array(
      '#type' => 'hidden',
      '#default_value' => $record['id']
    );

    $form['title'] = array(
      '#type' => 'hidden',
      '#default_value' => $record['title']
    );
  }

  $form['submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Delete'),
    '#disabled' => !user_access('edit in the studio content') ? TRUE : FALSE,
  );

  return $form;
}

function _miss_in_the_studio_delete_form_submit( $form, &$form_state ) {

  $edan = edan();
  $component = new InTheStudio_Component();

  // Unset any previous messages.
  drupal_get_messages();

  if(isset($form_state['values']['id'])) {

    // Instantiate the InTheStudio class.
    $its_obj = new InTheStudio();

    $component->id = $form_state['values']['id'];
    $component->title = $form_state['values']['title'];
    $component->status = -1;

    // Remove the component.
    $result = $component->delete($edan);

    // EDAN throws a 404 when deleting. Let's hack it for now...
    // TODO: Ask Andrew about this anomaly, and see if it can be fixed.
    if(($result['info']['http_code'] === 200) && ($component->status === -1)) {

      // Get the current ordering
      $service = 'miss/v1.0/nmaahc/in_the_studio/render.htm';
      $info = array();
      $current_ordering_results = $edan->sendRequest('', $service, FALSE, $info);
      $current_ordering_array = json_decode($current_ordering_results, TRUE);

      if(!empty($current_ordering_array)) {

        // Set the key to be removed.
        $remove_key = array_search($component->id, $its_obj->ordered);

        // Remove the key from the array.
        unset($its_obj->ordered[$remove_key]);
        // Re-index keys.
        $ordered_array = array_values($its_obj->ordered);

        // Reorder via the setSoundAssets.htm API endpoint.
        $ordering_result = $its_obj->reorder($ordered_array, $edan);

        // If the ID could not be removed from the ordered render array, set an error message to append to the first message.
        // TODO: Add all errors to the database via Watchdog (?) May be a good idea...
        $error = '';
        if($result['info']['http_code'] !== 200) {
          $error = ' The component ID (' . $component->id . ') still exists within the ordered render array.';
        }

      }

      drupal_set_message('The "' . $component->title . '" entry has been successfully deleted.', 'status');
    } else {
      drupal_set_message('The "' . $component->title . '" entry has not been deleted.' . $error, 'error');
    }
    
  } else {
    // The ID was not set.
    drupal_set_message('No record ID provided.', 'error');
  }

  drupal_goto('miss/manage/inTheStudio');
}

function _miss_in_the_studio_page_build() {

  if(!user_access('view in the studio content')){
    drupal_set_message('You do not have access to this page. Please contact an Administrator to resolve this issue.', 'error');
    drupal_goto('miss/list');
  }

  // Base will be miss/manage/inTheStudio
  $path_parts = explode('/', request_path());
  $obj = new InTheStudio();
  $non_audio_categories = _miss_in_the_studio_non_audio_categories();
  $mpath = 'miss/manage/inTheStudio';
  $output = '';

  // Default to listing
  if (!isset($path_parts[3])) {
    $path_parts[3] = 'list';
  }

  if ($path_parts[3] == 'add') {
    $out = '';
    $record = NULL;
    $category = filter_input(INPUT_GET, 'category', FILTER_SANITIZE_STRING);

    if ($category) {
      $record = array('category' => $category);
    }

    // Edit Form
    $form = drupal_get_form('_miss_in_the_studio_form', $record);

    // All categories need the table drag functionality, except for the non-audio categories (e.g pop-ups).
    if(in_array($category, $non_audio_categories)) {
      $out .= drupal_render($form);
    } else {
      // Intregrate table drag.
      $table_elements = miss_in_the_studio_table_drag_elements($form);
      // Build the output.
      $out .= '<form id="' . $form['#id'] . '" accept-charset="UTF-8" method="' . $form['#method'] . '" action="' . $form['#action'] . '">';
      $out .= drupal_render($form['featuredItem']);
      $out .= drupal_render($form['title']);
      $out .= drupal_render($form['popUpText']);
      $out .= drupal_render($form['icon']);
      $out .= drupal_render($form['iconCredit']);
      $out .= drupal_render($form['category']);
      $out .= drupal_render($form['loop']);
      unset($form['audioClips']);
      $out .= theme('table', array('header' => $table_elements['header'], 'rows' => $table_elements['rows'], 'attributes' => array('id' => 'in_the_studio_audio_clips')));
      $out .= drupal_render($form['add_item']);
      $out .= drupal_render($form['remove_item']);
      $out .= drupal_render_children($form);
    }
    $output = render($out);
  }
  else if ($path_parts[3] == 'edit') {
    $out = '';
    $record = NULL;
    $id = filter_input(INPUT_GET, 'id', FILTER_SANITIZE_STRING);

    if ($id) {
      $record = (array) $obj->byID($id);
    }

    // Edit Form
    $form = drupal_get_form('_miss_in_the_studio_form', $record);

    // All categories need the table drag functionality, except for the non-audio categories (e.g. pop-ups).
    if(in_array($record['category'], $non_audio_categories)) {
      $out .= drupal_render($form);
    } else {
      // Intregrate table drag.
      $table_elements = miss_in_the_studio_table_drag_elements($form);
      // Build the output.
      $out = '<form id="' . $form['#id'] . '" accept-charset="UTF-8" method="' . $form['#method'] . '" action="' . $form['#action'] . '">';
      $out .= drupal_render($form['featuredItem']);
      $out .= drupal_render($form['title']);
      $out .= drupal_render($form['popUpText']);
      $out .= drupal_render($form['icon']);
      $out .= drupal_render($form['iconCredit']);
      $out .= drupal_render($form['category']);
      $out .= drupal_render($form['loop']);
      unset($form['audioClips']);
      $out .= theme('table', array('header' => $table_elements['header'], 'rows' => $table_elements['rows'], 'attributes' => array('id' => 'in_the_studio_audio_clips')));
      $out .= drupal_render($form['add_item']);
      $out .= drupal_render($form['remove_item']);
      $out .= drupal_render_children($form);
    }
    $output = render($out);
  }
  else if ($path_parts[3] == 'delete') {

    $out = '';
    $record = NULL;
    $id = filter_input(INPUT_GET, 'id', FILTER_SANITIZE_STRING);

    if ($id) {
      $record = (array) $obj->byID($id);
    }

    drupal_set_title( 'Delete "' . $record['title'] . '"?' );
    drupal_set_message( 'Are you sure you want to delete the "' . $record['title'] . '" record?', 'warning' );

    // Edit Form
    $form = drupal_get_form('_miss_in_the_studio_delete_form', $record);
    // Build the output.
    $out .= drupal_render($form);
    $output = render($out);
  }
  else if ($path_parts[3] == 'mute') {
    $record = NULL;
    $id = filter_input(INPUT_GET, 'id', FILTER_SANITIZE_STRING);

    if ($id) {
      $record = $obj->byID($id);
    }

    // Edit Form
    $form = drupal_get_form('_miss_in_the_studio_mute_form', $record);

    $output = render($form);
  }
  else { // Listing
    // EDAN record dump
    if(isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
      $interactives = $obj->interactives();
      $render_obj = _miss_send_miss_request( $interactives['in_the_studio']['endpoints']['realRender'] );
      $render = '<h2>Render</h2><pre>' . var_export($render_obj, TRUE) . '</pre>';
      $object = '<h2>Object</h2><pre>' . var_export($obj, TRUE) . '</pre>';
      drupal_set_message( $render, 'status' );
      // drupal_set_message( $object, 'message' );
    }

    $sortable_js = "
    jQuery(document).ready(function(){
      // Apply Tabs
      jQuery('#studio-tabs').tabs();

      // Apply Sortable
      jQuery('table.sortable_miss tbody').sortable({
          helper: function(event, ui) {
            ui.children().each(function() {
                jQuery(this).width(jQuery(this).width());
            });
            return ui;
          }
      }).disableSelection();

      //
      jQuery('form#-miss-in-the-studio-reorder-form').submit(function(e) {
        var orderText = '';
        jQuery('input.sortable-value').each(function() {
          orderText += jQuery(this).val() + \"\\n\";
        });
        jQuery('input#miss-reorder-holder').val(orderText);
      });

      // Seed numbers
      //sortableEnumerator();

      // Enumerate fields
      function sortableEnumerator() {
        var sortable_counter = 0;
        jQuery('input.sortable-value').each(function() {
          jQuery(this).val(sortable_counter.toString());
          sortable_counter++;
        });
      }
    });
    ";

    drupal_add_css('.sortable_miss tr td {height: 3.5rem;} .sortable_miss tr th:nth-child(2) {width: 30%;}', 'inline');
    drupal_add_library('system', 'ui.tabs');
    drupal_add_library('system', 'ui.sortable');
    drupal_add_js($sortable_js, 'inline');


    // Tabs
    $tabs = array();
    $tab_contents = array();

    foreach ($obj->cats as $cat) {
      $prepend_label = ($cat === 'pop-ups') ? 'Additional ' : '';
      // TODO: This should just be url()
      $tabs[] = '<li><a href="#studio-tab-' . $cat . '">' . $prepend_label . ucwords($cat) . '</a></li>';

      // Table UI
      $rows = array();
      $header = array(
        t('Sound'),
        t('Actions')
      );

      // TODO: Clean up and use theme api
      $this_tab_contents = '<div id="studio-tab-' . $cat .  '" style="padding: 0 0 10px 0;">';

      if (isset($obj->categories[ $cat ])) {
        foreach ($obj->categories[ $cat ] as $o) {

          $status_text = !$o->status ? ' <span style="font-size: 0.675rem; padding: 0 2px; border: solid 1px #757575; background-color: #FDFCE1;">enabled</span>' : '';

          $actions = isset($o->audioClips[0]) ? l('Play', $o->audioClips[0]['url'], array('attributes' => array('class' => array('button'), 'target' => 'preview'))) : '<a href="javascript:void(0);" class="button">No Audio</a>';
          $actions .= l(!user_access('edit in the studio content') ? 'View' : 'Edit', $mpath . '/edit', array('attributes' => array('class' => array('button')), 'query' => array('id' => $o->id, 'destination' => $mpath)));
          $actions .= user_access('edit in the studio content') ? l('Delete', $mpath . '/delete', array('attributes' => array('class' => array('button')), 'query' => array('id' => $o->id, 'destination' => $mpath))) : '';
          // $mute_text = ($o->status == 0) ? 'Mute (Disable)' : 'Unmute (Enable)';
          // $actions .= l($mute_text, $mpath . '/mute', array('attributes' => array('class' => array('button')), 'query' => array('id' => $o->id, 'destination' => $mpath)));
          $actions .= '<input type="hidden" name="assets[' . $o->id . ']" value="' . $o->id . '" size="4" class="sortable-value">';

          $rows[] = array(
            '<strong>' . $o->title . '</strong>' . $status_text . '<br><small>' . $o->popUpText . '</small>',
            $actions
          );
        }
      }

      $this_tab_contents .= '';
      $this_tab_contents .= theme('table', array('empty' => 'No content', 'header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('sortable_miss'))));
      $this_tab_contents .= user_access('edit in the studio content') ? ('<div style="margin-top: 10px;">' . l('Add a New Record', $mpath . '/add', array('query' => array('category' => $cat), 'attributes' => array('class' => array('button')))) . '</div>') : '';
      $this_tab_contents .= '</div>';

      $tab_contents[] = $this_tab_contents;
    }

    $reorder_form = drupal_get_form('_miss_in_the_studio_reorder_form');

    // Output
    // TODO: Disabled on 2015-03-28 because the endpoint isn't updating.
    $output .= render($reorder_form);
    $output .= '<div id="studio-tabs">';
    $output .= "\n" . '<ul>' . implode("\n", $tabs) . '</ul>';
    $output .= "\n" . implode("\n", $tab_contents);
    $output .= "\n" . '</div>';

    #$output .= "\n\n" . '<pre>' . var_export($obj->ordered, TRUE) . '</pre>';
  }

  return $output;
}

// Hack to dump the page out
$output = _miss_in_the_studio_page_build();
