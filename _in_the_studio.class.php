<?php

/**
 * Base NMAAHC object for fetching record data from EDAN.
 */
class NMAAHC {
  var $errors = array();

  static function interactives() {
    return array(
      'in_the_studio' => array(
        'title' => 'In the Studio',
        'endpoints' => array(
          'saveOrder' => 'miss/v1.0/nmaahc/in_the_studio/setSoundAssets.htm',
          'saveSound' => 'miss/v1.0/nmaahc/in_the_studio/setSoundAsset.htm',
          'render' => 'miss/v1.0/nmaahc/in_the_studio/getComponents.htm',
          'realRender' => 'miss/v1.0/nmaahc/in_the_studio/render.htm',
        )
      )
    );
  }

  /**
   * Fetches the JSON of a supplied interactive.
   * @param $interactive
   * @return FALSE on failure; String on successful connection (JSON still needs to be evaluated)
   */
  function get_interactive($interactive) {
    $interactives = self::interactives();

    if (!isset($interactives[$interactive]['endpoints']['render'])) {
      return FALSE;
    }

    // Use this function to fix any whacked-out situations with sound assets.
    // reset_sound_assets();

    $endpoint = $interactives[$interactive]['endpoints']['render'];
    $edan = edan();
    $info = '';
    $result = $edan->sendRequest('renderAsArray=true', $endpoint, FALSE, $info);

    if (!isset($info['http_code'])) {
      $this->errors[] = '$interactive: ' . $interactive;
      $this->errors[] = '$info: ' . var_export($info, TRUE);
      $this->errors[] = '$result: ' . var_export($result, TRUE);
      #return FALSE; # TODO: 2016-03-28 info was not being populated, seems like it was cached? anyway returning false was not helping
    }
    else if ($info['http_code'] !== 200) {
      $this->errors[] = $endpoint . ' [' . $info['http_code'] . ']';
    }

    return $result;
  }

  /**
   * Fix any whacked-out situations with sound assets.
   */
  function reset_sound_assets() {

    // Supply the array of sound asset ids.
    $soundAssets = array(
      // EXAMPLES
      // 'abc-1468855733306-1469032575168-0',
      // 'abc-1464189887921-1464308699645-0',
      // 'abc-1464189887921-1464309140382-0',
    );

    $query = array();
    $query[] = sprintf('soundAssets=%s', urlencode(json_encode($soundAssets)));
    $query = implode('&', $query);

    $endpoint = 'miss/v1.0/nmaahc/in_the_studio/setSoundAssets.htm';
    $edan = edan();
    $info = '';
    $result = $edan->sendRequest($query, $endpoint, FALSE, $info);

    $data = json_decode($result, TRUE);

    dumper($data);
  }
}


/**
 * Base NMAAHC Component Class
 */
class NMAAHC_Component {}


/**
 * Provides In The Studio functionality to NMAAHC objects.
 */
class InTheStudio extends NMAAHC {
  public $cats = array(
    'melodies',
    'rhythms', // Replacement for beats
    'harmonies',
    'fills', // Replacement for sound_effects
    'pop-ups',
  );

  public $ordered = array();
  public $components = array();
  public $categories = array(); // Component refs grouped by category

  public $json = '';
  public $array = array();

  public function __construct() {
    $this->json = $this->get_interactive('in_the_studio');

    $this->array = json_decode($this->json, TRUE);

    // Temp array to store all components, there is a weird need to sort published elements
    // but we get back all elements, so we need to store them somewhere, sort them, then
    // map them to the object
    $temp_components1 = array();

    // Skip non-component elements
    foreach ($this->array['components'] as $component) {
      if (!isset($component['type'])) {
        continue; // Should this ever happen?
      }

      if ($component['type'] == 'miss_component') {
        $id = $component['id'];
        $temp_components1[$id] = $obj = new InTheStudio_Component();
        InTheStudio_Component::component_to_object($component, $temp_components1[$id]);
        #$this->categories[ $this->components[$id]->category ][] = &$this->components[$id];
      }
      else if ($component['type'] == 'miss_interactive') {
        $this->ordered = $component['content']['soundAssets'];
      }
    }

    // Grab all elements in order
    foreach ($this->ordered as $id) {
      if(!empty($id)) {
        $this->components[$id] = $temp_components1[$id];
        $this->categories[ $this->components[$id]->category ][] = &$this->components[$id];
      }
    }

    // Grab the rest
    $all_keys = array_keys($temp_components1);
    foreach ($temp_components1 as $id => $obj) {
      if (!in_array($id, $this->ordered)) {
        $this->components[$id] = $temp_components1[$id];
        $this->categories[ $this->components[$id]->category ][] = &$this->components[$id];
      }
    }
  }

  /**
   * Returns a specific component by it's ID.
   * @return InTheStudio_Component object.
   */
  public function byID($id) {
    if (isset($this->components[$id])) {
      return $this->components[$id];
    }

    return NULL;
  }

