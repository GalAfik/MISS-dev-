<?php
/**
 * Implements hook_form().
 */
function miss_visual_arts_tour_form( $form, &$form_state, $record = array() ) {

  $form = array();

  //hide upload/insert buttons for non editors
  $button_script = '
  jQuery(document).ready(function() {
     jQuery("a:contains(' . "'Upload / Insert'" . ')").hide();
  });
  ';
  if(!user_access('upload files')){
    drupal_add_js($button_script, 'inline');
  }

  $form['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('<b>Enabled</b>'),
    '#default_value' => !empty($record) ? ($record['status'] == 1 ? '0' : '1') : '0',
    '#return_value' => '1',
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Gallery Title'),
    // '#description' => t('Field-specific notes go here'),
    '#size' => 65,
    '#maxlength' => 524288,
    '#required' => TRUE,
    '#default_value' => !empty($record['caption']) ? $record['caption'] : '',
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['description'] = array(
    '#type' => 'text_format',
    '#title' => t('Gallery Description'),
    // '#description' => t('Field-specific notes go here'),
    '#rows' => 12,
    '#cols' => 10,
    '#required' => TRUE,
    '#resizable' => TRUE,
    '#format' => 'full_html',
    '#default_value' => !empty($record['text']) ? $record['text'] : '',
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['primary_image_credit'] = array(
    '#type' => 'hidden',
    '#default_value' => !empty($record['credit']) ? $record['credit'] : '',
    '#attributes' => array('id' => 'primary_image_credit'),
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['secondary_images'] = array(
    '#type' => 'textarea',
    '#title' => t('Secondary Images'),
    '#required' => FALSE,
    '#attributes' => array('maxlength' => 10000),
    '#default_value' => !empty($record['art_profiles']) ? json_encode($record['art_profiles']) : '',
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['file_upload'] = array(
    '#type' => 'hidden',
    '#size' => 65,
    '#maxlength' => 524288,
    '#attributes' => array(
      'class' => array(
        'miss-file-manage',
      ),
      'id' => 'file_upload',
    ),
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['preview'] = array(
    '#type' => 'button',
    '#attributes' => array('onclick' => 'event.preventDefault();'),
    '#value' => t('Preview'),
    '#prefix' => t('<div id="preview_gallery">'),
    '#suffix' => t('</div>'),
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['edan_id'] = array(
    '#type' => 'hidden',
    '#default_value' => !empty($record['id']) ? $record['id'] : '',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  return $form;
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function miss_form_miss_visual_arts_tour_form_alter(&$form, &$form_state, $form_id) {

  // CSS
  drupal_add_css(
    drupal_get_path('module', 'miss') . '/css/_visual_arts_tour.css',
    array('group' => CSS_THEME, 'type' => 'file', 'weight' => 2)
  );

  // Javascript
  drupal_add_js(
    '//cdnjs.cloudflare.com/ajax/libs/underscore.js/1.8.3/underscore-min.js',
    array('scope' => 'footer', 'type' => 'external', 'weight' => 51)
  );
  drupal_add_js(
    drupal_get_path('module', 'miss') . '/js/_visual_arts_tour.js',
    array('scope' => 'footer', 'weight' => 52)
  );
  drupal_add_js( array('ids_url' => variable_get('ids_url')), 'setting' );

  // jQueryUI
  drupal_add_library('system','ui.droppable');
  drupal_add_library('system','ui.sortable');
  drupal_add_library('system','ui.dialog');
  drupal_add_library('system','effects.slide');

  return $form;
}

/**
 * Implements hook_form_validate().
 */
// function miss_visual_arts_tour_form_validate($form, &$form_state) {
// }

/**
 * Implements hook_form_submit().
 */
function miss_visual_arts_tour_form_submit($form, &$form_state) {

  // TODO: Validation

  $params = array();
  $child_content = array();
  $secondary_images = json_decode($form_state['input']['secondary_images'], true);
  // Just in case... re-index the array.
  $secondary_images = array_values($secondary_images);

  $primary_image_array = $secondary_images[0];

  $primary_image = is_array($primary_image_array) ? $primary_image_array['artworkImage'] : '';
  $primary_image_edan_record_id = (is_array($primary_image_array) && isset($primary_image_array['linkedEdanId'])) ? $primary_image_array['linkedEdanId'] : '';

  if(!empty($form_state['input']['edan_id'])) {
    $params['id'] = $form_state['input']['edan_id'];
  }

  // set status based on form 'enabled' field
  $status = $form_state['values']['enabled'] == 0 ? 1 : 0;

  // setGalleryDescription
  $base_params = array(
    'caption' => $form_state['input']['title'],
    'text' => $form_state['input']['description']['value'],
    'credit' => $form_state['input']['primary_image_credit'],
    'imageUrl' => $primary_image,
    'linkedEdanId' => $primary_image_edan_record_id,
  );

  $params = array_merge($params, $base_params);
  $uri = drupal_http_build_query($params);

  // Send the request.
  $set_record_result = _miss_send_miss_request( variable_get('visualartsSetGalleryDescription'), $uri . '&status=' . $status);

  if(isset($set_record_result['id'])) {

    $params = array();
    $art_profile_ids_array = array();
    // Get all of the components.
    $get_components = _miss_send_miss_request( variable_get('visualartsGetComponents') );

    if(!empty($secondary_images)) {
      foreach($secondary_images as $key => $value) {

        $uri = '';
        $id_param = '';

        // Determine if there is an ID, indicating it's an existing component.
        $id = (isset($value['id']) && !empty($value['id'])) ? $value['id'] : '';

        if(!empty($id)) {

          // Get the unique title of the component.
          if(array_key_exists($id, $get_components['components'])) {

            // Set the unique title.
            $title = $get_components['components'][$id]['title'];

            // Get the component's child blocks
            $child_blocks_array = _visual_arts_tour_components();
            foreach($child_blocks_array as $child_key => $child_data) {
              $child_names[] = $child_data['edan_key'];
            }

            $child_content = array();

            // Loop through, and add any existing child block to the $child_content array,
            // so it can be merged back into the main $content array (otherwise, it will get lost).
            foreach($get_components['components'][$id]['content'] as $content_key => $content_value) {
              if(in_array($content_key, $child_names)) {
                $child_content[$content_key] = $get_components['components'][$id]['content'][$content_key];
              }
            }

          }

          // Set the id parameter for the uri.
          $id_param = '&id=' . $id;

        } else {
          // If the component is new, append a random integer to the title to ensure it's unique.
          $title = isset($value['artworkTitle']) ? $value['artworkTitle'] . ' - ' . mt_rand( 0, 0xffff ) : 'Image'  . ' - ' .  mt_rand( 0, 0xffff );
        }
        
        $params = array(
          'title' => $title,
        );

        $content = array(
          'artworkImage' => !empty($value['artworkImage']) ? str_replace('&max_w=110&max_h=160', '', $value['artworkImage']) : '',
          'artworkTitle' => !empty($value['artworkTitle']) ? $value['artworkTitle'] : '',
          'artworkDate' => !empty($value['artworkDate']) ? $value['artworkDate'] : '',
          'artworkMaterial' => !empty($value['artworkMaterial']) ? $value['artworkMaterial'] : '',
          'artworkContributor' => !empty($value['artworkContributor']) ? $value['artworkContributor'] : '',
          'firstName' => !empty($value['firstName']) ? $value['firstName'] : '',
          'lastName' => !empty($value['lastName']) ? $value['lastName'] : '',
          'linkedEdanId' => !empty($value['linkedEdanId']) ? $value['linkedEdanId'] : '',
        );

        // Add in any existing child block content
        if(!empty($id) && !empty($child_content)) {
          $content = array_merge($content, $child_content);
        }

        $uri = drupal_http_build_query($params);
        $uri = $uri . '&content=' . urlencode(json_encode($content)) . $id_param;

        // Set the Art Profile (setArtProfile).
        $set_art_profile_result = _miss_send_miss_request( variable_get('visualartsSetArtProfile'), $uri );

        // Gather all of the Art Profile ids.
        if(isset($set_art_profile_result['id'])) {
          array_push($art_profile_ids_array, $set_art_profile_result['id']);
        }

      }
    }

    if(!empty($art_profile_ids_array)) {
      // Add the Art Profile to the gallery (setArtProfiles).
      $uri_art_profiles = 'galleryId=' . $form_state['input']['edan_id'] . '&artProfiles=' . json_encode($art_profile_ids_array);
      $set_art_profiles_result = _miss_send_miss_request( variable_get('visualartsSetArtProfiles'), $uri_art_profiles );
      // Add the image ids to the gallery's Attact Image Loop (setAttactImageLoop).
      $uri_attract_loop = 'galleryId=' . $form_state['input']['edan_id'] . '&attractImageLoop=' . json_encode($art_profile_ids_array) . '&status=' . $status;
      $set_art_attract_image_loop = _miss_send_miss_request( variable_get('visualartsSetAttractImageLoop'), $uri_attract_loop ); 
    }

    // TODO: Error handling
    drupal_set_message( t('"' . $form_state['input']['title'] . '" has been successfully saved.') );
  }

  drupal_goto(current_path());
}

/**
 * Visual Arts Tour Components.
 */
function _visual_arts_tour_components() {
  return array(
    'imageMetadata' => array('title' => 'Image Metadata', 'edan_key' => false),
    'aboutTheArtist' => array('title' => 'About the Artist', 'edan_key' => 'makerDetails'),
    'aboutTheArtwork' => array('title' => 'About the Artwork', 'edan_key' => 'artworkDetails'),
    'artisticContext' => array('title' => 'Artistic Context', 'edan_key' => 'artisticContext'),
    'historicContext' => array('title' => 'Historic Context', 'edan_key' => 'historicContext'),
    'locationContext' => array('title' => 'Related Galleries &#38; Museums', 'edan_key' => 'locationContext'),
  );
}

/**
 * Implements hook_form().
 */
function miss_visual_arts_components_form( $form, &$form_state, $page = false ) {

  // EDAN key (e.g makerDetails, artworkDetails, artisticContext, historicContext, locationContext)
  $edan_key = $page['component']['edan_key'];

  $form = array();

  $form['text'] = array(
    '#type' => 'text_format',
    '#title' => t('Text'),
    '#rows' => 12,
    '#cols' => 10,
    '#required' => ($edan_key !== 'locationContext') ? TRUE : FALSE,
    '#resizable' => TRUE,
    '#format' => 'full_html',
    '#default_value' => !empty($page['record'][$edan_key]['text']) ? $page['record'][$edan_key]['text'] : '',
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['secondary_images'] = array(
    '#type' => 'textarea',
    '#title' => t('Images'),
    '#required' => FALSE,
    '#attributes' => array(
      'maxlength' => 10000,
      'class' => array($edan_key),
    ),
    '#default_value' => !empty($page['record'][$edan_key]['images']) ? json_encode($page['record'][$edan_key]['images']) : '',
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['file_upload'] = array(
    '#type' => 'hidden',
    '#size' => 65,
    '#maxlength' => 524288,
    '#attributes' => array(
      'class' => array(
        'miss-file-manage',
      ),
      'id' => 'file_upload',
    ),
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['#submit'][] = 'miss_visual_arts_components_form_submit';

  return $form;
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function miss_form_miss_visual_arts_components_form_alter(&$form, &$form_state, $form_id) {

  // CSS
  drupal_add_css(
    drupal_get_path('module', 'miss') . '/css/_visual_arts_tour.css',
    array('group' => CSS_THEME, 'type' => 'file', 'weight' => 2)
  );

  // Javascript
  drupal_add_js(
    '//cdnjs.cloudflare.com/ajax/libs/underscore.js/1.8.3/underscore-min.js',
    array('scope' => 'footer', 'type' => 'external', 'weight' => 2)
  );
  drupal_add_js(
    drupal_get_path('module', 'miss') . '/js/_visual_arts_tour.js',
    array('scope' => 'footer', 'weight' => 3)
  );
  drupal_add_js( array('ids_url' => variable_get('ids_url')), 'setting' );

  // jQueryUI
  drupal_add_library('system','ui.droppable');
  drupal_add_library('system','ui.sortable');
  drupal_add_library('system','ui.dialog');
  drupal_add_library('system','effects.slide');
  drupal_add_library('system', 'ui.tabs');

  return $form;
}

/**
 * Implements hook_form_submit().
 */
function miss_visual_arts_components_form_submit($form, &$form_state) {

  // TODO: Validation

  $get_key = filter_xss($_GET['key']);

  $params = array();
  $art_profile_ids_array = array();
  $parent_id = $form_state['build_info']['args'][0]['record']['id'];
  $component_title = $form_state['build_info']['args'][0]['component']['title'];
  $edan_key = $form_state['build_info']['args'][0]['component']['edan_key'];
  $content = $form_state['build_info']['args'][0]['record'];
  $id = (isset($content['id']) && !empty($content['id'])) ? '&id=' . $content['id'] : '';
  $secondary_images = json_decode($form_state['input']['secondary_images'], true);

  // Get the unique title of a component.
  $title = _miss_visual_arts_tour_get_component_title($content['id']);
  // Unset the id, so it's not mixed in with the content being sent back.
  unset($content['id']);

  $params = array(
    'title' => $title,
    'status' => 0, // 0 = published, 1 = not published, -1 = marked for deletion
  );

  if(!empty($secondary_images)) {
    foreach($secondary_images as $key => $value) {
      $images[] = array(
        'caption' => !empty($value['caption']) ? $value['caption'] : '',
        'imageUrl' => !empty($value['imageUrl']) ? str_replace('&max_w=110&max_h=160', '', $value['imageUrl']) : '',
        'credit' => !empty($value['credit']) ? $value['credit'] : '',
      );
    }
  }

  $content[$edan_key] =  array(
    'text' => $form_state['input']['text']['value'],
    'images' => !empty($images) ? $images : array(),
  );

  $new_content = urlencode(json_encode($content));

  $uri_params = drupal_http_build_query($params);
  $uri = $uri_params . '&content=' . $new_content . $id;

  // Set the Art Profile (setArtProfile).
  $set_art_profile_result = _miss_send_miss_request( variable_get('visualartsSetArtProfile'), $uri );

  if(!empty($set_art_profile_result['id'])) {
    drupal_set_message( t('The ' . $component_title . ' component for "' . strip_tags($form_state['build_info']['args'][0]['record']['artworkTitle']) . '" has been successfully saved.'), 'status' );
  } else {
    drupal_set_message( t('Could not save the ' . $component_title . ' component for "' . strip_tags($form_state['build_info']['args'][0]['record']['artworkTitle']) . '". Please check, and try again.'), 'error' );
  }

  drupal_goto('/' . current_path() , array('query' => array('key' => $get_key)));

}

/**
 * Implements hook_form().
 */
function miss_visual_arts_image_metadata_form( $form, &$form_state, $page = false ) {

  $record = array();
  $parent_record = array();
  $get_key = filter_xss($_GET['key']);
  // EDAN key (e.g makerDetails, artworkDetails, artisticContext, historicContext, locationContext)
  $edan_key = $page['component']['edan_key'];

  if(!empty($page['record']['art_profiles'][$get_key])) {
    $record = $page['record']['art_profiles'][$get_key];
  }

  $form = array();

  $result = _miss_send_miss_request( variable_get('visualartsGetComponents') );
  if(array_key_exists($record['id'], $result['components'])) {
    $record_status = $result['components'][$record['id']]['status'];
  }

  // dumper($result['components'][$record['id']]);

  $form['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('<b>Enabled</b>'),
    '#default_value' => !empty($record) ? ($record_status == 1 ? '0' : '1') : '0',
    '#return_value' => '1',
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['artworkTitle'] = array(
    '#type' => 'text_format',
    '#title' => t('Artwork Title'),
    '#rows' => 4,
    '#cols' => 6,
    '#required' => TRUE,
    '#resizable' => TRUE,
    '#format' => 'full_html',
    '#default_value' => !empty($record['artworkTitle']) ? $record['artworkTitle'] : '',
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['firstName'] = array(
    '#type' => 'textfield',
    '#title' => t('Artist\'s First Name'),
    '#size' => 65,
    '#maxlength' => 524288,
    '#required' => TRUE,
    '#default_value' => !empty($record['firstName']) ? $record['firstName'] : '',
    '#prefix' => '<div style="margin-top: 1rem;"></div>' . "\n",
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['lastName'] = array(
    '#type' => 'textfield',
    '#title' => t('Artist\'s Last Name'),
    '#size' => 65,
    '#maxlength' => 524288,
    '#required' => FALSE,
    '#default_value' => !empty($record['lastName']) ? $record['lastName'] : '',
    '#prefix' => '<div style="margin-top: 1rem;"></div>' . "\n",
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['artworkContributor'] = array(
    '#type' => 'text_format',
    '#title' => t('Credit/Contributor'),
    '#rows' => 4,
    '#cols' => 10,
    '#required' => FALSE,
    '#resizable' => TRUE,
    '#format' => 'full_html',
    '#default_value' => !empty($record['artworkContributor']) ? $record['artworkContributor'] : '',
    '#prefix' => '<div style="margin-top: 1rem;"></div>' . "\n",
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['artworkMaterial'] = array(
    '#type' => 'text_format',
    '#title' => t('Artwork Material'),
    '#rows' => 4,
    '#cols' => 10,
    '#required' => TRUE,
    '#resizable' => TRUE,
    '#format' => 'full_html',
    '#default_value' => !empty($record['artworkMaterial']) ? $record['artworkMaterial'] : '',
    '#prefix' => '<div style="margin-top: 1rem;"></div>' . "\n",
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['artworkDate'] = array(
    '#type' => 'textfield',
    '#title' => t('Artwork Date'),
    // '#description' => t('Field-specific notes go here'),
    '#size' => 35,
    '#maxlength' => 524288,
    '#required' => TRUE,
    '#default_value' => !empty($record['artworkDate']) ? $record['artworkDate'] : '',
    '#prefix' => '<div style="margin-top: 1rem;"></div>' . "\n",
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['#submit'][] = 'miss_visual_arts_image_metadata_form_submit';

  return $form;
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function miss_form_miss_visual_arts_image_metadata_form_alter(&$form, &$form_state, $form_id) {
  _miss_visual_arts_tour_image_metadata_css_js();
  return $form;
}

/**
 * Implements hook_form_submit().
 */
function miss_visual_arts_image_metadata_form_submit($form, &$form_state) {

  // TODO: Validation

  $get_key = filter_xss($_GET['key']);

  $params = array();
  $art_profile_ids_array = array();
  $parent_id = $form_state['build_info']['args'][0]['record']['id'];
  $component_title = $form_state['build_info']['args'][0]['component']['title'];
  $edan_key = $form_state['build_info']['args'][0]['component']['edan_key'];
  $content = $form_state['build_info']['args'][0]['record']['art_profiles'][$get_key];
  $id = (isset($content['id']) && !empty($content['id'])) ? '&id=' . $content['id'] : '';
  $status = $form_state['values']['enabled'] == 0 ? 1 : 0;

  unset($content['id']);
  unset($content['enabled']);

  $content['artworkTitle'] = $form_state['input']['artworkTitle']['value'];
  $content['firstName'] = $form_state['input']['firstName'];
  $content['lastName'] = $form_state['input']['lastName'];
  $content['artworkDate'] = $form_state['input']['artworkDate'];
  $content['artworkMaterial'] = $form_state['input']['artworkMaterial']['value'];
  $content['artworkContributor'] = $form_state['input']['artworkContributor']['value'];

  $params = array(
    // 'title' => $content['artworkTitle'] . ' - ' . mt_rand( 0, 0xffff ),
    'title' => !empty($id) ? $content['artworkTitle'] : $content['artworkTitle'] . ' - ' . mt_rand( 0, 0xffff ),
    'status' => $status, // 0 = published, 1 = not published, -1 = marked for deletion
  );

  $new_content = urlencode(json_encode($content));

  $uri_params = drupal_http_build_query($params);
  $uri = $uri_params . '&content=' . $new_content . $id;

  // Set the Art Profile (setArtProfile).
  $set_art_profile_result = _miss_send_miss_request( variable_get('visualartsSetArtProfile'), $uri );

  if(!empty($set_art_profile_result['id'])) {
    drupal_set_message( t('The ' . $component_title . ' component for "' . strip_tags($content['artworkTitle']) . '" has been successfully saved.'), 'status' );
  } else {
    drupal_set_message( t('Could not save the ' . $component_title . ' component for "' . strip_tags($content['artworkTitle']) . '". Please check, and try again.'), 'error' );
  }

  drupal_goto('/' . current_path() , array('query' => array('key' => $get_key)));
}

/**
 * Implements hook_form().
 */
function miss_visual_arts_components_image_metadata_form( $form, &$form_state, $page = false ) {

  $record = array();

  if(!empty($page['target_image'])) {
    $record = $page['target_image'];
  }

  $form = array();

  $form['credit'] = array(
    '#type' => 'text_format',
    '#title' => t('Credit'),
    '#rows' => 4,
    '#cols' => 10,
    '#required' => TRUE,
    '#resizable' => TRUE,
    '#format' => 'full_html',
    '#value' => !empty($record['credit']) ? $record['credit'] : '',
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['caption'] = array(
    '#type' => 'text_format',
    '#title' => t('Caption'),
    '#rows' => 14,
    '#cols' => 10,
    '#required' => TRUE,
    '#resizable' => TRUE,
    '#format' => 'full_html',
    '#value' => !empty($record['caption']) ? $record['caption'] : '',
    '#prefix' => '<div style="margin-top: 1.5rem;"></div>' . "\n",
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#disabled' => !user_access('edit visual arts content') ? TRUE : FALSE,
  );

  $form['#submit'][] = 'miss_visual_arts_components_image_metadata_form_submit';

  return $form;
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function miss_form_miss_visual_arts_components_image_metadata_form_alter(&$form, &$form_state, $form_id) {
  _miss_visual_arts_tour_image_metadata_css_js();
  return $form;
}

/**
 * Implements hook_form_submit().
 */
function miss_visual_arts_components_image_metadata_form_submit($form, &$form_state) {

  $path_parts = arg();
  $get_key = filter_xss($_GET['key']);
  $params = array();
  $art_profile_ids_array = array();
  $parent_id = $form_state['build_info']['args'][0]['record']['id'];
  $component_title = $form_state['build_info']['args'][0]['component']['title'];
  $edan_key = $form_state['build_info']['args'][0]['component']['edan_key'];

  // Pull out the record content.
  // $content = $form_state['build_info']['args'][0]['record']['artProfiles'][$get_key];
  $content = $form_state['build_info']['args'][0]['record']['art_profiles'][$path_parts[6]];

  $id = (isset($content['id']) && !empty($content['id'])) ? '&id=' . $content['id'] : '';
  // Get the unique title of a component.
  $title = _miss_visual_arts_tour_get_component_title($content['id']);
  // Unset the id, so it's not mixed in with the content being sent back.
  unset($content['id']);

  $content[$edan_key]['images'][$get_key]['credit'] = $form_state['input']['credit']['value'];
  $content[$edan_key]['images'][$get_key]['caption'] = $form_state['input']['caption']['value'];

  $params = array(
    'title' => $title,
    'status' => 0, // 0 = published, 1 = not published, -1 = marked for deletion
  );

  $new_content = urlencode(json_encode($content));
  $uri_params = drupal_http_build_query($params);
  $uri = $uri_params . '&content=' . $new_content . $id;

  // Set the Art Profile (setArtProfile).
  $set_art_profile_result = _miss_send_miss_request( 'miss/v1.0/nmaahc/visual_arts/setArtProfile.htm', $uri );

  if(!empty($set_art_profile_result['id'])) {
    drupal_set_message( t('The ' . $component_title . ' component for "' . strip_tags($content['artworkTitle']) . '" has been successfully saved.'), 'status' );
  } else {
    drupal_set_message( t('Could not save the ' . $component_title . ' component for "' . strip_tags($content['artworkTitle']) . '". Please check, and try again.'), 'error' );
  }

  drupal_goto('/' . current_path() , array('query' => array('key' => $get_key)));
}

/**
 * Include image metadata pages CSS and JS.
 */
function _miss_visual_arts_tour_image_metadata_css_js() {
  // CSS
  drupal_add_css(
    drupal_get_path('module', 'miss') . '/css/_visual_arts_tour.css',
    array('group' => CSS_THEME, 'type' => 'file', 'weight' => 2)
  );
  // Javascript
  drupal_add_js(
    drupal_get_path('module', 'miss') . '/js/_visual_arts_tour.js',
    array('scope' => 'footer', 'weight' => 3)
  );
  drupal_add_js( array('ids_url' => variable_get('ids_url')), 'setting' );
  // jQueryUI
  drupal_add_library('system', 'ui.tabs');
}

/**
 * Get the unique title of a component.
 */
function _miss_visual_arts_tour_get_component_title( $id = '' ) {

  $get_components = _miss_send_miss_request( variable_get('visualartsGetComponents') );

  if(!empty($id)) {

    // Get the unique title of the component.
    if(array_key_exists($id, $get_components['components'])) {

      // Set the unique title.
      $title = urlencode($get_components['components'][$id]['title']);

      // // Get the component's child blocks
      // $child_blocks_array = _visual_arts_tour_components();
      // foreach($child_blocks_array as $child_key => $child_data) {
      //   $child_names[] = $child_data['edan_key'];
      // }

      // // Loop through, and add any existing child block to the $child_content array,
      // // so it can be merged back into the main $content array (otherwise, it will get lost).
      // foreach($get_components['components'][$id]['content'] as $content_key => $content_value) {
      //   if(in_array($content_key, $child_names)) {
      //     $child_content[$content_key] = $get_components['components'][$id]['content'][$content_key];
      //   }
      // }

    }

  } else {
    // If the component is new, append a random integer to the title to ensure it's unique.
    $title = isset($value['artworkTitle']) ? $value['artworkTitle'] . ' - ' . mt_rand( 0, 0xffff ) : 'Image'  . ' - ' .  mt_rand( 0, 0xffff );
  }

  // $title = substr($title, 0, 200);
  return $title;
}

/**
 * Returns URLs found in a string.
 *
 * @param string 
 *   The string to search for URLs.
 */
function _miss_visual_arts_tour_get_urls( $string ) {
  $regex = '/https?\:\/\/[^\" ]+/i';
  preg_match_all($regex, $string, $matches);
  return $matches[0];
}

/*
 * PHP GD
 * Resize an image using GD library.
 */
function _miss_visual_arts_tour_resize_image( $file = false, $width = false ) {

  if($file && $width) {

    // Content type
    header('Content-Type: image/jpeg');

    //Get the original image dimensions + type
    list($source_width, $source_height, $source_type) = getimagesize($file);

    $ratio = $width / $source_width;
    $temp_width = $width;
    $temp_height = $source_height * $ratio;

    $source_gdim = imagecreatefromjpeg($file);
    $desired_gdim = imagecreatetruecolor($temp_width, $temp_height);
    imagecopyresampled(
      $desired_gdim,
      $source_gdim,
      0, 0,
      0, 0,
      $temp_width, $temp_height,
      $source_width, $source_height
    );

    // Output and free memory
    imagejpeg($desired_gdim);
    // ImageJpeg($desired_gdim, $destination, 100);

    imagedestroy($desired_gdim);

  }

}

/**
 * Build Gallery Data
 */
function _miss_visual_arts_tour_build_gallery_data($path_part = '') {

  $data = array();
  $art_profiles = array();

  if(!empty($path_part)) {
    // Get all of the components.
    $items = _miss_send_miss_request( 'miss/v1.0/nmaahc/visual_arts/getComponents.htm' );

    // If components don't exist, set a message, and go to the main page.
    if(empty($items['components'])) {
      drupal_set_message( t('No components found. (ID = ' . $path_part) . ')', 'error' );
      drupal_goto('miss/manage/visualArtsTour');
      exit;
    } else {
      // Construct arrays for the JSON to be used by javascript.
      // Gallery description.
      $gallery = $items['components'][$path_part]['content'];
      $gallery['id'] = $items['components'][$path_part]['id'];
      $gallery['status'] = $items['components'][$path_part]['status'];

      // Art profiles. Loop through to find the desired record set.
      if(!empty($items['components'][$path_part]['content']['artProfiles'])) {
        $i = 0;
        foreach($items['components'][$path_part]['content']['artProfiles'] as $item_key => $item_value) {
          $art_profiles['art_profiles'][$i] = $items['components'][$item_value]['content'];
          $art_profiles['art_profiles'][$i]['id'] = $item_value;
          $art_profiles['art_profiles'][$i]['status'] = $items['components'][$item_value]['status'];
          $i++;
        }
      }
      // Construct the data array.
      $data = array_merge($gallery, $art_profiles);
    }
  }

  return $data;

}

/**
 * Resize PNG Image.
 */
function _miss_visual_arts_tour_resize_png_image($filename, $percent) {

  header('Content-Type: image/jpeg');

  // Get new sizes
  list($width, $height) = getimagesize($filename);
  $newwidth = $width * $percent;
  $newheight = $height * $percent;

  // Load
  $thumb = imagecreatetruecolor($newwidth, $newheight);
  $source = imagecreatefrompng($filename);

  // Resize
  imagecopyresized($thumb, $source, 0, 0, 0, 0, $newwidth, $newheight, $width, $height);

  ob_start();
  imagejpeg($thumb);
  $imageString = ob_get_clean();

  // Output
  return $imageString;

}

/**
 * Builds the Browse and Manage pages.
 */
function _miss_visual_arts_tour_page_build($interactive = '') {

  if(!user_access('view visual arts content')){
    drupal_set_message('You do not have access to this page. Please contact an Administrator to resolve this issue.', 'error');
    drupal_goto('miss/list');
  }

  // Variables
  $data = array();
  $component_links = '';
  $current_path = current_path();
  $path_parts = arg();
  $types = _miss_types();
  $title = $types[$interactive];
  $components_menu_output = '';
  $out = '<p>No records found</p>';
  $components = _visual_arts_tour_components();

  ///////////////////////////////////////////////////
  // MANAGE GALLERY COMPONENTS //////////////////////
  ///////////////////////////////////////////////////
  if(!empty($path_parts[5]) && array_key_exists($path_parts[5], $components) && empty($path_parts[7])) {

    $get_key = filter_xss($_GET['key']);

    // Build the gallery data.
    $data['page']['record'] = _miss_visual_arts_tour_build_gallery_data($path_parts[4]);

    // Add the JSON to the output.
    drupal_add_js( array('record_json' => json_encode($data['page']['record'])), 'setting' );

    // EDAN record dump
    if(!empty($data['page']['record']['artProfiles'][$get_key]) && isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
      $dump = '<pre>' . var_export($data['page']['record']['artProfiles'][$get_key], TRUE) . '</pre>';
      drupal_set_message( $dump, 'status' );
    }

    // Set the component metadata (title, edan_key)
    $data['page']['component'] = $components[$path_parts[5]];

    // Get the forms.
    if($path_parts[5] === 'imageMetadata') {
      // Image metadata form.
      $data['page']['form'] = drupal_get_form('miss_visual_arts_image_metadata_form', $data['page']);
    } else {
      $this_record['record'] = $data['page']['record']['art_profiles'][$get_key];
      $this_record['component'] = $data['page']['component'];
       // Search EDAN field.
      $data['page']['miss_image_search_form'] = _miss_search_edan_form('images');
      // Components form.
      $data['page']['form'] = drupal_get_form('miss_visual_arts_components_form', $this_record);
    }

    // Sethe URL of the parent record.
    $base_record_path = request_uri();
    $parts = explode('/', $base_record_path);
    array_pop($parts);
    $data['page']['base_record_path'] = implode('/', $parts);

    // Set the title.
    drupal_set_title($title . ': ' . html_entity_decode($data['page']['component']['title']));

    // Build-out the breadcrumbs.
    $current_breadcrumbs = drupal_get_breadcrumb();
    $crumbs = array();
    foreach($current_breadcrumbs as $key => $value) {
      $crumbs[] = $value;
    }
    $crumbs[] = l(t('Manage Gallery: ' . $data['page']['record']['caption']), $data['page']['base_record_path']);
    $crumbs[] = t($title . ': ' . $data['page']['component']['title']);
    drupal_set_breadcrumb($crumbs);

    // Tabbed menu
    foreach($components as $component_key => $component_value) {
      $component_link_class = ($path_parts[5] === $component_key) ? ' class="gray-text"' : '';
      $component_links .= '<div class="component-nav-links"><a href="/' . str_replace($path_parts[5], $component_key, $current_path) . '?key=' . $get_key . '"' . $component_link_class . '>' . t($component_value['title']) . '</a></div>' . "\n";
    }

    // This component name.
    $data['page']['this_component_name'] = $path_parts[5];
    // All components.
    $data['page']['all_components'] = $components;
    // This image key.
    $data['page']['this_image_key'] = $get_key;

    // Set the dimensions for the image.
    // $data['page']['ids_processed_image'] = stristr($data['page']['record']['art_profiles'][$get_key]['artworkImage'], '//ids.si.edu/ids/deliveryService/') 
    //   ? $data['page']['record']['art_profiles'][$get_key]['artworkImage'] . '&max_w=110&max_h=160' 
    //   : variable_get('ids_url') . '?id=' . $data['page']['record']['art_profiles'][$get_key]['artworkImage'] . '&max_w=110&max_h=160';

    // $data['page']['ids_processed_image'] = _miss_visual_arts_tour_resize_png_image($data['page']['record']['art_profiles'][$get_key]['artworkImage'], 0.04);
    $data['page']['ids_processed_image'] = $data['page']['record']['art_profiles'][$get_key]['artworkImage'];

    // Tabbed menu containing all available components.
    $data['page']['components_menu'] = '<div id="component-liks">';
    $data['page']['components_menu'] .= "\n" . $component_links;
    $data['page']['components_menu'] .= '</div>';

    return theme('miss_visual_arts_tour_manage_components', $data);
  }

  ///////////////////////////////////////////////////////////////////////
  // MANAGE IMAGE METADATA - SECONDARY IMAGES WITHIN A COMPONENT ////////
  ///////////////////////////////////////////////////////////////////////
  if(!empty($path_parts[7])) {

    $get_key = filter_xss($_GET['key']);

    // Build the gallery data.
    $data['page']['record'] = _miss_visual_arts_tour_build_gallery_data($path_parts[4]);

     // EDAN's component key.
    $edan_key = $components[$path_parts[5]]['edan_key'];
    // The target component image.
    $data['page']['target_image'] = $data['page']['record']['art_profiles'][$path_parts[6]][$edan_key]['images'][$get_key];
    // Set the component metadata (title, edan_key)
    $data['page']['component'] = $components[$path_parts[5]];
    // Image metadata form.
    $data['page']['form'] = drupal_get_form('miss_visual_arts_components_image_metadata_form', $data['page']);

    // Set the URL of the parent record.
    $base_record_path = request_uri();
    $parts = explode('/', $base_record_path);
    array_pop($parts);
    $component_image_key = array_pop($parts);
    $data['page']['base_record_path'] = implode('/', $parts) . '?key=' . $component_image_key;

    // Set the title.
    drupal_set_title($data['page']['record']['caption'] . '/' . $data['page']['component']['title'] . ': Image Metadata');

    // Build-out the breadcrumbs.
    $current_breadcrumbs = drupal_get_breadcrumb();
    $crumbs = array();
    foreach($current_breadcrumbs as $key => $value) {
      $crumbs[] = $value;
    }
    $crumbs[] = '<a href="' . $data['page']['base_record_path'] . '">' . t('Manage Gallery: ' . $data['page']['record']['caption']) . '</a>';
    $crumbs[] = t($title . ': ' . $data['page']['component']['title']);
    drupal_set_breadcrumb($crumbs);

    // This component name.
    $data['page']['this_component_name'] = $path_parts[5];
    // All components.
    $data['page']['all_components'] = $components;
    // This image key.
    $data['page']['this_image_key'] = $get_key;

    // Set the dimensions for the image.
    $data['page']['ids_processed_image'] = stristr($data['page']['target_image']['imageUrl'], '//ids.si.edu/ids/deliveryService/') 
      ? $data['page']['target_image']['imageUrl'] . '&max_w=110&max_h=160' 
      : variable_get('ids_url') . '?id=' . $data['page']['target_image']['imageUrl'] . '&max_w=110&max_h=160';

    return theme('visual_arts_tour_manage_components_image_metadata', $data);
  }

  /////////////////////////////////////
  // BROWSE PAGE //////////////////////
  /////////////////////////////////////
  if(empty($path_parts[3])) {

    drupal_set_title($title . ': Browse');

    // Get the all of the Visual Arts Tour records.
    // $data['record'] = _miss_send_miss_request( variable_get('visualartsRender') );
    $components_results = _miss_send_miss_request( variable_get('visualartsGetComponents') );
    $data['record'] = $components_results['components'];

    $galleries = array();
    foreach ($data['record'] as $value) {
      if(isset($value['content']['subType']) && $value['content']['subType'] == 'gallery_description'){
        array_push($galleries, $value);
      }
    }

    // dumper($galleries);

    // Add a new gallery link.
    // $data['page'] = '<p>' . l(t('+ Add a new gallery'), 'miss/manage/visualArtsTour/add') . '</p>';

    if(!empty($galleries)) {

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
        $dump = '<pre>' . var_export($galleries, TRUE) . '</pre>';
        drupal_set_message( $dump, 'status' );
      }

      // Table top
      $data['page'] = '<table id="browse_visual_arts_tour" class="display" cellspacing="0" width="100%">' . "\n";
      $data['page'] .= '<thead>' . "\n";
      $data['page'] .= '<tr>' . "\n";
      $data['page'] .= '<th width="75%">Title</th>' . "\n";
      $data['page'] .= '<th width="150" class="no-sort" style="text-align: center;">Status</th>' . "\n";
      $data['page'] .= '<th width="150" class="no-sort" style="text-align: center;">Actions</th>' . "\n";
      $data['page'] .= '</tr>' . "\n";
      $data['page'] .= '</thead>' . "\n";
      $data['page'] .= '<tbody>' . "\n";

      // Loop through all of the records to build the table rows.
      foreach($galleries as $key => $value) {
        $data['page'] .= '<tr>' . "\n";
        $data['page'] .= '<td><strong>' . $value['content']['caption'] . '</strong></td>' . "\n";
        $data['page'] .= '<td>' . ($value['status'] == 0 ? 'Published' : '<span style="color:red;">Unpublished</span>') . '</td>' . "\n";
        $data['page'] .= '<td style="text-align: center;">' . "\n";
        $data['page'] .= '<a href="/' . current_path() . '/edit/' . $value['id'] . '" style="margin-right: 16px;">' . (!user_access('edit visual arts content') ? 'View' : 'Edit') . '</a> ' . "\n";
        $data['page'] .= '</td>' . "\n";
        $data['page'] .= '</tr>' . "\n";
      }

      // Table bottom
      $data['page'] .= '</tbody>' . "\n";
      $data['page'] .= '</table>' . "\n";
    } else {
      drupal_set_message( t('No records found.'), 'error' );
    }

    return theme('miss_visual_arts_tour_browse', $data);
  }
  /////////////////////////////////////////////
  // MANAGE GALLERY PAGE //////////////////////
  /////////////////////////////////////////////
  else {

    switch($path_parts[3]) {

      case 'render':

        $out = '';

        // Get the record, only for the title.
        $record = _miss_visual_arts_tour_build_gallery_data($path_parts[4]);
        // Get the all of the Visual Arts Tour records.
        $render = _miss_send_miss_request( variable_get('visualartsRender') );

        // Get this gallery's data.
        if(!empty($path_parts[4])) {
          foreach($render['galleryDescriptions'] as $gk => $gv) {
            if($gv['id'] === $path_parts[4]) {
              $this_gallery_array = $render['galleryDescriptions'][$gk];
            }
          }
        }

        // Output
        $out .= '<h2>' . $record['caption'] . '</h2>' . "\n";
        $out .= '<textarea style="width: 100%; height: 100rem; border: 2px solid #555555; padding: 1rem;">' . "\n";

        $i = 0;
        foreach($this_gallery_array['artProfiles'] as $gk => $gv) {
          $out .= ($i !== 0) ? "\n\n\n\n" : '';
          $out .= '// ' . ucwords(strip_tags($gv['artworkTitle'])) . "\n\n\n";
          $out .= json_encode($gv, JSON_PRETTY_PRINT);
          $i++;
        }

        $out .= '</textarea>' . "\n";

        return $out;
        break;

      // Add a new record.
      case 'add':
        $data['page']['miss_image_search_form'] = _miss_search_edan_form('images');
        // $data['page']['miss_metadata_search_form'] = _miss_search_edan_form('metadata');
        $data['page']['form'] = drupal_get_form('miss_visual_arts_tour_form');
        drupal_set_title($title . ': Add a New Gallery');
        return theme('miss_visual_arts_tour_manage', $data);
        break;

      // Edit an existing record.
      case 'edit':
        $data['page']['miss_image_search_form'] = _miss_search_edan_form('images');

        // Get the record from EDAN.
        if(!empty($path_parts[4])) {

          // Build the gallery data.
          $data['page']['record'] = _miss_visual_arts_tour_build_gallery_data($path_parts[4]);
          
          if(!empty($data['page']['record'])) {

            // Add the JSON to the output.
            drupal_add_js( array('record_json' => json_encode($data['page']['record'])), 'setting' );

            // Set the title.
            drupal_set_title($title . ': Manage Gallery "' . $data['page']['record']['caption'] . '"');
            // Build-out the breadcrumbs.
            $current_breadcrumbs = drupal_get_breadcrumb();
            $crumbs = array();
            foreach($current_breadcrumbs as $key => $value) {
              $crumbs[] = $value;
            }
            $crumbs[] = t('Manage Gallery: ' . $data['page']['record']['caption']);
            drupal_set_breadcrumb($crumbs);

            // EDAN record dump
            if(!empty($data['page']['record']) && isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
              $dump = '<pre>' . var_export($data['page']['record'], TRUE) . '</pre>';
              drupal_set_message( $dump, 'status' );
            }

            // If a record doesn't exist, set a message.
            if(empty($data['page']['record'])) {
              drupal_set_message( t('The gallery record could not be found. (ID = ' . $path_parts[4]) . ')', 'error' );
              drupal_goto('miss/manage/visualArtsTour');
            }

            // Get the form.
            $data['page']['form'] = drupal_get_form('miss_visual_arts_tour_form', $data['page']['record']);

            $base_record_path = request_uri();
            global $user;
            if (in_array('administrator', array_values($user->roles)) && $data['page']['record']['status'] == 0) {
              $data['page']['render_link'] = '<p style="margin-top: 1rem;"><a href="/miss/manage/visualArtsTour/render/' . $path_parts[4] . '">View Render Array</a></p>' . "\n";
            }else{
              $data['page']['render_link'] = '';
            }

          } else {
            // If a record doesn't exist, set a message.
            drupal_set_message( t('The gallery record could not be found. (ID = ' . $path_parts[4]) . ')', 'error' );
            drupal_goto('miss/manage/visualArtsTour');
          }

        }

        return theme('miss_visual_arts_tour_manage', $data);
        break;

      // Send a 404.
      default:
        return MENU_NOT_FOUND;
    }
   
  }

}

$output = _miss_visual_arts_tour_page_build($interactive);
