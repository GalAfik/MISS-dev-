<?php

/**
 * MISS Statistics UI
 *
 * @file _miss_statistics.inc
 * @version 0.1
 */

require '_miss_statistics.class.php';
use NMAAHC\MISS\Statistics;

/**
 * Implements hook_form().
 */
function miss_statistics_form( $form, &$form_state, $types = array() ) {

  $form = array();

  // Add the -Select- option to the interactive types.
  array_merge($types, array('-Select-'));

  // Start date
  $form['startDate'] = array(
    '#prefix' => '<div class="miss-inline-form" style="display: inline; float: left;">',
    '#type' => 'date_popup',
    '#title' => t('Start Date'),
    '#date_label_position' => 'none',
    '#required' => FALSE,
    '#date_format' => 'm/d/Y',
    '#default_value' => '2017-03-01',
    '#suffix' => '</div>',
  );

  // End date
  $form['endDate'] = array(
    '#prefix' => '<div class="miss-inline-form" style="display: inline; float: left;">',
    '#type' => 'date_popup',
    '#title' => t('End Date'),
    '#date_label_position' => 'none',
    '#required' => FALSE,
    '#date_format' => 'm/d/Y',
    '#default_value' => '2017-03-31',
    '#suffix' => '</div>',
  );

  $form['interactive'] = array(
    '#prefix' => '<div class="miss-inline-form" style="display: inline; float: left;">',
    '#type' => 'select',
    '#title' => t('Interactive'),
    '#required' => TRUE,
    '#options' => $types,
    '#suffix' => '</div>',
  );

  $form['submit'] = array(
    '#prefix' => '<div class="miss-inline-form" style="display: inline; float: left; margin: 1.1rem 0 0 0.5rem;">',
    '#type' => 'submit',
    '#value' => t('View'),
    '#suffix' => '</div>',
  );

  return $form;
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
// function miss_form_statistics_form_alter(&$form, &$form_state, $form_id) {
//
//   // Alter if need be...
//
//   return $form;
// }

/**
 * Implements hook_form_validate().
 */
function miss_statistics_form_validate($form, &$form_state) {

  // Validating the date range.
  if (!empty($form_state['values']['startDate']) && !empty($form_state['values']['endDate'])) {

    if (!is_array($form_state['values']['startDate'])
     && !is_array($form_state['values']['endDate'])
     && strtotime($form_state['values']['startDate'])
     > strtotime($form_state['values']['endDate'])) {
     form_set_error('file', t('Start Date cannot exceed the End Date.'));
    }

  }

}

/**
 * Implements hook_form_submit().
 */
function miss_statistics_form_submit($form, &$form_state) {

  $options = array();

  if(!empty($form_state['values']['startDate']) && !empty($form_state['values']['endDate'])) {

    $start_date = DateTime::createFromFormat('Y-m-d', $form_state['values']['startDate']);
    $start_date = $start_date->format('m/d/Y');
    $end_date = DateTime::createFromFormat('Y-m-d', $form_state['values']['endDate']);
    $end_date = $end_date->format('m/d/Y');

    $options = array('query' => array('startDate' => $start_date, 'endDate' => $end_date));

  }

  drupal_goto('miss/stats/home/' . $form_state['values']['interactive'], $options);

}

// visualArtsTour
// inTheStudio
// reflectionsBooth
// theHub
// recordStore
// theArc
// lunchCounter

// ???
// journey
// greenbook

/**
 * Builds the Home and Manage pages.
 */
function _miss_statistics_page_build() {

  // Variables
  $obj = new Statistics();
  $data = $this_data = array();
  $current_path = current_path();
  $path_parts = arg();
  $types = _miss_types();
  $title = 'Statistics';
  $this_page_title = 'Home';
  $out = '';
  $stats_json = $obj->stats_json();
  $multifile = FALSE;

  $add = array('journey' => 'Journey', 'greenbook' => 'Green Book');

  // Add journey to the $types array
  $types = array_merge($add, $types);

  switch($path_parts[2]) {

    // HOME PAGE
    case 'home':

    if(isset($path_parts[3])) {
      $this_page_title = $types[$path_parts[3]];
    }

      // Set the title of the page.
      drupal_set_title($title . ': ' . $this_page_title);

      // Set the flag for stats containing multiple files.
      if(isset($path_parts[3]) && (($path_parts[3] === 'journey') || ($path_parts[3] === 'reflectionsBooth'))) {
        $multifile = TRUE;
      }

      if($multifile) {
        // Get the JSON.
        foreach($stats_json[$path_parts[3]] as $rbk => $rbv) {
          $this_data[$rbk] = json_decode(file_get_contents($rbv), TRUE);
        }
      } else {
        // Get the JSON.
        if(isset($path_parts[3]) && isset($stats_json[$path_parts[3]])) {
          $this_data = json_decode(file_get_contents($stats_json[$path_parts[3]]), TRUE);
        }
      }

      // THE FORM: date range picker and interactive selection form.
      // Add a wrapper div for the form.
      $out .= '<div style="clear: both; width: auto; height: 125px;">';
      $form = drupal_get_form('miss_statistics_form', $types);
      $out .= drupal_render($form);
      $out .= '</div>';

      if(!empty($this_data)) {

        $this_converted_data = $obj->convertJsonToHighcharts($this_data, $multifile);

        if(!empty($this_converted_data)) {
          // Set the JavaScript variable for the converted JSON.
          drupal_add_js(
            array(
              'miss' => array(
                'statsData' => $this_converted_data,
                'multiFile' => $multifile,
                'interactiveName' => $types[$path_parts[3]]
              )
            ), 'setting'
          );
        }

        // Add JavaScript.
        drupal_add_js(drupal_get_path('module', 'miss') . '/plugins/highcharts/highcharts.js');

        if(isset($_GET['theme'])){
          drupal_add_js(drupal_get_path('module', 'miss') . '/plugins/highcharts/themes/monokai.js');
        }

        drupal_add_js(drupal_get_path('module', 'miss') . '/js/highcharts/createCharts.js');

        // Support for stats with multiple files.
        if($multifile) {

          foreach($stats_json[$path_parts[3]] as $rbk => $rbv) {
            // Expand abbreviated titles to their full title.
            $keyLabel = $obj->expand_abbreviated($rbk);
            $out .= '<div id="' . $keyLabel . '" style="width: 100%; height: 400px; margin-bottom: 3rem;"></div>' . "\n";
          }

        } else {
          $out .= '<div id="total" style="width: 100%; height: 400px;"></div>' . "\n";
        }

        // $out .= '<div id="booths" style="width:70%; height:400px; float:left;"></div>' . "\n";
        // $out .= '<div id="pieChart" style="width:30%; height:400px; float:left;"></div>' . "\n";

      }

      if(isset($path_parts[3]) && empty($this_data)) {
        drupal_set_message( t('No data available for ' . $types[$path_parts[3]]), 'error' );
      }

      return $out;
      break;

    // Send a 404.
    default:
      return MENU_NOT_FOUND;
  }

}
