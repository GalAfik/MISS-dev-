<?php

/**
 * Implements hook_form().
 */
function miss_arc_form( $form, &$form_state, $record = array() ) {

  $form = array();
  $form['#tree'] = TRUE;

  //hide upload/insert buttons for non editors
  $button_script = '
  jQuery(document).ready(function() {
   jQuery("a:contains(' . "'Upload / Insert'" . ')").hide();
 });
 ';
 if(!user_access('upload files')){
  drupal_add_js($button_script, 'inline');
}

$form['id'] = array(
  '#type' => 'hidden',
  '#default_value' => !empty($record['id']) ? $record['id'] : '',
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
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['artifact_name'] = array(
  '#required' => TRUE,
  '#type' => 'text_format',
  '#title' => t('Artifact Name'),
  '#resizable' => TRUE,
  '#default_value' => !empty($record['content']['name']) ? $record['content']['name'] : '',
  '#format' => 'full_html',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['artifact_image'] = array(
  '#required' => TRUE,
  '#type' => 'textfield',
  '#title' => t('Artifact Image'),
  '#size' => 65,
  '#maxlength' => 524288,
  '#default_value' => !empty($record['content']['image']['url']) ? $record['content']['image']['url'] : '',
  '#suffix' => !empty($record['content']['image']['url']) ? '<div style="margin-bottom: 0rem;"><a href="' . $record['content']['image']['url'] . '" target="_blank">View Image</a> (opens in a new window/tab)</div>' : '',
  '#attributes' => array(
    'class' => array(
      'miss-file-manage',
      ),
    ),
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['artifact_image_source'] = array(
  '#type' => 'text_format',
  '#title' => t('Artifact Image Credit(s)'),
  '#resizable' => TRUE,
  '#default_value' => !empty($record['content']['image']['source']) ? $record['content']['image']['source'] : '',
  '#format' => 'full_html',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['year'] = array(
  '#required' => TRUE,
  '#type' => 'text_format',
  '#title' => t('Year(s)'),
  '#resizable' => TRUE,
  '#default_value' => !empty($record['content']['years']) ? $record['content']['years'] : '',
  '#format' => 'full_html',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['gallery_location'] = array(
  '#type' => 'select',
  '#title' => t('Gallery Location'),
  '#required' => TRUE,
  '#options' => _miss_arc_gallery_location_field_options(),
  '#default_value' => !empty($record['content']['linkedGalleries']) ? _miss_arc_get_default_galleries($record['content']['linkedGalleries']) : '',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['story_title'] = array(
  '#required' => TRUE,
  '#type' => 'text_format',
  '#title' => t('Story Title'),
  '#resizable' => TRUE,
  '#default_value' => !empty($record['content']['story']['storyTitle']) ? $record['content']['story']['storyTitle'] : '',
  '#format' => 'full_html',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['story_text'] = array(
  '#required' => TRUE,
  '#type' => 'text_format',
  '#title' => t('Story Text'),
  '#resizable' => TRUE,
  '#default_value' => !empty($record['content']['story']['storyText']) ? $record['content']['story']['storyText'] : '',
  '#format' => 'full_html',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['supporting_image'] = array(
  '#type' => 'textfield',
  '#title' => t('Supporting Image'),
  '#size' => 65,
  '#maxlength' => 524288,
  '#default_value' => !empty($record['content']['image']['supporting_image_url']) ? $record['content']['image']['supporting_image_url'] : '',
  '#suffix' => !empty($record['content']['image']['supporting_image_url']) ? '<div style="margin-bottom: 0rem;"><a href="' . $record['content']['image']['supporting_image_url'] . '" target="_blank">View Image</a> (opens in a new window/tab)</div>' : '',
  '#attributes' => array(
    'class' => array(
      'miss-file-manage',
      ),
    ),
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['supporting_image_thumbnail'] = array(
  '#type' => 'textfield',
  '#title' => t('Supporting Image Thumbnail'),
  '#size' => 65,
  '#maxlength' => 524288,
  '#default_value' => !empty($record['content']['image']['supporting_thumbnail_url']) ? $record['content']['image']['supporting_thumbnail_url'] : '',
  '#suffix' => !empty($record['content']['image']['supporting_thumbnail_url']) ? '<div style="margin-bottom: 0rem;"><a href="' . $record['content']['image']['supporting_thumbnail_url'] . '" target="_blank">View Image</a> (opens in a new window/tab)</div>' : '',
  '#attributes' => array(
    'class' => array(
      'miss-file-manage',
      ),
    ),
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['supporting_image_caption'] = array(
  '#type' => 'text_format',
  '#title' => t('Supporting Image Caption'),
  '#resizable' => TRUE,
  '#default_value' => !empty($record['content']['image']['supporting_caption']) ? $record['content']['image']['supporting_caption'] : '',
  '#format' => 'full_html',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['supporting_image_source'] = array(
  '#type' => 'text_format',
  '#title' => t('Supporting Image Credit(s)'),
  '#resizable' => TRUE,
  '#default_value' => !empty($record['content']['image']['supporting_source']) ? $record['content']['image']['supporting_source'] : '',
  '#format' => 'full_html',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['primary_story_arc'] = array(
  '#type' => 'select',
  '#multiple' => FALSE,
  '#title' => t('Primary Story Arc'),
  '#required' => TRUE,
  '#options' => _miss_arc_story_arcs_field_options(),
  '#default_value' => !empty($record['content']['primaryStory']) ? _miss_arc_get_default_primary_story($record['content']['primaryStory']) : 0,
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['story_arcs'] = array(
  '#type' => 'select',
  '#multiple' => TRUE,
  '#title' => t('Secondary Story Arc(s) (select all that apply)'),
  '#options' => _miss_arc_story_arcs_field_options(),
  '#default_value' => !empty($record['content']['linkedStories']) ? _miss_arc_get_default_stories($record['content']['linkedStories']) : '',
  '#attributes' => array('data-placeholder' => 'Choose tags...', 'style' => 'width: 545px;'),
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['video'] = array(
  '#type' => 'textfield',
  '#title' => t('Video'),
  '#size' => 65,
  '#maxlength' => 524288,
  '#default_value' => !empty($record['content']['video']['url']) ? $record['content']['video']['url'] : '',
  '#suffix' => !empty($record['content']['video']['url']) ? '<div style="margin-bottom: 0rem;"><a href="' . $record['content']['video']['url'] . '" target="_blank">View Video</a> (opens in a new window/tab)</div>' : '',
  '#attributes' => array(
    'class' => array(
      'miss-file-manage',
      ),
    ),
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['video_caption'] = array(
  '#type' => 'text_format',
  '#title' => t('Video Caption'),
  '#resizable' => TRUE,
  '#default_value' => !empty($record['content']['video']['description']) ? $record['content']['video']['description'] : '',
  '#format' => 'full_html',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['video_source'] = array(
  '#type' => 'text_format',
  '#title' => t('Video Credit(s)'),
  '#resizable' => TRUE,
  '#default_value' => !empty($record['content']['video']['source']) ? $record['content']['video']['source'] : '',
  '#format' => 'full_html',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['animation_type'] = array(
  '#type' => 'select',
  '#title' => t('Animation Type'),
  '#options' => _miss_arc_animation_type_field_options(),
  '#default_value' => !empty($record['content']['animationType']) ? $record['content']['animationType'] : '',
  '#prefix' => '<hr style="box-sizing: content-box; margin-top: 20px; margin-bottom: 20px; border: 0; border-top: 1px solid #DEDEDE;">',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

  // unset specific select options if user is not an admin

global $user;
if (!in_array('administrator', array_values($user->roles))) {
  unset($form['animation_type']['#options']['Image Sequence']);
  unset($form['animation_type']['#options']['Custom']);
  unset($form['animation_type']['#options']['Dissolve']);
}

  //Image Sequence form elements

$form['image_sequence_container'] = array(
  '#type' => 'container',
  '#states' => array(
    'visible' => array(
      ':input[name="animation_type"]' => array('value' => 'Image Sequence'),
      ),
    ),
  );

$form['image_sequence_container']['play_on_start'] = array(
  '#type' => 'checkbox',
  '#title' => t('Play on start'),
  '#default_value' => !empty($record['content']['animation']['playOnStart']) && $record['content']['animation']['playOnStart'] == 1 ? '1' : '0',
  '#return_value' => '1',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['image_sequence_container']['frame_rate'] = array(
  '#type' => 'textfield',
  '#title' => t('Frame Rate (frames per second)'),
  '#size' => 20,
  '#maxlength' => 524288,
  '#default_value' => !empty($record['content']['animation']['frameRate']) ? $record['content']['animation']['frameRate'] : '',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['image_sequence_container']['number_loops'] = array(
  '#type' => 'textfield',
  '#title' => t('Number of Loops'),
  '#size' => 20,
  '#maxlength' => 524288,
  '#default_value' => !empty($record['content']['animation']['loops']) ? $record['content']['animation']['loops'] : '',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['image_sequence_container']['animation_sound'] = array(
  '#type' => 'textfield',
  '#title' => t('Animation Sound'),
  '#size' => 65,
  '#maxlength' => 524288,
  '#default_value' => !empty($record['content']['animation']['sound']) ? $record['content']['animation']['sound'] : '',
  '#suffix' => !empty($record['content']['animation']['sound']) ? '<div style="margin-bottom: 0rem;"><a href="' . $record['content']['animation']['sound'] . '" target="_blank">Preview Audio</a> (opens in a new window/tab)</div>' : '',
  '#attributes' => array(
    'class' => array(
      'miss-file-manage',
      ),
    ),
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['markupContainerImageSequence'] = array(
  '#type' => 'item',
  '#states' => array(
    'visible' => array(
      ':input[name="animation_type"]' => array('value' => 'Image Sequence'),
      ),
    ),
  '#markup' => '<p>&#10068 To reorder images, drag and drop image containers.</p>',
  );


if(empty($form_state['image_sequence_num_items']) && empty($record)) {
  $form_state['image_sequence_num_items'] = 1;
}

$form_state['image_sequence_num_items'] = !empty($form_state['image_sequence_num_items']) ? $form_state['image_sequence_num_items'] : 0;

if(!empty($record)) {
  if(!$form_state['image_sequence_num_items'] && !empty($record['content']['animation']['images'])) {
    $form_state['image_sequence_num_items'] = count($record['content']['animation']['images']);
  }
}

$sortable_js = "
jQuery(document).ready(function(){
  jQuery('#sortable').sortable(
  {
    helper: 'original',
    placeholder : 'ui-state-highlight',
    start: function(e, ui){
      ui.placeholder.height(ui.item.height());
    },
    axis: 'y',
    cursorAt : { top : 0 },
    update: function(event, ui) {

     jQuery('#sortable').children().each( function(e) {
      jQuery(this).find('#weight').val(jQuery(this).index() + 1);
    });
  },
  scroll: false,
}
).disableSelection();

jQuery('#sortable_book').sortable(
{
  helper: 'original',
  placeholder : 'ui-state-highlight',
  start: function(e, ui){
    ui.placeholder.height(ui.item.height());
  },
  axis: 'y',
  cursorAt : { top : 0 },
  update: function(event, ui) {

   jQuery('#sortable_book').children().each( function(e) {
    jQuery(this).find('#weight').val(jQuery(this).index() + 1);
  });
},
scroll: false,
}
).disableSelection();

jQuery('#sortable_dissolve').sortable(
{
  helper: 'original',
  placeholder : 'ui-state-highlight',
  start: function(e, ui){
    ui.placeholder.height(ui.item.height());
  },
  axis: 'y',
  cursorAt : { top : 0 },
  update: function(event, ui) {

   jQuery('#sortable_dissolve').children().each( function(e) {
    jQuery(this).find('#weight').val(jQuery(this).index() + 1);
  });
},
scroll: false,
}
).disableSelection();
});
";

$image_css = "
div[class*='form-item-image-sequence-field-'], div[class*='form-item-book-image-field-'], div[class*='form-item-dissolve-animation-'], div[id*='edit-book-image-field-']{
  float: left;
  padding-right: 10px;
}
.ui-sortable fieldset legend,
.ui-sortable fieldset .fieldset-wrapper {
  cursor: move;
}
    #edit-markupcontainerimagesequence p, #edit-markupcontainerbook p, #edit-markupcontainerdissolve p{
color: #ca9b0d;
}
";

drupal_add_library('system', 'ui.sortable');
drupal_add_css($image_css, 'inline');
drupal_add_js($sortable_js, 'inline');

$form['start_sort'] = array(
  '#markup' => '<div id="sortable" style="position: relative;">',
  '#states' => array(
    'visible' => array(
      ':input[name="animation_type"]' => array('value' => 'Image Sequence'),
      ),
    ),
  );


  // Build the number of item fieldsets indicated by $form_state['image_sequence_num_items']
for ($i = 1; $i <= $form_state['image_sequence_num_items']; $i++) {
  $form['image_sequence_field'][$i] = array(
    '#type' => 'fieldset',
    '#title' => 'Image #' . $i,
    '#states' => array(
      'visible' => array(
        ':input[name="animation_type"]' => array('value' => 'Image Sequence'),
        ),
      ),
    );

  $form['image_sequence_field'][$i]['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#size' => 40,
    '#maxlength' => 524288,
    '#default_value' => !empty($record['content']['animation']['images'][$i-1]['title']) ? $record['content']['animation']['images'][$i-1]['title'] : '',
    '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );

  $form['image_sequence_field'][$i]['image_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Image URL'),
    '#size' => 40,
    '#maxlength' => 524288,
    '#default_value' => !empty($record['content']['animation']['images'][$i-1]['url']) ? $record['content']['animation']['images'][$i-1]['url'] : '',
    '#suffix' => !empty($record['content']['animation']['images'][$i-1]['url']) ? '<div style="margin-bottom: 0rem; clear: both;margin-left: 350px;"><a href="' . $record['content']['animation']['images'][$i-1]['url'] . '" target="_blank">View Image</a> (opens in a new window/tab)</div>' : '',
    '#attributes' => array(
      'class' => array(
        'miss-file-manage',
        ),
      ),
    '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );

  $form['image_sequence_field'][$i]['weight'] = array(
    '#type' => 'hidden',
    '#title' => t('Weight'),
    '#size' => 3,
    '#maxlength' => 524288,
    '#default_value' => $i,
    '#attributes' => array(
      'id' => array(
        'weight',
        ),
      ),
    );
}

$form['end_sort'] = array(
  '#markup' => '</div>',
  '#states' => array(
    'visible' => array(
      ':input[name="animation_type"]' => array('value' => 'Image Sequence'),
      ),
    ),
  );

  // Adds a button to add more items.
$form['add_image_sequence_item'] = array(
  '#name' => 'add_image_sequence_item',
  '#type' => 'submit',
  '#value' => t('Add an image'),
  '#submit' => array('_miss_arc_image_sequence_form_add_item'),
  '#attributes' => array('style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 0rem;'),
  '#states' => array(
    'visible' => array(
      ':input[name="animation_type"]' => array('value' => 'Image Sequence'),
      ),
    ),
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

  // If we have more than one item, this button allows removal of the last item.
if ($form_state['image_sequence_num_items'] > 1) {
  $form['remove_image_sequence_item'] = array(
    '#name' => 'remove_image_sequence_item',
    '#type' => 'submit',
    '#value' => t('Remove latest image'),
    '#submit' => array('_miss_arc_image_sequence_form_remove_item'),
    '#limit_validation_errors' => array(),
    '#attributes' => array('style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 0rem;'),
    '#states' => array(
      'visible' => array(
        ':input[name="animation_type"]' => array('value' => 'Image Sequence'),
        ),
      ),
    '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );
}


  //Book form elements

$form['book_container'] = array(
  '#type' => 'container',
  '#states' => array(
    'visible' => array(
      ':input[name="animation_type"]' => array('value' => 'Book'),
      ),
    ),
  );

$form['book_container']['book_animation_sound'] = array(
  '#type' => 'textfield',
  '#title' => t('Book Animation Sound'),
  '#size' => 65,
  '#maxlength' => 524288,
  '#default_value' => !empty($record['content']['animation']['sound']) ? $record['content']['animation']['sound'] : '',
  '#suffix' => !empty($record['content']['animation']['sound']) ? '<div style="margin-bottom: 0rem;"><a href="' . $record['content']['animation']['sound'] . '" target="_blank">Preview Audio</a> (opens in a new window/tab)</div>' : '',
  '#attributes' => array(
    'class' => array(
      'miss-file-manage',
      ),
    ),
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['markupContainerBook'] = array(
  '#type' => 'item',
  '#states' => array(
    'visible' => array(
      ':input[name="animation_type"]' => array('value' => 'Book'),
      ),
    ),
  '#markup' => '<p>&#10068 To reorder images, drag and drop image containers.</p>',
  );

if(empty($form_state['book_num_items']) && empty($record)) {
  $form_state['book_num_items'] = 1;
}

$form_state['book_num_items'] = !empty($form_state['book_num_items']) ? $form_state['book_num_items'] : 0;

if(!empty($record)) {
  if(!$form_state['book_num_items'] && !empty($record['content']['animation']['images'])) {
    $form_state['book_num_items'] = count($record['content']['animation']['images']);
  }
}

$form['start_sort_book'] = array(
  '#markup' => '<div id="sortable_book" style="position: relative;">',
  '#states' => array(
    'visible' => array(
      ':input[name="animation_type"]' => array('value' => 'Book'),
      ),
    ),
  );

  // Build the number of item fieldsets indicated by $form_state['book_num_items']
for ($i = 1; $i <= $form_state['book_num_items']; $i++) {

  $form['book_image_field'][$i] = array(
    '#type' => 'fieldset',
    '#title' => t('Book Image Sequence #' . $i),
    '#states' => array(
      'visible' => array(
        ':input[name="animation_type"]' => array('value' => 'Book'),
        ),
      ),
    );

  $form['book_image_field'][$i]['book_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#size' => 40,
    '#maxlength' => 524288,
    '#default_value' => !empty($record['content']['animation']['images'][$i-1]['title']) ? $record['content']['animation']['images'][$i-1]['title'] : '',
    '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );

  $form['book_image_field'][$i]['book_image_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Image URL'),
    '#size' => 40,
    '#maxlength' => 524288,
    '#default_value' => !empty($record['content']['animation']['images'][$i-1]['url']) ? $record['content']['animation']['images'][$i-1]['url'] : '',
    '#suffix' => !empty($record['content']['animation']['images'][$i-1]['url']) ? '<div style="clear: both; margin-left: 350px;"><a href="' . $record['content']['animation']['images'][$i-1]['url'] . '" target="_blank">View Image</a> (opens in a new window/tab)</div>' : '',
    '#attributes' => array(
      'class' => array(
        'miss-file-manage',
        ),
      ),
    '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );

  $form['book_image_field'][$i]['page_type_field']['page_type'] = array(
    '#type' => 'radios',
    '#options' => array(0 => 'Hard Page', 1 => 'Soft Page'),
    '#default_value' => !empty($record['content']['animation']['images'][$i-1]['pageType']) ? $record['content']['animation']['images'][$i-1]['pageType'] : '',
    '#prefix' => '<div style="float:left">',
    '#suffix' => '</div>',
    '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );

  $form['book_image_field'][$i]['weight'] = array(
    '#type' => 'hidden',
    '#title' => t('Weight'),
    '#size' => 3,
    '#maxlength' => 524288,
    '#default_value' => $i,
    '#attributes' => array(
      'id' => array(
        'weight',
        ),
      ),
    );
}

$form['end_sort_book'] = array(
  '#markup' => '</div>',
  '#states' => array(
    'visible' => array(
      ':input[name="animation_type"]' => array('value' => 'Book'),
      ),
    ),
  );

  // Adds a button to add more items.
$form['add_book_item'] = array(
  '#name' => 'add_book_item',
  '#type' => 'submit',
  '#value' => t('Add an image'),
  '#submit' => array('_miss_arc_book_form_add_item'),
  '#attributes' => array('style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 0rem;'),
  '#states' => array(
    'visible' => array(
      ':input[name="animation_type"]' => array('value' => 'Book'),
      ),
    ),
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

  // If we have more than one item, this button allows removal of the last item.
if ($form_state['book_num_items'] > 1) {
  $form['remove_book_item'] = array(
    '#name' => 'remove_book_item',
    '#type' => 'submit',
    '#value' => t('Remove latest image'),
    '#submit' => array('_miss_arc_book_form_remove_item'),
      // Since we are removing an item, don't validate until later.
    '#limit_validation_errors' => array(),
    '#attributes' => array('style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 0rem;'),
    '#states' => array(
      'visible' => array(
        ':input[name="animation_type"]' => array('value' => 'Book'),
        ),
      ),
    '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );
}


  //Dissolve form elements

  $form['dissolve_container'] = array(
    '#type' => 'container',
    '#states' => array(
      'visible' => array(
        ':input[name="animation_type"]' => array('value' => 'Dissolve'),
      ),
    ),
  );

  $form['dissolve_container']['play_on_start'] = array(
    '#type' => 'checkbox',
    '#title' => t('Play on start'),
    '#default_value' => !empty($record['content']['animation']['playOnStart']) && $record['content']['animation']['playOnStart'] == 1 ? '1' : '0',
    '#return_value' => '1',
    '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

  $form['dissolve_container']['speed'] = array(
      '#type' => 'textfield',
      '#title' => t('Speed (seconds to fade between images)'),
      '#size' => 65,
      '#maxlength' => 524288,
      '#default_value' => !empty($record['content']['animation']['speed']) ? $record['content']['animation']['speed'] : '',
      '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );


  $form['dissolve_container']['dissolve_animation_sound'] = array(
    '#type' => 'textfield',
    '#title' => t('Animation Sound'),
    '#size' => 65,
    '#maxlength' => 524288,
    '#default_value' => !empty($record['content']['animation']['sound']) ? $record['content']['animation']['sound'] : '',
    '#suffix' => !empty($record['content']['animation']['sound']) ? '<div style="margin-bottom: 0rem;"><a href="' . $record['content']['animation']['sound'] . '" target="_blank">Preview Audio</a> (opens in a new window/tab)</div>' : '',
    '#attributes' => array(
      'class' => array(
        'miss-file-manage',
      ),
    ),
    '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

  $form['markupContainerDissolve'] = array(
    '#type' => 'item',
    '#states' => array(
      'visible' => array(
        ':input[name="animation_type"]' => array('value' => 'Dissolve'),
      ),
    ),
    '#markup' => '<p>&#10068 To reorder images, drag and drop image containers.</p>',
    );


  $form['start_sort_dissolve'] = array(
    '#markup' => '<div id="sortable_dissolve" style="position: relative;">',
    '#states' => array(
      'visible' => array(
        ':input[name="animation_type"]' => array('value' => 'Dissolve'),
      ),
    ),
    );

  for ($i = 1; $i <= 2; $i++){
    $form['dissolve_animation'][$i] = array(
      '#type' => 'fieldset',
      '#title' => 'Image #' . $i,
      '#states' => array(
        'visible' => array(
          ':input[name="animation_type"]' => array('value' => 'Dissolve'),
        ),
      ),
    );

    $form['dissolve_animation'][$i]['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Title'),
      '#size' => 40,
      '#maxlength' => 524288,
      '#default_value' => !empty($record['content']['animation']['images'][$i-1]['title']) ? $record['content']['animation']['images'][$i-1]['title'] : '',
      '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );

    $form['dissolve_animation'][$i]['image_url'] = array(
      '#type' => 'textfield',
      '#title' => t('Image URL'),
      '#size' => 40,
      '#maxlength' => 524288,
      '#default_value' => !empty($record['content']['animation']['images'][$i-1]['url']) ? $record['content']['animation']['images'][$i-1]['url'] : '',
      '#suffix' => !empty($record['content']['animation']['images'][$i-1]['url']) ? '<div style="margin-bottom: 0rem; clear: both;margin-left: 350px;"><a href="' . $record['content']['animation']['images'][$i-1]['url'] . '" target="_blank">View Image</a> (opens in a new window/tab)</div>' : '',
      '#attributes' => array(
        'class' => array(
          'miss-file-manage',
        ),
      ),
      '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );

    $form['dissolve_animation'][$i]['weight'] = array(
      '#type' => 'hidden',
      '#title' => t('Weight'),
      '#size' => 3,
      '#maxlength' => 524288,
      '#default_value' => $i,
      '#attributes' => array(
        'id' => array(
          'weight',
        ),
      ),
    );
  }

  $form['end_sort_dissolve'] = array(
    '#markup' => '</div>',
    '#states' => array(
      'visible' => array(
        ':input[name="animation_type"]' => array('value' => 'Dissolve'),
      ),
    ),
    );

  //Custom form elements

$form['custom_container'] = array(
  '#type' => 'container',
  '#states' => array(
    'visible' => array(
      ':input[name="animation_type"]' => array('value' => 'Custom'),
      ),
    ),
  );

$form['custom_container']['custom_animation'] = array(
  '#type' => 'select',
  '#title' => t('Custom Animation Selection'),
  '#options' => _miss_arc_custom_animation_field_options(),
  '#default_value' => !empty($record['content']['animation']['selection']) ? strip_tags($record['content']['animation']['selection']) : '',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['custom_container']['custom_animation_sound'] = array(
  '#type' => 'textfield',
  '#title' => t('Custom Animation Sound'),
  '#size' => 65,
  '#maxlength' => 524288,
  '#default_value' => !empty($record['content']['animation']['sound']) ? $record['content']['animation']['sound'] : '',
  '#suffix' => !empty($record['custom_animation_sound']) ? '<div style="margin-bottom: 0rem;"><a href="' . $record['custom_animation_sound'] . '" target="_blank">Preview Audio</a> (opens in a new window/tab)</div>' : '',
  '#attributes' => array(
    'class' => array(
      'miss-file-manage',
      ),
    ),
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

$form['submit'] = array(
  '#prefix' => '<hr style="box-sizing: content-box; border: 0; border-top: 1px solid #FFFFFF;">',
  '#type' => 'submit',
  '#value' => 'Submit',
  '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
  );

return $form;
}

function _miss_arc_get_default_galleries($record){
  $json = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/render.htm' );
  for ($i=0; $i < count($json['galleries']); $i++) { 
    if($json['galleries'][$i]['id'] == $record[0]){
      return $i;
    }
  }
  return 0;
}

function _miss_arc_get_gallery_id($choice){
  $json = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/render.htm' );
  return $json['galleries'][$choice]['id'];
}

function _miss_arc_get_story_arc_id($choice){
  $json = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/render.htm' );
  return $json['stories'][$choice]['id'];
}

function _miss_arc_get_default_primary_story($record){
  $json = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/render.htm' );
  for ($i=0; $i < count($json['stories']); $i++) { 
    if($json['stories'][$i]['id'] == $record){
      return $i;
    }
  }
  return 0;
}

function _miss_arc_get_default_stories($record){
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

function _miss_arc_image_sequence_form_add_item($form, &$form_state) {
  $form_state['image_sequence_num_items']++;
  $form_state['rebuild'] = TRUE;
}

function _miss_arc_image_sequence_form_remove_item($form, &$form_state) {
  if ($form_state['image_sequence_num_items'] > 1) {
    $form_state['image_sequence_num_items']--;
  }
  $form_state['rebuild'] = TRUE;
}

function _miss_arc_book_form_add_item($form, &$form_state) {
  $form_state['book_num_items']++;
  $form_state['rebuild'] = TRUE;
}

function _miss_arc_book_form_remove_item($form, &$form_state) {
  if ($form_state['book_num_items'] > 1) {
    $form_state['book_num_items']--;
  }
  $form_state['rebuild'] = TRUE;
}

function _miss_arc_gallery_location_field_options() {
  $json = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/render.htm' );
  $sanitized = array();
  $unwanted_tags = array('amp;', '\n');
  if(!empty($json['galleries'])){
    foreach ($json['galleries'] as $item){
      array_push($sanitized, str_replace($unwanted_tags, '', strip_tags($item['name'])));
    }
  }
  return $sanitized;
}

function _miss_arc_story_arcs_field_options() {
  $json = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/render.htm' );
  $sanitized = array();
  $unwanted_tags = array('amp;', '\n');
  if(!empty($json['stories'])){
    foreach ($json['stories'] as $item){
      array_push($sanitized, str_replace($unwanted_tags, '', strip_tags($item['name'])));
    }
  }      
  return $sanitized;
}

function _miss_arc_custom_animation_field_options() {
  $json = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/render.htm' );
  $custom = array();
  if(!empty($json['custom'])){
    foreach ($json['custom'] as $id){
      array_push($custom, $id['id'] . ': ' . strip_tags($id['description']));
    }
  }

  return $custom;
}

function _miss_arc_animation_type_field_options() {
  return array(
    'None' => t('None'),
    'Image Sequence' => t('Image Sequence'),
    'Book' => t('Book'),
    'Dissolve' => t('Dissolve'),
    'Custom' => t('Custom'),
    );
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
// function miss_form_miss_hub_form_alter(&$form, &$form_state, $form_id) {
function miss_form_miss_arc_form_alter(&$form, &$form_state, $form_id) {
  $js = 'jQuery().ready(function(){
    jQuery(".filter-wrapper").hide();
  });
  jQuery().ready(function(){
    jQuery("#edit-gallery-location").css( "font-style", "italic");
  });';
  drupal_add_js($js, 'inline');
}

/**
 * Implements hook_form_submit().
 */
function miss_arc_form_submit($form, &$form_state) {

  $id = !empty($form_state['input']['id']) ? '&id=' . $form_state['input']['id'] : '';
  $stories = array();
  if(!empty($form_state['input']['story_arcs'])){
    foreach($form_state['input']['story_arcs'] as $value){
      array_push($stories, _miss_arc_get_story_arc_id($value));
    }
  }

  $linkedGalleries = _miss_arc_get_gallery_id($form_state['input']['gallery_location']);
  $primaryStory = _miss_arc_get_story_arc_id($form_state['input']['primary_story_arc']);

  $content = array(
    'name' => $form_state['input']['artifact_name']['value'],
    'years' => $form_state['input']['year']['value'],
    'story' => array(
      'storyTitle' => $form_state['input']['story_title']['value'],
      'storyText' => $form_state['input']['story_text']['value'],
      ),
    'linkedGalleries' => array(
      $linkedGalleries,
      ),
    'primaryStory' => $primaryStory,
    'linkedStories' => $stories,
    'image' => array(
      'source' => $form_state['input']['artifact_image_source']['value'],
      'url' => $form_state['input']['artifact_image'],
      'supporting_image_url' => $form_state['input']['supporting_image'],
      'supporting_thumbnail_url' => $form_state['input']['supporting_image_thumbnail'],
      'supporting_source' => $form_state['input']['supporting_image_source']['value'],
      'supporting_caption' => $form_state['input']['supporting_image_caption']['value'],
      ),
    'video' => array(
      'description' => $form_state['input']['video_caption']['value'],
      'source' => $form_state['input']['video_source']['value'],
      'url' => $form_state['input']['video'],
      ),
    'animationType' => $form_state['input']['animation_type'],
    );

  //adds animation section to json content based on which animation type was selected on the form
  switch ($form_state['input']['animation_type']) {
    case 'Image Sequence':
      //builds array of dynamic images to add to the json
    $imagesArray = array();
    for ($i=1; $i <= $form_state['image_sequence_num_items']; $i++) {
      array_push($imagesArray, array(
        'title' => $form_state['input']['image_sequence_field'][$i]['title'],
        'url' => $form_state['input']['image_sequence_field'][$i]['image_url'],
        'weight' => $form_state['input']['image_sequence_field'][$i]['weight'],
        )
      );
    }


    $tempArray = array();
    foreach ($imagesArray as $key => $row){
      $tempArray[$key] = $row['weight'];
    }
    array_multisort($tempArray, SORT_ASC, $imagesArray);


      //checks that all images are of the same size
      // if(!empty($imagesArray[1]['url'])){
      //   $imageSize = getimagesize($imagesArray[1]['url']);
      //   for ($i=1; $i <= count($imagesArray); $i++) { 
      //     if(!empty($imagesArray[$i]['url'])){
      //       var_dump($imageSize);
      //       if($imageSize != getimagesize($imagesArray[$i]['url'])){
      //         drupal_set_message('Animation image #' . $i . "'s size does not match.", 'error');
      //         drupal_goto(current_path());
      //       }
      //     }
      //   }
      // }

      //add json structure for specific animation type
    $content['animation'] = array(
      'playOnStart' => ($form_state['input']['image_sequence_container']['play_on_start'] == 1 ? '1' : '0'),
      'frameRate' => $form_state['input']['image_sequence_container']['frame_rate'],
      'loops' => $form_state['input']['image_sequence_container']['number_loops'],
      'sound' => $form_state['input']['image_sequence_container']['animation_sound'],
          //this is where the image array we built earlier gets added to the json
      'images' => (!empty($imagesArray) ? $imagesArray : new stdClass),
      );
    break;

    case 'Book':
    $imagesArray = array();
    for ($i=1; $i <= $form_state['book_num_items']; $i++) { 
      array_push($imagesArray, array(
        'title' => $form_state['input']['book_image_field'][$i]['book_title'],
        'url' => $form_state['input']['book_image_field'][$i]['book_image_url'],
        'pageType' => $form_state['input']['book_image_field'][$i]['page_type_field']['page_type'],
        'weight' => $form_state['input']['book_image_field'][$i]['weight'],
        )
      );
    }

    $tempArray = array();
    foreach ($imagesArray as $key => $row){
      $tempArray[$key] = $row['weight'];
    }
    array_multisort($tempArray, SORT_ASC, $imagesArray);

      //checks that all images are of the same size
      // if(!empty($imagesArray[1]['url'])){
      //   $imageSize = getimagesize($imagesArray[1]['url']);
      //   for ($i=1; $i <= count($imagesArray); $i++) { 
      //     if(!empty($imagesArray[$i]['url'])){
      //       var_dump($imageSize);
      //       if($imageSize != getimagesize($imagesArray[$i]['url'])){
      //         drupal_set_message('Animation image #' . $i . "'s size does not match.", 'error');
      //         drupal_goto(current_path());
      //       }
      //     }
      //   }
      // }

    $content['animation'] = array(
      'sound' => $form_state['input']['book_container']['book_animation_sound'],
      'images' => (!empty($imagesArray) ? $imagesArray : new stdClass),
      );
    break;

    case 'Dissolve':
      $imagesArray = array();
      for ($i=1; $i <= 2; $i++) { 
        array_push($imagesArray, array(
          'title' => $form_state['input']['dissolve_animation'][$i]['title'],
          'url' => $form_state['input']['dissolve_animation'][$i]['image_url'],
          'weight' => $form_state['input']['dissolve_animation'][$i]['weight'],
          )
        );
      }

      $tempArray = array();
      foreach ($imagesArray as $key => $row){
          $tempArray[$key] = $row['weight'];
      }
      array_multisort($tempArray, SORT_ASC, $imagesArray);

      //checks that all images are of the same size
      // if(!empty($imagesArray[1]['url'])){
      //   $imageSize = getimagesize($imagesArray[1]['url']);
      //   for ($i=1; $i <= count($imagesArray); $i++) { 
      //     if(!empty($imagesArray[$i]['url'])){
      //       var_dump($imageSize);
      //       if($imageSize != getimagesize($imagesArray[$i]['url'])){
      //         drupal_set_message('Animation image #' . $i . "'s size does not match.", 'error');
      //         drupal_goto(current_path());
      //       }
      //     }
      //   }
      // }

      $content['animation'] = array(
          'playOnStart' => ($form_state['input']['dissolve_container']['play_on_start'] == 1 ? '1' : '0'),
          'speed' => $form_state['input']['dissolve_container']['speed'],
          'sound' => $form_state['input']['dissolve_container']['dissolve_animation_sound'],
          'images' => (!empty($imagesArray) ? $imagesArray : new stdClass),
      );
      break;

    case 'Custom':
    $content['animation'] = array(
      'selection' => $form_state['input']['custom_container']['custom_animation'],
      'sound' => $form_state['input']['custom_container']['custom_animation_sound'],
      );
    break;

    default:
      //if no animation type is selected, an empty animation section will be added
    $content['animation'] = new stdClass;
    break;
  }

  // encodes new json object into uri to be sent to the endpoint for adding
  $status = isset($form_state['input']['enabled_field']['enabled']) ? $form_state['input']['enabled_field']['enabled'] : '1';
  $uri = 'content=' . urlencode(json_encode($content)) . $id . '&status=' . $status;
  $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/setArtifact.htm', $uri );


  if(!empty($set['id']) && !empty($set['content'])) {
    drupal_set_message( t('The record has been successfully saved.') );
  } else {
    drupal_set_message( t('Could not save the record. Please check, and try again.'), 'error' );
  }

  drupal_goto('miss/manage/theArc');
}

/**
 * Implements hook_form().
 */
function miss_arc_gallery_locations_form( $form, &$form_state, $record = array() ) {

  $form = array();
  $form['#tree'] = TRUE;

  if(empty($form_state['num_gallery_locations_items']) && empty($record)) {
    $form_state['num_gallery_locations_items'] = 1;
  }

  $form_state['num_gallery_locations_items'] = !empty($form_state['num_gallery_locations_items']) ? $form_state['num_gallery_locations_items'] : 0;

  if(!empty($record)) {
    if(!$form_state['num_gallery_locations_items']) {
      $form_state['num_gallery_locations_items'] = count($record);
    }
  }

  // Build the number of item fieldsets indicated by $form_state['num_gallery_locations_items']
  for ($i = 1; $i <= $form_state['num_gallery_locations_items']; $i++) {

    $form['galleryLocation'][$i] = array(

      'galleryLocationContent' => array(
        '#type' => 'text_format',
        '#base_type' => 'textarea',
        '#title' => t('Gallery Location #' . $i),
        '#rows' => 3,
        '#cols' => 10,
        '#required' => FALSE,
        '#resizable' => TRUE,
        '#format' => 'full_html',
        '#default_value' => !empty($record[$i-1]['name']) ? $record[$i-1]['name'] : '',
        '#prefix' => '<div style="margin-top: 0rem;"></div>' . "\n",
        '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
        ),
      'id' => array(
        '#type' => 'hidden',
        '#default_value' => !empty($record[$i-1]['id']) ? $record[$i-1]['id'] : '',
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
  $form['add_gallery_locations_item'] = array(
    '#name' => 'add_gallery_locations_item',
    '#type' => 'submit',
    '#value' => t('Add a new Gallery Location'),
    '#submit' => array('_miss_arc_gallery_locations_form_add_item'),
    '#attributes' => array('style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 0rem;'),
    '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );

  // If we have more than one item, this button allows removal of the last item.
  if ($form_state['num_gallery_locations_items'] > 1) {
    $form['remove_gallery_location_item'] = array(
      '#name' => 'remove_gallery_location_item',
      '#type' => 'submit',
      '#value' => t('Remove the latest Gallery Location'),
      '#submit' => array('_miss_arc_gallery_locations_form_remove_item'),
      // Since we are removing an item, don't validate until later.
      '#limit_validation_errors' => array(),
      '#attributes' => array(
        'style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 0rem;'
        ),
      '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
      );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#default_value' => 'Submit',
    '#prefix' => '<div class="clearfix"></div>' . "\n",
    '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );

  return $form;
}

function _miss_arc_gallery_locations_form_add_item($form, &$form_state) {
  $form_state['num_gallery_locations_items']++;
  $form_state['rebuild'] = TRUE;
}

function _miss_arc_gallery_locations_form_remove_item($form, &$form_state) {
  if ($form_state['num_gallery_locations_items'] > 1) {
    drupal_set_message('* Changes made in this table will not be saved until the form is submitted.', 'warning');
    $form_state['num_gallery_locations_items']--;
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
// function miss_form_miss_hub_form_alter(&$form, &$form_state, $form_id) {
function miss_form_miss_arc_gallery_locations_form_alter(&$form, &$form_state, $form_id) {
  $js = 'jQuery().ready(function(){
    jQuery(".filter-wrapper").hide();
  });';
  drupal_add_js($js, 'inline');
}

/**
 * Implements hook_form_submit().
 */
function miss_arc_gallery_locations_form_submit($form, &$form_state) {

  $id = '';
  $content = array();

  $tempArray = array();
  foreach ($form_state['values']['galleryLocation'] as $key => $row){
    $tempArray[$key] = $row['weight'];
  }
  array_multisort($tempArray, SORT_ASC, $form_state['values']['galleryLocation']);

  for ($i = 0; $i < $form_state['num_gallery_locations_items']; $i++) {
    //check if the assigned id is empty, and generate a unique nonce if it is
    if($form_state['values']['galleryLocation'][$i]['id'] == ''){
      $id = uniqid() . '-' . uniqid() . '-' . uniqid() . '-' . uniqid();
    }else{
      $id = $form_state['values']['galleryLocation'][$i]['id'];
    }
    //add the gallery and id to the endpoint request
    if(!empty($form_state['values']['galleryLocation'][$i]['galleryLocationContent']['value'])){
      array_push($content, array(
        'name' => $form_state['values']['galleryLocation'][$i]['galleryLocationContent']['value'],
        'id' => $id,
        )
      );
    }
  }

  $uri = 'content=' . urlencode(json_encode($content));
  $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/setGalleries.htm', $uri );

  drupal_goto(current_path());
}

/**
 * Implements hook_form().
 */
function miss_arc_story_arcs_form( $form, &$form_state, $record = array() ) {

  $form = array();
  $form['#tree'] = TRUE;

  if(empty($form_state['num_story_arcs_items']) && empty($record)) {
    $form_state['num_story_arcs_items'] = 1;
  }

  $form_state['num_story_arcs_items'] = !empty($form_state['num_story_arcs_items']) ? $form_state['num_story_arcs_items'] : 0;

  if(!empty($record)) {
    if(!$form_state['num_story_arcs_items']) {
      $form_state['num_story_arcs_items'] = count($record);
    }
  }

  // Build the number of item fieldsets indicated by $form_state['num_story_arcs_items']
  for ($i = 1; $i <= $form_state['num_story_arcs_items']; $i++) {

    $form['storyArc'][$i] = array(

      'storyArcContent' => array(
        '#type' => 'text_format',
        '#base_type' => 'textarea',
        '#title' => t('Story Arc #' . $i),
        '#rows' => 3,
        '#cols' => 10,
        '#required' => FALSE,
        '#resizable' => TRUE,
        '#format' => 'full_html',
        '#default_value' => !empty($record[$i-1]['name']) ? $record[$i-1]['name'] : '',
        '#prefix' => '<div style="margin-top: 0rem;"></div>' . "\n",
        '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
        ),
      'id' => array(
        '#type' => 'hidden',
        '#default_value' => !empty($record[$i-1]['id']) ? $record[$i-1]['id'] : '',
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
  $form['add_story_arcs_item'] = array(
    '#name' => 'add_story_arcs_item',
    '#type' => 'submit',
    '#value' => t('Add a new Story Arc'),
    '#submit' => array('_miss_arc_story_arcs_form_add_item'),
    '#attributes' => array('style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 0rem;'),
    '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );

  // If we have more than one item, this button allows removal of the last item.
  if ($form_state['num_story_arcs_items'] > 1) {
    $form['remove_story_arcs_item'] = array(
      '#name' => 'remove_story_arcs_item',
      '#type' => 'submit',
      '#value' => t('Remove the latest Story Arc'),
      '#submit' => array('_miss_arc_story_arcs_form_remove_item'),
      // Since we are removing an item, don't validate until later.
      '#limit_validation_errors' => array(),
      '#attributes' => array(
        'style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 0rem;'
        ),
      '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
      );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#default_value' => 'Submit',
    '#prefix' => '<div class="clearfix"></div>' . "\n",
    '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );

  return $form;
}

function _miss_arc_story_arcs_form_add_item($form, &$form_state) {
  $form_state['num_story_arcs_items']++;
  $form_state['rebuild'] = TRUE;
}

function _miss_arc_story_arcs_form_remove_item($form, &$form_state) {
  if ($form_state['num_story_arcs_items'] > 1) {
    drupal_set_message('* Changes made in this table will not be saved until the form is submitted.', 'warning');
    $form_state['num_story_arcs_items']--;
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
// function miss_form_miss_hub_form_alter(&$form, &$form_state, $form_id) {
function miss_form_miss_arc_story_arcs_form_alter(&$form, &$form_state, $form_id) {
  $js = 'jQuery().ready(function(){
    jQuery(".filter-wrapper").hide();
  });';
  drupal_add_js($js, 'inline');
}

/**
 * Implements hook_form_submit().
 */
function miss_arc_story_arcs_form_submit($form, &$form_state) {

  $id = '';
  $content = array();
  
  $tempArray = array();
  foreach ($form_state['values']['storyArc'] as $key => $row){
    $tempArray[$key] = $row['weight'];
  }
  array_multisort($tempArray, SORT_ASC, $form_state['values']['storyArc']);

  for ($i = 0; $i < $form_state['num_story_arcs_items']; $i++) {
    if($form_state['values']['storyArc'][$i]['id'] == ''){
      $id = uniqid() . '-' . uniqid() . '-' . uniqid() . '-' . uniqid();
    }else{
      $id = $form_state['values']['storyArc'][$i]['id'];
    }
    if(!empty($form_state['values']['storyArc'][$i]['storyArcContent']['value'])){
      array_push($content, array(
        'name' => $form_state['values']['storyArc'][$i]['storyArcContent']['value'],
        'id' => $id,
        )
      );
    }
  }

  $uri = 'content=' . urlencode(json_encode($content));
  $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/setStories.htm', $uri );

  drupal_goto(current_path());
}

function miss_arc_custom_animations_form( $form, &$form_state, $record = array() ) {

  $form = array();
  $form['#tree'] = TRUE;

  if(empty($form_state['num_custom_animations_items']) && empty($record)) {
    $form_state['num_custom_animations_items'] = 1;
  }

  $form_state['num_custom_animations_items'] = !empty($form_state['num_custom_animations_items']) ? $form_state['num_custom_animations_items'] : 0;

  if(!empty($record)) {
    if(!$form_state['num_custom_animations_items']) {
      $form_state['num_custom_animations_items'] = count($record);
    }
  }

  // Build the number of item fieldsets indicated by $form_state['num_custom_animations_items']
  for ($i = 1; $i <= $form_state['num_custom_animations_items']; $i++) {

    $form['customAnimation'][$i] = array(

      'customAnimationContent' => array(
        'id' => array(
          '#type' => 'textfield',
          '#title' => t('Custom Animation ID'),
          '#size' => 65,
          '#maxlength' => 524288,
          '#default_value' => !empty($record[$i-1]['id']) ? $record[$i-1]['id'] : '',
          '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
          ),
        'description' => array(
          '#type' => 'text_format',
          '#base_type' => 'textarea',
          '#title' => t('Custom Animation Description'),
          '#rows' => 3,
          '#cols' => 10,
          '#required' => FALSE,
          '#resizable' => TRUE,
          '#format' => 'full_html',
          '#default_value' => !empty($record[$i-1]['description']) ? $record[$i-1]['description'] : '',
          '#prefix' => '<div style="margin-top: 0rem;"></div>' . "\n",
          '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
          ),
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
  $form['add_custom_animations_item'] = array(
    '#name' => 'add_custom_animations_item',
    '#type' => 'submit',
    '#value' => t('Add a new Custom Animation'),
    '#submit' => array('_miss_arc_custom_animations_form_add_item'),
    '#attributes' => array('style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 0rem;'),
    '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );

  // If we have more than one item, this button allows removal of the last item.
  if ($form_state['num_custom_animations_items'] > 1) {
    $form['remove_custom_animations_item'] = array(
      '#name' => 'remove_custom_animations_item',
      '#type' => 'submit',
      '#value' => t('Remove the latest Custom Animation'),
      '#submit' => array('_miss_arc_custom_animations_form_remove_item'),
      // Since we are removing an item, don't validate until later.
      '#limit_validation_errors' => array(),
      '#attributes' => array('style' => 'font-size: 0.8rem; padding: 6px 8px; border-radius: 4px; margin-bottom: 0rem;'),
      '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
      );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#default_value' => 'Submit',
    '#prefix' => '<div class="clearfix"></div>' . "\n",
    '#disabled' => !user_access('edit arc content') ? TRUE : FALSE,
    );

  return $form;
}

function _miss_arc_custom_animations_form_add_item($form, &$form_state) {
  $form_state['num_custom_animations_items']++;
  $form_state['rebuild'] = TRUE;
}

function _miss_arc_custom_animations_form_remove_item($form, &$form_state) {
  if ($form_state['num_custom_animations_items'] > 1) {
    drupal_set_message('* Changes made in this table will not be saved until the form is submitted.', 'warning');
    $form_state['num_custom_animations_items']--;
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
// function miss_form_miss_hub_form_alter(&$form, &$form_state, $form_id) {
function miss_form_miss_arc_custom_animations_form_alter(&$form, &$form_state, $form_id) {
  $js = 'jQuery().ready(function(){
    jQuery(".filter-wrapper").hide();
  });';
  drupal_add_js($js, 'inline');
}

/**
 * Implements hook_form_submit().
 */
function miss_arc_custom_animations_form_submit($form, &$form_state) {

  $content = array();
  $form_state['input']['customAnimation'] = array_values($form_state['input']['customAnimation']);
  for ($i = 0; $i <= $form_state['num_custom_animations_items']; $i++) {

    if(!empty($form_state['input']['customAnimation'][$i]['customAnimationContent']['description']['value'])){
      $item = array(
        'id' => $form_state['input']['customAnimation'][$i]['customAnimationContent']['id'],
        'description' => $form_state['input']['customAnimation'][$i]['customAnimationContent']['description']['value'],
        );
      array_push($content, $item);
    }
  }
  $uri = 'content=' . urlencode(json_encode($content));
  $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/setCustomType.htm', $uri );

  drupal_goto(current_path());
}

/**
 * Table drag elements.
 */
function miss_arc_gallery_locations_table_drag_elements($vars) {

  drupal_add_tabledrag('arc_gallery_locations', 'order', 'sibling', 'item-row-weight');
  
  $data['header'] = array(
    'galleryLocationContent' => t('Gallery Locations'), 
    'weight' => t('Weight'),
    );
  
  $rows = array();
  foreach (element_children($vars['galleryLocation']) as $key) {
    $row = array();
    $row['data'] = array();
    foreach ($data['header'] as $fieldname => $title) {
      $field = $vars['galleryLocation'][$key][$fieldname];
      $row['data'][] = drupal_render($field);
      $row['class'] = array('draggable');
    }
    $data['rows'][] = $row;
  }

  return $data;
}

/**
 * Table drag elements.
 */
function miss_arc_story_arcs_table_drag_elements($vars) {

  drupal_add_tabledrag('arc_story_arcs', 'order', 'sibling', 'item-row-weight');
  
  $data['header'] = array(
    'storyArcContent' => t('Story Arcs'), 
    'weight' => t('Weight'),
    );
  
  $rows = array();
  foreach (element_children($vars['storyArc']) as $key) {
    $row = array();
    $row['data'] = array();
    foreach ($data['header'] as $fieldname => $title) {
      $field = $vars['storyArc'][$key][$fieldname];
      $row['data'][] = drupal_render($field);
      $row['class'] = array('draggable');
    }
    $data['rows'][] = $row;
  }

  return $data;
}

/**
 * Table drag elements.
 */
function miss_arc_custom_animations_table_drag_elements($vars) {

  drupal_add_tabledrag('arc_custom_animations', 'order', 'sibling', 'item-row-weight');
  
  $data['header'] = array(
    'customAnimationContent' => t('Custom Animations'), 
    'weight' => t('Weight'),
    );
  
  $rows = array();
  foreach (element_children($vars['customAnimation']) as $key) {
    $row = array();
    $row['data'] = array();
    foreach ($data['header'] as $fieldname => $title) {
      $field = $vars['customAnimation'][$key][$fieldname];
      $row['data'][] = drupal_render($field);
      $row['class'] = array('draggable');
    }
    $data['rows'][] = $row;
  }

  return $data;
}

/**
 * Builds the Browse and Manage pages.
 */
function _miss_arc_page_build($interactive = '') {

  if(!user_access('view arc content')){
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

    drupal_set_title('The Arc: Browse');

    // Get all of the components.
    $missresult = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/getComponents.htm' );
    $result = array();

    if(!isset($missresult['components'])){
      drupal_set_message('Could not retrieve EDAN records.', 'error');
    }
    else{
      foreach ($missresult['components'] as $component){
        if(!empty($component['content']['subType']) && $component['content']['subType'] == 'artifact'){
          array_push($result, $component);
        }
      }
    }

    $data['record'] = $result;

    global $user;
    // view links
    $data['page'] = '<p><strong>View:</strong> ' . l(t('Statistics'), 'miss/stats/home/theArc') . ' | ' . l(t('Backups'), 'miss/backup/browse') . "<br>";
    // manage links
    $data['page'] .= '<strong>Manage:</strong> ' . l(t('Gallery Locations'), 'miss/manage/theArc/galleryLocations') . ' | ' . l(t('Story Arcs'), 'miss/manage/theArc/storyArcs') . (in_array('administrator', array_values($user->roles)) ? ' | ' . l(t('Custom Animations'), 'miss/manage/theArc/customAnimations') : '') .  "<p>";

    // Add a new set of prompt questions link.
    $data['page'] .= '<p>' . (!user_access('edit arc content') ? '<span style="color:gray;">Add a New Record</span>' : l(t('Add a New Record'), 'miss/manage/theArc/add')) . '<br>' . "</p>";

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
    // Delete a record confirmation.
    $delete_js = 'jQuery(document).ready(function($) {
      $(".delete-link").on("click", function(){
        var r = confirm("Are you sure you want to delete this record?");
        if (r == true) {
          window.location.href = $(this).attr("data-link");
        }
      });
    });';
    drupal_add_js($delete_js, 'inline');

    // EDAN record dump
    if(!empty($data) && isset($_GET['dump']) && (variable_get('allow_dump') === '2')) {
      $dump = '<pre>' . var_export($data['record'], TRUE) . '</pre>';
      drupal_set_message( $dump, 'status' );
    }

    // DIV
    $data['page'] .=' <div class="datatables-body">' . "\n";
    // Table top
    $data['page'] .= '<table id="browse_the_arc" class="display" cellspacing="0" width="100%">' . "\n";
    $data['page'] .= '<thead>' . "\n";
    $data['page'] .= '<tr>' . "\n";
    $data['page'] .= '<th width="40%">Artifact</th>' . "\n";
    $data['page'] .= '<th width="20%">Story</th>' . "\n";
    $data['page'] .= '<th width="20%">Status</th>' . "\n";
    $data['page'] .= '<th width="20%" class="no-sort" style="text-align:center;">Actions</th>' . "\n";
    $data['page'] .= '</tr>' . "\n";
    $data['page'] .= '</thead>' . "\n";
    $data['page'] .= '<tbody>' . "\n";

    // Loop through all of the records to build the table rows.
    if(!empty($data['record'])) {
      foreach($data['record'] as $value) {
        // start adding row
        $data['page'] .= '<tr>' . "\n";
        // add artifact name column
        $data['page'] .= '<td><strong>' . (!empty($value['content']['name']) ? strip_tags($value['content']['name']) : '*Name Not Found') . '</strong></td>' . "\n";
        // add story name column
        $data['page'] .= '<td><strong>' . (!empty($value['content']['story']['storyTitle']) ? strip_tags($value['content']['story']['storyTitle']) : '*Story Not Found') . '</strong></td>' . "\n";
        // add status column
        $data['page'] .= '<td style="text-align: center;">' . ($value['status'] == 0 ? 'Enabled' : 'Disabled') . '</td>' . "\n";
        // add actions column
        $data['page'] .= '<td style="text-align: center;">' . "\n";
        $data['page'] .= '<a href="/' . current_path() . '/edit/' . $value['id'] . '" style="margin-right: 4px;">' . (!user_access('edit arc content') ? 'View' : 'Edit') . '</a> | ' . "\n";
        $data['page'] .= !user_access('edit arc content') ? '<span style="color:gray;">Delete</span>' : '<a href="javascript:void(0);" data-link="/' . current_path() . '/delete/' . $value['id'] . '" style="margin-left: 4px;" class="delete-link">Delete</a>' . "\n";
        $data['page'] .= '</td>' . "\n";
        // end adding row
        $data['page'] .= '</tr>' . "\n";
      }
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
        $('#edit-story-arcs').chosen();
        $('#edit-story-arcs').next().on('click', function(){ 
          var list_items = $('#edit-story-arcs').find('option');
          $.each( list_items, function( key, value ) {
            if($(value).text() != $('#edit-primary-story-arc option:selected').text()){
              $(value).show();
            }else{
              $(value).hide();
              $('#edit-story-arcs').trigger('chosen:updated');
            }
          });
        });
      });
      ";
      drupal_add_js($chosen_js, 'inline');

        // Get the form.
      $form = drupal_get_form('miss_arc_form');
      drupal_set_title('The Arc: Add a New Record');

        // return $output;
      return $form;
      break;

      // Edit an existing record.
      case 'edit':

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
        $('#edit-story-arcs').chosen();
        $('#edit-story-arcs').next().on('click', function(){ 
          var list_items = $('#edit-story-arcs').find('option');
          $.each( list_items, function( key, value ) {
            if($(value).text() != $('#edit-primary-story-arc option:selected').text()){
              $(value).show();
            }else{
              $(value).hide();
              $('#edit-story-arcs').trigger('chosen:updated');
            }
          });
        });
      });
      ";
      drupal_add_js($chosen_js, 'inline');

      if(!isset($path_parts[4])){
        drupal_set_message('The record could not be found.', 'error');
        drupal_goto('miss/manage/theArc');
      }

      $id = $path_parts[4];
      $form = array();
      $record = null;

      if(!empty($id)) {
        
        $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/getComponents.htm' );
        if(!isset($result['components'])){
          drupal_set_message('Could not retrieve EDAN records.', 'error');
          drupal_goto('miss/manage/theArc');
        }
        if(!isset($result['components'][$path_parts[4]])){
          drupal_set_message('The record could not be found.', 'error');
          drupal_goto('miss/manage/theArc');
        }

        $record = $result['components'][$path_parts[4]];
          // Get the form.
        $form = drupal_get_form('miss_arc_form', $record);
        drupal_set_title('Edit record: ' . strip_tags( html_entity_decode($record['content']['name'])));

      }else {
            // If a record doesn't exist, set a message.
        drupal_set_message( t('The record could not be found.'), 'error' );
        drupal_goto('miss/manage/theArc');
      }

        // return $output;
      return $form;
      break;

      // Delete a record.
      case 'delete':

      if(!isset($path_parts[4])){
        drupal_set_message( t('The record could not be found.'), 'error' );
        drupal_goto('miss/manage/theArc');
      }

      $id = $path_parts[4];

      if(!empty($id)) {

        if(!user_access('edit arc content')){
          drupal_set_message( t('You do not have permission to view this page. Please contact a site administrator for further instructions.'), 'error' );
          drupal_goto('miss/manage/theArc');
        }

        $uri = 'id=' . $id . '&status=-1';
        $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/setArtifact.htm', $uri );

        if(empty($set['id'])) {
          drupal_set_message( t('The record has been successfully deleted.') );
        } else {
          drupal_set_message( t('Could not delete the record. Please check, and try again.'), 'error' );
        }

        drupal_goto('miss/manage/theArc');

      }
      else {
          // If a record doesn't exist, set a message.
        drupal_set_message( t('The record record could not be found.'), 'error' );
        drupal_goto('miss/manage/theArc');
      }

      break;

      case 'galleryLocations':

        // Get all of the records.
      $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/render.htm' );

      if(!isset($result['galleries'])){
        drupal_set_message('Could not retrieve EDAN records.', 'error');
      }

      $record = isset($result['galleries']) ? $result['galleries'] : array();

        // Set the title.
      drupal_set_title('Manage Gallery Locations');
        // Build-out the breadcrumbs.
      $current_breadcrumbs = drupal_get_breadcrumb();
      $crumbs = array();
      foreach($current_breadcrumbs as $key => $value) {
        $crumbs[] = $value;
      }
      $crumbs[] = t(drupal_get_title());
      drupal_set_breadcrumb($crumbs);

        // Get the form.
      $form = drupal_get_form('miss_arc_gallery_locations_form', $record);
        // Intregrate table drag.
      $table_elements = miss_arc_gallery_locations_table_drag_elements($form);
        // Begin building the output.
      $out = '<form id="' . $form['#id'] . '" accept-charset="UTF-8" method="' . $form['#method'] . '" action="' . $form['#action'] . '">';
        // Unset the callToAction field.
      unset($form['galleryLocation']);
      $out .= theme('table', array('header' => $table_elements['header'], 'rows' => $table_elements['rows'], 'attributes' => array('id' => 'arc_gallery_locations')));
        // Add buttons and the rest of the form.
      $out .= drupal_render($form['add_item']);
      $out .= drupal_render($form['remove_item']);
      $out .= drupal_render_children($form);
      return $out;

      break;

      case 'storyArcs':

        // Get all of the records.
      $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/render.htm' );

      if(!isset($result['stories'])){
        drupal_set_message('Could not retrieve EDAN records.', 'error');
      }

      $record = isset($result['stories']) ? $result['stories'] : array();

        // Set the title.
      drupal_set_title('Manage Story Arcs');
        // Build-out the breadcrumbs.
      $current_breadcrumbs = drupal_get_breadcrumb();
      $crumbs = array();
      foreach($current_breadcrumbs as $key => $value) {
        $crumbs[] = $value;
      }
      $crumbs[] = t(drupal_get_title());
      drupal_set_breadcrumb($crumbs);

        // Get the form.
      $form = drupal_get_form('miss_arc_story_arcs_form', $record);
        // Intregrate table drag.
      $table_elements = miss_arc_story_arcs_table_drag_elements($form);
        // Begin building the output.
      $out = '<form id="' . $form['#id'] . '" accept-charset="UTF-8" method="' . $form['#method'] . '" action="' . $form['#action'] . '">';
        // Unset the callToAction field.
      unset($form['storyArc']);
      $out .= theme('table', array('header' => $table_elements['header'], 'rows' => $table_elements['rows'], 'attributes' => array('id' => 'arc_story_arcs')));
        // Add buttons and the rest of the form.
      $out .= drupal_render($form['add_item']);
      $out .= drupal_render($form['remove_item']);
      $out .= drupal_render_children($form);
      return $out;

      break;

      case 'customAnimations':
      
        // check if user is an administrator and has access to the custom animations page
      global $user;
      if (!in_array('administrator', array_values($user->roles))) {
        drupal_set_message('You do not have access to this page. Please contact an Administrator to resolve this issue.', 'error');
        drupal_goto('miss/manage/theArc');
      }

        // Get all of the records.
      $result = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/render.htm' );

      if(!isset($result['custom'])){
        drupal_set_message('Could not retrieve EDAN records.', 'error');
      }

      $record = isset($result['custom']) ? $result['custom'] : array();


        // Set the title.
      drupal_set_title('Manage Custom Animations');
        // Build-out the breadcrumbs.
      $current_breadcrumbs = drupal_get_breadcrumb();
      $crumbs = array();
      foreach($current_breadcrumbs as $key => $value) {
        $crumbs[] = $value;
      }
      $crumbs[] = t(drupal_get_title());
      drupal_set_breadcrumb($crumbs);

        // Get the form.
      $form = drupal_get_form('miss_arc_custom_animations_form', $record);
        // Intregrate table drag.
      $table_elements = miss_arc_custom_animations_table_drag_elements($form);
        // Begin building the output.
      $out = '<form id="' . $form['#id'] . '" accept-charset="UTF-8" method="' . $form['#method'] . '" action="' . $form['#action'] . '">';
        // Unset the callToAction field.
      unset($form['customAnimation']);
      $out .= theme('table', array('header' => $table_elements['header'], 'rows' => $table_elements['rows'], 'attributes' => array('id' => 'arc_custom_animations')));
        // Add buttons and the rest of the form.
      $out .= drupal_render($form['add_item']);
      $out .= drupal_render($form['remove_item']);
      $out .= drupal_render_children($form);
      return $out;

      break;

      case 'overwrite' : 

        //This page is used to go through all Arc records and save them again using any new fields to be added to the JSON.
        //In order to use this function, you must add the new field as it will appear in the Json in the area specified below.

      $missresult = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/getComponents.htm' );
      $result = array();

      if(!isset($missresult['components'])){
        drupal_set_message('Could not retrieve EDAN records.', 'error');
      }
      else{
        foreach ($missresult['components'] as $component){
          if(!empty($component['content']['subType']) && $component['content']['subType'] == 'artifact'){
            array_push($result, $component);
          }
        }
      }

      foreach ($result as $item) {
        $id = !empty($item['id']) ? $item['id'] : '';

        $stories = array();
        for ($i=0; $i < sizeof($item['content']['linkedStories']); $i++) { 
          $stories[$i] = $item['content']['linkedStories'][$i];
        }

          // new JSON implementation. ADD ANY JSON CHANGES HERE!!! make sure to provide default values for records that do not yeat implement your changes.
        $content = array(
          'name' => (!empty($item['content']['name']) ? $item['content']['name'] : ''),
          'years' => (!empty($item['content']['years']) ? $item['content']['years'] : ''),
          'story' => array(
            'storyTitle' => (!empty($item['content']['story']['storyTitle']) ? $item['content']['story']['storyTitle'] : ''),
            'storyText' => (!empty($item['content']['story']['storyText']) ? $item['content']['story']['storyText'] : ''),
            ),
          'linkedGalleries' => array(
            !empty($item['content']['linkedGalleries'][0]) ? $item['content']['linkedGalleries'][0] : '',
            ),
          'primaryStory' => (!empty($item['content']['primaryStory']) ? $item['content']['primaryStory'] : ''),
          'linkedStories' => $stories,
          'image' => array(
            'source' => (!empty($item['content']['image']['source']) ? $item['content']['image']['source'] : ''),
            'url' => (!empty($item['content']['image']['url']) ? $item['content']['image']['url'] : ''),
            'supporting_image_url' => (!empty($item['content']['image']['supporting_image_url']) ? $item['content']['image']['supporting_image_url'] : ''),
            'supporting_thumbnail_url' => (!empty($item['content']['image']['supporting_thumbnail_url']) ? $item['content']['image']['supporting_thumbnail_url'] : ''),
            'supporting_source' => (!empty($item['content']['image']['supporting_source']) ? $item['content']['image']['supporting_source'] : ''),
            'supporting_caption' => (!empty($item['content']['image']['supporting_caption']) ? $item['content']['image']['supporting_caption'] : ''),
            ),
          'video' => array(
            'description' => (!empty($item['content']['video']['description']) ? $item['content']['video']['description'] : ''),
            'source' => (!empty($item['content']['video']['source']) ? $item['content']['video']['source'] : ''),
            'url' => (!empty($item['content']['video']['url']) ? $item['content']['video']['url'] : ''),
            ),
          'animationType' => (!empty($item['content']['animationType']) ? $item['content']['animationType'] : ''),
          'animation' => (!empty($item['content']['animation']) ? $item['content']['animation'] : new stdClass),
          );

          // encodes new json object into uri to be sent to the endpoint for adding
        $uri = 'content=' . urlencode(json_encode($content)) . '&id=' . $id . '&status=' . $item['status'];
        $set = _miss_send_miss_request( 'miss/v1.0/nmaahc/the_arc/setArtifact.htm', $uri );

        if(empty($set['id']) || empty($set['content'])) {
          drupal_set_message( t('Could not overwrite the records. Please check, and try again.'), 'error' );
          drupal_goto('miss/manage/theArc');
        }
      }
      
      drupal_set_message( t('The records has been successfully overwritten.') );
      drupal_goto('miss/manage/theArc');

      break;

      // Send a 404.
      default:
      return MENU_NOT_FOUND;

    }
    
  }

}

$output = _miss_arc_page_build($interactive);