(function($) {
	
	Drupal.textsize = {
		
		setSizeClass: function(size) {
			var s = Drupal.settings.textsize, classes = s.h.attr('class');
			s.selectors.find('li').removeClass('selected');
			s.selectors.find('li.' + size).addClass('selected');
			if (typeof classes == 'string') {
				if (classes.length> 8) {
					classes = classes . replace(/textsize-\w+\s*/,'');
					classes = $.trim(classes);
					
				}
			} else {
				classes = '';
			}
			classes += ' textsize-' + size;
			s.h.attr('class',classes);
			s.mode = size;
			$.cookie("textsize", size, { expires : 7 });
		},
		
		init: function() {
			var s = Drupal.settings.textsize;
			s.selectors = $('.textsize-selector');
			s.h = $('html');
			if (s.selectors.length>0) {
				
				s.mode = $.cookie("textsize");

				if (typeof s.mode == 'string' && s.mode.length > 4 && s.mode != 'medium') {
					Drupal.textsize.setSizeClass(s.mode);
				}
				
				s.selectors.find('li').on('click', function(){
					var it = $(this), sz = it.attr('data-size'),s = Drupal.settings.textsize;
					if (sz) {
						Drupal.textsize.setSizeClass(sz);
					}
				});
			}
		}// end of init()
	};
	
	Drupal.behaviors.textsize = {
		attach: function(context) {
			// inner variables
			if (context == document) {
				if (!Drupal.settings.textsize) {
					Drupal.settings.textsize = {};
				}
				Drupal.textsize.init();
			}
		}
	}
})(jQuery);