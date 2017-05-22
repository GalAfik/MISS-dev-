<?php

// Test from Gor - 2016-09-16 - for Git

namespace NMAAHC\MISS;

class MusicTrack extends RecordStoreComponent {
  public function fields() {

    $interactive = $this->getInteractive();
    $genres_array = $interactive['content']['genres'];

    foreach ($genres_array as $key => $value) {
      $genres[] = $value['name'];
    }

    $relatedGenres = array(
      '#title' => 'Related Genres',
      '#collapsible' => FALSE,
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#description' => t('To remove a related genre, choose "- None -" from the drop-down list.'),
      '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
      );

    for ($rgi = 0; $rgi < 18; $rgi++) {
     $relatedGenreNext = array(
       '#type' => 'select',
       '#options' => $genres,
       '#empty_value' => '0',
       '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
       );
     if ($rgi > 2){
       $relatedGenreNext['#attributes'] = array(
         'style' => 'display: none;',
         );
     }
     $relatedGenres[] = $relatedGenreNext;
   }

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
    'artistName' => array(
      'name' => 'Artist Name',
      'drupalForm' => array(
        '#type' => 'textfield',
        '#maxlength' => 524288,
        '#required' => FALSE,
        '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
        ),
      ),
    'songTitle' => array(
      'name' => 'Song Title',
      'drupalForm' => array(
        '#type' => 'text_format',
        '#format' => 'full_html',
        '#required' => TRUE,
        '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
        ),
      ),
    'songDesc' => array(
      'name' => 'Song Description',
      'drupalForm' => array(
        '#type' => 'text_format',
        '#format' => 'full_html',
        '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
        ),
      ),
    'musicTrack' => array(
      'name' => 'Music Track',
      'drupalForm' => array(
        '#type' => 'textfield',
        '#maxlength' => 524288,
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'miss-file-manage',
            ),
          ),
        '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
        ),
      ),
    'albumCover' => array(
      'name' => 'Album Cover',
      'drupalForm' => array(
        '#type' => 'textfield',
        '#maxlength' => 524288,
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array(
            'miss-file-manage',
            ),
          ),
        '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
        ),
      ),
    'credit' => array(
      'name' => 'Album Cover Image Credit',
      'drupalForm' => array(
        '#type' => 'text_format',
        '#format' => 'full_html',
        '#required' => FALSE,
        '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
        ),
      ),
    'genre' => array(
      'name' => 'Genre',
      'drupalForm' => array(
        '#type' => 'select',
        '#options' => $genres,
        '#empty_value' => '0',
        '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
        ),
      ),
    'relatedGenre' => array(
      'drupalForm' => $relatedGenres,
      ),
    'musicDates' => array(
      'name' => 'Music Track Dates',
      'drupalForm' => array(
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#tree' => TRUE,
        'dateRelease' => array(
          '#title' => 'Date of Track Release',
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'writtenYear' => array(
          '#title' => 'Written Year',
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'performanceDate' => array(
          '#title' => 'Performance Date',
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'dateUsed' => array(
          '#title' => 'Date Used for Timeline',
          '#type' => 'select',
          '#options' => array(
            'Written Date' => 'Written Date',
            'Released Date' => 'Released Date',
            'Performance Date' => 'Performance Date'
            ),
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        ),
      ),
    'productionInformation' => array(
      'name' => 'Production / Performance Information',
      'drupalForm' => array(
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#tree' => TRUE,
        'label' => array(
          '#title' => 'Music Label',
          '#type' => 'text_format',
          '#format' => 'full_html',
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'songwriter' => array(
          '#title' => 'Songwriter',
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#description' => 'Field should be comma separated',
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'producer' => array(
          '#title' => 'Producer of Track',
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#description' => 'Field should be comma separated',
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'performer' => array(
          '#title' => 'Performer',
          '#type' => 'text_format',
          '#format' => 'full_html',
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'composer' => array(
          '#title' => 'Composer',
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'librettist' => array(
          '#title' => 'Librettist',
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'samplesSounds' => array(
          '#title' => 'Music Samples Used in Track',
          '#type' => 'text_format',
          '#format' => 'full_html',
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'show' => array(
          '#title' => 'Show',
          '#type' => 'text_format',
          '#format' => 'full_html',
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'region' => array(
          '#title' => 'Region',
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        ),
      ),
    'spotlight' => array(
      'name' => 'Spotlight',
      'drupalForm' => array(
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#tree' => TRUE,
        'title' => array(
          '#title' => 'Title',
          '#type' => 'text_format',
          '#format' => 'full_html',
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'imageUrl' => array(
          '#title' => 'Image URL',
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#attributes' => array(
            'class' => array(
              'miss-file-manage',
              ),
            ),
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'credit' => array(
          '#title' => 'Image Credit',
          '#type' => 'text_format',
          '#format' => 'full_html',
          '#required' => FALSE,
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'caption' => array(
          '#title' => 'Caption',
          '#type' => 'text_format',
          '#format' => 'full_html',
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        ),
      ),
    'influence' => array(),
    );

return $fields;
}

  /**
   *
   */
  public function getComponentTitle($component) {
    return (isset($component->content['songTitle']) ? $component->content['songTitle'] : FALSE);
  }


  /**
   *
   */
  public function getDrupalFormTitle($action, $component) {
    return parent::drupalFormTitle($action, $component, 'Music Track', $this->getComponentTitle($component));
  }


  /**
   *
   */
  public function getEdanComponent($id) {
    $data = $this->getContentComponents('music');

    if (!isset($data[$id])) {
      watchdog('miss_record_store', 'Could not load Record Store Music Track :id.', array(':id' => $id));
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
    $title = (isset($form_values['content']['songTitle']) && !empty($form_values['content']['songTitle'])) ? $form_values['content']['songTitle'] : 'No Title (empty title)';

    $interactive = $this->getInteractive();
    $genres_array = $interactive['content']['genres'];

    $query = array(
      'content' => array(),
      );

    foreach ($this->vars as $key => $value) {
      if(isset($value)) {
        switch ($key) {
          case 'id':
          $query[$key] = $value;
          break;

          case 'status':
          $query['status'] = ($value == 1 ? 0 : 1);
          break;

          case 'influence':
          $influences = array();
          foreach ($value as $influence) {
            if (!array_filter($influence)) {
              continue;
            }
            $influence_query = array();
            foreach ($influence as $key => $value) {
              if ($key == 'id') {
                $influence_query[$key] = $value;
              }
              elseif($key == 'credit') {
                $influence_query['content'][$key] = $value['value'];
              }
              else {
                $influence_query['content'][$key] = $value;
              }
            }

            $influence_query['content'] = json_encode($influence_query['content']);
            $uri = drupal_http_build_query($influence_query);

            $response = $this->edanRequest('setInfluence', $uri);

            if ($response['info']['http_code'] == 200) {
              $data = json_decode($response['data']);
              $influences[] = $data->id;
            }
            else {
              drupal_set_message(sprintf('There was an error processing your request: %s', $response['data']), 'error');
              return FALSE;
            }
          }
          break;

          // Remove text_format extras from Spotlight caption and title
          case 'label':
          if (isset($value['label']['value'])) {
            $query['label'] = $value['label']['value'];
          }
          break;
          case 'performer':
          if (isset($value['performer']['value'])) {
            $query['performer'] = $value['performer']['value'];
          }
          break;
          case 'samplesSounds':
          if (isset($value['samplesSounds']['value'])) {
            $query['samplesSounds'] = $value['samplesSounds']['value'];
          }
          break;
          case 'show':
          if (isset($value['show']['value'])) {
            $query['show'] = $value['show']['value'];
          }
          break;
          case 'spotlight':
          $caption = '';
          if (isset($value['caption']['value'])) {
            $caption = $value['caption']['value'];
          }
          $value['caption'] = $caption;
          $title = '';
          if (isset($value['title']['value'])) {
            $title = $value['title']['value'];
          }
          $value['title'] = $title;
          $credit = '';
          if (isset($value['credit']['value'])) {
            $credit = $value['credit']['value'];
          }
          $value['credit'] = $credit;

          $query['content'][$key] = $value;
          break;
          case 'genre':
            // Return the genre as the genre string.
          $query['content'][$key] = ($value !== '0') ? $genres_array[$value]['name'] : '';
          break;
          case 'relatedGenre':
          $relatedGenres = array_values(array_filter($value));
            // Loop through relatedGenres and return as genre strings.
          foreach($relatedGenres as $rkey => $rvalue) {
            $query['content'][$key][] = $genres_array[$rvalue]['name'];
          }
          break;
          default:
          $query['content'][$key] = $value;
          break;
        }
      }
    }

    // If deleting component, unset content.
    if (isset($query['status']) && $query['status'] === -1) {
      unset($query['content']);
    }

    if(isset($query['content'])) {
      $query['content'] = json_encode($query['content']);
    }

    $uri = drupal_http_build_query($query);
    $response = $this->edanRequest('setMusicObject', $uri);

    // EDAN throws a 404 when deleting. Let's hack it for now...
    // TODO: Ask Andrew about this anomaly, and see if it can be fixed.
    if (($response['info']['http_code'] !== 200) && ($query['status'] !== -1)) {
      drupal_set_message(sprintf('There was an error processing your request: %s', $response['data']), 'error');
      return FALSE;
    }
    $data = json_decode($response['data']);

    // Connect with the Influences array
    if (!empty($influences)) {
      $query = array(
        'id' => $data->id,
        'influence' => json_encode($influences),
        );
      $uri = drupal_http_build_query($query);
      $response = $this->edanRequest('setInfluenceArray', $uri);

      if ($response['info']['http_code'] !== 200) {
        drupal_set_message(sprintf('There was an error processing your request: %s', $response['data']), 'error');
        return FALSE;
      }
    }

    // If the record has not been deleted...
    if(!isset($query['status'])) {

      $interactive = $this->getInteractive();
      if (!in_array($data->id, $interactive['content']['music'])) {
        $music = array(
          'music' => $interactive['content']['music'],
          );
        $music['music'][] = $data->id;
        $music['music'] = json_encode($music['music']);
        $uri = drupal_http_build_query($music);
        $response = $this->edanRequest('setMusic', $uri);

        if ($response['info']['http_code'] !== 200) {
          drupal_set_message(sprintf('There was an error processing your request: %s', $response['data']), 'error');
          return FALSE;
        }
      }

      drupal_set_message( sprintf('Music Track "%s" has been saved.', strip_tags($data->content->songTitle) ));
      return TRUE;
    }

    if(isset($query['status']) && ($query['status'] === -1)) {

      drupal_set_message( sprintf('Music Track "%s" has been successfully deleted.', strip_tags($title) ));
      return TRUE;

    }

  }

  /**
   *
   */
  public function setDrupalFormFields(&$form, &$form_state, $component) {
    parent::setDrupalFormFields($form, $form_state, $component);

    if (isset($component->content['relatedGenre'])) {
      $num_found_genres = sizeof($component->content['relatedGenre']);
      for($genre_index = 0; $genre_index < $num_found_genres; $genre_index++) {
        if (!empty($form['relatedGenre'][$genre_index]['#attributes']['style'])) {
          $form['relatedGenre'][$genre_index]['#attributes']['style'] = str_replace(
            'display: none;',
            '',
            $form['relatedGenre'][$genre_index]['#attributes']['style']
            );
        }
      }
    }
    $influences = array();
    if (isset($component->content['influence']) && !empty($component->content['influence'])) {
      $influences = $component->content['influence'];
    }

    // Extend for Influences
    $form['influence_container'] = array(
      '#type' => 'fieldset',
      '#title' => 'Influences',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => '<p style="color:#ca9b0d;">&#10068 Each Music Track should contain no more than <b>4</b> unique influences.</p>',
      '#prefix' => '<div id="influences-fieldset-wrapper">',
      '#suffix' => '</div>',
      );

    if (empty($form_state['num_influences'])) {
      if (!empty($influences)) {
        $form_state['num_influences'] = count($influences);
      }
      else {
        $form_state['num_influences'] = 1;
      }
    }

    $form['influence_container']['influence'] = array(
      '#tree' => TRUE,
      );

    for ($i = 1; $i <= $form_state['num_influences']; $i++) {
      $influence = array();
      if (isset($influences[$i - 1])) {
        $influence = (array) parent::getEdanComponent($influences[$i - 1]);
      }

      $form['influence_container']['influence'][$i] = array(
        '#type' => 'fieldset',
        'title' => array(
          '#title' => 'Title',
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#default_value' => isset($influence['content']['title']) ? $influence['content']['title'] : '',
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'imageUrl' => array(
          '#title' => 'Image URL',
          '#type' => 'textfield',
          '#maxlength' => 524288,
          '#default_value' => isset($influence['content']['imageUrl']) ? $influence['content']['imageUrl'] : '',
          '#attributes' => array(
            'class' => array(
              'miss-file-manage',
              ),
            ),
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        'credit' => array(
          '#title' => 'Image Credit',
          '#type' => 'text_format',
          '#format' => 'full_html',
          '#required' => FALSE,
          '#default_value' => isset($influence['content']['credit']) ? $influence['content']['credit'] : '',
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          ),
        );

      if (isset($influence['id'])) {
        $form['influence_container']['influence'][$i]['id'] = array(
          '#type' => 'value',
          '#value' => $influence['id'],
          '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
          );
      }
    }

    $form['influence_container']['add_influence'] = array(
      '#type' => 'submit',
      '#value' => t('Add an influence'),
      '#submit' => array('_miss_record_store_edit_component_form__record_store__add_influence'),
      '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
      );

    if ($form_state['num_influences'] > 1) {
      $form['influence_container']['remove_influence'] = array(
        '#type' => 'submit',
        '#value' => t('Remove latest influence'),
        '#submit' => array('_miss_record_store_edit_component_form__record_store__remove_influence'),
        '#limit_validation_errors' => array(),
        '#disabled' => !user_access('edit record store content') ? TRUE : FALSE,
        );
    }

    $js = 'jQuery().ready(function(){
      jQuery(".filter-wrapper").hide();
    });';
    drupal_add_js($js, 'inline');
  }
}
