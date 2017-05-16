
jQuery(document).ready(function($) {

  // Remove table states, due to updates to the code.
  $(document).on( 'init.dt', function (e, settings) {
    // console.log(new Date().getTime());
    Object.keys(localStorage)
    .forEach(function(key){
      if (/^DataTables_browse_/.test(key)) {
        var data = JSON.parse(localStorage[key]);
        // console.log(data.time);
        if(data.time < 1492618789309) {
          localStorage.removeItem(key);
        }
      }
    });
  });

  // jQuery Datatables - The Arc
  $('#browse_the_arc').dataTable({
    "columnDefs" : [ {
        // Enable sorting on the first and second column.
        'sortable' : true,
        // Disable sorting on the columns with the 'no-sort' css class.
        'sortable' : false,
        'targets' : [ 'no-sort' ],
        // Actions column width.
        'width': '12%',
        'targets': 1
      } ],
      "order": [[ 0, 'asc' ]],
      'lengthChange' : true,
      "lengthMenu": [ [25, 50, 100, -1], [25, 50, 100, "All"] ],
      "searching": true,
      "stateSave": true
    });

  // jQuery Datatables - Visual Arts Tour
  $('#browse_visual_arts_tour').dataTable({
    "columnDefs" : [ {
        // Enable sorting on the first and second column.
        'sortable' : true,
        // Disable sorting on the columns with the 'no-sort' css class.
        'sortable' : false,
        'targets' : [ 'no-sort' ],
        // Actions column width.
        'width': '2%',
        'targets': 1
      } ],
      "order": [[ 0, 'asc' ]],
      'lengthChange' : true,
      "lengthMenu": [ [25, 50, 100, -1], [25, 50, 100, "All"] ],
      "searching": true,
      "stateSave": true
    });

  // jQuery Datatables - Reflections Booth
  $('#browse_reflections_booth').dataTable({
    "columnDefs" : [ {
        // Enable sorting on the first and second column.
        'sortable' : true,
        // Disable sorting on the columns with the 'no-sort' css class.
        'sortable' : false,
        'targets' : [ 'no-sort' ]
      } ],
      "order": [[ 0, 'asc' ]],
      'lengthChange' : true,
      "lengthMenu": [ [25, 50, 100, -1], [25, 50, 100, "All"] ],
      "searching": true,
      "stateSave": true
    });

  // // jQuery Datatables - Reflections Booth Videos - Approved
  // $('#browse_reflections_booth_videos').dataTable({
  //   "columnDefs" : [ {
  //       // Enable sorting on the first and second column.
  //       'sortable' : true,
  //       // Disable sorting on columns.
  //       'sortable' : false,
  //       'targets' : [ 0, 4 ]
  //     } ],
  //     "order": [[ 3, 'desc' ]],
  //     'lengthChange' : true,
  //     "lengthMenu": [ [25, 50, 100, -1], [25, 50, 100, "All"] ],
  //     "searching": true,
  //     "stateSave": true
  //   });

  var currentUrlPath = window.location.pathname.split('/'),
      galleryTagId = currentUrlPath[currentUrlPath.length - 1];

  // jQuery Datatables - Reflections Booth Videos - Approved
  $('#browse_reflections_booth_videos').on('init.dt', function(){
    // Do stuff here upon initialization, if need be...
    // $('#page-wrapper').attr('style', 'background-color: #FFFFFF;');
  }).on('processing.dt', function ( e, settings, processing ) {
    // Show / hide notification of table processing.
    // https://datatables.net/reference/event/processing
    $('#processingIndicator').css( 'display', processing ? 'block' : 'none' );
  }).dataTable({
    "columns": [
    { "data": "content_processed.thumb" },
    { "data": "content_processed.question" },
    { "data": "content_processed.status" },
    { "data": "content_processed.timestamp" },
    { "data": "content_processed.reviewer" },
    { "data": "content_processed.tags" },
    { "data": "content_processed.age" },
    { "data": "content_processed.gender" },
    { "data": "content_processed.race" },
    { "data": "content_processed.groupType" },
    { "data": "content_processed.location" },
    { "data": "content_processed.recordId" },
    ],
    "columnDefs" : [ {
      // Enable sorting on all columns.
      'sortable' : true,
      // Disable sorting on the columns 1, 2, and 4.
      'sortable' : false,
      'targets' : [ 0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11 ],
      // Apply classes.
      // {'targets': [ 0, 2 ], className: 'centered-table-cell'}
      'targets' : 3,
      'createdCell':  function (td, cellData, rowData, row, col) {
        $(td).attr('data-order', rowData.lastTimeUpdated).attr('style', 'width: 12%');
      }
    } ],
    // All data management will be done on the server side.
    "serverSide": true,
    // Path to the file that is going to handle the queries.
    "ajax": {
      "url": "/miss/reflections-booth/ajax",
      "data": function ( d ) {
        d.gallery_tag_id = galleryTagId;
        d.status = '0';
      }
    },
    // Method type.
    "serverMethod": "GET",
    "order": [[ 3, 'desc' ]],
    'lengthChange' : true,
    "lengthMenu": [ [10, 25, 50, 100], [10, 25, 50, 100] ],
    "searching": false,
    "stateSave": true,
    "processing": true,
    "dom": '<rlfip<t>ip>',
    "pagingType": 'full_numbers',
    "infoCallback": function( settings, start, end, max, total, pre ) {
      return 'Showing ' + start + ' to ' + end + ' of ' + total + ' records';
    },
    "rowCallback": function( row, data, index ) {
      // Add the edit link.
      var recordLink = $('<a />').attr('href', '/miss/manage/reflectionsBooth/videos/' + data.content_processed.galleryTagId + '/' + data.content_processed.recordId).text('Edit');
      $(row).find('td:eq(11)').html(recordLink);
      if(data.content_processed.deletionRecommendation == true){
        for (var i = 0; i <= 11; i++) {
          $(row).find('td:eq(' + i + ')').css('background-color', '#FFCCCB');
        }
      }
    }
  });

  // jQuery Datatables - Reflections Booth Videos - Unapproved
  $('#browse_reflections_booth_videos_unapproved').on('init.dt', function(){
    // Do stuff here upon initialization, if need be...
    // $('#page-wrapper').attr('style', 'background-color: #FFFFFF;');
  }).on('processing.dt', function ( e, settings, processing ) {
    // Show / hide notification of table processing.
    // https://datatables.net/reference/event/processing
    $('#processingIndicator').css( 'display', processing ? 'block' : 'none' );
  }).dataTable({
    "columns": [
    { "data": "content_processed.thumb" },
    { "data": "content_processed.question" },
    { "data": "content_processed.status" },
    { "data": "content_processed.timestamp" },
    { "data": "content_processed.reviewer" },
    { "data": "content_processed.tags" },
    { "data": "content_processed.age" },
    { "data": "content_processed.gender" },
    { "data": "content_processed.race" },
    { "data": "content_processed.groupType" },
    { "data": "content_processed.location" },
    { "data": "content_processed.recordId" },
    ],
    "columnDefs" : [ {
      // Enable sorting on all columns.
      'sortable' : true,
      // Disable sorting on the columns 1, 2, and 4.
      'sortable' : false,
      'targets' : [ 0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11 ],
      // Apply classes.
      // {'targets': [ 0, 2 ], className: 'centered-table-cell'}
    } ],
    // All data management will be done on the server side.
    "serverSide": true,
    // Path to the file that is going to handle the queries.
    "ajax": {
      "url": "/miss/reflections-booth/ajax",
      "data": function ( d ) {
        d.gallery_tag_id = galleryTagId;
        d.status = '1';
      }
    },
    // Method type.
    "serverMethod": "GET",
    "order": [[ 3, 'desc' ]],
    'lengthChange' : true,
    "lengthMenu": [ [10, 25, 50, 100], [10, 25, 50, 100] ],
    "searching": false,
    "stateSave": true,
    "processing": true,
    "dom": '<rlfip<t>ip>',
    "pagingType": 'full_numbers',
    "infoCallback": function( settings, start, end, max, total, pre ) {
      return 'Showing ' + start + ' to ' + end + ' of ' + total + ' records';
    },
    "rowCallback": function( row, data, index ) {
      // Add the edit link.
      var recordLink = $('<a />').attr('href', '/miss/manage/reflectionsBooth/videos/' + data.content_processed.galleryTagId + '/' + data.content_processed.recordId).text('Edit');
      $(row).find('td:eq(11)').html(recordLink);
      if(data.content_processed.deletionRecommendation == true){
        for (var i = 0; i <= 11; i++) {
          $(row).find('td:eq(' + i + ')').css('background-color', '#FFCCCB');
        }
      }
    }
  });

  // jQuery Datatables - Reflections Booth Videos - Pending Review
  // $('#browse_reflections_booth_videos_unapproved').dataTable({
  //   "columnDefs" : [ {
  //       // Enable sorting on the first and second column.
  //       'sortable' : true,
  //       // Disable sorting on columns.
  //       'sortable' : false,
  //       'targets' : [ 0, 3 ]
  //     } ],
  //     "order": [[ 2, 'desc' ]],
  //     'lengthChange' : true,
  //     "lengthMenu": [ [25, 50, 100, -1], [25, 50, 100, "All"] ],
  //     "searching": true,
  //     "stateSave": true
  //   });

  // jQuery Datatables - The Hub
  $('#browse_the_hub').dataTable({
    "columnDefs" : [ {
        // Enable sorting on the first and second column.
        'sortable' : true,
        // Disable sorting on the columns with the 'no-sort' css class.
        'sortable' : false,
        'targets' : [ 'no-sort' ]
      } ],
      "order": [[ 0, 'asc' ]],
      'lengthChange' : true,
      "lengthMenu": [ [25, 50, 100, -1], [25, 50, 100, "All"] ],
      "searching": true,
      "stateSave": true
    });

  // jQuery Datatables - Record Store
  $('.miss_datatables').dataTable({
    "columnDefs" : [ {
        // Enable sorting on the first and second column.
        'sortable' : true,
        // Disable sorting on the columns with the 'no-sort' css class.
        'sortable' : false,
        'targets' : [ 'no-sort' ]
      } ],
      "order": [[ 0, 'asc' ]],
      'lengthChange' : true,
      "lengthMenu": [ [25, 50, 100, -1], [25, 50, 100, "All"] ],
      "searching": true,
      "stateSave": true
    });

  // jQuery Datatables - File UI
  $('#browse_files').on('init.dt', function(){
    // Do stuff here upon initialization, if need be...
    $('#page-wrapper').attr('style', 'background-color: #FFFFFF;');
  }).on('processing.dt', function ( e, settings, processing ) {
    // Show / hide notification of table processing.
    // https://datatables.net/reference/event/processing
    $('#processingIndicator').css( 'display', processing ? 'block' : 'none' );
  }).dataTable({
    "columns": [
    { "data": "content.fileId" },
    { "data": "content.originalName" },
    { "data": "content.filescanner[0].scanDate" },
    { "data": "content.fileId" },
    ],
    "columnDefs" : [ {
      // Enable sorting on all columns.
      'sortable' : true,
      // Disable sorting on the columns 1, 2, and 4.
      'sortable' : false,
      'targets' : [ 0, 1, 3 ],
      // Apply classes.
      // {'targets': [ 0, 2 ], className: 'centered-table-cell'}
    } ],
    // All data management will be done on the server side.
    "serverSide": true,
    // Path to the file that is going to handle the queries.
    "ajax": "/miss/files/ajax",
    // Method type.
    "serverMethod": "GET",
    "order": [[ 2, 'asc' ]],
    'lengthChange' : true,
    "lengthMenu": [ [10, 25, 50, 100], [10, 25, 50, 100] ],
    "searching": false,
    "stateSave": true,
    "processing": true,
    "dom": '<rlfip<t>ip>',
    "pagingType": 'simple',
    "rowCallback":function( row, data, index ) {
      // Create the checkboxes.
      var insertLink = $('<a />')
      .addClass('insert-button')
      .addClass('button')
      .attr('style', 'float:left;')
      .attr('href', 'javascript:void(0);')
      .attr('insert-link', data.content.filepath)
      // .attr('onclick', 'if (event.preventDefault) event.preventDefault(); else event.returnValue = false;')
      .text('Insert')
      .click(function(e){
        e.preventDefault();
        window.opener.missPerformInsert(window.name, jQuery(this).attr("insert-link"));
      });

      $(row).find('td:eq(0)')
        .html(insertLink)
        .addClass("manage_column");

      $(row).find('td:eq(1)')
        .html('<a href="' + data.content.filepath + '" target="_blank">' + data.content.filepath + '</a>');

      var rotateLink = $('<a />')
        .addClass('view-rotate-button')
        .addClass('button')
        .attr('style', 'float:left;')
        .attr('view-rotate-url', data.content.filepath)
        .attr('edan-id', data.id)
        .attr('onclick', 'if (event.preventDefault) event.preventDefault(); else event.returnValue = false;')
        .text('View/Rotate')
        .click(function(){
          var previewRotateUrl = (jQuery(this).attr("view-rotate-url"));
          var edanId = (jQuery(this).attr("edan-id"));
          var imageHolder = '<div class="image-holder-outer">';
          imageHolder += '<div class="image-holder-inner">';
          imageHolder += '<div class="loading-div">Loading...</div>';
          imageHolder += '<img src="'+previewRotateUrl+'" class="rotatable rotated-none">';
          imageHolder += '</div>';
          imageHolder += '<a class="button rotater-cw">Clockwise</a>';
          imageHolder += '<a class="button rotater-save" edan-id="'+edanId+'">Save</a>';
          imageHolder += '</div>';
          setTimeout(function(){ jQuery(".loading-div").hide(); }, 1000);
          jQuery(this).parent().append(imageHolder);
          jQuery(this).hide();
          jQuery(".rotater-cw").click(function(){
            if (jQuery(this).parent().find("img").hasClass("rotated-none")) {
              jQuery(this).parent().find("img").removeClass("rotated-none");
              jQuery(this).parent().find("img").addClass("rotated-cw");
            }
            else if (jQuery(this).parent().find("img").hasClass("rotated-cw")) {
              jQuery(this).parent().find("img").removeClass("rotated-cw");
              jQuery(this).parent().find("img").addClass("rotated-180");
            }
            else if (jQuery(this).parent().find("img").hasClass("rotated-180")) {
              jQuery(this).parent().find("img").removeClass("rotated-180");
              jQuery(this).parent().find("img").addClass("rotated-ccw");
            }
            else if (jQuery(this).parent().find("img").hasClass("rotated-ccw")) {
              jQuery(this).parent().find("img").removeClass("rotated-ccw");
              jQuery(this).parent().find("img").addClass("rotated-none");
            }
          });
      });

      if(data.content.filepath.substring(data.content.filepath.length-4) === 'jpeg'
        || data.content.filepath.substring(data.content.filepath.length-3) === 'jpg'
        || data.content.filepath.substring(data.content.filepath.length-3) === 'png'
        || data.content.filepath.substring(data.content.filepath.length-3) === 'gif') {
        $(row).find('td:eq(3)').html(rotateLink);
      } else {
        $(row).find('td:eq(3)').html('');
      }

    }
  });

  // jQuery Datatables - MISS Backups
  $('#browse_data_backups').dataTable({
    "columnDefs" : [ {
        // Enable sorting on the first and second column.
        'sortable' : true,
        // Disable sorting on the columns with the 'no-sort' css class.
        'sortable' : false,
        'targets' : [ 1 ]
      } ],
      "order": [[ 0, 'desc' ]],
      'lengthChange' : true,
      "lengthMenu": [ [25, 50, 100, -1], [25, 50, 100, "All"] ],
      "searching": true,
      "stateSave": true
    });
});
