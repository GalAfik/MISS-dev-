jQuery(document).ready(function(){
 jQuery(".view-rotate-button").each(function(){
  jQuery(this).click(function(){
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
    jQuery(".rotater-save").click(function(){
      var edanId = (jQuery(this).attr("edan-id"));
      var imageObj = jQuery(this).parent().find("img");
      var rotationAmount = imageObj.hasClass("rotated-cw")*90 + imageObj.hasClass("rotated-180")*180 + imageObj.hasClass("rotated-ccw")*270;
      var myParent = jQuery(this).parent();
      if (rotationAmount === 0) return;
      jQuery.ajax({
        'url': '/miss/files/image_rotate/'+edanId+'/'+rotationAmount,
        'dataType':'json',
        'success' : function(ajax_return) {
           if (ajax_return['success']) {
             myParent.append("<div class='saved-rotate'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;SAVED</div>");
             var mainSrc = imageObj.attr("src");
             var mainDir = '/';
             if (mainSrc.indexOf("?") != -1) mainSrc = mainSrc.substring(0,mainSrc.indexOf('?'));
             if (mainSrc.indexOf("/") != -1) mainDir = mainSrc.substring(0,mainSrc.lastIndexOf('/')+1);
             var newSrc = mainDir+ajax_return['content']['fileId'];
             imageObj.attr("src",newSrc);
             myParent.closest("tr").find(".miss-direct-link a").attr("href",newSrc).text(newSrc);
             myParent.closest("tr").find(".insert-button").attr("insert-link",newSrc);
             imageObj.attr("class","rotatable rotated-none");
             setTimeout(function(){
               jQuery('.saved-rotate').fadeOut(3000,function(){
                 jQuery('.saved-rotate').remove();
               });
             },3000);
           }
         }
      })
    });
  });
 });
});

