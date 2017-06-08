jQuery(document).ready(function($) {

  /**
   * Populate images from textarea.
   */
  populate_images_from_textarea();

  /**
   * Listener for incoming uploaded files.
   */
  uploadChecker();
  var startUploadChecker = setInterval(uploadChecker, 500);

  /**
   * Prevent the form from being submitted on return key press.
   */ 
  $('.query-edan').keypress(function(event) {
    var thisElement = $(this);
    if (event.keyCode == 13) {
      event.preventDefault();
      $( thisElement ).parent().parent().find('.search-edan-button' ).trigger( 'click' );
    }
  });

  // Initially, make the secondary images area sortable.
  if($('#edan_dropped_images').length) {
    $('#edan_dropped_images ul').sortable({
      items: 'li:not(.placeholder)',
      sort: function() {
        $( this ).removeClass( 'ui-state-default' );
      },
      stop: function( event, ui ) {
        // Save to textarea
        saveToTextarea( this );
      }
    });
  }

  /**
   * EDAN Search
   */
  $('.search-edan-button').click(function(){

    var thisElement = $(this),
        idsThumbUrl = Drupal.settings.ids_url + '?max_w=110&max_h=160&id=',
        query = $(thisElement).prev().val(),
        search_type = $(thisElement).attr('id'),
        appPreloader = (search_type === 'search_images') ? $('#edan_images_container').find('.app_preloader') : $('#edan_data_container').find('.app_preloader'),
        metadataPlaceholder = '<p class="placeholder">Add metadata below this line</p><ol class="editable"></ol>',
        imagesPlaceholder = '<p class="placeholder">Drag and drop gallery images here</p><ul><li class="placeholder"></li></ul>',
        imagePlaceholder = '<p class="placeholder">Drag and drop the main gallery image here</p><ul></ul>';

    if(search_type === 'search_images') {
      $('#image_results_grid').empty();
    } else {
      $('#edan_search_results').empty();
      $('#edan_dropped_items').empty();
      $('#edan_dropped_items').append(metadataPlaceholder);
    }

    appPreloader.show();

    $.ajax({
      type: 'POST'
      ,dataType: 'json'
      ,url: '/search_edan/ajax'
      ,data: ({query: query, search_type: search_type})
      ,success: function(results){

        appPreloader.hide();

        if(!results.error) {

          // Image search
          if(search_type === 'search_images') {

            // The div which will contain all of the images.
            var divTag = $('<div />').attr('class', 'single-image-container');

            $.each(results, function(result_key, result_value) {
              $.each(result_value['record_images'], function(key, value) {

                // Only process if there is an idsId
                if(typeof value['idsId'] !== 'undefined') {
                  // Is the idsId in the $('body').data('images_idsids') array?
                  // If it is, then don't include it in the search results.
                  var idsid_match = $.inArray(value['idsId'], $('body').data('images_idsids'));
                  // Build out the image container.
                  var unchosenImageClassPrefix = '';
                  // Build it all out within the anchor tag.
                  var anchorTag = $('<a />')
                    .attr('href', Drupal.settings.ids_url + '?id=' + value['idsId'])
                    .attr('target', '_blank');
                  var figureTag = $('<figure />');
                  var imageTag = $('<img />')
                    .addClass('th ' + unchosenImageClassPrefix + 'chosen-image new-image')
                    .attr('src', idsThumbUrl + value['idsId'])
                    .attr('alt', 'Search result image')
                    .attr('title', 'An image thumbnail of ' + value['record_title'] + '. Click to view a larger version (opens in a new window).')
                    .attr('data-edan-id', value['record_id'])
                    .attr('data-title', value['record_title'])
                    .attr('data-date', value['record_date'])
                    .attr('data-material', value['record_material'])
                    .attr('data-contributor', value['record_contributor'])
                    .attr('data-regular-size', value['regular_size']);
                  // Put it all together.
                  $(figureTag).append(imageTag);
                  $(anchorTag).append(figureTag);
                  $(divTag).append(anchorTag);
                }
              });
            });
            
            // Append all of the images tot the div.single-image-container.
            $('#image_results_grid').append(divTag);

            // Draggable
            var listItems = $('#image_results_grid').find('div.single-image-container a').find('figure');
            $(listItems).draggable({
              appendTo: 'body',
              helper: 'clone'
            });

            // If we're replacing...
            if($( '#edan_dropped_images ul' ).find('li').hasClass('replacing')) {

              var target = $( '#edan_dropped_images ul' ).find('li.replacing');

              // Droppabale
              $(target).find('.image-wrapper').droppable({
                activeClass: 'ui-state-default',
                hoverClass: 'ui-state-hover',
                accept: ':not(.ui-sortable-helper)',
                tolerance: 'touch',
                drop: function( event, ui ) {

                  var thisImage = $( this ).find( 'img' ),
                      newImageHtml = ui.draggable.html(),
                      newImage = $('<img />')
                        .attr('src', $(newImageHtml).attr('src'))
                        .attr('class', thisImage.attr('class'))
                        .attr('alt', thisImage.attr('alt'))
                        .attr('title', thisImage.attr('title'))
                        .attr('data-firstName', thisImage.attr('data-firstName'))
                        .attr('data-lastName', thisImage.attr('data-lastName'))
                        .attr('data-title', thisImage.attr('data-title'))
                        .attr('data-edan-id', thisImage.attr('data-edan-id'))
                        .attr('data-date', thisImage.attr('data-date'))
                        .attr('data-material', thisImage.attr('data-material'))
                        .attr('data-contributor', thisImage.attr('data-contributor'))
                        .attr('data-id', thisImage.attr('data-id'))
                        .attr('data-status', thisImage.attr('data-status'))
                        .attr('data-regular-size', $(newImageHtml).attr('src'));
                  
                  $( thisImage ).remove();
                  $( this ).append( newImage );
                  // Save to textarea
                  saveToTextarea( target.parent() );

                }
              });

            } else {

              // Droppabale
              $( '#edan_dropped_images ul' ).droppable({
                activeClass: 'ui-state-default',
                hoverClass: 'ui-state-hover',
                accept: ':not(.ui-sortable-helper)',
                tolerance: 'touch',
                drop: function( event, ui ) {

                  // Remove the placeholder.
                  $( this ).parent().find( '.placeholder' ).remove();
                  
                  // Build-out the image container and edit buttons.
                  build_image_list_item( this, ui.draggable.html() );

                  // Save to textarea
                  saveToTextarea( this );

                  // Highlight the container when items are dropped.
                  $( this ).parent().parent().effect( 'highlight', { color: '#EFEFEF' } );
                  $( this ).parent().parent().find('.edan-header-container').effect( 'highlight', { color: '#EFEFEF' } );
                }
              }).sortable({
                items: 'li:not(.placeholder)',
                sort: function() {
                  // gets added unintentionally by droppable interacting with sortable
                  // using connectWithSortable fixes this, but doesn't allow you to customize active/hoverClass options
                  $( this ).removeClass( 'ui-state-default' );
                },
                stop: function( event, ui ) {
                  // Save to textarea
                  saveToTextarea( this );
                }
              });

            }
        
          }
          // Record search
          else {

            var numFound = (typeof results.numFound !== 'undefined') ? results.numFound : 'No';
            var total_results = '<p class="result-total">' + numFound + ' results</p>';
            $('#edan_search_results').append(total_results);

            if(typeof results.numFound !== 'undefined') {
              var dl = $('<dl />');

              $.each(results.items, function(key, value) {
                var dt = $('<dt />');
                var dd1 = '<dd class="record-title"><a href="javascript:void(0);" class="preview_record" data-url="' + value.url + '" title="Preview this record">' + value.content.descriptiveNonRepeating.title.content + '</a></dt>';
                var dd2 = '<dd>Score: ' + value.score + '</dt>';
                var dd3 = '<dd>EDAN URL: ' + value.url + '</dt>';
                dt.append(dd1, dd2, dd3);
                dl.append(dt);
              });

              var edan_query = encodeURI('"' + query + '"');
              edan_query = edan_query.replace(/'/g, "%27");

              $('#edan_search_results').append(dl);

              var previewRecordTrigger = $('#edan_search_results').find('a.preview_record');
              $(previewRecordTrigger).click(function(event) {
                previewRecord($(this));
              });
            }

          }

        } else {
          var heading = '<h2>No Results</h2>';
          var p1 = '<p>Please try again.</p>';
          $('#edan_search_results').append(heading, p1);
        }

      }

    });

  });

  /**
   * Replace image
   */
  $('#edan_dropped_images_container').on('click', '.replace-image-trigger', function() {

    $('#image_results_grid').empty();

    var thisTrigger = $(this),
        thisImageListItem = thisTrigger.parent().parent().parent().parent(),
        thisImageList = thisTrigger.parent().parent().parent().parent().parent(),
        thisImageContainer = thisTrigger.parent().parent().parent().parent().parent().parent(),
        originalImage = thisImageListItem.find('.image-wrapper').find('img'),
        clearfix = $('<div />').attr('class', 'clearfix'),
        instructions = $('<p />').attr('class', 'gray-text').text('Search for an EDAN image to the left and drag a new image to this spot, or click the Upload/Insert button');

    $(thisImageListItem).addClass('replacing').attr('style', 'border: 1px dashed #E4C3C3; padding-top: 4px; background-color: #f3f3f3; cursor: default;');
    $(thisImageListItem).find('a:not(.replace-image-trigger)').hide();
    $(thisTrigger).attr('data-original-image-source', $(originalImage).attr('src'));
    $(thisTrigger).text('Cancel Replace');
    $(thisTrigger).parent().prepend(clearfix, instructions);
    $(thisTrigger).removeClass('replace-image-trigger').addClass('cancel-replace-trigger');

    // Hide the controls for all other images... image replacement needs to happen one at time.
    $(thisImageList).find('li:not(.replacing)').find('.edan-image-caption').hide();

    // Disable draggable functionalities everywhere.
    if($( '#edan_dropped_images ul' ).data('uiSortable')) {
      $( '#edan_dropped_images ul' ).sortable( 'option', 'disabled', true );
      $( '#edan_dropped_images ul' ).find('li:not(.replacing)').attr('style', 'cursor: default;');
    }
    // Disable droppable functionalities everywhere.
    if($( '#edan_dropped_images ul' ).data('uiDroppable')) {
      $( '#edan_dropped_images ul' ).droppable( 'option', 'disabled', true );
    }
    // Enable droppable for this element.
    var target = $( '#edan_dropped_images ul' ).find('li.replacing').find('.image-wrapper'); 
    if($(target).data('uiDroppable')) {
      $(target).droppable( 'option', 'disabled', false );
    }
  });

  /**
   * Cancel replace image
   */
  $('#edan_dropped_images_container').on('click', '.cancel-replace-trigger', function() {
    var thisTrigger = $(this),
        thisImageListItem = thisTrigger.parent().parent().parent().parent(),
        thisImageList = thisTrigger.parent().parent().parent().parent().parent(),
        thisImageContainer = thisTrigger.parent().parent().parent().parent().parent().parent(),
        originalImageSource = thisTrigger.attr('data-original-image-source'),
        currentImage = thisImageListItem.find('.image-wrapper').find('img'),
        clearfix = $('<div />').attr('class', 'clearfix'),
        instructions = $('<p />').attr('class', 'gray-text').text('Search for an EDAN image to the left and drag a new image to this spot, or click the Upload/Insert button');

    $(thisImageListItem).removeClass('replacing').removeAttr('style');
    $(thisImageListItem).find('a').show();
    $(thisTrigger).text('Replace Image');
    $(thisTrigger).parent().find('.clearfix').remove();
    $(thisTrigger).parent().find('.gray-text').remove();
    $(thisTrigger).removeClass('cancel-replace-trigger').addClass('replace-image-trigger');
    $(currentImage).attr('src', originalImageSource);

    // Show the controls for all other images.
    $(thisImageList).find('li').find('.edan-image-caption').show();

    // Disable the droppable functionality for this element.
    if(thisImageListItem.find('.image-wrapper').data('uiDroppable')) {
      thisImageListItem.find('.image-wrapper').droppable( 'option', 'disabled', true );
    }

    // Enable draggable functionalities everywhere.
    if($( '#edan_dropped_images ul' ).data('uiSortable')) {
      $( '#edan_dropped_images ul' ).sortable( 'option', 'disabled', false );
      $( '#edan_dropped_images ul li' ).removeAttr('style');
    }
    // Enable droppable functionalities everywhere.
    if($( '#edan_dropped_images ul' ).data('uiDroppable')) {
      $( '#edan_dropped_images ul' ).droppable( 'option', 'disabled', false );
    }
  });

  /**
   * Remove image
   */
  $('#edan_dropped_images_container').on('click', '.remove-image-trigger', function() {
    var thisTrigger = $(this);
    var thisImageListItem = thisTrigger.parent().parent().parent().parent();
    var thisImageContainer = thisTrigger.parent().parent().parent().parent().parent().parent();

    if(confirm('Are you sure you want to remove this image?') == true) {
      // Clear the value of the file_uplaod input so it won't re-populate.
      $('#edan_image_search input#file_upload').val('');
      thisImageListItem.fadeOut();
      setTimeout(function() {
        thisImageListItem.remove();
        saveToTextarea(thisImageContainer.find('ul'));
      }, 1000);
    }

  });

  /**
   * View EDAN Record
   */
  $('#view_edan_record_trigger').on('click', function(event){

    $('#view_edan_record_container').find('.app_preloader').show();

    var edanRecordId = $(this).attr('data-edan-record-id');
    // AJAX request, getting the record.
    $.ajax({
      type: 'POST'
      ,dataType: 'json'
      ,url: '/get_record/ajax'
      ,data: ({query_id: edanRecordId})
      ,success: function(results){

        if(results) {
          $('#view_edan_record_container').find('.app_preloader').hide();
          // Record title
          var record_title = results.title;
          // AJAX request, formatting the record into an unordered list.
          $.ajax({
            type: 'POST'
            ,dataType: 'json'
            ,url: '/format_edanmdm_record/ajax'
            ,data: ({ record: results, divs: 'false' })
            ,success: function(ajax_return){
              if(ajax_return.length > 0) {
                // Append to the div.
                $('#edan_record_view').empty();
                $('#edan_record_view').append( ajax_return );
                $('#edan_record_view').toggle();
                if($('#edan_record_view').is(':visible')) {
                  $('#view_edan_record_trigger').text('Hide EDAN Record');
                } else {
                  $('#view_edan_record_trigger').text('View EDAN Record');
                }
                
              }
            }
          });
        } else {
          $('#edan_record_view').empty();
          $('#edan_record_view').append( '<p class="red-text">Error: EDAN data unavailable.</p>' );
          $('#edan_record_view').toggle();
        }

      }
    });
    
  });

  /**
   * Trigger the preview record modal.
   */
  $('#edit-preview').on('click', function(event){
    event.preventDefault();
    previewRecordDialog.dialog('open');
  });

  /**
   * Preview one EDAN record (e.g. edanmdm:AAADCD_15403)
   */  
  function previewRecord(this_link) {

    var results;

    $('#edan_search_results').empty();
    $('#edan_search_results_container').find('.app_preloader').show();
    var edan_url = this_link.data('url');

    // AJAX request, getting the record.
    $.ajax({
      type: 'POST'
      ,dataType: 'json'
      ,url: '/get_record/ajax'
      ,data: ({query: edan_url})
      ,success: function(results){

        if(results) {

          var ajax_return;
          // Set the record title.
          var record_title = results.title;
          // AJAX request, formatting the record into an unordered list.
          $.ajax({
            type: 'POST'
            ,dataType: 'json'
            ,url: '/format_edanmdm_record/ajax'
            ,data: ({ record: results })
            ,success: function(ajax_return){

              $('#edan_search_results_container').find('.app_preloader').hide();
              $('#edan_search_results').empty();

              if(ajax_return.length > 0) {
                var heading = '<h3>' + record_title + '</h3>';
                // Append to the modal.
                $('#edan_search_results').append(heading, ajax_return);

                // Draggable
                var listItems = $('#edan_search_results').find('#edanmdm').find('li');
                $(listItems).draggable({
                  appendTo: 'body',
                  helper: 'clone'
                });

                // Droppabale
                $( '#edan_dropped_items ol' ).droppable({
                  activeClass: 'ui-state-default',
                  hoverClass: 'ui-state-hover',
                  accept: ':not(.ui-sortable-helper)',
                  tolerance: 'touch',
                  drop: function( event, ui ) {
                    $( this ).parent().find( '.placeholder, li.placeholder' ).remove();
                    $( '<li></li>' ).text( ui.draggable.text() ).appendTo( this );
                    // Highlight the container when items are dropped.
                    $( '#edan_dropped_items_container, #edan_dropped_items_container .edan-header-container' ).effect( 'highlight', { color: '#EFEFEF' } );
                  }
                }).sortable({
                  items: 'li:not(.placeholder)',
                  sort: function() {
                    // gets added unintentionally by droppable interacting with sortable
                    // using connectWithSortable fixes this, but doesn't allow you to customize active/hoverClass options
                    $( this ).removeClass( 'ui-state-default' );
                  }
                });

              } else {
                var heading = '<h2>No Result</h2>';
                var text = '<p>EDAN did not respond. Please try again.</p>';
                $('#edan_search_results').append(heading, text);
              }
            }
          });

        } else {
          $('#edan_search_results_container').find('.app_preloader').hide();
          var heading = '<h2>No Results</h2>';
          var p1 = '<p>Please try again.</p>';
          $('#edan_search_results').append(heading, p1);
        }
      }
    });

  }

  /**
   * Preview the full gallery record in a modal
   */
  if($('#edan_dropped_images').length && typeof Drupal.settings.record_json !== 'undefined') {

    var dialog, 
      previewRecordDialog,
      recordJson = JSON.parse(Drupal.settings.record_json),
      artProfiles = recordJson.artProfiles;

    previewRecordDialog = $('#preview_record_modal').dialog({
      autoOpen: false,
      height: 575,
      width: '80%',
      modal: true,
      // buttons: {
      //   Close: function() {
      //     previewRecordDialog.dialog('close');
      //   }
      // },
      open: function( event, ui ) {

        // Freeze the parent window to prevent unusual scrolling behaviors.
        $('body').css('overflow', 'hidden');
        $('#edan_image_search_results_container').css('overflow-y', 'hidden');

        var palette = $('#preview_record_modal');

        // Clear the palette
        palette.empty();

        // If there is no title, append a message to the modal.
        if(!$('#edit-title').val()) {
          gallery_message('Please enter a gallery title before previewing the gallery.');
          return;
        }

        // Set the background color of the palette.
        var currentStyles = $(palette).attr('style');

        // Gather all of the content.
        var recordTitle = $('#edit-title').val(),
            mainBody = $('#edit-description-value').val();

        // Containerize all of the gathered content.
        var recordTitleHeader = $('<h2 />').attr('id', 'mainTitle').text(recordTitle),
            recordTitleContainer = $('<div />').attr('id', 'recordTitleContainer').append(recordTitleHeader);

        var helpDialogText = $('<p />').attr('style', 'font-weight: bold;').text('Scroll through the collection. Touch a painting to learn more.'),
            mainBodyContainer = $('<div />').attr('id', 'mainBodyContainer').append( mainBody );
        $(mainBodyContainer).append(helpDialogText);

        var secondaryImagesContainer = $('<div />').attr('id', 'secondaryImagesContainer'),
            secondaryImages = $('#edan_dropped_images').find('.image-wrapper img').clone(),
            secondaryImagesCaptions = $('#edan_dropped_images').find('.edan-image-caption').find('textarea');

        // Loop through images and wrap in div containers, swap out titles, limit to 10.
        $.each(secondaryImages, function(imgKey, imgValue) {
          if($(imgValue).attr('data-status') != 1){
            
            var secondaryImageContainer = $('<div />').attr('class', 'secondaryImageContainer'),
                imgSrc = $(imgValue).attr('src'),
                imgSrcLarge = imgSrc.replace('&max_w=110&max_h=160', '&max_h=465'), // max_w=600&
                imgSrcRaw = imgSrc.replace('&max_w=110&max_h=160', ''),
                recordId = $(imgValue).attr('data-id'),
                imageTitle = $(imgValue).attr('data-title'),
                imageDate = $(imgValue).attr('data-date'),
                imageMaterial = $(imgValue).attr('data-material'),
                imageContributor = $(imgValue).attr('data-contributor'),
                imgValueLarge = $(imgValue).removeAttr('src').attr('src', imgSrcLarge).attr('title', 'Gallery image of ' + imageTitle);

            // Scale the image.
            // scaleImage(imgSrcRaw, 300, 300, function(canvas){
              var gallery_image = $('<img />')
                .attr('src', imgSrcLarge)
                .attr('class', 'chosen-image')
                .attr('data-title', imageTitle)
                .attr('data-date', imageDate)
                .attr('data-material', imageMaterial)
                .attr('data-contributor', imageContributor)
                .attr('data-imgsrc', imgSrcRaw)
                .attr('data-record-id', recordId);
              secondaryImageContainer.append(gallery_image);
              secondaryImageContainer.append('<p class="image-title">' + imageTitle + '</p><p class="image-sub-title">' + imageDate + '</p>');
              secondaryImagesContainer.append(secondaryImageContainer);
            // });
          }

        });
        
        // Record title
        palette.append(recordTitleContainer);
        // Main body + extended body
        palette.append(mainBodyContainer);
        // Secondary images container
        palette.append(secondaryImagesContainer);

        /**
         * Single Gallery Item
         */
        $('#secondaryImagesContainer').on('click', '.chosen-image', function () {

          var artProfileId = $(this).attr('data-record-id'),
              profileKey = _.findKey(artProfiles, (val) => val === artProfileId),
              thisProfile = recordJson.art_profiles[profileKey];

          var allContent = {
            // Header Content (all sections)
            headerContent: {
              galleryTitle: recordJson.caption,
              artworkTitle: thisProfile.artworkTitle.replace('<p>', '').replace('</p>', ''),
              artistName: thisProfile.firstName + ' ' + thisProfile.lastName,
              artworkMaterial: thisProfile.artworkMaterial.replace('<p>', '').replace('</p>', ''),
              artworkContributor: thisProfile.artworkContributor.replace('<p>', '').replace('</p>', '')
            },
            // About the Artist
            aboutTheArtist: {
              images: ((typeof thisProfile.makerDetails !== 'undefined') && (typeof thisProfile.makerDetails.images !== 'undefined')) ? thisProfile.makerDetails.images : {},
              text: ((typeof thisProfile.makerDetails !== 'undefined') && (typeof thisProfile.makerDetails.text !== 'undefined')) ? thisProfile.makerDetails.text : ''
            },
            // About the Artwork
            aboutTheArtwork: {
              images: ((typeof thisProfile.artworkDetails !== 'undefined') && (typeof thisProfile.artworkDetails.images !== 'undefined')) ? thisProfile.artworkDetails.images : {},
              text: ((typeof thisProfile.artworkDetails !== 'undefined') && (typeof thisProfile.artworkDetails.text !== 'undefined')) ? thisProfile.artworkDetails.text : ''
            },
            // Artistic Context
            artisticContext: {
              images: ((typeof thisProfile.artisticContext !== 'undefined') && (typeof thisProfile.artisticContext.images !== 'undefined')) ? thisProfile.artisticContext.images : {},
              text: ((typeof thisProfile.artisticContext !== 'undefined') && (typeof thisProfile.artisticContext.text !== 'undefined')) ? thisProfile.artisticContext.text : ''
            },
            // Historic Context
            historicContext: {
              images: ((typeof thisProfile.historicContext !== 'undefined') && (typeof thisProfile.historicContext.images !== 'undefined')) ? thisProfile.historicContext.images : {},
              text: ((typeof thisProfile.historicContext !== 'undefined') && (typeof thisProfile.historicContext.text !== 'undefined')) ? thisProfile.historicContext.text : ''
            },
            // Location Context
            locationContext: {
              images: ((typeof thisProfile.locationContext !== 'undefined') && (typeof thisProfile.locationContext.images !== 'undefined')) ? thisProfile.locationContext.images : {},
              text: ((typeof thisProfile.locationContext !== 'undefined') && (typeof thisProfile.locationContext.text !== 'undefined')) ? thisProfile.locationContext.text : ''
            }
          }

          var chosenImage = this;

          // Header content output.
          var gallery_title = palette.find('#mainTitle').text(),
              record_title = $(chosenImage).attr('data-title').replace('<p>', '').replace('</p>', ''),
              record_contributor = $(chosenImage).attr('data-contributor'),
              list = $('<ul />'),
              list_item_one = $('<li />').text(allContent.headerContent.artworkMaterial),
              list_item_two = $('<li />').text(allContent.headerContent.artworkContributor);

          list.append(list_item_one, list_item_two);

          gallery_record_block( gallery_title, record_title, list[0].outerHTML, $(chosenImage).attr('data-imgsrc'), allContent );

        });

        /**
         * Scrolling
         */
        $(palette).append('<span id="panLeft" class="panner" data-scroll-modifier="-1"><a href="javascript:void(0);" class="button">&larr;</a></span>');
        $(palette).append('<span id="panRight" class="panner" data-scroll-modifier="1"><a href="javascript:void(0);" class="button">&rarr;</a></span>');
        // Start scrolling
        $('.panner').on('mouseenter', function () {
            var data = $(this).data('scroll-modifier'),
                direction = parseInt(data, 10),
                scrollStep = 6;
            $(this).find('a.button').addClass('active');
            startScrolling(direction, scrollStep, $('#secondaryImagesContainer'));
        });
        // Stop scrolling
        $('.panner').on('mouseleave', function () {
            stopScrolling();
            $(this).find('a.button').removeClass('active');
        });
        
      },
      close: function() {
        // Un-freeze the parent window to restore scrolling.
        $('body').css('overflow', 'auto');
        $('#edan_image_search_results_container').css('overflow-y', 'scroll');
      }
    });
  }

  /**
   * Scroll handlers
   */
  var scrollHandle = 0;

  // Start scrolling handler
  function startScrolling(modifier, step, parent) {
    if(scrollHandle === 0) {
      scrollHandle = setInterval(function () {
        var newOffset = parent.scrollLeft() + (step * modifier);
        parent.scrollLeft(newOffset);
      }, 10);
    }
  }

  // Stop scrolling handler
  function stopScrolling() {
    clearInterval(scrollHandle);
    scrollHandle = 0;
  }

  /**
   * Save to textarea.
   */
  function saveToTextarea(thisElement) {
    var parentDivId = $( thisElement ).parent().attr('id');
    var thisUl = $( thisElement ).clone();
    var textareaIdPart = (parentDivId === 'edan_dropped_image') ? 'primary-image' : 'secondary-images';

    // Remove content from the textarea.
    $('#edit-' + textareaIdPart).empty();
    // Only populate the textarea if there are list items (<li>).
    if($(thisUl).find('li').length) {

      var listItems = $(thisUl).find('li');
      var imagesObject = [];

      // Determine if this is a component or not.
      var components = get_components();
      var last_in_url_path = window.location.pathname.split('/').pop();
      var component_in_path = $.inArray(last_in_url_path, components);

      $.each(listItems, function(item_key, item_value) {

         if(component_in_path === -1) {
          imagesObject.push(
            {
              artworkImage: $(item_value).find('.image-wrapper img').attr('src').replace('max_w=110&max_h=160&', ''),
              artworkTitle: $(item_value).find('.image-wrapper img').attr('data-title'),
              firstName: $(item_value).find('.image-wrapper img').attr('data-firstName'),
              lastName: $(item_value).find('.image-wrapper img').attr('data-lastName'),
              artworkDate: $(item_value).find('.image-wrapper img').attr('data-date'),
              artworkMaterial: $(item_value).find('.image-wrapper img').attr('data-material') ? $(item_value).find('.image-wrapper img').attr('data-material') : '',
              artworkContributor: $(item_value).find('.image-wrapper img').attr('data-contributor') ? $(item_value).find('.image-wrapper img').attr('data-contributor') : '',
              id: $(item_value).find('.image-wrapper img').attr('data-id'),
              status: $(item_value).find('.image-wrapper img').attr('data-status'),
              linkedEdanId: $(item_value).find('.image-wrapper img').attr('data-edan-id')
            }
          );
        } else {
          imagesObject.push(
            {
              caption: $(item_value).find('.image-wrapper img').attr('data-title'),
              imageUrl: $(item_value).find('.image-wrapper img').attr('src').replace('max_w=110&max_h=160&', ''),
              credit: $(item_value).find('.image-wrapper img').attr('data-contributor'),
              linkedEdanId: $(item_value).find('.image-wrapper img').attr('data-edan-id')
            }
          );
        }
      });

      $('#edit-' + textareaIdPart).text( JSON.stringify(imagesObject) );
    }
  }

  /**
   * Get File Status
   * Check to see if a file request throws a 200.
   */
  function getFileStatus(url) {

    url = url.replace('http://ids.si.edu/', 'https://ids.si.edu/');

    var http = new XMLHttpRequest();
    http.open('HEAD', url, false); // false = synchronous
    http.send(null); // It will stop here until this http request is complete.

    // If we get here, we already have a response, since it's synchronous XHR.
    if(http.status === 200) {
      return true;
    } else {
      return false;
    }
    
  }

  /**
   * Populate images from textarea.
   */
  function populate_images_from_textarea() {

    var secondaryImagesValue = $('#edit-secondary-images').text();

    // Build-out the image container and edit buttons for the secondary image.
    if(secondaryImagesValue) {

      $.each(JSON.parse(secondaryImagesValue), function(item_key, item_value) {

        var caption_present = _.has(item_value, 'caption'),
            max_h = '',
            imageUrl = '',
            get_file = false;

        // If there is no caption present, construct the image for the gallery UI.
        if(!caption_present) {

          if( (typeof item_value.artworkImage !== 'undefined') 
              && (item_value.artworkImage.indexOf('//ids.si.edu/ids/deliveryService/') !== -1) ) {

            imageUrl = (item_value.artworkImage.indexOf('&max_w=110&max_h=160') === -1) 
              ? item_value.artworkImage + '&max_w=110&max_h=160' : item_value.artworkImage;
            max_h = '&max_h=465';

          } else {
            imageUrl = Drupal.settings.ids_url + '?id=' + item_value.artworkImage + '&max_w=110&max_h=160';
          }

          // Check to see if IDS throws a 200.
          get_file = getFileStatus( imageUrl );
          // If IDS doesn't return a status of 200, replace with a generic image.
          if(!get_file) {
            imageUrl = item_value.artworkImage;
          }

          var imageTag = $('<img />')
            .addClass('th chosen-image')
            .attr('src', imageUrl)
            .attr('alt', 'Search result image')
            .attr('title', 'An image thumbnail of ' + item_value.artworkTitle + '.')
            .attr('data-edan-id', item_value.linkedEdanId)
            .attr('data-title', item_value.artworkTitle)
            .attr('data-firstName', item_value.firstName)
            .attr('data-lastName', item_value.lastName)
            .attr('data-date', item_value.artworkDate)
            .attr('data-material', item_value.artworkMaterial)
            .attr('data-contributor', item_value.artworkContributor)
            .attr('data-id', item_value.id)
            .attr('data-status', item_value.status)
            .attr('data-regular-size', item_value.artworkImage + max_h)
            .attr('width', '110')
            .attr('title', 'Click to view the full-sized image (opens in a new window/tab)')
            .on('click', function() {
              // window.location.href = item_value.artworkImage;
              window.open(
                item_value.artworkImage,
                '_blank'
              );
            });

        }
        // If there is a caption present, construct the image for a component UI.
        // (e.g makerDetails, artworkDetails, artisticContext, historicContext, locationContext)
        else {

          if( (typeof item_value.imageUrl !== 'undefined') 
              && (item_value.imageUrl.indexOf('//ids.si.edu/ids/deliveryService/') !== -1) ) {

            imageUrl = (item_value.imageUrl.indexOf('&max_w=110&max_h=160') === -1) 
              ? item_value.imageUrl + '&max_w=110&max_h=160' : item_value.imageUrl;
            max_h = '&max_h=465';
            
          } else {
            imageUrl = Drupal.settings.ids_url + '?id=' + item_value.imageUrl + '&max_w=110&max_h=160';
          }

          // Check to see if IDS throws a 200.
          get_file = getFileStatus( imageUrl );
          // If IDS doesn't return a status of 200, replace with a generic image.
          if(!get_file) {
            imageUrl = item_value.imageUrl;
          }

          var imageTag = $('<img />')
            .addClass('th chosen-image')
            .attr('src', imageUrl)
            .attr('alt', 'Search result image')
            .attr('data-title', item_value.caption)
            .attr('data-contributor', item_value.credit)
            .attr('data-regular-size', imageUrl.replace('&max_w=110&max_h=160', '&max_h=465') )
            .attr('width', '110')
            .attr('title', 'Click to view the full-sized image (opens in a new window/tab)')
            .on('click', function() {
              // window.location.href = item_value.imageUrl;
              window.open(
                item_value.imageUrl,
                '_blank'
              );
            });

        }

        build_image_list_item( $('#edan_dropped_images ul'), imageTag, item_key );
      });

    }
  
  }

  function get_components() {
    return [
      'aboutTheArtist',
      'aboutTheArtwork',
      'artisticContext',
      'historicContext',
      'locationContext',
    ];
  }

  /**
   * Build Image List Item
   */
  function build_image_list_item(thisElement, imageHtml, item_key) {

    var contributor = '';
    if(typeof $(imageHtml).attr('data-contributor') !== 'undefined') {
      contributor = $(imageHtml).attr('data-contributor');
    }

    var status = '';
    if(typeof $(imageHtml).attr('data-status') !== 'undefined') {
      status = $(imageHtml).attr('data-status');
    }

    var this_item_key = (typeof item_key !== 'undefined') ? item_key : 0;
    var components = get_components();

    // Build-out the list item, containing the EDAN image and EDAN image caption textarea.
    var listItem = $('<li />');
        edanImageCaptionContainer = $('<div />')
          .attr('class', 'edan-image-caption-container'),
        creditText = $('<p />')
          .html('<strong>Credit:</strong><br>' + contributor),
        edanImageCaption = $('<div />')
          .attr('class','form-textarea-wrapper resizable edan-image-caption'),
        edanImage = $('<div />')
          .attr('class', 'image-wrapper')
          .html( imageHtml ),
        replaceImageTrigger = $('<a />')
          .attr('href', 'javascript:void(0);')
          .attr('class', 'button search-edan-button replace-image-trigger edan-app-button-link')
          .text('Replace Image'),
        removeImageTrigger = $('<a />')
          .attr('href', 'javascript:void(0);')
          .attr('class', 'button search-edan-button remove-image-trigger edan-app-button-link')
          .text('Remove'),
        instructions = $('<p />')
          .attr('style', 'color: #777; font-size: 0.7rem;')
          .text('* Note: About the Artwork, About the Artist, Artistic Context, and Historic Context will be available to edit after the record has been saved'),
        muteImageTrigger = $('<a />')
          .attr('href', 'javascript:void(0);')
          .attr('class', 'button search-edan-button mute-image-trigger edan-app-button-link')
          .text('Mute (hide)'),
        linkSeparator = $('<span />')
          .attr('class', 'edan-app-button-link-separator')
          .html('&nbsp|&nbsp;'),
        edanActionsContainer = $('<div />').attr('class', 'edan-actions-container'),
        edanActionsContainerComponentLink = $('<a />')
          .attr('href', window.location.pathname + '/imageMetadata?key=' + this_item_key)
          .attr('class', 'button search-edan-button edan-app-button-link')
          .text('Manage Components'),
        statusIndicator = $('<span />')
          .css('float', 'left')
          .css('color', 'red')
          .css('font-weight', 'bold')
          .text(status == 1 ? 'Unpublished' : '');

    // Append the (3) buttons and the mute image link to the edan-actions-container,
    // but only if it's a pre-existing image. Otherwise, append instructional text.
    var last_in_url_path = window.location.pathname.split('/').pop();
    var component_in_path = $.inArray(last_in_url_path, components);

    // GALLERY IMAGES
    // Append buttons.
    if( (typeof $(imageHtml).attr('data-id') !== 'undefined') && (component_in_path === -1) ) {
      // edanActionsContainer.append( edanActionsContainerComponentLink, muteImageTrigger, replaceImageTrigger);
      edanActionsContainer.append( edanActionsContainerComponentLink, replaceImageTrigger);
    }
    // Append instructional text.
    if( $(imageHtml).hasClass('new-image') ) {
      edanActionsContainer.append(instructions);
    }

    // COMPONENT IMAGES
    var url_parts = window.location.pathname.split('/'),
        last_url_part = url_parts.pop(),
        arr = document.URL.match(/key=([0-9]+)/),
        key = arr ? '/' + arr[1] : '';

    // Append buttons.
    if( (component_in_path !== -1) && !$(imageHtml).hasClass('new-image') ) {
      var edanActionsContainerMetadataLink = $('<a />')
          .attr('href', window.location.pathname + key + '/imageMetadata?key=' + this_item_key)
          .attr('class', 'button search-edan-button edan-app-button-link')
          .text('Manage Image Metadata');
      edanActionsContainer.append(edanActionsContainerMetadataLink);
      // edanActionsContainer.append(muteImageTrigger, replaceImageTrigger);
      edanActionsContainer.append(replaceImageTrigger);
    }
    // Append instructional text.
    if( (component_in_path !== -1) && $(imageHtml).hasClass('new-image') ) {
      edanActionsContainer.append(instructions);
    }

    // Append the remove image trigger button to the edan-actions-container.
    edanActionsContainer.append(removeImageTrigger);
    // Append the word Unpublished to indicate the status of the art profile record.
    edanActionsContainer.append( statusIndicator );
    // Append the textarea and edan-actions-container to the edan-image-caption div.
    edanImageCaption.append(edanActionsContainer);
    // Append the EDAN image and EDAN image caption to the edan-image-caption-container div.
    edanImageCaptionContainer.append( edanImage, edanImageCaption );
    // Append the image and caption container to the list item. 
    listItem.append(edanImageCaptionContainer);
    // Lastly, append the list item to the unordered list.
    listItem.appendTo( thisElement );
  }

  var scaleFunction = {
    scaleImg: function(imgSource){
      scaleImage(imgSource.record_image, 475, 475, function(canvas){
        $(canvas).attr('class', 'canvas-image');
        $('.gallery-record-column.column-right').append($(canvas));
      })
    }
  }

  /**
   * Underscore-based gallery record template
   */
  function gallery_record_block( gallery_title, record_title, record_data, record_image, allContent ) {

    var scale = '',
        gallery_record_container_id = _.uniqueId(),
        gallery_record_template = _.template( $('#gallery_record_template').html() ),
        edan_app_message_container = '';

    var gallery_record_markup = gallery_record_template({
        'record_id': gallery_record_container_id,
        'gallery_title': gallery_title,
        'record_title': record_title,
        'artist_name': allContent.headerContent.artistName,
        'record_data': record_data,
        'record_image': record_image,
        'edan_app_message': $(edan_app_message_container).html(),
        'scaleImg': scaleFunction.scaleImg
    });

    var palette = $('#preview_record_modal');
    palette.children().hide();
    palette.append(gallery_record_markup);

    // Initially, populate data from the About the Artist section.
    $('.app_preloader').show();
    // Scale and append the images.
    $.each(allContent['aboutTheArtist'].images, function(image_key, image_value) {
      $('<img/>',{
          src: image_value.imageUrl,
          load: function(){
            // Calculate aspect ratio.
            var imgDimensions = calculateAspectRatioFit(this.width, this.height, 350, 500);
            // Scale the image.
            scaleImage(image_value.imageUrl, imgDimensions.width, imgDimensions.height, function(canvas){
              $(canvas).attr('class', 'section-image').attr('style', 'display:none;');
              $('.record-body').append(canvas);
            });
          }
      });
    });
    // Append the body text
    setTimeout(function() {
      $('.app_preloader').hide();
      // Prepend the section title.
      $('.record-body').prepend( $('<h4 />').html('ABOUT THE ARTIST') );
      $('.record-body').find('.section-image').fadeIn();
      $('.record-body').append(allContent['aboutTheArtist'].text);
    }, 3000);

    // When a navigation item is clicked, load that section's data.
    $('.gallery-navigation-trigger').on('click', function(){

      $('.gallery-navigation').find('li').removeClass('bold-font');
      $(this).attr('class', 'bold-font');

      // Set up the data.
      var section_title = $(this).html().replace('<br>', ' '),
          destination = $(this).attr('data-destination'),
          data = allContent[destination];

      // Remove contents from the record-body container.
      $('.record-body').empty();
      $('.app_preloader').show();
      // Scale and append the images.
      $.each(data.images, function(image_key, image_value) {
        $('<img/>',{
            src: image_value.imageUrl,
            load: function(){
              // Calculate aspect ratio.
              var imgDimensions = calculateAspectRatioFit(this.width, this.height, 350, 500);
              // Scale the image.
              scaleImage(image_value.imageUrl, imgDimensions.width, imgDimensions.height, function(canvas){
                $(canvas).attr('class', 'section-image').attr('style', 'display:none;');
                $('.record-body').append(canvas);
              });
            }
        });
      });
      // Append the body text
      setTimeout(function() {
        $('.app_preloader').hide();
        // Prepend the section title.
        $('.record-body').prepend( $('<h4 />').html(section_title) );
        $('.record-body').find('.section-image').fadeIn();
        $('.record-body').append(data.text);
      }, 3000);

    });

    // Go back to the main gallery interface.
    $('.gallery-navigation li .go-back-trigger').on('click', function(){
      $('.gallery-record').remove();
      $('.gallery-navigation').remove();
      palette.children().fadeIn();
    });
  }

  /**
  * Conserve aspect ratio of the orignal region. Useful when shrinking/enlarging
  * images to fit into a certain area.
  *
  * @param {Number} srcWidth Source area width
  * @param {Number} srcHeight Source area height
  * @param {Number} maxWidth Fittable area maximum available width
  * @param {Number} maxHeight Fittable area maximum available height
  * @return {Object} { width, heigth }
  */
  function calculateAspectRatioFit(srcWidth, srcHeight, maxWidth, maxHeight) {
    var ratio = Math.min(maxWidth / srcWidth, maxHeight / srcHeight);
    return { width: srcWidth*ratio, height: srcHeight*ratio };
  }

  /**
   * Check if an image has been uploaded.
   */
  function uploadChecker() {

    var all_image_sources = [],
        file_upload_source = $('#edan_image_search #file_upload').val(),
        all_images = $('#edan_dropped_images ul').find('img');

    $.each(all_images, function(image_key, image_value) {
      all_image_sources.push($(image_value).attr('src'));
    });

    if((typeof file_upload_source !== 'undefined') && file_upload_source.length) {

      if(all_image_sources.indexOf(file_upload_source) === -1) {

        // If we're replacing...
        if($( '#edan_dropped_images ul' ).find('li').hasClass('replacing')) {

          var target = $( '#edan_dropped_images ul' ).find('li.replacing'),
              thisImage = $(target).find('.image-wrapper').find( 'img' ),
              imageTag = $('<img />')
                .attr('src', Drupal.settings.ids_url + '?max_w=110&max_h=160&id=' + file_upload_source)
                .attr('class', thisImage.attr('class'))
                .attr('alt', thisImage.attr('alt'))
                .attr('title', thisImage.attr('title'))
                .attr('data-firstName', thisImage.attr('data-firstName'))
                .attr('data-lastName', thisImage.attr('data-lastName'))
                .attr('data-title', thisImage.attr('data-title'))
                .attr('data-edan-id', thisImage.attr('data-edan-id'))
                .attr('data-date', thisImage.attr('data-date'))
                .attr('data-material', thisImage.attr('data-material'))
                .attr('data-contributor', thisImage.attr('data-contributor'))
                .attr('data-id', thisImage.attr('data-id'))
                .attr('data-status', thisImage.attr('data-status'))
                .attr('data-regular-size', file_upload_source);

          $( thisImage ).remove();
          $( target ).find('.image-wrapper').append( imageTag );

          clearInterval(startUploadChecker);

          // Save to textarea
          saveToTextarea( target.parent() );

        } else {

          var imageTag = $('<img />')
            .addClass('th new-image')
            .attr('src', file_upload_source)
            .attr('alt', 'Search result image')
            .attr('data-regular-size', file_upload_source)
            .attr('width', '110');

            build_image_list_item( $('#edan_dropped_images ul'), imageTag, all_images.length );
            saveToTextarea( $('#edan_dropped_images ul') );
          
        }

      }



    }

  }

  /**
   * Scales images with canvas then runs a callback.
   */
  function scaleImage(url, width, height, callback) {

    var img = new Image(),
    width = width,
    height = height,
    callback;

    // When the images is loaded, resize it in canvas.
    img.onload = function() {
      var canvas = document.createElement("canvas"),
      ctx = canvas.getContext("2d");

      canvas.width = width;
      canvas.height= height;

      // draw the img into canvas
      ctx.drawImage(this, 0, 0, width, height);

      // Run the callback on what to do with the canvas element.
      callback(canvas);
    };

    img.src = url;
  }

  /**
   * Underscore-based message template
   */
  function gallery_message(message) {
    var gallery_message_container_id = _.uniqueId(),
        gallery_message_template = _.template( $('#gallery_message_template').html() ),
        gallery_message_markup = gallery_message_template({
          'message': message
        });
    $('#preview_record_modal').append(gallery_message_markup);
  }

});