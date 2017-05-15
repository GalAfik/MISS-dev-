<?php

/**
 * /content/v1.1/fileupload/upload.htm
 * /content/v1.1/fileupload/getFileList.htm
 * /content/v1.1/fileupload/getToken.htm
 * /content/v1.1/admincontent/releaseContent.htm
 *
    Endpoints:
           upload
           getFileList
           getToken
       A token record is created for every valid request
       Tokens are 128bit encrypted strings that contain:
           AppID -
           TimeCreated -
           ID of db row -
       Tokens are checked against that record when using the upload end point
       Always expect a json response with a "success" value boolean, and if 0 then there will be a "message" value describing why it failed
       Service renames uploaded file with a hash, with the original file name stored in DB record. For example, image.png will be renamed b5740832-f79e-4b53-a554-19464173935c.png
       It is expected that the user will go through Apache to get the file. So, as an example, once uploaded a user can get to it by <url>/files/b5740832-f79e-4b53-a554-19464173935c.png
       All files will be put under the same directory as specified by the config
       Configuration includes:
           File Upload Path
           Token Expiration (in minutes)
           Max Upload Size
           File URL (domain just for display purposes)
       Modifications:
           Apache:
               Any requests to /content/v1.1/fileupload/upload.htm should be passed through to jetty-edan
               <site>:<port>/edan/content/v1.1/fileupload/upload.htm, rather than going to MDS for authentication
 *
 */


/**
* File Listing callback returns JSON
*/
function _miss_file_listing_callback() {

  $start = filter_input(INPUT_GET, 'start', FILTER_SANITIZE_STRING);
  $length = filter_input(INPUT_GET, 'length', FILTER_SANITIZE_STRING);
  $sort = (isset($_GET['order']) && !empty($_GET['order'])) ? (int)$_GET['order'][0]['column'] : '';
  $sortDir = (isset($_GET['order']) && !empty($_GET['order'])) ? (string)$_GET['order'][0]['dir'] : '';

  $info = '';
  $result = _miss_get_file_list((int)$start, (int)$length, $sort, $sortDir, $info);

  //  dumper(count($result['rows']),0);
  //  dumper($result['rowCount']);

  // dumper($result);

  $data['data'] = $result['rows'];
  $data['recordsTotal'] = count($result['rows']);
  $data['recordsFiltered'] = $result['rowCount'];

  echo json_encode($data);
  exit();
}

 // http://nmaahc.geekt.in/miss/files/ajax?draw=1&columns%5B0%5D%5Bdata%5D=0&columns%5B0%5D%5Bname%5D=&columns%5B0%5D%5Bsearchable%5D=true&columns%5B0%5D%5Borderable%5D=true&columns%5B0%5D%5Bsearch%5D%5Bvalue%5D=&columns%5B0%5D%5Bsearch%5D%5Bregex%5D=false&columns%5B1%5D%5Bdata%5D=1&columns%5B1%5D%5Bname%5D=&columns%5B1%5D%5Bsearchable%5D=true&columns%5B1%5D%5Borderable%5D=true&columns%5B1%5D%5Bsearch%5D%5Bvalue%5D=&columns%5B1%5D%5Bsearch%5D%5Bregex%5D=false&columns%5B2%5D%5Bdata%5D=2&columns%5B2%5D%5Bname%5D=&columns%5B2%5D%5Bsearchable%5D=true&columns%5B2%5D%5Borderable%5D=true&columns%5B2%5D%5Bsearch%5D%5Bvalue%5D=&columns%5B2%5D%5Bsearch%5D%5Bregex%5D=false&order%5B0%5D%5Bcolumn%5D=2&order%5B0%5D%5Bdir%5D=asc&start=0&length=5&search%5Bvalue%5D=&search%5Bregex%5D=false&_=1485458103080

