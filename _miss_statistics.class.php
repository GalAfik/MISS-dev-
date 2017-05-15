<?php

/**
 * MISS Statistics UI
 *
 * @file _miss_statistics.class.inc
 * @version 0.1
 */

namespace NMAAHC\MISS;

/**
 * Class for the MISS Statistics UI
 */
class Statistics {

  private $content_cache_server_url = 'http://172.18.24.60/Analytics/reporting/app';

  /**
   * Convert JSON to Highcharts Format
   */
  public function convertJsonToHighcharts($this_data = array(), $mutifile = false) {

    $data = array();

    if(!empty($this_data)) {

      // Parse stats containing multiple files (loop).
      if($mutifile) {

        foreach($this_data as $this_data_key => $this_data_values) {

          $start_date_clean = str_replace('/Date(', '', $this_data_values['StartDate']);
          $start_date_clean = str_replace(')/', '', $start_date_clean);

          $year = date('Y', (int)$start_date_clean/1000);

          foreach($this_data_values['Points'] as $pkey => $pval) {

            $unix_date_format = strtotime($pval['Label'] . ', ' . $year);

            $data[$this_data_key]['categories'][] = date('Y-m-d', (int)$unix_date_format);
            $data[$this_data_key]['totals'][] = $pval['Value'];

          }

        }

      }

      // Parse stats with a single file.
      if(!$mutifile) {

        $start_date_clean = str_replace('/Date(', '', $this_data['StartDate']);
        $start_date_clean = str_replace(')/', '', $start_date_clean);

        $year = date('Y', (int)$start_date_clean/1000);

        foreach($this_data['Points'] as $pkey => $pval) {

          $unix_date_format = strtotime($pval['Label'] . ', ' . $year);

          $data['categories'][] = date('Y-m-d', (int)$unix_date_format);
          $data['totals'][] = $pval['Value'];

        }

      }

    }

    return $data;
  }

  /**
   * Stats JSON
   * sites/default/files/stats_json/
   */
  public function stats_json() {

    $path = variable_get('file_public_path', 'sites/default/files') . '/stats_json/';

    return array(
      'theArc' => $path . 'Arc-Analytics-2017-03-29.json',
      'greenbook' => $path . 'Greenbook-Analytics-2017-03-29.json',
      'theHub' => $path . 'Hub-Analytics-2017-03-29.json',
      'inTheStudio' => $path . 'In-The-Studio-Analytics-2017-03-29.json',
      'journey' => array(
        'journey1968' => $path . 'Journey-1968-Analytics-2017-03-29.json',
        'journeyEos' => $path . 'Journey-EoS-Analytics-2017-03-29.json',
        'journeySf' => $path . 'Journey-SF-Analytics-2017-03-29.json',
      ),
      'recordStore' => $path . 'Record-Store-Analytics-2017-03-29.json',
      'reflectionsBooth' => array(
        '1968' => $path . 'Refl-Booth-1968-Analytics-2017-03-29.json',
        'eos' => $path . 'Refl-Booth-EoS-Analytics-2017-03-29.json',
        'sf' => $path . 'Refl-Booth-SF-Analytics-2017-03-29.json',
      ),
      'visualArtsTour' => $path . 'Visual-Arts-Analytics-2017-03-29.json',
    );
  }

  /**
   * Expand Abbreviated
   *
   * Expand abbreviated titles to their full title.
   */
  public function expand_abbreviated($abbreviation = NULL) {

    switch($abbreviation) {
      case '1968':
      case 'journey1968':
        return 'Beyond_1968';
        break;
      case 'eos':
      case 'journeyEos':
        return 'The_End_of_Segregation';
        break;
      case 'sf':
      case 'journeySf':
        return 'Slavery_&_Freedom';
        break;
      default:
        return $abbreviation;
    }

  }

}
