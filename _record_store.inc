<?php

// Test from Gor - 2016-09-16 - for Git

/**
 * @file
 * NMAAHC MISS Record Store functionality.
 *
 * API reference:
 * http://edandev.si.edu/applications/NMAAHC/#api-record_store
 */

require '_record_store.class.php';
require '_record_store__music_track.class.php';
require '_record_store__timeline_event.class.php';

use NMAAHC\MISS\RecordStore;
use NMAAHC\MISS\MusicTrack;
use NMAAHC\MISS\TimelineEvent;


/**
 * The Record Store menu path
 */
define('MISS_RECORD_STORE_MENU_PATH', 'miss/manage/recordStore');



/**
 * Route Record Store requests.
 *
 * @return
 *   A string of the rendered page text.
 */
function _miss_record_store_router() {
  $page = ' ';

  // Parse path for proper routing
  $args = array_slice(arg(), count(explode('/', MISS_RECORD_STORE_MENU_PATH)));

  // Adding an EDAN component
  if (count($args) == 2 && $args[1] == 'add') {
    $page = _miss_record_store_edit_component($args[0]);
  }
  // Editing an EDAN component
  else if (count($args) == 3 && $args[1] == 'edit') {
    $page = _miss_record_store_edit_component($args[0], $args[2]);
  }
  else if (count($args) == 3 && $args[1] == 'delete') {
    $page = _miss_record_store_delete_component($args[0], $args[2]);
  }
  else {
    // Fallback: table listing of music tracks and timeline events
    $page = _miss_record_store_content_overview();
  }

  return $page;
}


/**
 * Sets the title for a Record Store page.
 */
function _miss_record_set_title($title) {
  drupal_set_title('Record Store: ' . strip_tags($title));
}

/**
 * Page callback: EDAN Component edit form.
 */
function _miss_record_store_edit_component($type, $id = null) {
  $page = ' ';

  switch ($type) {
    case 'music':
      $obj = new MusicTrack();
      break;

    case 'timelineEvent':
      $obj = new TimelineEvent();
      break;

    case 'genre':
      $form = drupal_get_form('_miss_record_store_edit_genre_form', $id);
      $page .= render($form);
      return $page;
      break;

    default:
      watchdog('miss_record_store', 'Invalid Record Store component type :type.', array(':type', $type));
      return $page;
      break;
  }

  if ($id) {
    $component = $obj->getEdanComponent($id);

    if (!$component) {
      $page = sprintf('Could not load Record Store component "%s".', $id);
      return $page;
    }
  }
  else {
    $component = new stdClass();
  }

  _miss_record_set_title($obj->getDrupalFormTitle('edit', $component));
  $form = drupal_get_form('_miss_record_store_edit_component_form', $component, $obj);
  $page .= render($form);

  return $page;
}
/**
 * implements hook_form
 */
function _miss_record_store_edit_genre_form($form, &$form_state, $genre_id = NULL) {

  $genre_name = isset($genre_name) ? $genre_name : NULL;
  $description_to_use = NULL;

  if(isset($genre_id)) {
    $genre_list = _miss_record_store_get_genres_object();
    if (is_array($genre_list)) {
      foreach($genre_list as $genre_key => $genre_value) {
        if ($genre_id == $genre_key) {
          $genre_name = $genre_value['name'];
          $description_to_use = $genre_value['description'];
        }
      }
    }
  }

  $form['original_genre_name'] = array(
    '#type' => 'value',
    '#value' => $genre_name,
  );
  $form['name'] = array(
    '#title' => 'Name',
    '#type' => 'textfield',
    '#maxlength' => 524288,
    '#default_value' => $genre_name,
    '#disabled' => !user_access('edit miss content') ? TRUE : FALSE,
  );
  $form['new_description'] = array(
    '#title' => 'Description',
    '#type' => 'textarea',
    '#attributes' => array('maxlength' => 1000),
    '#default_value' => $description_to_use,
    '#disabled' => !user_access('edit miss content') ? TRUE : FALSE,
  );
  $add_or_edit = (empty($genre_name)) ? 'Add' : 'Edit';
  return user_access('edit miss content') ? confirm_form($form,
    t($add_or_edit.' Genre: %name', array('%name' => $genre_name)),
    MISS_RECORD_STORE_MENU_PATH,
    t(''),
    t('Save'),
    t('Cancel')
  ) : $form;
}

function _miss_record_store_edit_genre_form_validate($form, &$form_state) {

  $genre_names = array();
  $original_name = $form_state['values']['original_genre_name'];
  $new_name = $form_state['values']['name'];
  $genre_list = _miss_record_store_get_genres_object();

  if(is_array($genre_list)) {
    foreach($genre_list as $genre_key => $genre_value) {
      $genre_names[] = $genre_value['name'];
    }
  }
  else {
    form_set_error('','Could not obtain list of genres');
  }

  if(!empty($genre_names)) {
    if ($new_name !== $original_name) {
      if(in_array($new_name, $genre_names)){
        form_set_error('name','A genre of that name already exists.');
      }
    }
  }

}

