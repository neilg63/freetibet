(function($) {
	
	Drupal.fto = {
    
    addSuggestedSingles: function(mode) {
      var s = Drupal.settings.fto,
      i=0,
      np = s.suggested.length,op;
      s.suggestedSingle = s.form.find('.suggested-amounts');
      var hasSuggestions = s.suggestedSingle.length>0, pOpt = $("<li></li>"),vl;
      if (!hasSuggestions) {
        s.suggestedSingle = $('<ul class="suggested-amounts"></ul>');
        for (;i<np;i++) {
          op = pOpt.clone();
          vl = s.suggested[i];
          op.attr('data-val',vl).html(vl);
          s.suggestedSingle.append(op);
        }
      }
      
      if (mode == 'mobile') {
        s.form.find('.form-actions').before(s.suggestedSingle);
      } else {
        s.amount_field.before(s.suggestedSingle);
      }
      if (!hasSuggestions) {  
        s.suggestedSingle.find('li').on('click',function(e){
          var s = Drupal.settings.fto, it=$(this),vl = it.attr('data-val');
          if (vl > 0) {
            s.amount_field.val(vl);
          }
        });
      }
      if (s.context == 'page') {
        s.form.addClass('single-mode');
      }
    },
    
    managePresets: function(self,e) {
      var s = Drupal.settings.fto, it = self, href = it.attr('href');
      if (href) {
        var parts = href.split('//').pop().split('/'),path='',mode='';
        if (parts.length>1) {
          parts.shift();
          parts = parts.join('/').split('#');
          path = parts.shift();
          if (parts.length>0) {
            mode = parts[0];
          }
          switch (path) {
            case 'donate':
            case 'join':  
            case 'renewal':
            case 'renew':
              s.form.removeClass('contracted');
              s.blockNode.removeClass('form-contracted').addClass('form-expanded');
              e.preventDefault()
              switch (mode) {
                case 'give-now':
                  s.presets.find('dt.donation-single').trigger('click');
                  s.form.find('.form-item-cycle').addClass('hidden');
                  break;
                case 'give-monthly':
                  s.presets.find('dt.donation-monthly').trigger('click');
                  s.form.find('.form-item-cycle').removeClass('hidden');
                  s.cycle_field.find('option[value=single]').hide();
                  break;
                default:
                  s.form.find('.form-item-cycle').removeClass('hidden');
                  break;
              }
              break;
          }
          if (/^http/.test(href) && /freetibet.org/.test(href) == false) {
            it.attr('target','_blank');
          }
        }
      }
    },
    
    handlePresets: function(self,e) {
      var it = self, s = Drupal.settings.fto;
      it.parent().find('.selected').removeClass('selected');
      if (!it.hasClass('donation-single')) {
        s.form.find('.form-item-cycle').addClass('hidden');
        s.form.addClass('regular-mode').removeClass('single-mode');
      } else {
        s.form.addClass('single-mode').removeClass('regular-mode');
      }
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
          var ams = $.parseJSON(it.attr('data-amount')), currency = s.currency_field.val(),vl;
          if (typeof ams == 'object' && typeof currency == 'string') {
            if (ams[currency]) {
              vl = ams[currency];
              if (vl >= s.amount_field.val()) {
                s.amount_field.val(vl);
              }
            }
          }
        }
      }
    },
    
    setPresetVal: function(self) {
      var s = Drupal.settings.fto,it=self,sv=it.val();
      if (s.currMonthly.length>0) {
        var opts = s.currMonthly.find('option'),numOpts=opts.length,i=0,vl;
        for (;i<numOpts;i++) {
          vl = opts.eq(i).attr('value');
          if (vl) {
            if (vl.split(':').shift() == sv) {
              if (s.presets.find('dt.donation-monthly').hasClass('selected')) {
                s.currMonthly.val(vl);
                s.amount_field.val(vl.split(':').pop());
              }
              break;
            }
          }
        }
      }
      if (s.hasCurrencyOpts && sv.length>0) {
        if (s.currencyDefaults[sv]) {
          s.amount_field.val(s.currencyDefaults[sv]);
        }
      }
    },
    
    validate: function(resetOnly) {
      var s = Drupal.settings.fto, valid = true, error_msgs = [],desc='';
      s.amount_desc.addClass('hidden');
      if (s.amount_desc.hasClass('error')) {
        s.amount_field.removeClass('error');
        s.amount_desc.addClass('error');
        if (s.amount_desc.attr('title')) {
          desc = s.amount_desc.attr('title');
          s.amount_desc.html(desc);              
        }
      }
      if (resetOnly !== true) {
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
          if (desc.length<1) {
            desc = s.amount_desc.text();
            s.amount_desc.attr('title',desc);
          };
          s.amount_desc.html(error_msgs.join(' and ').replace(/\band\s+please\b/i,'and'));
          s.amount_field.addClass('error');
          s.amount_desc.addClass('error').removeClass('hidden');
        }
        return valid;
      }
    },
    
    handleSubmit: function(e) {
      var s = Drupal.settings.fto, valid = Drupal.fto.validate();
      if (!valid) {
        e.preventDefault();
      } else {
        s.form.attr('target','_blank');
      }
    },
    
    setJoinVal: function() {
      var s = Drupal.settings.fto,curr = s.currency_field.val(),period=s.cycle_field.val();
      if (curr) {
        if (s.presetOpts[period]) {
          if (s.presetOpts[period][curr]) {
            var vl = s.presetOpts[period][curr];
            s.amount_field.val(vl);
            s.min_donation.val(vl);
            s.min_donation_amount = vl;
          }
        }
      }
      Drupal.fto.validate(true);
    },
			
		init: function() {
			var s = Drupal.settings.fto;
			s.min_donation = $('#edit-min-donation-amount');
      s.form = $('#payment-donation-form');
      s.default_donation = $('#edit-default-donation-amount');
      s.amount_field = $('#edit-payment-donation-amount-amount');
      if (s.default_donation.length>0) {
        s.fixed = s.default_donation.attr('data-fixed') == 1;
        if (s.fixed) {
          s.amount_field.addClass('fixed').attr('disabled', true);
        }
      }
      s.currency_field = $('#edit-payment-donation-amount-currency-code');
      s.hasCurrencyOpts = false;
      if (s.currency_field.length > 0) {
        var strOpts = s.currency_field.attr('data-currency');
        if (strOpts) {
          s.currencyDefaults = $.parseJSON(strOpts);
          s.hasCurrencyOpts = true;
        }
      }
      s.suggested = [];
      var suggested_str = s.form.attr('data-suggested'),sgs;
      if (suggested_str) {
        sgs = $.parseJSON(suggested_str);
        if (sgs instanceof Array) {
          s.suggested = sgs;
        }
      }
      s.cycle_field = $('#edit-cycle');
      if (s.cycle_field.length>0) {
        s.cycleOpts = s.cycle_field.find('option');
      }
      s.amount_rgx = new RegExp('\\d+\\.');
      s.min_donation_amount = 0;
      s.amount_desc = s.form.find('.description:first');
      s.amount_desc.addClass('hidden');
      s.error_msg = '';
      s.comma_error_msg = '';
      s.context = null;
      s.presets = $('#donation-presets');
      s.currMonthly = $('#edit-donation-preset-monthly');
      s.amount_field.attr('pattern','^\\s*\\d+(\\.\\d\\d)?\\s*$')
      s.amount_field.on('blur',function(e){
        var it = $(this), sv = it.val();
        if (typeof sv == 'string') {
          sv = sv.replace(/[^0-9\.,]/g,'');
          it.val(sv);
          if (sv.length>0 && /^\d+/.test(sv)) {
            Drupal.fto.validate();
          }
        }
      });
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
      if (s.suggested.length>0) {
        var mode = Drupal.ft.widthMode == 'mobile'? 'mobile' : 'desktop';
        Drupal.fto.addSuggestedSingles(mode);
        $(document).on('widthModeSwitch',function(e){
          Drupal.fto.addSuggestedSingles(e.mode);
        });
      }
      
      if (s.form.length>0) {
        s.block = $('#block-payment-donation-payment-donation');
        s.blockNode = s.block.parent();
        s.donateControls = s.form.find('.donation-controls');
        if (s.context != 'page') {
          s.form.addClass('controls-contracted');
          s.blockNode.addClass('form-contracted');
        }
        s.tabs = $('.node-donate .field-name-field-action li a');
        s.tabs.on('click',function(e){
          Drupal.fto.managePresets($(this),e);
        });
        s.form.on('submit', function(e){
          Drupal.fto.handleSubmit(e);
        });
        
        
        if (/(join|renewal|renew)/.test(s.context) == false) {
          //s.presets.parent().removeClass('hidden');
          s.currency_field.on('change',function(e){
            Drupal.fto.setPresetVal($(this));
          });
          
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
          
          
          for (i=0;i<s.cycleOpts.length;i++) {
            switch (s.cycleOpts.eq(i).attr('value')) {
              case 'single':
                s.cycleOpts.eq(i).remove();
                break;
              case s.firstPeriod:
                s.cycle_field.val(s.firstPeriod);
                s.amount_field.val(s.presetOpts[s.firstPeriod].GBP);
                break;
            }
          }
          Drupal.fto.setJoinVal();
          s.cycle_field.on('change',function(e){
            Drupal.fto.setJoinVal();
          });
          
          s.currency_field.on('change',function(e){
            Drupal.fto.setJoinVal();
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
          Drupal.fto.handlePresets($(this),e);
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