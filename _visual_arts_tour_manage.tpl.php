<?php if(!empty($page['form'])): ?>

  <div id="console" class="clearfix">
    <div class="messages status edan-app-message light-yellow-background">
      <p style="margin: 0;">To preview the gallery as it's being built, scroll to the bottom of the page and click "Preview".</p>
    </div>
  </div>

  <?php print $page['render_link']; ?>

  <?php print '<form id="' . $page['form']['#id'] . '" accept-charset="UTF-8" method="' . $page['form']['#method'] . '" action="' . $page['form']['#action'] . '">'; ?>


    <fieldset class="form-wrapper" id="description_content">
      <legend><span class="fieldset-legend">Gallery Descriptive Content</span></legend>
      <?php print drupal_render($page['form']['enabled']); ?>
      <?php print drupal_render($page['form']['title']); ?>
      <?php print drupal_render($page['form']['description']); ?>
    </fieldset>

    <div class="clearfix"></div>

    <fieldset class="form-wrapper" id="edan_images_container">

      <legend><span class="fieldset-legend">Gallery Images</span></legend>

      <div id="edan_image_search">
        <?php print $page['miss_image_search_form']; ?>
        <?php print drupal_render($page['form']['file_upload']); ?>
      </div>

      <div class="clearfix"></div>

      <div id="edan_image_search_results_container">
        <div class="edan-header-container">
          <h2>EDAN Image Search Results</h2>
        </div>
        <div class="app_preloader">
          Processing<br>
          <img src="/<?php echo drupal_get_path('module', 'miss'); ?>/images/indicator-big.gif" alt="Preloader image">
        </div>
        <div id="edan_image_search_results">
          <div id="image_results_grid">
          </div>
        </div>
      </div>

      <!-- <div id="edan_dropped_image_container">
        <div class="edan-header-container">
          <h2>Primary Gallery Image <span class="form-required" title="This field is required.">*</span></h2>
        </div>
        <div id="edan_dropped_image">
          <?php
          // if(!empty($form['miss_primary_image']['und'][0]['value']['#value'])) {
          //   print $form['miss_primary_image']['und'][0]['value']['#value'];
          // } else {
            // print '<p class="placeholder">Drag and drop the main gallery image here. This image will be set as the first image in the gallery.</p>' . "\n";
            // print '<ul></ul>' . "\n";
          // }
          ?>
        </div>
      </div> -->

      <div id="edan_dropped_images_container">

        <div class="edan-header-container">
          <h2>Gallery Images <span class="header-note">(drag and drop to reorder images)</span></h2>
        </div>
        
        <div class="clearfix"></div>

        <div id="edan_dropped_images">
          <p class="placeholder">Drag and drop gallery images here</p>
          <ul></ul>
        </div>
      </div>

    </fieldset>

    <div class="edan-dropped-image-hidden-textarea">
      <?php print drupal_render($page['form']['primary_image']); ?>
    </div>

    <div class="edan-dropped-images-hidden-textarea">
      <?php print drupal_render($page['form']['secondary_images']); ?>
    </div>

    <div class="clearfix"></div>

    <?php print drupal_render($page['form']['preview']); ?>

    <div id="submit_gallery">
      <?php print drupal_render($page['form']['submit']); ?>
    </div>

    <div id="preview_record_modal" title="Preview Gallery"></div>
    <div id="edit_edan_image_metadata_modal" title="Edit EDAN Image Metadata"></div>

    <?php print drupal_render_children($page['form']); ?>

  </form>

  <script id="gallery_record_template" type="text/template">
    <div class="gallery-record">
      <div class="record-options">
        <%= edan_app_message %>
      </div>
      <div class="edan-header-container-transparent">
        <h2><%- gallery_title %></h2>
      </div>
      <div class="clearfix"></div>
      <div class="gallery-record-container">
        <div class="gallery-record-column column-left">
          <h3><em><%= record_title %></em></h3>
          <h4><%- artist_name %></h4>
          <div class="editable"><%= record_data %></div>
          <div class="app_preloader" style='padding: 0; margin-top: 2.2rem; font-family: "Helvetica Neue", Helvetica, Arial, sans-serif;'>
            Processing<br>
            <img src="/<?php echo drupal_get_path('module', 'miss'); ?>/images/indicator-big.gif" alt="Preloader image">
          </div>
          <div class="record-body">
          </div>
        </div>
        <div class="gallery-record-column column-right">
          <%= scaleImg({record_image}) %>
        </div>
      </div>
    </div>
    <div class="clearfix"></div>
    <div class="gallery-navigation">
      <ul>
        <li style="width: 3rem;"><a href="javascript:void(0);" title="Go back to the main page" class="go-back-trigger"><img src="/misc/menu-collapsed-rtl.png" alt="Back icon" style="vertical-align: middle;"> BACK</a></li>
        <li class="gallery-navigation-trigger bold-font" data-destination="aboutTheArtist">ABOUT<br>THE ARTIST</li>
        <li class="gallery-navigation-trigger" data-destination="aboutTheArtwork">ABOUT<br>THE ARTWORK</li>
        <li class="gallery-navigation-trigger" data-destination="artisticContext">ARTISTIC CONTEXT</li>
        <li class="gallery-navigation-trigger" data-destination="historicContext">HISTORIC CONTEXT</li>
        <li class="gallery-navigation-trigger" data-destination="locationContext">RELATED GALLERIES &#38; MUSEUMS</li>
      </ul>
    </div>
  </script>

  <script id="gallery_message_template" type="text/template">
    <div id="gallery_message" class="clearfix">
      <div class="messages error">
        <h2 class="element-invisible">Error message</h2>
        <%- message %>
      </div>
    </div>
  </script>

<?php endif; ?>