<?php
/**
 * Implements hook_form().
 */
function miss_data_backups_form( $form, &$form_state, $record = array() ) {

  $form = array();

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Field-specific notes go here'),
    '#size' => 65,
    '#maxlength' => 524288, 
    '#required' => TRUE,
    '#default_value' => !empty($record['SOMETHING']) ? $record['SOMETHING'] : '',
    );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    );

  return $form;
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function miss_form_data_backups_form_alter(&$form, &$form_state, $form_id) {

  // // CSS
  // drupal_add_css(
  //   drupal_get_path('module', 'miss') . '/css/_visual_arts_tour.css',
  //   array('group' => CSS_THEME, 'type' => 'file', 'weight' => 2)
  // );

  // // Javascript
  // drupal_add_js(
  //   '//cdnjs.cloudflare.com/ajax/libs/underscore.js/1.8.3/underscore-min.js',
  //   array('scope' => 'footer', 'type' => 'external', 'weight' => 51)
  // );
  // drupal_add_js(
  //   drupal_get_path('module', 'miss') . '/js/_visual_arts_tour.js',
  //   array('scope' => 'footer', 'weight' => 52)
  // );
  // drupal_add_js( array('ids_url' => variable_get('ids_url')), 'setting' );

  // // jQueryUI
  // drupal_add_library('system','ui.droppable');
  // drupal_add_library('system','ui.sortable');
  // drupal_add_library('system','ui.dialog');
  // drupal_add_library('system','effects.slide');

  return $form;
}

/**
 * Implements hook_form_validate().
 */
// function miss_data_backups_form_validate($form, &$form_state) {
// }

/**
 * Implements hook_form_submit().
 */
function miss_data_backups_form_submit($form, &$form_state) {

  // $params = array();

  // // setGalleryDescription
  // $base_params = array(
  //   'caption' => $form_state['input']['title'],
  //   'text' => $form_state['input']['description']['value'],
  //   'credit' => $form_state['input']['primary_image_credit'],
  //   'imageUrl' => $primary_image,
  //   'linkedEdanId' => $primary_image_edan_record_id,
  // );

  // $params = array_merge($params, $base_params);
  // $uri = drupal_http_build_query($params);

  // // Send the request.
  // $set_record_result = _miss_send_miss_request( variable_get('visualartsSetGalleryDescription'), $uri );

  // if(isset($set_record_result['id'])) {
  //   drupal_set_message( t('"' . $form_state['input']['title'] . '" has been successfully saved.') );
  // }

  drupal_goto(current_path());
}

/**
 * MISS EDAN routes.
 */
function _miss_data_backups_edan_routes() {
  return array(
    'visual_arts' => 'Visual Arts Tour',
    'in_the_studio' => 'In The Studio',
    'reflections_booth' => 'Reflections Booth',
    'the_hub' => 'The Hub',
    'record_store' => 'Record Store',
    'the_arc' => 'The Arc',
    'lunch_counter' => 'Lunch Counter',
    );
}

/**
 * Reads the filesystem and retrieves all of the backups.
 */
function _miss_data_backups_get_backups() {

  $data = array();
  $backup_files_path = DRUPAL_ROOT . '/' . variable_get('file_public_path', 'sites/default/files') . '/miss_backups';

  // Create the directory for backups if it isn't already present.
  if(!file_exists($backup_files_path)) {
    mkdir($backup_files_path, 0755, true);
  }

  $data = scandir($backup_files_path);
  unset($data[0], $data[1]);
  $data = array_values($data);

  // dumper($data);

  return $data;
}

function miss_data_backups_restore_form( $form, &$form_state) {

  $form = array();

  $form['json'] = array(
   '#type' => 'textarea',
   '#title' => t('Backup Json to Restore'),
   '#required' => true,
   );

  $form['explanation_text'] = array(
   '#type' => 'item',
   '#markup' => '<p>Input should be in the form:</p>
   <b>{<br>
    &emsp;"components" : {<br>
    &emsp;&emsp;"component-id" : {},<br>
    &emsp;&emsp;"component-id" : {},<br>
    &emsp;&emsp;"component-id" : {},<br>
    &emsp;&emsp;"component-id" : {},<br>
    &emsp;&emsp;"component-id" : {}<br>
    &emsp;}<br>
    }</b>',
   );

  $form['submit'] = array(
    '#type' => 'submit',
    '#default_value' => 'Submit',
    );

  return $form;
}