function _miss_record_store_edit_genre_form_submit($form, &$form_state) {

  $genre_names = array();
  $original_name = $form_state['values']['original_genre_name'];
  $new_name = $form_state['values']['name'];
  $new_description = $form_state['values']['new_description'];
  $genre_list = _miss_record_store_get_genres_object();

  if(is_array($genre_list)){

    // If a new genre is being updated, assign the new values.
    foreach($genre_list as $genre_key => $genre_value) {
      $genre_names[] = $genre_value['name'];
      if($genre_value['name'] === $original_name) {
        $genre_list[$genre_key] = array('name' => $new_name, 'description' => $new_description);
      }
    }

    // If a new genre is being added, push to the $genre_list array.
    if(empty($original_name) && !in_array($new_name, $genre_names)) {
      array_push($genre_list, array('name' => $new_name, 'description' => $new_description));
    }

    $genre_list = array_values($genre_list);

    $uri = 'genres=' . urlencode(json_encode($genre_list));
    $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/record_store/setGenres.htm', $uri);
    drupal_set_message( sprintf('The "%s" genre has been saved.', $new_name ));
    $form_state['redirect'] = MISS_RECORD_STORE_MENU_PATH;
  }
}

/**
 * Implements hook_form().
 *
 * Generic function to delete a genre
 */
function _miss_record_store_delete_genre_confirm($form, &$form_state, $genre_id) {

  $genres = _miss_record_store_get_genres_object();

  $form['name'] = array(
    '#type' => 'value',
    '#value' => $genres[$genre_id]['name']
  );

  return confirm_form($form,
    t('Are you sure you want to delete the %name genre?', array('%name' => $genres[$genre_id]['name'])),
    MISS_RECORD_STORE_MENU_PATH,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );

  return $form;
}

/**
 *
 */
function _miss_record_store_delete_genre_confirm_submit($form, &$form_state) {

  $name = $form_state['values']['name'];
  $genre_list = _miss_record_store_get_genres_object();

  if(is_array($genre_list)){
    foreach ($genre_list as $key => $value) {

      if($value['name'] === $name) {

        unset($genre_list[$key]);

        $genre_list = array_values($genre_list);

        // global $user;

        $uri = 'genres=' . urlencode(json_encode($genre_list));
        $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/record_store/setGenres.htm', $uri);

        // if($user->name === 'ghalusa') {
        //   echo '<pre>';
        //   var_dump($uri);
        //   echo '</pre>';
        //   echo '<pre>';
        //   var_dump($set);
        //   echo '</pre>';
        //   die();
        // }

        drupal_set_message( sprintf('The "%s" genre has been successfully deleted.', $name ));
        $form_state['redirect'] = MISS_RECORD_STORE_MENU_PATH;

      }

    }
  }

}

/**
 * Page callback: EDAN Component delete form.
 */
function _miss_record_store_delete_component($type, $id = null) {
  $page = ' ';

  switch ($type) {
    case 'music':
      $obj = new MusicTrack();
      break;

    case 'timelineEvent':
      $obj = new TimelineEvent();
      break;

    case 'genre':
      $form = drupal_get_form('_miss_record_store_delete_genre_confirm', $id);
      $page .= render($form);
      return $page;
      break;

    default:
      watchdog('miss_record_store', 'Invalid Record Store component type :type.', array(':type', $type));
      return $page;
      break;
  }

  if ($id) {
    $component = $obj->getEdanComponent($id);

    if (!$component) {
      $page = sprintf('Could not load Record Store component "%s".', $id);
      return $page;
    }
  }
  else {
    $component = new stdClass();
  }

  $form = drupal_get_form('_miss_record_store_delete_component_confirm', $component, $obj);
  $page .= render($form);

  return $page;
}

/**
 * Page callback: Displays all the content used by the MISS Record Store.
 */
