<?php print $page['components_menu']; ?>

<div class="subheader-container">
  <div class="subheader-image-container">
    <a href="<?php echo $page['record']['art_profiles'][$_GET['key']]['artworkImage']; ?>" target="_blank" title="View the full-sized image (opens in a new window/tab)"><img src="<?php echo $page['ids_processed_image']; ?>" alt="Gallery image" width="110"></a>
  </div>
  <div>
    <h2><?php print t($page['record']['caption']); ?></h2>
    <?php print $page['record']['text']; ?>
    <a href="<?php print $page['base_record_path']; ?>">Back to the parent gallery</a>
  </div> 
</div>

<div class="clearfix"></div>

<?php if(!empty($page['record']['artProfiles'][$page['this_image_key']]['linkedEdanId'])): ?>
  <div id="view_edan_record_container">
    <a href="javascript:void(0);" id="view_edan_record_trigger" class="button search-edan-button" data-edan-record-id="<?php echo $page['record']['artProfiles'][$page['this_image_key']]['linkedEdanId']; ?>">View EDAN Record</a>
    <a href="javascript:void(0);" id="hide_edan_record_trigger">Hide</a>
    <div class="app_preloader">
      Processing<br>
      <img src="/<?php echo drupal_get_path('module', 'miss'); ?>/images/indicator-big.gif" alt="Preloader image">
    </div>
  </div>
<?php endif; ?>

<div id="edan_record_view"></div>

<?php print '<form id="' . $page['form']['#id'] . '" accept-charset="UTF-8" method="' . $page['form']['#method'] . '" action="' . $page['form']['#action'] . '">'; ?>
  
  <?php
  if($page['this_component_name'] === 'imageMetadata') {
    print drupal_render($page['form']['enabled']);
    print drupal_render($page['form']['artworkTitle']);
    print drupal_render($page['form']['firstName']);
    print drupal_render($page['form']['lastName']);
    print drupal_render($page['form']['artworkDate']);
    print drupal_render($page['form']['artworkContributor']);
    print drupal_render($page['form']['artworkMaterial']);
  } else {
    print drupal_render($page['form']['text']);
  }
  ?>

  <?php if($page['this_component_name'] !== 'imageMetadata'): ?>
    <fieldset class="form-wrapper" id="edan_images_container">

      <legend><span class="fieldset-legend">Component Images</span></legend>
      
      <div id="edan_image_search">
        <?php print $page['miss_image_search_form']; ?>
        <?php print drupal_render($page['form']['file_upload']); ?>
      </div>

      <div class="clearfix"></div>

      <div id="edan_image_search_results_container" class="scrollable_deactivated">
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

      <div id="edan_dropped_images_container" class="scrollable_deactivated component-images-container">
        <div class="edan-header-container">
          <h2>Images <span class="header-note">(drag and drop to reorder images)</span></h2>
        </div>
        <div id="edan_dropped_images" class="components-dropped-images">
          <p class="placeholder">Drag and drop gallery images here</p>
          <ul></ul>
        </div>
         <div class="edan-dropped-images-hidden-textarea">
          <?php print drupal_render($page['form']['secondary_images']); ?>
        </div>
      </div>

    </fieldset>
  <?php endif; ?>

  <div class="clearfix"></div>

  <div id="submit_gallery">
    <?php print drupal_render($page['form']['submit']); ?>
  </div>

  <?php print drupal_render_children($page['form']); ?>

</form>