function _miss_file_upload_page_callback() {

  //check for permission to upload files and kick out anyone without permissions
   if(!user_access('upload files')){
    drupal_access_denied();
   }

  // Add jQuery UI Tabs.
  drupal_add_library('system','ui.tabs');

  // Add DataTables CSS and Javascript.
  drupal_add_css(
    '//cdnjs.cloudflare.com/ajax/libs/datatables/1.10.11/css/jquery.dataTables.min.css',
    array('group' => CSS_THEME, 'type' => 'external', 'weight' => 50)
  );
  drupal_add_js(
    '//cdnjs.cloudflare.com/ajax/libs/datatables/1.10.11/js/jquery.dataTables.min.js',
    array('scope' => 'footer', 'type' => 'external', 'weight' => 50)
  );
  // Required file manangement JS
  drupal_add_js(
    drupal_get_path('module', 'miss') . '/js/_miss_file_manage.js',
    array('scope' => 'footer', 'weight' => 51)
  );
  drupal_add_js(
    drupal_get_path('module', 'miss') . '/js/datatables_directives.js',
    array('scope' => 'footer', 'weight' => 52)
  );

  $output = '<div id="tabs">';
  $output .= '<ul><li><a href="#file-tab">'.t('Files').'</li><li><a href="#edan-search">'.t('EDAN').'</a></li></ul>';
  $output .= '<div id="file-tab">';
  $output .= _miss_file_upload_only_callback();
  $output .= _miss_file_list_as_table();
  $output .= '</div>';
  $output .= '<div id="edan-search">';
  $edan_search_form = drupal_get_form('_miss_edan_search_form');
  $output .= render($edan_search_form);

  # TODO: Fix this up to use the edan lib search routines
  $edan = edan();
  $endpoint = 'metadata/v1.0/metadata/search.htm';
  $info = '';
  $uri_query = empty($_GET['edan_search']) ? NULL : $_GET['edan_search'];

  if ($uri_query != NULL) {
    $fqs = array(
      'type:edanmdm',
      'online_media_type:"Images"'
    );

    $uri = 'fqs=' . json_encode($fqs) . '&rows=20&q='.urlencode($uri_query);

    $result = $edan->sendRequest($uri, $endpoint, FALSE, $info);
    $edan_results = json_decode($result, TRUE);
    $output .= '<table>';

    foreach($edan_results['items'] as $item) {
      $online_medias = array();
      $rowspan = 1;

      if (!empty($item['content']['descriptiveNonRepeating']['online_media']['mediaCount'])) {
        $rowspan = $item['content']['descriptiveNonRepeating']['online_media']['mediaCount'];
      }

      $output .= '<tr>';
      $output .= '<td colspan="' . $rowspan . '">';
      $output .= '<strong>' . $item['content']['descriptiveNonRepeating']['title']['content'] . '</strong> | ';
      $output .= _miss_link_yourself($item['content']['descriptiveNonRepeating']['record_link'], 'View');
      #$output .= '</td>';
      #$output .= '<td colspan="3">'._miss_link_yourself($item['content']['descriptiveNonRepeating']['record_link']).'</td>';

      if ($rowspan > 1) {
        #$output .= '</tr><tr>';
        $output .= '<div style="display: -webkit-flex; display: flex; -webkit-flex-wrap: wrap; flex-wrap: wrap;">';
        foreach($item['content']['descriptiveNonRepeating']['online_media']['media'] as $curr_media) {
          $output .= '<div style="width: 120px; margin: 10px;">';
          $output .= '<img src="' . $curr_media['thumbnail'] . '" alt="" style="max-width: 120px;"><br>';
          $output .= _miss_create_insert_link($curr_media['content']);
          $output .= ' | <small>' . _miss_link_yourself($curr_media['content'], 'View') . '</small>';
          #$output .= '<td>' . ($curr_media['type']) . '</td>';
          $output .= '</div>';
        }
        $output .= '</div>';
      }

      $output .= '</tr>';
    }
    $output .= '</table>';
  }

  $output .= '</div>'; // Close edan-search tab
  $output .= '</div>'; // Close the tabs

  drupal_add_js ( 'jQuery(document).ready(function(){
    jQuery("#tabs").tabs();
    if (window.opener != null) {
      jQuery(".insert-button").click(function(){
        window.opener.missPerformInsert(window.name, jQuery(this).attr("insert-link"));
      });
    }
  });' , 'inline' );
  drupal_add_js(
    drupal_get_path('module', 'miss') . '/js/_miss_file_image_rotate_ui.js',
    array('scope' => 'footer', 'weight' => 3)
  );
  drupal_add_js(
    drupal_get_path('module', 'miss') . '/js/_miss_file_delete.js',
    array('scope' => 'footer', 'weight' => 4)
  );
  return $output;
}
/**
 * Obtains a token to use in order to do a file uploadA
 *
 * @param $display_drupal_message_on_error
 *   default TRUE
 *   If TRUE will display a message in Drupal when there is an error getting token and return NULL
 *   If FALSE will not display message when there is an error getting token and return NULL
 *
 * @returns a string token to use or NULL if problems occurred
 */