  /**
   *
   */
  public function reorder($order, &$edan) {
    // Strip status:1 from array before sending
    $new_order = array();

    foreach ($order as $id) {
      if ($this->components[$id]->status === 0) {
        $new_order[] = $id;
      }
    }
    // List of interactives
    $interactives = NMAAHC::interactives();

    // To querystring, this is a JSON array
    $query = array();
    $query[] = sprintf('soundAssets=%s', urlencode(json_encode($new_order)));

    $query = implode('&', $query);

    // Get our endpoint
    $endpoint = $interactives['in_the_studio']['endpoints']['saveOrder'];

    $info = '';
    $result = $edan->sendRequest($query, $endpoint, TRUE, $info);

    return array('info' => $info, 'data' => $result);
  }
}


/**
 * http://edan.si.edu/miss/v1.0/nmaahc/in_the_studio/setComponent.htm
 *
 * Field               Type      Description
 * id (optional)       String    The id of the component. used for editing.
 * title               String    The title of the component.
 * icon                String    The url of the icon.
 * popUpTitle          String    The popUpTitle.
 * popUpText           String    The popUpText.
 * category            String    The category [sound_effects, beats, harmonies].
 * loop                Boolean   Determing if component is loopable [true,false].
 */
class InTheStudio_Component extends NMAAHC_Component {
  var $id = ''; // String
  var $title = ''; // String: Rich-text?
  var $icon = ''; // String: URI
  var $iconCredit = ''; //String: rich text
  var $popUpText = ''; // String: Rich-text?
  var $category = ''; // String
  var $audioClips = ''; // Array: tempo, key, url
  var $loop = FALSE;
  var $status = 1; // Int: -1, 0, 1; 0 = published

  /**
   * Flips status between 0 and 1 depending on the current state.
   */
  public function mute(&$edan) {
    if (empty($this->id)) {
      return array();
    }

    // List of interactives
    $interactives = NMAAHC::interactives();

    // Flips status based on current status:
    $this->status = ($this->status == 0) ? 1 : 0;

    // To querystring, we want to skip id if it is empty
    $query = $this->to_params();
    $query = implode('&', $query);

    // Get our endpoint
    $endpoint = $interactives['in_the_studio']['endpoints']['saveSound'];

    $info = '';
    $result = $edan->sendRequest($query, $endpoint, TRUE, $info);

    return array('info' => $info, 'data' => $result);
  }

  /**
   * Sets the status to -1 (marked for deletion).
   */
  public function delete(&$edan) {

    if(empty($this->id)) {
      return array();
    }

    // List of interactives
    $interactives = NMAAHC::interactives();
    // The query
    $query = 'id=' . $this->id . '&status=' . $this->status;
    // Get the endpoint
    $endpoint = $interactives['in_the_studio']['endpoints']['saveSound'];
    // Send the API request
    $info = '';
    $result = $edan->sendRequest($query, $endpoint, TRUE, $info);

    return array('info' => $info, 'data' => $result);
  }

  public function save(&$edan) {

    // List of interactives
    $interactives = NMAAHC::interactives();

    // To querystring, we want to skip id if it is empty
    $query = $this->to_params();
    $query = implode('&', $query);
    // Get the endpoint
    $endpoint = $interactives['in_the_studio']['endpoints']['saveSound'];
    // Send the API request
    $info = '';
    $result = $edan->sendRequest($query, $endpoint, TRUE, $info);

    return array('info' => $info, 'data' => $result);
  }

  /**
   * Deal with params
   */
  public function to_params() {
    $query = array();

    foreach ($this as $k => $v) {
      if ($k == 'id') {
        if (!empty($this->id)) {
          $query[] = sprintf('%s=%s', $k, $v);
        }
      } else if ($k == 'status') {
        if (!in_array((int) $v, array(0, 1))) {
          $v = 1;
        }
        $query[] = sprintf('%s=%s', $k, $v);
      } else {
        $query[] = sprintf('%s=%s', $k, urlencode($v));
      }
    }

    return $query;
  }

  /**
   * Sets an object with values from an array.
   * @param array
   * @param object reference
   * @return object or NULL on failure
   */
  public static function component_to_object($component = array(), &$obj) {
    if (!is_array($component)) {
      return NULL;
    }

    if (!isset($component['type']) || $component['type'] != 'miss_component') {
      return NULL;
    }

    $obj->id = $component['id'];
    $obj->title = $component['content']['title'];
    $obj->icon = $component['content']['icon'];
    $obj->iconCredit = isset($component['content']['iconCredit']) ? $component['content']['iconCredit'] : '';
    $obj->popUpText = $component['content']['popUpText'];
    $obj->category = $component['content']['category'];
    $obj->audioClips = $component['content']['audioClips'];
    $obj->loop = (bool) $component['content']['loop'];
    $obj->status = (int) $component['status'];

    return $obj;
  }
}

