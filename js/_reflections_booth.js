(function($) {

  "use strict";

  var video = document.getElementById('video_element'),
      $output,
      scale = 0.085,
      poster_scale = 0.5;

  // On pause, record the current time.
  video.onpause = function() {
    $('.markerStartTime, .markerEndTime').attr('data-current-time' ,video.currentTime);
  };
  // When user is finished moving/skipping to a new position, record the current time.
  video.onseeked = function() {
    $('.markerStartTime, .markerEndTime').attr('data-current-time' ,video.currentTime);
  };

  // Set the input value to the current time.
  $('.markerStartTime, .markerEndTime').on('click', function(event){
    event.preventDefault();
    var thisButtonClass = $(this).attr('class');
    var currentVideoTime = $(this).attr('data-current-time');
    $('input[name="' + thisButtonClass + '"]').val( currentVideoTime );
  });

  // Generate a poster image
  var initialize = function() {
    $output = $('#screenShot');
    // $('setPosterImage').click(captureImage);
    // Set the poster image.
    $('.setPosterImage').on('click', function(event){
      event.preventDefault();
      $('#screenShot').empty();
      captureImage();
    });             
  };

  var captureImage = function() {

    // POSTER IMAGE
    var canvas = document.getElementById('canvas');
    canvas.width = video.videoWidth;
    canvas.height = video.videoHeight;

    // Draw the image.
    canvas.getContext('2d').drawImage(video, 0, 0, canvas.width, canvas.height);

    var img = new Image();
    img.src = canvas.toDataURL("image/png");
    // Output the image to the UI.
    $output.prepend(img).fadeIn();
    // Write the image source to the hidden input.
    $('.video-poster-image').val( $(img).attr('src') );

    // THUMBNAIL IMAGE
    var canvasThumb = document.getElementById('canvasThumb');
    canvasThumb.width = video.videoWidth * scale;
    canvasThumb.height = video.videoHeight * scale;

    // Draw the image.
    canvasThumb.getContext('2d').drawImage(video, 0, 0, canvasThumb.width, canvasThumb.height);

    var imgThumb = new Image();
    imgThumb.src = canvasThumb.toDataURL("image/png");
    // Write the image source to the hidden input.
    $('.video-thumbnail-image').val( $(imgThumb).attr('src') );

  };

  $(initialize);
  

}(jQuery));