function _miss_get_token($display_drupal_message_on_error = TRUE) {

  // For file stuff, we need to point to production.
  $edan_server = 'http://edan.si.edu/';
  $edan_auth_key = variable_get('edan_auth_key');
  $edan_tier_type = variable_get('edan_tier_type');
  $edan_app_id = variable_get('edan_app_id');
  $edan = new EDANInterface($edan_server, $edan_app_id, $edan_auth_key, $edan_tier_type);

  $endpoint = 'content/v1.1/fileupload/getToken.htm';

  $info = '';
  $result = $edan->sendRequest('', $endpoint, FALSE, $info);
  $token_object = json_decode($result);

  if ($token_object->success !== 1 && $token_object->success !== '1') {
    if ($display_drupal_message_on_error) {
      drupal_set_message($token_object->message);
    }
    return NULL;
  }
  return $token_object->token;
}

/*
 *
 */
function _miss_file_upload_only_callback() {
  $form = drupal_get_form('_miss_file_upload_form');
  $output = render($form);
  return $output;
}

/**
 * After the form is submitted, redirect to a page which will show the results of the search
 *
 * Pull the only pertinent information out of the POST, put as parameter
 * and add the fragment so the proper tab shows up
 */
function _miss_edan_search_form_submit($form, &$form_state) {
  $edan_search = empty($_POST['edan_search']) ? NULL : $_POST['edan_search'];
  $form_state['redirect'] = array(
    $form['#action'],
    array('query' => array(
      'edan_search' => $edan_search,
    ),
   'fragment' => 'edan-search'),
  );
}
/**
 * EDAN search form
 *
 * The form is using POST so that all of the other form information (like form_id) does not appear in the URL,
 * but the methods still use the parameter information from the URL to do the search. The params
 * are added in the form_submit method
 */
function _miss_edan_search_form($form, &$form_state) {
  $form['edan_search'] = array(
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 400,
    '#default_value' => empty($_GET['edan_search']) ? NULL : $_GET['edan_search']
  );
  $form['submit_button'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );
  $form['#action'] = strtok($_SERVER["REQUEST_URI"],'?'); //POST to current URL without parameters
  return $form;
}
/**
 * hook_form()
 */
function _miss_file_upload_form($form, &$form_state) {

  _miss_show_content_only();

  $form = array();

  // Get token, if we fail indicate it and stop rendering the form.
  $edan = edan();
  $token = _miss_get_token(FALSE);
  if ($token === NULL) {
    $form['item'] = array(
      '#type' => 'item',
      '#markup' => t('Could not get a token to upload to the server'),
      '#title' => 'Error',
    );
    return $form;
  }

  $form['#action'] = url('http://review:600Maryland@edan.si.edu/miss/nmaahc/upload.htm', array('external' => true));

  $form['#attributes'] = array('enctype' => "multipart/form-data", 'target' => '_blank');

  $form['token'] = array(
    '#type' => 'hidden',
    '#value' => $token,
  );

  $form['fileField'] = array(
    '#type' => 'item',
    '#markup' => '<input type="file" name="fileField">',
    '#title' => 'File to Upload', // TODO: Are there HTML5 attributes that can tie into mime/extension/size limits?
  );

  $form['submit'] = array(
    '#type' => 'item',
    '#markup' => '<input type="submit" name="submit" value="Upload" class="button">',
  );

  drupal_add_js ( 'jQuery(document).ready(function(){
    /*
     * The edan upload uses the "name" attribute of the file uploaded to set the "original name"
     * So this will set the name attribute to the name of the file being uploaded
     */
    jQuery("#edit-filefield input").change(function() {
      var filename = jQuery(this).val();
      var lastIndex = filename.lastIndexOf("\\\\");
      if (lastIndex >= 0) {
          filename = filename.substring(lastIndex + 1);
      }
      jQuery("#edit-filefield input").attr("name",filename);
    });
    jQuery("#-miss-file-upload-form").submit(function(){
      setTimeout(function(){window.location.reload();},1000);
    });
  });' , 'inline' );

  return $form;
}

