(function($) {

  Drupal.behaviors.missFileManage = {
    attach: function (context, settings) {

      $('.delete-button', context).on('click', function(event){

        var filepath_string = $(this).parent().parent().find('td:nth-child(2)').find('.miss-direct-link').find('a').attr('href');

        $.ajax({
          type: 'GET'
          ,dataType: 'json'
          ,url: '/miss/files/delete_file/ajax'
          ,data: ({filepath_string: filepath_string})
          ,success: function(results){
            
            if(results.error) {
              alert(results.error);
              return;
            }

            // If there are no errors, process.
            if(results.message === 'content released') {
              alert('The file asset has been deleted (EDAN message: ' + results.message + '). The file listing will reload automatically after dismissing this message.');
              location.reload();
            }

          }
        });

      });
    }
  };

}(jQuery));