function _miss_record_store_content_overview() {

  if(!user_access('view record store content')){
    drupal_set_message('You do not have access to this page. Please contact an Administrator to resolve this issue.', 'error');
    drupal_goto('miss/list');
  }

  // Add DataTables CSS and Javascript.
  drupal_add_css(
    '//cdnjs.cloudflare.com/ajax/libs/datatables/1.10.11/css/jquery.dataTables.min.css',
    array('group' => CSS_THEME, 'type' => 'external', 'weight' => 50)
  );
  drupal_add_css(
    drupal_get_path('module', 'miss') . '/css/_record_store.css',
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

  $page = ' ';
  $rs = new RecordStore();

  // EDAN record dump
  if(isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
    $render = $rs->edanRequest('render');
    $dump = '<pre>' . var_export($render, TRUE) . '</pre>';
    drupal_set_message( $dump, 'status' );
  }

  _miss_record_set_title('Browse');

  $types = array(
    // Music Tracks
    (object) array(
      'title' => 'Music Tracks',
      'path' => 'music',
      'data' => $rs->getContentComponents('music'),
      'fields' => array(
        'artistName' => 'Artist',
        'songTitle' => 'Song Title',
        'genre' => 'Genre',
      ),
    ),
    // Timeline Events
    (object) array(
      'title' => 'Timeline Events',
      'path' => 'timelineEvent',
      'data' => $rs->getContentComponents('event'),
      'fields' => array(
        'year' => 'Year',
        'caption' => 'Caption',
      ),
    ),
  );
  $link_options = array(
    'attributes' => array(
      'class' => array(
        'button',
      ),
    ),
  );

  foreach ($types as $type) {
    $table = array(
      '#theme' => 'table',
      '#header' => array_values($type->fields),
      '#rows' => array(),
      '#sticky' => FALSE,
      '#attributes' => array('class' => array('miss_datatables display')),
    );
    $table['#header'][] = 'Actions';

    foreach ($type->data as $id => $value) {

      $row = array();
      foreach ($type->fields as $field => $name) {
        $row['data'][] = isset($value[$field]) && !empty($value[$field]) ? strip_tags($value[$field]) : '';
      }

      $buttons = l((!user_access('edit miss content') ? 'View' : 'Edit'), MISS_RECORD_STORE_MENU_PATH . '/' . $type->path . '/edit/' . $id, $link_options);
      // Expose the Delete button for timeline events.
      // if($type->path === 'timelineEvent') {
        $buttons .= '&nbsp;' . (user_access('edit miss content') ? l('Delete', MISS_RECORD_STORE_MENU_PATH . '/' . $type->path . '/delete/' . $id, $link_options) : '');
      // }
      $row['data'][] = $buttons;
      $table['#rows'][] = $row;
    }

    $page .= user_access('edit miss content') ? l('Add ' . substr($type->title, 0, strlen($type->title) - 1), MISS_RECORD_STORE_MENU_PATH . '/' . $type->path . '/add', array('attributes' => array('class' => array('button'), 'style' => 'float: right;'))) : '';
    $page .= '<h2>' . $type->title . '</h2>';
    $page .= drupal_render($table);
  }

  //Genres are a stored differently than the above info, genres are all in a single component and therefore do not have unique IDs to edit
  $table = array(
    '#theme' => 'table',
    '#header' => array('Name', 'Description', 'Actions'),
    '#rows' => array(),
    '#sticky' => FALSE,
    '#attributes' => array('class' => array('miss_datatables display')),
  );
  $genre_list = _miss_record_store_get_genres_object($rs);
  foreach($genre_list as $genre_key => $genre_value) {
    $row = array();//'data'=>array());
    $row['data'][] = $genre_value['name'];
    $row['data'][] = $genre_value['description'];
    $buttons = l((!user_access('edit miss content') ? 'View' : 'Edit'), MISS_RECORD_STORE_MENU_PATH . '/genre/edit/'.$genre_key, $link_options);
    $buttons .= user_access('edit miss content') ? l('Delete', MISS_RECORD_STORE_MENU_PATH . '/genre/delete/'.$genre_key, $link_options) : '';
    $row['data'][] = $buttons;
    $table['#rows'][] = $row;
  }
  $page .= user_access('edit miss content') ? l('Add Genre', MISS_RECORD_STORE_MENU_PATH . '/genre/add', array('attributes' => array('class' => array('button'), 'style' => 'float: right;'))) : '';
  $page .= '<h2>Genres</h2>';
  $page .= drupal_render($table);

  return $page;
}
/**
 * Implements hook_form().
 *
 * Generic function to edit EDAN components.
 */
function _miss_record_store_edit_component_form($form, &$form_state, $component, $obj) {

  $form['id'] = array(
    '#type' => 'value',
    '#value' => isset($component->id) ? $component->id : NULL,
  );

  $obj->setDrupalFormFields($form, $form_state, $component);

  $access_rights = user_access('edit miss content') ? 'true' : 'false';
  $inline_js = '
jQuery(document).ready(function(){
  if(' . $access_rights . '){
    jQuery("<a>",{
      text: "Add Another Related Genre",
      title: "Add Another Related Genre",
      id: "add-another-related-genre-link-button",
      href: "#",
      class: "button",
      style: "margin-left: 0.6rem;",
      click: function(event){
        event.preventDefault();
        if (jQuery("#edit-relatedgenre select").not(":visible").first().show().length == 0){
          jQuery("#add-another-related-genre-link-button").hide();
        }
        return false;
      }
    }).appendTo(jQuery("#edit-relatedgenre"));
  }
  else{
    jQuery("<a>",{
      text: "Add Another Related Genre",
      title: "Add Another Related Genre",
      id: "add-another-related-genre-link-button",
      href: "#",
      class: "button",
      style: "margin-left: 0.6rem;",
      click: function(event){
        return false;
      }
    }).appendTo(jQuery("#edit-relatedgenre"));
  }
});
';
  drupal_add_js($inline_js, 'inline');

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#disabled' => !user_access('edit miss content') ? TRUE : FALSE,
  );

  return $form;
}