function _miss_show_content_only(){
  drupal_add_css( '#header, #breadcrumb, #sidebar-first, #footer-wrapper { display: none; }', array('group' => CSS_THEME, 'type' => 'inline') );
  drupal_add_js ( 'jQuery(document).ready(function(){
    //Show what I want to
    var selector = "#content";
    jQuery(selector).parents().show();
    jQuery(selector).show();
    jQuery(selector).find("*").show();
  });' , 'inline' );
}

/**
 * hook_form_validate()
 */
function _miss_file_upload_form_validate($form, &$form_state) {
  return;
  $file = file_save_upload('files_list_wrapper', array(
  ));
  // If the file passed validation:
  if ($file) {
    /*
    // Move the file into the Drupal file system.
    if ($file = file_move($file, 'public://')) {
      // Save the file for use in the submit handler.
    */
      $form_state['storage']['file'] = $file;
    /*
    }
    else {
      form_set_error('file', t("Failed to write the uploaded file to the site's file folder."));
    }
    */
  }
  else {
    form_set_error('file', t('No file was uploaded.'));
    var_dump($_FILES['files']['name']);
  }
}

/**
 * hook_form_submit()
 */
function _miss_file_upload_form_submit($form, &$form_state) {
  //extract data from the post
  //set POST variables
  $url = 'http://domain.com/get-post.php';

  $fields = array(
    'fileField' => ($_POST['fileField']),
    'fname' => urlencode($_POST['first_name']),
    'title' => urlencode($_POST['title']),
    'company' => urlencode($_POST['institution']),
    'age' => urlencode($_POST['age']),
    'email' => urlencode($_POST['email']),
    'phone' => urlencode($_POST['phone'])
  );

  //url-ify the data for the POST
  foreach($fields as $key => $value) {
    $fields_string .= $key.'='.$value.'&';
  }

  rtrim($fields_string, '&');

  //open connection
  $ch = curl_init();

  //set the url, number of POST vars, POST data
  curl_setopt($ch,CURLOPT_URL, $url);
  curl_setopt($ch,CURLOPT_POST, count($fields));
  curl_setopt($ch,CURLOPT_POSTFIELDS, $fields_string);

  //execute post
  $result = curl_exec($ch);

  //close connection
  curl_close($ch);

  return;

  // Eventually allow this to be local-only:true
  #$file = $form_state['storage']['file'];

  // TODO: use EDAN configuration
}

