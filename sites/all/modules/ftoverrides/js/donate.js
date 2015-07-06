(function($) {
	
	Drupal.fto = {
			
		init: function() {
			var s = Drupal.settings.fto;
			s.min_donation = $('#edit-min-donation-amount');
      s.form = $('#payment-donation-form');
      s.default_donation = $('#edit-default-donation-amount');
      s.amount_field = $('#edit-payment-donation-amount-amount');
      s.amount_rgx = new RegExp('\\d+\\.');
      s.min_donation_amount = 0;
      s.amount_desc = s.form.find('.description:first');
      s.amount_desc.addClass('hidden');
      s.error_msg = '';
      s.comma_error_msg = '';
			if (s.min_donation.length>0) {
				s.min_donation_amount = parseInt(s.min_donation.val());
        s.error_msg = $('#edit-donation-error-msg').val()
        s.comma_error_msg = $('#edit-comma-error-msg').val();
			}
      
			if (s.default_donation.length>0) {
				s.default_donation_amount = parseInt(s.default_donation.val());
        if (s.amount_field.length>0) {
          s.amount_val = s.amount_field.val();
          if (typeof s.amount_val == 'string')  {
            s.amount_val = $.trim(s.amount_val);
          } else {
            s.amount_val = '';
          }
          if (s.amount_val.length < 1) {
            s.amount_field.val(s.default_donation_amount);
          }
        }
			}
      if (s.form.length>0) {
        s.form.on('submit', function(e){
          var s = Drupal.settings.fto, valid = true, error_msgs = [],desc='';
          if (s.amount_desc.hasClass('error')) {
            s.amount_field.removeClass('error').addClass('hidden');
            if (s.amount_desc.attr('title')) {
              desc = s.amount_desc.attr('title');
              s.amount_desc.html(desc);
            }
          }
          s.amount_val = s.amount_field.val();
          if (typeof s.amount_val == 'string')  {
            s.amount_val = $.trim(s.amount_val);
          } else {
            s.amount_val = '';
          }
          if (/[,:/-]\d\d?\b/.test(s.amount_val)) {
             error_msgs.push(s.comma_error_msg);
             valid = false;
             var parts = s.amount_val.split(/[,:/-]/);
             parts.pop();
             s.amount_val = parts.join(',');
          }
          s.amount_val = s.amount_val.replace(/[^0-9.]/,'');
          if (s.amount_val.length < 1) {
            s.amount_val = 0;
          }
          s.amount_val = parseInt(s.amount_val);
          s.amount_field.val(s.amount_val);
          if (s.amount_val < s.min_donation_amount) {
            valid = false;
            error_msgs.push(s.error_msg);
          }
          if (!valid) {
            e.preventDefault();
            if (desc.length<1) {
              desc = s.amount_desc.text();
              s.amount_desc.attr('title',desc);
            };
            s.amount_desc.html(error_msgs.join(' and ').replace(/\band\s+please\b/i,'and'));
            s.amount_field.addClass('error');
            s.amount_desc.addClass('error').removeClass('hidden');
          }
        });
      }
		}
	};
	
  Drupal.behaviors.fto = {
    attach : function(context) {
      Drupal.settings.fto = {};
    	Drupal.fto.init();
    }
  };
}(jQuery));