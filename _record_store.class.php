<?php

// Test from Gor - 2016-09-16 - for Git

namespace NMAAHC\MISS;

/**
 *
 */
class RecordStore {
  //
  private $components = array();

  //
  private $componentsByType = array();

  //
  private $interactive = array();

  //
  private $endpoints = array(
    'getComponents' => array(
      'path' => 'getComponents.htm',
      'post' => TRUE,
    ),
    'render' => array(
      'path' => 'render.htm',
      'post' => FALSE,
    ),
    'setEvent' => array(
      'path' => 'setEvent.htm',
      'post' => TRUE,
    ),
    'setInfluence' => array(
      'path' => 'setInfluence.htm',
      'post' => TRUE,
    ),
    'setInfluenceArray' => array(
      'path' => 'setInfluenceArray.htm',
      'post' => TRUE,
    ),
    'setMusic' => array(
      'path' => 'setMusic.htm',
      'post' => TRUE,
    ),
    'setMusicObject' => array(
      'path' => 'setMusicObject.htm',
      'post' => TRUE,
    ),
    'setTimeline' => array(
      'path' => 'setTimeline.htm',
      'post' => TRUE,
    ),
  );

  //
  private $endpoints_path = 'miss/v1.0/nmaahc/record_store';


  /**
   * TODO: Protect when done development
   */
  public function edanRequest($ept, $uri = '') {
    if (!$this->endpoints[$ept]) {
      watchdog('miss_record_store', 'Record Store endpoint :endpoint not defined.', array(':endpoint', $ept));
      // TODO return something else?
      return array();
    }

    $endpoint = $this->endpoints_path . '/' . $this->endpoints[$ept]['path'];

    $edan = edan();
    $info = null;
    $response = $edan->sendRequest($uri, $endpoint, $this->endpoints[$ept]['post'], $info);

    return array(
      'info' => $info,
      'data' => $response,
    );
  }


  /**
   *
   */
  public function getContentComponents($type) {
    $data = &$this->componentsByType;

    // Store components for reuse
    if (empty($this->componentsByType)) {
      $components = $this->GetEdanComponents();

      foreach ($components as $id => $component) {
        if ($component['type'] != 'miss_component') {
          continue;
        }

        if (isset($component['content']['subType'])) {
          $subtype = $component['content']['subType'];
          $data[$subtype][$id] = $component['content'];
        }

        $data[$id] = $component['content'];
      }
    }

    if (!isset($data[$type])) {
      watchdog('miss_record_store', 'Could not load Record Store :type content components.', array(':type', $type));
      return array();
    }

    // Need to massage (fix) genres due to structursal changes with the JSON coming out of EDAN.
    $interactive = $this->getInteractive();
    $genres_array = $interactive['content']['genres'];

    foreach($data[$type] as $dkey => $dval) {
      if(isset($dval['relatedGenre'])) {
        foreach($dval['relatedGenre'] as $gkey => $gval) {
          if(is_numeric($gval)) {
            $data[$type][$dkey]['relatedGenre'][$gkey] = $genres_array[$gval]['name'];
          } else {
            foreach ($genres_array as $genres_key => $genres_value) {
              if($genres_value['name'] === $gval) {
                $data[$type][$dkey]['relatedGenre'][$gkey] = $genres_value['name'];
              }
            }
          }
        }
        $genre = $data[$type][$dkey]['genre'];
        $data[$type][$dkey]['genre'] = is_numeric($genre) ? $genres_array[$genre]['name'] : $genre;
      }
    }

    return $data[$type];
  }


  /**
   * @return
   *   An associative array of MISS components, keyed by their ID.
   */
  protected function getEdanComponents() {
    // Store components for reuse
    if (!empty($this->components)) {
      return $this->components;
    }

    $response = $this->edanRequest('getComponents');
    $data = json_decode($response['data'], TRUE);
    if ($response['info']['http_code'] !== 200 || !isset($data['components'])) {
      watchdog('miss_record_store', 'Could not load Record Store components.');
      return array();
    }

    $this->components = $data['components'];
    return $this->components;
  }