/**
 * Obtain a file list and give it back as an associative array
 * @param &$info
 *   Object to put curl information into
 *
 * @return
 *   Returns an associative array of the file listing or NULL if
 *   problems occurred.
 *   A expected return would be a jsondecode of something like this:
{
  "success": "1",
  "count": 3,
  "files": [
    {
      "filescanner": [
        {
          "scanDate": "2016-04-07 12:33:34.744",
          "scanType": "antivirus",
          "scanResult": "ERROR"
        }
      ],
      "originalName": "image",
      "fileLocation": "/data/files/MISS_NMAAHC/1cef9eb7-5919-48a6-b3b9-c5c42b3f22ec.ogg",
      "fileSize": 8,
      "fileType": "OGG",
      "mimeType": "application/octet-stream",
      "fileId": "1cef9eb7-5919-48a6-b3b9-c5c42b3f22ec.ogg",
      "filepath": "http://edan.si.edu/file/MISS_NMAAHC/1cef9eb7-5919-48a6-b3b9-c5c42b3f22ec.ogg"
    },
    {
      "filescanner": [
        {
          "scanDate": "2016-04-07 12:45:58.108",
          "scanType": "antivirus",
          "scanResult": "ERROR"
        }
      ],
      "originalName": "image",
      "fileLocation": "/data/files/MISS_NMAAHC/9d07154e-9bcc-47fe-ab4e-17edd98033d4.ogg",
      "fileSize": 8,
      "fileType": "OGG",
      "mimeType": "application/octet-stream",
      "fileId": "9d07154e-9bcc-47fe-ab4e-17edd98033d4.ogg",
      "filepath": "http://edan.si.edu/file/MISS_NMAAHC/9d07154e-9bcc-47fe-ab4e-17edd98033d4.ogg"
    },
    {
      "filescanner": [
        {
          "scanDate": "2016-04-07 13:36:57.467",
          "scanType": "antivirus",
          "scanResult": "ERROR"
        }
      ],
      "originalName": "image",
      "fileLocation": "/data/files/MISS_NMAAHC/1c63f562-faa1-4f90-bdd9-ca4802243a2d.ogg",
      "fileSize": 8,
      "fileType": "OGG",
      "mimeType": "application/octet-stream",
      "fileId": "1c63f562-faa1-4f90-bdd9-ca4802243a2d.ogg",
      "filepath": "http://edan.si.edu/file/MISS_NMAAHC/1c63f562-faa1-4f90-bdd9-ca4802243a2d.ogg"
    }
  ]
}
 *
 *
 */
 function _miss_get_file_list($start, $length, $sort = 'timestamp', $sortDir = 'desc', &$info = NULL) {

  if ($info === NULL) $info = '';

  // For file listings, we need to point to production.
  $edan_server = (($_SERVER['HTTP_HOST'] === 'dev.miss.dev1.myquotient.net') || ($_SERVER['HTTP_HOST'] === 'nmaahc.geekt.in')) ? 'http://edandev.si.edu/' : 'http://edan.si.edu/';
  $edan_auth_key = variable_get('edan_auth_key');
  $edan_tier_type = variable_get('edan_tier_type');
  $edan_app_id = variable_get('edan_app_id');
  $edan = new EDANInterface($edan_server, $edan_app_id, $edan_auth_key, $edan_tier_type);

  // Parameters:
  // start, rows, sort, sortDir...
  // Acceptable sort parameter arguments:
  // title, url, id, last_time_updated, timestamp, unit_code, status
  // Example:
  // start=0&rows=50&sort=timestamp&sortDir=desc
  // TODO: Need to add search, if possible... example:
  // start=0&rows=50&sort=timestamp&sortDir=desc&q=place_search_query_here
  $endpoint = 'content/v1.1/fileupload/getFileRows.htm';
  $uri = 'start=' . $start . '&rows=' . $length . '&sort=' . $sort . '&sortDir=' . $sortDir;

  $result = $edan->sendRequest($uri, $endpoint, FALSE, $info);

  if (empty($result)) {
   return NULL;
  } else {
   return json_decode($result, TRUE);
  }

}

/**
 * Delete a file asset from EDAN's File Services.
 * Supports the filepath parameter using a manual function call or a $GET-based filepath parameter (for AJAX calls).
 * @param $filepath_string
 *   The file path string used to obtain the file ID (EDAN field: fileId).
 *
 * @param $_GET['filepath_string']
 *   The GET file path string used to obtain the file ID  (EDAN field: fileId).
 *
 * @return
 *   Returns either an EDAN error, or an array resulting from a successful EDAN call.
 */
