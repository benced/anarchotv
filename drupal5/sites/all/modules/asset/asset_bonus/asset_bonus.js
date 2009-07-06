if (Drupal.jsEnabled) {
  $(document).ready(function () {
    var form = $('#asset-bonus-admin-mp3player');
    inputs = [];
    var hooks = [];
    var locks = [];
    var focused = null;

    // Add Farbtastic
    $(form).prepend('<div id="placeholder"></div>');
    var farb = $.farbtastic('#placeholder');

    function loadAudioPlayer(){
      var swfPath = Drupal.settings.asset_bonus_swf;
      if(!swfPath){
        return;
      }
      var oSwf = new SWFObject(swfPath,"color-preview","290","24","7","#FFFFFF");
      oSwf.addVariable('playerID','color-preview');
      oSwf.addVariable('soundFile',Drupal.settings.asset_bonus_test_mp3);
      oSwf.addVariable('autostart','yes');
      oSwf.addVariable('loop','yes');
      oSwf.addParam('wmode','transparent');
      oSwf.addParam('menu','false');
      
      for(var i=0;i<inputs.length;i++){
        if(inputs[i].value){
          oSwf.addVariable(inputs[i].key,inputs[i].value.replace(/#/,'0x'));
        }
      }
      oSwf.write('color-map');
    }

    function preview(){
      var audioplayer = document.getElementById("color-preview");
      if(audioplayer){
        for(var i=0;i<inputs.length;i++){
          audioplayer.SetVariable(inputs[i].key, inputs[i].value.replace("#", "0x"));
        }
        audioplayer.SetVariable("setcolors", 1);
      }
    }
    
    /**
     * Callback for Farbtastic when a new color is chosen.
     */
    function callback(input, color, propagate, colorscheme) {
      // Set background/foreground color
      $(input).css({
        backgroundColor: color,
        color: farb.RGBToHSL(farb.unpack(color))[2] > 0.5 ? '#000' : '#fff'
      });

      // Change input value
      if (input.value && input.value != color) {
        input.value = color;
      }
      
      preview();
      
    }

    // Focus the Farbtastic on a particular field.
    function focus() {
      var input = this;
      // Remove old bindings
      focused && $(focused).unbind('keyup', farb.updateValue)
          .unbind('keyup', preview).unbind('blur',preview)
          .parent().removeClass('item-selected');

      // Add new bindings
      focused = this;
      farb.linkTo(function (color) { callback(input, color, true, false) });
      farb.setColor(this.value);
      $(focused).keyup(farb.updateValue).keyup(preview).blur(preview)
        .parent().addClass('item-selected');
    }

    // Initialize color fields
    $('#palette input.form-text', form).each(function () {
      // Extract palette field name
      this.key = this.id.substring(27);

      // Link to color picker temporarily to initialize.
      farb.linkTo(function () {}).setColor('#000').linkTo(this);

      inputs.push(this);
    })
    .focus(focus);

    // Focus first color
    focus.call(inputs[0]);

    // Render preview
    loadAudioPlayer();
  });
}