function miss_data_backups_restore_form_submit($form, &$form_state){
  $json = json_decode($form_state['input']['json'], TRUE);

  if($json == null){
    drupal_set_message('The input could not be interpreted', 'error');
    drupal_goto('miss/backup/browse');
  }

  if(!isset($json['components'])){
    $json = array(
      'components' => $json,
      );
  }

  $successful_total = 0;
  $total = 0;
  foreach ($json['components'] as $record) {
    if(isset($record['content']['subType'])){
      switch ($record['content']['subType']) {

        case 'gallery_tag':
          $attract_media_loop = $record['content']['attractMediaLoop'];
          $uri = 'tag=' . urlencode($record['content']['galleryTag']);
          $uri .= '&galleryImageUrl=' . urlencode($record['content']['galleryImageUrl']);
          $uri .= '&galleryImageCredit=' . urlencode($record['content']['galleryImageCredit']);
          $uri .= '&promptQuestions=' . json_encode($record['content']['promptQuestions']);
          $uri .= '&id=' . $record['id'];
          $set = _miss_send_miss_request( variable_get('reflectionsSetGalleryTag'), $uri );
          $uri = 'galleryId=' . $record['id'] . '&attractMediaLoop=' . json_encode($attract_media_loop);
          $set = _miss_send_miss_request( variable_get('reflectionsSetAttractMediaLoop'), $uri );
        break;

        case 'attract_video':
          $uri = 'content=' . urlencode(json_encode($record['content'])) . '&id=' . $record['id'];
          $set = _miss_send_miss_request( variable_get('reflectionsSetAttractVideo'), $uri );
        break;

        case 'prompt_question':
          $uri = 'text=' . urlencode(json_encode($record['content']['text'])) . '&id=' . $record['id'];
          $set = _miss_send_miss_request( variable_get('reflectionsSetPromptQuestion'), $uri);
        break;

        case 'art_profile':
          $uri = 'content=' . urlencode(json_encode($record['content'])) . '&id=' . $record['id'];
          $set_art_profile_result = _miss_send_miss_request( variable_get('visualartsSetArtProfile'), $uri );
        break;

        case 'gallery_description':
          $uri = 'content=' . urlencode(json_encode($record['content'])) . '&id=' . $record['id'];
          $set_record_result = _miss_send_miss_request( variable_get('visualartsSetGalleryDescription'), $uri );
        break;

        case 'story':
          $uri = 'content=' . urlencode(json_encode($record['content'])) . '&id=' . $record['id'];
          $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_hub/setStory.htm', $uri );
        break;

        case 'artifact':
          $uri = 'content=' . urlencode(json_encode($record['content'])) . '&id=' . $record['id'];
          $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/setArtifact.htm', $uri );
        break;

        case 'music':
        break;

        case 'influence_image':
        break;

        case 'event':
        break;

        case 'sound_asset':
          $uri = 'content=' . urlencode(json_encode($record['content'])) . '&id=' . $record['id'];
          $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/in_the_studio/setSoundAsset.htm', $uri );
        break;
        
        default:
        drupal_set_message('Record ' . $record['id'] . ' item could not be identified as a subtype', 'error');
        break;
      }
      if(isset($set['id'])){
        $successful_total++;
        $total++;
      }else{
        $total++;
      }
      dumper($set);
    }
  }
  drupal_set_message( $successful_total . ' out of ' . $total . ' records successfully updated');
  drupal_goto('miss/backup/browse');
}

/**
 * Builds the Browse and Manage pages.
 */