function _miss_delete_file($filepath_string = NULL) {

  $filepath_string = !empty($_GET['filepath_string']) ? filter_xss($_GET['filepath_string']) : $filepath_string;

  if($filepath_string === NULL) return MENU_NOT_FOUND;

  $fileId = '';

  // Query EDAN for all of the files.
  $file_list = _miss_get_file_list();

  // Loop through and get the file's id.
  foreach($file_list['files'] as $fkey => $fvalue) {
    if($fvalue['filepath'] === $filepath_string) {
      $fileId = $fvalue['id'];
    }
  }

  // Pass the fileId to releaseContent.htm.
  $uri = 'type=file&id=' . $fileId;

  // For file management, we need to point to production.
  $edan_server = (($_SERVER['HTTP_HOST'] === 'dev.miss.dev1.myquotient.net') || ($_SERVER['HTTP_HOST'] === 'nmaahc.geekt.in')) ? 'http://edandev.si.edu/' : 'http://edan.si.edu/';
  $edan_auth_key = variable_get('edan_auth_key');
  $edan_tier_type = variable_get('edan_tier_type');
  $edan_app_id = variable_get('edan_app_id');
  $edan = new EDANInterface($edan_server, $edan_app_id, $edan_auth_key, $edan_tier_type);

  $endpoint = 'content/v1.1/admincontent/releaseContent.htm';
  $result = $edan->sendRequest($uri, $endpoint, FALSE, $info);
  $result = json_decode($result, TRUE);

  if ($info['http_code'] !== 200) {
    return drupal_json_output(array('error' => 'EDAN responded with a status code of: ' . $info['http_code'] . ' (' . $result['error'] . '). URI: ' . $edan_server . $endpoint . '?' . $uri));
  }
  else {
    return drupal_json_output($result);
  }

}