/**
 * Form submission handler for _miss_record_store_edit_component_form().
 * TODO: Build out error checking
 */
function _miss_record_store_edit_component_form_submit($form, &$form_state) {
  $obj = $form_state['build_info']['args'][1];
  if ($obj->setComponent($form_state)) {
    $form_state['redirect'] = MISS_RECORD_STORE_MENU_PATH;
  }
}

/**
 * Implements hook_form().
 *
 * Generic function to delete EDAN components.
 */
function _miss_record_store_delete_component_confirm($form, &$form_state, $component, $obj) {

  $form['id'] = array(
    '#type' => 'value',
    '#value' => $component->id,
  );

  $form['status'] = array(
    '#type' => 'value',
    '#value' => -1,
  );

  return confirm_form($form,
    t('Are you sure you want to delete the %title ' . $component->content['subType'] . '?', array('%title' => strip_tags($obj->getComponentTitle($component)))),
    MISS_RECORD_STORE_MENU_PATH,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );

  return $form;
}

/**
 *
 */
function _miss_record_store_delete_component_confirm_submit($form, &$form_state) {
  $obj = $form_state['build_info']['args'][1];

  // TODO: Deleting currently returns a 404, but appears to work.
  if ($obj->setComponent($form_state)) {
    $form_state['redirect'] = MISS_RECORD_STORE_MENU_PATH;
  }
}

/**
 * Drupal submit function doesn't support object callbacks yet.
 */
function _miss_record_store_edit_component_form__record_store__add_influence($form, &$form_state) {
  if($form_state['num_influences'] < 4){
    $form_state['num_influences']++;
  }
  else{
    drupal_set_message('Each Music Track should contain no more than 4 unique influences!', 'error');
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Drupal submit function doesn't support object callbacks yet.
 */
function _miss_record_store_edit_component_form__record_store__remove_influence($form, &$form_state) {
  if ($form_state['num_influences'] > 1) {
    $form_state['num_influences']--;
  }
  $form_state['rebuild'] = TRUE;
}
/**
 * Helper function to get the list of genres
 *
 * @param $rs
 *   A record store object to use, optional if NULL then creates a new RecordStore object
 * @returns
 *   The object list from edan, expected to be an array with index name and value description. e.g.
 *     {
 *       "Blues": "default description",
 *       "Country": "default description",
 *       ...
 *     }
 */
function _miss_record_store_get_genres_object($rs = NULL) {
  //Calling for the records twice in the same REQUEST would give second time as NULL, so implemented this rudimentary caching
  global $_miss_record_store_genres_object;
  if ($rs === NULL) {
    $rs = new RecordStore();
  }
  if (empty($_miss_record_store_genres_object)) {
    $interactive = $rs->getInteractive();
    $_miss_record_store_genres_object = $interactive['content']['genres'];
  }
  // // Remove the '- None -' entry.
  // array_shift($_miss_record_store_genres_object);

  foreach ($_miss_record_store_genres_object as $key => $value) {
    if($value['name'] === '- None -') {
      unset($_miss_record_store_genres_object[$key]);
    }
  }

  return $_miss_record_store_genres_object;
}

/**
 * Helper function originally for the genre listing that will preserve array order but rename a specific key
 * since doing
 * $array[$new_key] = $array[$old_key]; unset($array[$old_key]);
 * will put the new key at the end of the order
 * Assumes that new_key does not exist in the array
 *
 * @param $array
 *   Array to replace key
 * @param $old_key
 *   Key for the array that should not be in new array
 * @param $new_key
 *   Replace old_key with new_key
 * @returns
 *   new array with the key replaced
 */
function _miss_record_store_change_array_key($array, $old_key, $new_key) {
  if( ! array_key_exists( $old_key, $array ) ){
    return $array;
  }
  $keys = array_keys( $array );
  $keys[ array_search( $old_key, $keys ) ] = $new_key;

  return array_combine( $keys, $array );
}

$output = _miss_record_store_router();
