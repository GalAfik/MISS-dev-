(function($) {

var missUploadFormWindow;

missCreateUploadFor = function(selector, windowName) {
  var addInsertTo = $(selector);
  missOpenUploadFor = function(toInsertNameInto) {


    if ((window.missUploadForm == null) || (typeof window.missUploadForm === 'undefined')) {
      // missUploadFormWindow = window.open(Drupal.settings.basePath + 'miss/files/base_form', '_blank');
      missUploadFormWindow = window.open(Drupal.settings.basePath + 'miss/files/base_form', windowName);
      missUploadFormWindow.focus();
    }
  };

  $('<a>',{
    text: 'Upload / Insert',
    title: 'Upload / Insert',
    href: '#',
    class: 'button',
    click: function(){
      window.selectorForWindow[windowName] = selector;
      missOpenUploadFor( addInsertTo );
      return false;
    }
  }).insertAfter(addInsertTo);

  if (typeof(window.selectorForWindow) == 'undefined') window.selectorForWindow = {};
  window.selectorForWindow[windowName] = selector;
}

missPerformInsert = function(window_name, insertText) {

  console.log(window_name);
  console.log(insertText);

  var selector = window.selectorForWindow[window_name];
  //
  // console.log('hellooooooooooo?');
  // console.log(selector);
  // console.log(insertText);

  $(selector).val(insertText);
  missUploadFormWindow.close();
}

Drupal.behaviors.missFileManage = {
  attach: function (context, settings) {
    $('input.miss-file-manage', context).once(function() {
      missCreateUploadFor(this, 'upload_form');
    });
  }
};

}(jQuery));