  /**
   *
   */
  public function getEdanComponent($id) {
    $request = $this->getEdanComponents();

    if (!isset($request[$id])) {
      watchdog('miss_record_store', 'Could not load Record Store component :id.', array(':id' => $id));
      return FALSE;
    }

    // Need to massage (fix) genres due to structursal changes with the JSON coming out of EDAN.
    $interactive = $this->getInteractive();
    $genres_array = $interactive['content']['genres'];

    // Hit the genre.
    $genre = isset($request[$id]['content']['genre']) ? $request[$id]['content']['genre'] : false;

    if($genre && !is_numeric($genre)) {
      foreach ($genres_array as $genres_key => $genres_value) {
        if($genres_value['name'] === $genre) {
          $request[$id]['content']['genre'] = $genres_key;
        }
      }
    }

    // Hit the relatedGenre.
    if(isset($request[$id]['content']['relatedGenre'])) {
      foreach($request[$id]['content']['relatedGenre'] as $gkey => $gvalue) {
        foreach ($genres_array as $genres_key => $genres_value) {
          if($genres_value['name'] === $gvalue) {
            $request[$id]['content']['relatedGenre'][$gkey] = $genres_key;
          }
        }
      }
    }

    return (object) $request[$id];
  }


  /**
   * TODO: Switch to protected once done dev.
   */
  public function getInteractive() {
    // Store components for reuse
    if (!empty($this->interactive)) {
      return $this->interactive;
    }

    $data = $this->getEdanComponents();

    if (empty($data)) {
      watchdog('miss_record_store', 'Could not load Record Store interactive components.');
      return array();
    }

    $data = array_filter($data, function($value) {
      return ($value['type'] == 'miss_interactive');
    });

    if (count($data) > 1) {
      watchdog('miss_record_store', 'Multiple interactive components found for Record Store.');
      return array();
    }

    $this->interactive = array_shift($data);

    // Genres need a little more love due to structural changes to the JSON data.
    $none = array('name' => '- None -', 'description' => '');
    array_unshift($this->interactive['content']['genres'], $none);

    return $this->interactive;
  }
}


/**
 *
 */
abstract class RecordStoreComponent extends RecordStore {
  abstract public function fields();
  abstract public function getComponentTitle($component);
  abstract public function getDrupalFormTitle($action, $component);
  abstract public function setComponent($form_state);

  protected $vars = array();


  /**
   *
   */
  public function __construct() {
    foreach ($this->fields() as $key => $field) {
      $this->vars[$key] = null;
    }
  }


  /**
   *
   */
  protected function drupalFormTitle($action, $component, $type, $name) {
    switch ($action) {
      case 'edit':
        if (property_exists($component, 'id')) {
          $title = sprintf('Edit %s "%s"', $type, $name);
        }
        else {
          $title = 'Create ' . $type;
        }
        break;

      case 'delete':
        $title = sprintf('Delete %s "%s"', $type, $name);
        break;
    }

    return $title;
  }


  /**
   *
   */
  public function setDrupalFormFields(&$form, &$form_state, $component) {
    $fields = $this->fields();

    foreach ($fields as $key => $field) {
      if (!isset($field['drupalForm'])) {
        continue;
      }

      $form[$key] = $field['drupalForm'];

      // Process each fieldset item individually
      if ($field['drupalForm']['#type'] == 'fieldset') {
        foreach ($field['drupalForm'] as $fieldset_key => $fieldset_value) {
          if (substr($fieldset_key, 0, 1) == '#') {
            continue;
          }

          $form[$key][$fieldset_key] += array(
            '#default_value' => !empty($component->content[$key][$fieldset_key]) ? $component->content[$key][$fieldset_key] : '',
          );
        }
      }
      else {

        switch ($key) {
          case 'status':
            $form[$key] += array(
              '#default_value' => isset($component->status) ? $component->status : 0,
            );
            break;

          case 'albumCover':
            $form[$key] += array(
              '#default_value' => !empty($component->content[$key]) ? $component->content[$key] : '',
              '#suffix' => !empty($component->content[$key]) ? '<div style="margin-bottom: 1.5rem;"><a href="' . $component->content[$key] . '" target="_blank">View Image</a> (opens in a new window/tab)</div>' : '',
            );
            break;

          default:
            $form[$key] += array(
              '#default_value' => !empty($component->content[$key]) ? $component->content[$key] : '',
            );
            break;
        }
      }

      if (isset($field['name'])) {
        $form[$key]['#title'] = $field['name'];
      }
    }

  }


  /**
   *
   */
  public function setPropertiesFromDrupalForm($form_state) {
    $fields = $this->fields();

    foreach ($form_state['values'] as $key => $field) {
      if (!isset($fields[$key])) {
        continue;
      }

      if (isset($form_state['values'][$key]['value'])) {
        $this->vars[$key] = $form_state['values'][$key]['value'];
      }
      else {
        $this->vars[$key] = $form_state['values'][$key];
      }
    }
  }
}