/**
 * Returns a table of the file list with links to the files themselves and an insert link that is able to be highjacked by javascript
 * for doing the actual insertion
 *
 * @param $existing_table_definition
 *   Pass in an existing table to add rows to, and headers to if the headers are blank
 * @param $cannot_reach_server_message
 *   Message to output to end users when server can't be reached
 * @param $file_listing_is_bad_message
 *   Message to output to end users when the file listing info isn't correctly structured
 * @param $return_table_even_if_file_listing_is_bad
 *   If true, returns a table of properly structured files in the file listing, ignoring poorly structured files
 *
 * @return
 *   Returns a Drupal themed table of the file list with links to the files themselves and an insert link
 *   that is able to be highjacked by javascript for doing the actual insertion
 */
 function _miss_file_list_as_table(
   $existing_table_definition = NULL,
   $cannot_reach_server_message = 'Cannot contact server to get file listing',
   $file_listing_is_bad_message = 'Server did not return expected file information',
   $return_table_even_if_file_listing_is_bad = FALSE
 ){
   // if (empty($existing_table_definition)) {
   //   $existing_table_definition = array(
   //     'header' => array(),
   //     'rows' => array(),
   //     'attributes' => array('id' => 'browse_files', 'style' => 'width: 100%;'),
   //     'caption' => NULL,
   //     'colgroups' => array(),
   //     'sticky' => TRUE,
   //     'empty' => 'No Files',
   //   );
   // }
   //
   // $table_definition = $existing_table_definition;
   //
   // if (empty($table_definition['header'])) {
   //   $table_definition['header'] = array('Actions', 'File', 'Date', 'View/Rotate');
   // }
   drupal_add_css( '
 td:nth-child(2) { word-break:break-all; }
 td:nth-child(3) { min-width:200px; }
 .image-holder-outer { height:250px;width:200px;display:block; }
 .image-holder-inner { height:200px;width:200px;display:table-cell;vertical-align:middle;: }
 .one-sidebar #content { width:950px; }
 .loading-div { float:left;display:block;z-index:0;position:absolute; }
 .rotater-cw, .rotater-ccw, .rotater-180, .rotater-save { display:inline;width:60px; height:15px; position:relative; top:20px;}
 .rotatable { max-height:200px;max-width:200px;display:block; margin:auto;z-index:10}
 .rotated-cw {
   -ms-transform: rotate(90deg);
   -webkit-transform: rotate(90deg);
   transform: rotate(90deg);
 }
 .rotated-ccw {
   -ms-transform: rotate(270deg);
   -webkit-transform: rotate(270deg);
   transform: rotate(270deg);
 }
 .rotated-180 {
   -ms-transform: rotate(180deg);
   -webkit-transform: rotate(180deg);
   transform: rotate(180deg);
 }
 .saved-rotate {
   display:block;
   background: lightgreen;
   position: relative;
   top: -200px;
 }
 .centered-table-cell {
   text-align: center;
 }
 ', array('group' => CSS_THEME, 'type' => 'inline') );

   // // Query EDAN for the file list.
   // $file_list = _miss_get_file_list();
   //
   // if (!empty($file_list['files'])){
   //   // Go through them in reverse time order so newest shows up on top
   //   for ($index = count($file_list['files'])-1; $index >= 0; $index--) {
   //
   //     $item = $file_list['files'][$index];
   //
   //     if (is_array($item) && array_key_exists('originalName', $item) && array_key_exists('filepath', $item)) {
   //
   //       $supported_image = array('gif', 'jpg', 'jpeg', 'png');
 	//       $ext = strtolower(pathinfo($item['filepath'], PATHINFO_EXTENSION)); // Using strtolower to overcome case sensitive
   //       $image_rotate_info = '';
   //
   //     	if (in_array($ext, $supported_image)) {
   //         // Replace edandev with edan, since images don't live out on edandev...
   //         // And add MISS_NMAAHC to the file path since edandev is not returning it.
   //         $miss_nmaahc_route = !stristr($item['filepath'], 'MISS_NMAAHC/') ? 'MISS_NMAAHC/' : '';
   //         $item['filepath'] = str_replace('edandev.si.edu/file/', 'edan.si.edu/file/' . $miss_nmaahc_route, $item['filepath']);
   //         $image_rotate_info = _miss_create_view_rotate_link($item['filepath'], $item['id']);
   //       }
   //
   //       $table_definition['rows'][] = array(
           // _miss_create_insert_link($item['filepath']) . _miss_create_delete_link(),
   //         $item['originalName'] . '<div class="miss-direct-link"><small>' . _miss_link_yourself($item['filepath'], $item['filepath']) . '</small></div>',
   //         date( 'Y-m-d', strtotime($item['filescanner'][0]['scanDate']) ),
   //         $image_rotate_info,
   //       );
   //
   //     }
   //     elseif (!$return_table_even_if_file_listing_is_bad) {
   //       return t($file_listing_is_bad_message);
   //     }
   //   }
   //
   // }
   // else {
   //   return t($cannot_reach_server_message);
   // }
   // return theme_table($table_definition);

   $out = '';
   $out .= '<table id="browse_files" class="table table-striped table-bordered" cellspacing="0" width="100%">' . "\n";
   $out .= '<thead>' . "\n";
   $out .= '<tr>' . "\n";
   $out .= '<th class="text-center">Actions</th>' . "\n";
   $out .= '<th class="text-center">File</th>' . "\n";
   $out .= '<th class="text-center">Date</th>' . "\n";
   $out .= '<th class="text-center">View/Rotate</th>' . "\n";
   $out .= '</tr>' . "\n";
   $out .= '</thead>' . "\n";
   $out .= '</table>' . "\n";

  //  $yyy = rawurldecode('?draw=1&columns%5B0%5D%5Bdata%5D=content.fileId&columns%5B0%5D%5Bname%5D=&columns%5B0%5D%5Bsearchable%5D=true&columns%5B0%5D%5Borderable%5D=true&columns%5B0%5D%5Bsearch%5D%5Bvalue%5D=&columns%5B0%5D%5Bsearch%5D%5Bregex%5D=false&columns%5B1%5D%5Bdata%5D=content.originalName&columns%5B1%5D%5Bname%5D=&columns%5B1%5D%5Bsearchable%5D=true&columns%5B1%5D%5Borderable%5D=true&columns%5B1%5D%5Bsearch%5D%5Bvalue%5D=&columns%5B1%5D%5Bsearch%5D%5Bregex%5D=false&columns%5B2%5D%5Bdata%5D=content.filescanner%5B0%5D.scanDate&columns%5B2%5D%5Bname%5D=&columns%5B2%5D%5Bsearchable%5D=true&columns%5B2%5D%5Borderable%5D=true&columns%5B2%5D%5Bsearch%5D%5Bvalue%5D=&columns%5B2%5D%5Bsearch%5D%5Bregex%5D=false&columns%5B3%5D%5Bdata%5D=content.fileId&columns%5B3%5D%5Bname%5D=&columns%5B3%5D%5Bsearchable%5D=true&columns%5B3%5D%5Borderable%5D=true&columns%5B3%5D%5Bsearch%5D%5Bvalue%5D=&columns%5B3%5D%5Bsearch%5D%5Bregex%5D=false&order%5B0%5D%5Bcolumn%5D=2&order%5B0%5D%5Bdir%5D=desc&start=75&length=25&search%5Bvalue%5D=&search%5Bregex%5D=false&_=1485463831579');
   //
  //  dumper($yyy);

   return $out;
 }

/**
 * Callback to send the rotation to the MISS endpoint (Save)
 * @param $edan_id
 *   EDAN's file id
 * @param $rotate_degrees
 *   string or integer to indicate the number of degrees, currently backend only support increments of 90
 *
 * @returns
 *   none
 */
function _miss_file_image_rotate_callback($edan_id, $rotate_degrees) {
  $edan = edan();
  $endpoint = 'miss/v1.0/nmaahc/files/editFile.htm';
  $uri = 'id='.$edan_id.'&rotation='.$rotate_degrees;
  $rotate_output = _miss_send_miss_request( $endpoint, $uri );
  if (!empty($rotate_output['id'])){
    $rotate_output['success'] = TRUE;
    print json_encode($rotate_output);
  }
  else {
    print '{"success":false}';
  }
}
/**
 * Convenience method to build a link to insert the given URL. Requires seperate javascript to do the insertion
 * @param $url
 *   URL to href
 * @param $text
 *   default to "View/Rotate", the text of the link
 *
 * @returns
 *   a l() link
 */
function _miss_create_view_rotate_link($url, $edan_id, $text = 'View/Rotate') {
  return l(
    $text,
    '',
    array(
      'fragment' => 'content',
      'external' => TRUE,
      'attributes' => array(
        'class' => array('button','view-rotate-button'),
        'view-rotate-url' => $url,
        'edan-id' => $edan_id,
        'onclick' => 'if (event.preventDefault) event.preventDefault(); else event.returnValue = false;',
      ),
    )
  );
}

/**
 * Convenience method to build a link to insert the given URL. Requires seperate javascript to do the insertion
 * @param $url
 *   URL to href
 * @param $text
 *   default to "Insert", the text of the link
 *
 * @returns
 *   a l() link
 */
function _miss_create_insert_link($url, $text = 'Insert') {
  return l(
    $text,
    '',
    array(
      'fragment' => 'content',
      'external' => TRUE,
      'attributes' => array(
        'class' => array('button','insert-button'),
        'style' => 'float: left;',
        'insert-link' => $url,
        'onclick' => 'if (event.preventDefault) event.preventDefault(); else event.returnValue = false;',
      ),
    )
  );
}

/**
 * Convenience method to build a link to remove a file asset.
 * @param $text
 *   default to "Insert", the text of the link
 *
 * @returns
 *   a l() link
 */
function _miss_create_delete_link($url = 'javascript:void(0);', $text = 'Delete') {

  if(stristr($_SERVER['SERVER_NAME'], 'dev.miss') || stristr($_SERVER['SERVER_NAME'], 'geekt.in')) {
    return l(
      $text,
      '',
      array(
        'fragment' => 'content',
        'external' => TRUE,
        'attributes' => array(
          'class' => array('button', 'delete-button'),
          'style' => 'float: left;',
          'delete-link' => $url,
          'onclick' => 'if (event.preventDefault) event.preventDefault(); else event.returnValue = false;',
        ),
      )
    );
  }

}

/**
 * Convenience method to build a link to the URL with the name of the URL
 * @param $url
 *   URL to href and to put as text
 * @param $text
 *   URL to href and to put as text
 * @param $target
 *   default to blank, where the a href target should be, NULL for no target
 *
 * @returns
 *   a l() link
 */

function _miss_link_yourself($url, $text = '', $target = '_blank') {
  $attr = array('target' => $target);
  if ($target === NULL) {
    $attr = array();
  }
  return l(
    $text,
    $url,
    array(
      'attributes' => $attr,
    )
  );
}
