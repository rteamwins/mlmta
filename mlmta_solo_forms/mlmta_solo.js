(function ($) {
  $(function() {
    console.log('Here we go!');

    $(".theory-test").on('change', function(event){
      var value = $(this).val();
      switch (value) {
        case 'Piano':
          setOptions(8);
          break;
        case 'Treble clef':
        case 'Alto clef':
        case 'Bass clef':
        case 'Guitar':
          setOptions(6);
          break;
        case 'Vocal bass clef':
        case 'Vocal treble clef':
          setOptions(4);
      }      
    });
    
    var setOptions = function(level) {
      var levels = ['P', '1', '2', '3', '4', '5', '6', '7', '8'];
      $(".theory-level").empty().append($('<option>', {
          value: '_none',
          text: '- Select -',
        }));
      for (var i=0; i<= level; i++) {
        $(".theory-level").append($('<option>',{
          value: levels[i],
          text: levels[i],
        }));
      }
      $(".theory-level").trigger("chosen:updated");
    }
  });
})(jQuery)