function _miss_data_backups_page_build() {

  // Variables
  $data = array();
  $current_path = current_path();
  $path_parts = arg();
  $types = _miss_types();
  $title = 'MISS Backups';
  $out = '';

  switch($path_parts[2]) {

    // BROWSE PAGE
    case 'browse':

    drupal_set_title($title . ': Browse');

     // Delete a record confirmation.
     $delete_js = 'jQuery(document).ready(function($) {
              $(".delete-link").on("click", function(){
                var r = confirm("Are you sure you want to delete this backup? This action cannot be undone.");
                if (r == true) {
                    window.location.href = $(this).attr("data-link");
                }
              });
    });';
    drupal_add_js($delete_js, 'inline');

    $backup_files_path = variable_get('file_public_path', 'sites/default/files') . '/miss_backups/';
    $edan_routes_array = _miss_data_backups_edan_routes();
    foreach($edan_routes_array as $rkey => $rvalue) {
      $routes[] = $rkey;
    }

    $out .= '<p><a href="/miss/backup/create">Create a New Set of Backups</a></p>' . "\n";
    $out .= '<p><a href="/miss/backup/restore">Restore Data from a Backup</a></p>' . "\n";

      // Get the all of the data backups by reading the filesystem for JSON files.
    $data = _miss_data_backups_get_backups();

    if(!empty($data)) {

        // EDAN record dump
      if(!empty($data) && isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
        $dump = '<pre>' . var_export($data, TRUE) . '</pre>';
        drupal_set_message( $dump, 'status' );
      }

        // Add DataTables CSS and Javascript.
      drupal_add_css(
        '//cdnjs.cloudflare.com/ajax/libs/datatables/1.10.11/css/jquery.dataTables.min.css',
        array('group' => CSS_THEME, 'type' => 'external', 'weight' => 50)
        );
      drupal_add_js(
        '//cdnjs.cloudflare.com/ajax/libs/datatables/1.10.11/js/jquery.dataTables.min.js', 
        array('scope' => 'footer', 'type' => 'external', 'weight' => 50)
        );
      drupal_add_js(
        drupal_get_path('module', 'miss') . '/js/datatables_directives.js',
        array('scope' => 'footer', 'weight' => 51)
        );

      $out .= '<hr style="height: 0; margin-top: 20px; margin-bottom: 20px; border: 0; border-top: 1px solid #bbbbbb; box-sizing: content-box;">' . "\n";
      $out .= '<p><strong>Tip:</strong> EDAN routes are searchable, as are the dates (e.g. 2016-08-23).</p>';
      $out .= '<p><strong>Possible EDAN routes:</strong> ' . implode(', ', $routes) . '</p>';

        // Table top
      $out .= '<table id="browse_data_backups" class="display" cellspacing="0" width="100%">' . "\n";
      $out .= '<thead>' . "\n";
      $out .= '<tr>' . "\n";
      $out .= '<th width="75%">Back-ups</th>' . "\n";
      $out .= '<th width="150" class="no-sort" style="text-align: center;">Actions</th>' . "\n";
      $out .= '</tr>' . "\n";
      $out .= '</thead>' . "\n";
      $out .= '<tbody>' . "\n";

        // Loop through all of the records to build the table rows.
      foreach($data as $key => $value) {

        $value_parts = explode('_', $value);
        $date_time = $value_parts[0] . ' at ' . str_replace('-', ':', $value_parts[1]);

        unset($value_parts[0], $value_parts[1]);
        array_values($value_parts);

        $filename = implode('_', $value_parts);

        $out .= '<tr>' . "\n";
        $out .= '<td><strong>' . $date_time . ' - ' . $filename . '</strong></td>' . "\n";
        $out .= '<td style="text-align: center;">' . "\n";
        $out .= '<a href="/' . $backup_files_path . $value . '"">View</a> | ' . "\n";
        $out .= '<a href="delete/' . $value . '" style="margin-right: 16px;" class="delete-link">Delete</a> ' . "\n";
        $out .= '</td>' . "\n";
        $out .= '</tr>' . "\n";
      }

        // Table bottom
      $out .= '</tbody>' . "\n";
      $out .= '</table>' . "\n";
    } else {
      drupal_set_message( t('No backups found.'), 'error' );
    }

    return $out;
    break;

    // VIEW BACKUPS PAGE
    case 'view':

    drupal_set_title($title . ': ' . ucwords($path_parts[2]) . ' Backups');

    $out = '';
    $out .= '<p>' . drupal_get_title() . '...</p>' . "\n";

    return $out;
    break;

    // CREATE BACKUPS PAGE
    case 'create':

    drupal_set_title($title . ': ' . ucwords($path_parts[2]) . ' Backups');

    $messages = '';
    $result = _miss_data_backups_create_backups();

    if(!isset($result['errors'])) {
      drupal_set_message( t('Backups have been successfully generated.') );
    } else {
      foreach ($result['errors'] as $key => $value) {
        $messages .= t('<p>' . $key . ': ' . $value . '</p>') . "\n";
      }
      drupal_set_message( $messages, 'error' );
    }

    drupal_goto('miss/backup/browse');

    break;

    // RESTORE FROM BACKUP PAGE
    case 'restore':

      drupal_set_title($title . ': ' . ucwords($path_parts[2]) . ' From Backups');

        // Get the form.
      $form = drupal_get_form('miss_data_backups_restore_form');
      $out = drupal_render($form);


    return $out;
    break;

    case 'delete':

      if(isset($path_parts[3])){
        $file = variable_get('file_public_path', 'sites/default/files') . '/miss_backups/' . $path_parts[3];
        if(is_file($file)){
          unlink($file); // delete file
          drupal_set_message(t('The file was successfully deleted.'));
        }
      } else{
        drupal_set_message(t('The requested file could not be found.'), 'error');
        drupal_goto('miss/backup/browse');
      }
      drupal_goto('miss/backup/browse');

    break;

    // Send a 404.
    default:
    return MENU_NOT_FOUND;
  }


}
