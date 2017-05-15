<?php

// Test from Gor - 2016-09-16 - for Git

namespace NMAAHC\MISS;

class TimelineEvent extends RecordStoreComponent {
  public function fields() {

    //hide upload/insert buttons for non editors
  $button_script = '
  jQuery(document).ready(function() {
     jQuery("a:contains(' . "'Upload / Insert'" . ')").hide();
  });
  ';
  if(!user_access('upload files')){
    drupal_add_js($button_script, 'inline');
  }
  
    $fields = array(
      'id' => array(),
      'status' => array(
        'name' => '<b>Enabled</b>',
        'drupalForm' => array(
          '#type' => 'checkbox',
          '#required' => FALSE,
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
        ),
      ),
      'year' => array(
        'name' => 'Year',
        'drupalForm' => array(
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#required' => FALSE,
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
        ),
      ),
      'caption' => array(
        'name' => 'Caption',
        'drupalForm' => array(
          '#type' => 'text_format',
          '#format' => 'full_html',
          '#required' => TRUE,
          '#suffix' => '<hr style="box-sizing: content-box; margin-top: 20px; margin-bottom: 20px; border: 0; border-top: 1px solid #FFFFFF;">',
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
        ),
      ),
      'imageUrl' => array(
        'name' => 'Image',
        'drupalForm' => array(
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#description' => 'URL',
          '#attributes' => array(
            'class' => array(
              'miss-file-manage',
            ),
          ),
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
        ),
      ),
      'credit' => array(
        'name' => 'Image Credit',
        'drupalForm' => array(
          '#type' => 'text_format',
          '#format' => 'full_html',
          '#required' => FALSE,
          '#suffix' => '<hr style="box-sizing: content-box; margin-top: 20px; margin-bottom: 20px; border: 0; border-top: 1px solid #FFFFFF;">',
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
        ),
      ),
    );

    return $fields;
  }


  /**
   *
   */
  public function getComponentTitle($component) {
    // $title = isset($component->content['text']) ? $component->content['text'] : FALSE;
    $title = isset($component->content['year']) ? $component->content['year'] : FALSE;
    return $title;
  }


  /**
   *
   */
  public function getDrupalFormTitle($action, $component) {
    return parent::drupalFormTitle($action, $component, 'Timeline Event', $this->getComponentTitle($component));
  }


  /**
   *
   */
  public function getEdanComponent($id) {
    $data = $this->getContentComponents('event');

    if (!isset($data[$id])) {
      watchdog('miss_record_store', 'Could not load Record Store Timeline Event :id.', array(':id' => $id));
      return FALSE;
    }

    return parent::getEdanComponent($id);
  }

  /**
   *
   */
  public function setComponent($form_state) {

    $this->setPropertiesFromDrupalForm($form_state);
    $form_values = (array)$form_state['build_info']['args'][0];
    $year = (isset($form_values['content']['year']) && !empty($form_values['content']['year'])) ? $form_values['content']['year'] : 'No Title (empty year)';

    $query = array(
      'content' => array(),
    );

    foreach ($this->vars as $key => $value) {

      switch ($key) {
        case 'id':
          $query[$key] = $value;
          break;

        case 'status':
          if ($value !== FALSE) {
            $query[$key] = $value;
          }
          break;

        default:
          $query['content'][$key] = $value;
          break;
      }
    }

    // If deleting component, unset content.
    if (isset($query['status']) && $query['status'] === -1) {
      unset($query['content']);
    }

    // Only supply content if it exists.
    if(isset($query['content'])) {
      $query['content'] = json_encode($query['content']);
    }

    $uri = drupal_http_build_query($query);
    $response = $this->edanRequest('setEvent', $uri);

    // EDAN throws a 404 when deleting. Let's hack it for now...
    // TODO: Ask Andrew about this anomaly, and see if it can be fixed.
    if (($response['info']['http_code'] !== 200) && ($query['status'] !== -1)) {
      drupal_set_message(sprintf('There was an error processing your request: %s', $response['data']), 'error');
      return FALSE;
    }

    // If the record has not been deleted...
    if($query['status'] !== -1) {
      $data = json_decode($response['data']);

      // Add to the interactive
      // TODO: How to handle when events are deleted?
      $interactive = $this->getInteractive();
      if (!in_array($data->id, $interactive['content']['timelineEvents'])) {
        $timeline = array(
          'timeline' => $interactive['content']['timelineEvents'],
        );
        $timeline['timeline'][] = $data->id;
        $timeline['timeline'] = json_encode($timeline['timeline']);
        $uri = drupal_http_build_query($timeline);
        $response = $this->edanRequest('setTimeline', $uri);

        if ($response['info']['http_code'] !== 200) {
          drupal_set_message(sprintf('There was an error processing your request: %s', $response['data']), 'error');
          return FALSE;
        }
      }
      drupal_set_message(sprintf('Timeline Event "%s" has been saved.', $data->content->year));
      return TRUE;
    }

    if($query['status'] === -1) {
      drupal_set_message(sprintf('Timeline Event "%s" has been successfully deleted.', $year));
      return TRUE;
    }

  }
}
