(function($) {
	
	Drupal.fto = {
			
		init: function() {
			var s = Drupal.settings.fto;
			s.min_donation = $('#edit-min-donation-amount');
      s.form = $('#payment-donation-form');
      s.default_donation = $('#edit-default-donation-amount');
      s.amount_field = $('#edit-payment-donation-amount-amount');
      s.currency_field = $('#edit-payment-donation-amount-currency-code');
      s.cycle_field = $('#edit-cycle');
      s.amount_rgx = new RegExp('\\d+\\.');
      s.min_donation_amount = 0;
      s.amount_desc = s.form.find('.description:first');
      s.amount_desc.addClass('hidden');
      s.error_msg = '';
      s.comma_error_msg = '';
      s.context = null;
      var contextEl = $('#edit-context');
      if (contextEl.length>0) {
        s.context = contextEl.val();
      }
      if (!s.context) {
        s.context = 'donate';
      }
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
        s.block = $('#block-payment-donation-payment-donation');
        s.blockNode = s.block.parent();
        s.donateControls = s.form.find('.donation-controls');
        s.form.addClass('controls-contracted');
        s.blockNode.addClass('form-contracted');
        s.tabs = $('.node-donate .field-name-field-action li a');
        s.tabs.on('click',function(e){
          var it = $(this), href = it.attr('href');
          if (href) {
            var parts = href.split('//').pop().split('/'),path='';
            if (parts.length>1) {
              parts.shift();
              path = parts.join('/').split('#').shift();
              switch (path) {
                case 'donate':
                case 'join':  
                  s.form.removeClass('contracted');
                  s.blockNode.removeClass('form-contracted').addClass('form-expanded');
                  e.preventDefault()
                  break;
              }
              if (/^http/.test(href) && /freetibet.org/.test(href) == false) {
                it.attr('target','_blank');
              }
            }
          }
        });
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
        
        s.presets = $('#donation-presets');
        if (s.context != 'join') {
          s.presets.parent().removeClass('hidden');
        } else {
          s.form.addClass('controls-expanded');
          s.presetOpts = {};
          var presets = s.presets.find('select'),i=0,j=0,sel,vals=[],opts=[],parts=[];
          s.firstPeriod = null;
          for (;i<presets.length;i++) {
            vals = [];
            sel = presets.eq(i);
            opts = sel.find('option');
            for (j=0;j<opts.length;j++) {
              parts = opts.eq(j).attr('value').split(':');
              if (parts.length>1) {
                vals[parts[0]] = parseFloat(parts[1]);
              }
            }
            period = sel.attr('name').split('_').pop().toLowerCase();
            switch (period) {
              case 'annual':
              case 'annually':
              case 'yearly':  
                period = 'year';
                break;
              case 'monthly':
                period = 'month';
                break;  
            }
            if (!s.firstPeriod) {
              s.firstPeriod = period;
            }
            s.presetOpts[period] = vals;
          }
          
          var cycleOpts = s.cycle_field.find('option');
          for (i=0;i<cycleOpts.length;i++) {
            switch (cycleOpts.eq(i).attr('value')) {
              case 'single':
                cycleOpts.eq(i).remove();
                break;
              case s.firstPeriod:
                s.cycle_field.val(s.firstPeriod);
                s.amount_field.val(s.presetOpts[s.firstPeriod].GBP);
                break;
            }
          }
          
          s.cycle_field.on('change',function(e){
            var s = Drupal.settings.fto,curr = s.currency_field.val(),period=$(this).val();
            if (curr) {
              if (s.presetOpts[period]) {
                if (s.presetOpts[period][curr]) {
                  s.amount_field.val(s.presetOpts[period][curr]);
                }
              }
            }
          });
          
          s.currency_field.on('change',function(e){
            var s = Drupal.settings.fto,period=s.cycle_field.val(),curr=$(this).val();
            if (period) {
              if (s.presetOpts[period]) {
                if (s.presetOpts[period][curr]) {
                  s.amount_field.val(s.presetOpts[period][curr]);
                }
              }
            }
          });
          
        }
        
        s.currencySels = s.presets.find('select');
        s.currencySels.on('change',function(e){
          var it = $(this), s = Drupal.settings.fto, sv = it.val();
          if (sv) {
            var parts = sv.split(':');
            if (parts.length>1) {
              var currency = $.trim(parts[0]), amount = parseInt(parts[1]);
              s.amount_field.val(amount);
              s.currency_field.val(currency);
            }
          }
        });
        s.presets.find('dt').on('click', function(e){
          var it = $(this), s = Drupal.settings.fto;
          it.parent().find('.selected').removeClass('selected');
          it.addClass('selected');
          if (s.form.hasClass('controls-contracted')) {
            s.form.removeClass('controls-contracted').addClass('controls-expanded');
          }
          if (it.next().length>0 && it.next().prop('tagName').toLowerCase() == 'dd') {
            it.next().addClass('selected');
            var sel = it.next().find('select');
            if (sel.length>0) {
              var sv = sel.val();
              if (typeof sv == 'string') {
                var parts = sv.split(':');
                if (parts.length>1) {
                  var currency = $.trim(parts[0]), amount = parseInt(parts[1]);
                  s.amount_field.val(amount);
                  s.currency_field.val(currency);
                }
              }
            }
            if (it.attr('data-cycle')) {
              s.cycle_field.val(it.attr('data-cycle'));
            }
            if (it.attr('data-amount')) {
              var ams = $.parseJSON(it.attr('data-amount')), currency = s.currency_field.val();
              if (typeof ams == 'object' && typeof currency == 'string') {
                if (ams[currency]) {
                  s.amount_field.val(ams[currency]);
                }
              }
            }
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