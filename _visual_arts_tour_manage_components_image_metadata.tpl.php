<div class="subheader-container">
  <div class="subheader-image-container">
    <a href="<?php print $page['target_image']['imageUrl']; ?>" target="_blank" title="View the full-sized image (opens in a new window/tab)"><img src="<?php echo $page['ids_processed_image']; ?>" alt="Gallery image" width="110"></a>
  </div>
  <div>
    <p><?php print t($page['target_image']['caption']); ?></p>
    <a href="<?php print $page['base_record_path']; ?>">Back to the "<?php print $page['component']['title']; ?>" component</a>
  </div> 
</div>

<div class="clearfix"></div>

<?php if(!empty($page['target_image']['linkedEdanId'])): ?>
  <div id="view_edan_record_container">
    <a href="javascript:void(0);" id="view_edan_record_trigger" class="button search-edan-button" data-edan-record-id="<?php echo $page['target_image']['linkedEdanId']; ?>">View EDAN Record</a>
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
  print drupal_render($page['form']['credit']);
  print drupal_render($page['form']['caption']);
  ?>

  <div class="clearfix"></div>

  <div id="submit_gallery">
    <?php print drupal_render($page['form']['submit']); ?>
  </div>

  <?php print drupal_render_children($page['form']); ?>

</form>
