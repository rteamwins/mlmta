(function ($) {
  $(function() {
    console.log('Here we go!');
    if ($('#edit-student-1-new-teacher').is(":checked")) {
      // $("#teacher-1-fieldset :input").prop('disabled', false);
      $(".form-item-student-1-previous-teachers").hide('fast');
      $("#edit-student-1-teacher-fieldset-message").hide('fast');
    }
    var fields;
    $("#edit-student-1-new-teacher").once('createNewTeacher').on('change', function(){
      console.log('process');
      $("#teacher-1-fieldset :input").prop('disabled', false).val('');
      if ($('#edit-student-1-new-teacher').is(":checked")) {
        $(".form-item-student-1-previous-teachers").hide('fast');
        $("#edit-student-1-teacher-fieldset-message").hide('fast');
        //fields = [];
        //$("#teacher-1-fieldset :input").each(function() {
        //  fields.push($(this).val());
        //});
      } else {
        $(".form-item-student-1-previous-teachers").show('fast');
        $("#edit-student-1-previous-teachers").val('_blank');
        $("#edit-student-1-previous-teachers").trigger('chosen:updated');
        $("#edit-student-1-teacher-fieldset-message").show('fast');
        //$("#teacher-1-fieldset :input").prop('disabled', true)   
        //for (var i = 0; i < fields.length; i++) {
        //  $("#teacher-1-fieldset :input")[i].value  = fields[i];
        //}
      }        
    });
  });
})(jQuery)

