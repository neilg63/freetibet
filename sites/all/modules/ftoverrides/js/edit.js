(function($) {
	
	Drupal.fto = {
    
    toggleFixedDonation: function(self,mode) {
      var it = self, s = Drupal.settings.fto,
      dlb = s.defField.parent().find('label'),
      fixed = it.is(':checked'),
      fto,vl,lb;
      switch (mode) {
        case 1:
          fixed = false;
          break;
        case 2:
          fixed = true;
          break;
      }
      if (fixed) {
        vl = s.minField.val();
        if (/^\s*\d+/.test(vl)) {
          vl = vl - 0;
        } else {
          vl = 1;
        }
        if (vl < 0) {
          vl = 1;
        }
        s.minField.val('-1').attr('data-val',vl);
        s.minField.hide();
        s.minField.parent().find('label').first().hide();
        lb = dlb.first().text();
        dlb.html("Set amount").attr("data-label",lb);
      } else {
        s.minField.show();
        s.minField.parent().find('label').show()
        vl = s.minField.attr('data-val');
        lb = dlb.attr('data-label');
        if (lb) {
          dlb.html();
        }
        if (vl) {
          s.minField.val(vl);
        }
      }
    },
    
    donationBlockEnabled: function() {
      var s = Drupal.settings.fto;
      s.blockWidgetVal = s.blockWidget.val();
      s.donationBlockEnabled = false;
      if (typeof s.blockWidgetVal == 'object') {
        if (s.blockWidgetVal instanceof Array) {
          s.donationBlockEnabled = s.blockWidgetVal.indexOf(s.donationBlockVal) >= 0;
        }
        s.donationBlockSingle = false;
      } else {
        s.donationBlockEnabled = s.blockWidgetVal == s.donationBlockVal;
        s.donationBlockSingle = true;
      }
      return s.donationBlockEnabled;
    },
    
    manageDonationSettings: function() {
      var s = Drupal.settings.fto, inps = s.donationSettings.find('input.form-text'),i=0,id;
      if (inps.length>1) {
        s.donationSettings.find('fieldset:first legend:first').remove();
        s.donationDesc = s.donationSettings.find('fieldset:first .fieldset-description:first');
        s.blockWidget = $('select#edit-field-block-und');
        if (s.blockWidget.length>0) {
          s.enableBt = $('<button id="enable-donation-form">Enable donation form</button>');
          s.donationBlockVal = 'payment_donation:payment_donation';
          s.donationDesc.append(s.enableBt);
          Drupal.fto.donationBlockEnabled();
          s.enableBt.bind('click', function(e){
            e.preventDefault();
            var s = Drupal.settings.fto;
            Drupal.fto.donationBlockEnabled();
            if (!s.donationBlockEnabled) {
              s.blockWidget.val(s.donationBlockVal);
            }
          });
        }
        for (;i<2;i++) {
          id = inps.eq(i).attr('id');
          if (/min/.test(id)) {
            s.minField = inps.eq(i);
          } else if (/default/.test(id)) {
            s.defField = inps.eq(i);
          }
        }
        if (s.minField.length>0 && s.defField.length>0) {
          s.fixedCB = $('<input type="checkbox" class="form-checkbox" value="1" id="edit-fixed-amount">');
        
          s.fixedLb = $('<label for="edit-fixed-amount">Fixed value</label>');
          s.minField.after(s.fixedLb);
          s.minField.after(s.fixedCB);
          s.minVal = s.minField.val();
          if (/^\s*-?\d+\s*/.test(s.minVal)) {
            s.minVal = s.minVal - 0;
          } else {
            s.minVal = 0;
          }
          if (s.minVal < 0) {
            s.fixedCB.attr('checked','checked');
            Drupal.fto.toggleFixedDonation(s.fixedCB,2);
          }
          s.fixedAmount = (s.minVal && s.minVal < 0);
          s.minField.parent().parent().find('.description').hide();
          s.fixedCB.bind('click',function(e){
            e.stopImmediatePropagation();
            Drupal.fto.toggleFixedDonation($(this));
          });
        }
      }
    },
			
		init: function() {
			var s = Drupal.settings.fto;
			s.donationSettings = $('#edit-field-donation-settings');
      if (s.donationSettings.length>0) {
        Drupal.fto.manageDonationSettings();
      }
		}
	};
	
  Drupal.behaviors.fto = {
    attach: function(context) {
      if (context instanceof HTMLDocument) {
        Drupal.settings.fto = {};
      	Drupal.fto.init();
      }
    }
  };
}(jQuery));