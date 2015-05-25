(function($) {

	$.fn.fitIframes = function() {
	   var items = this, numItems = items.length,item, i=0,w,h,fr,fw,fh,ar;
	   if (numItems>0) {
		   for (;i<numItems;i++) {
			   item = $(items[i]);
			   w = item.width();
			   fr = item.find('iframe');
			   if (fr.length>0) {
				   fw = fr.attr('width');
				   if (fw) {
					   fh = fr.attr('height');
					   if ($.isNumeric(fw) && $.isNumeric(fh)) {
						   ar = fw/fh;
					   }
				   }
				   else {
					   ar = 16/9.5;
				   }
				   h = w / ar;
				   fr.css({width:'100%',height: h + 'px' });
			   }
			   
		   }
	   }
	   return this;
	}
	
	
	Drupal.ft = {
			
		resizeIframes: function() {
			var fc = Drupal.settings.ft.iframes;
			if (fc.length>0) {
				fc.fitIframes();
			}
		},

		addTouchSupport: function() {
			document.addEventListener("touchstart", function(){
			}, true);
		},
		
		shareThisLabelClick: function() {
			var stLabels = $('.sharethis-wrapper .label');
			if (stLabels.length>0) {
				stLabels.addClass('share-label').css('cursor','pointer');
				stLabels.on('click',function(e){
					var b = $(this).parent().find('.stButton .chicklets');
					if (b.length>0) {
						b.trigger('click');
					}
				});
			}
		},

		menuToggle: function() {
			$('.menu-toggle').on('click', function(e){
				e.preventDefault();
				e.stopImmediatePropagation();
				var s = Drupal.settings.ft;
				if (s.b.hasClass('menu-expanded')) {
					s.b.removeClass('menu-expanded');
				} else {
					s.b.addClass('menu-expanded');
				}
			});
		},
			
		init: function() {
			Drupal.settings.ft = {};
			var s = Drupal.settings.ft;
			s.iframes = $('.file-video, .file-audio-soundcloud');
			s.b = $('body');
			this.resizeIframes();
			//this.highlightPlatform3();
			this.shareThisLabelClick();
			this.addTouchSupport();
			this.menuToggle();
			$(window).on('resize',Drupal.ft.resizeIframes);
		}
	};
	
  Drupal.behaviors.ft = {
    attach : function(context) {
    	// now initialize
    	Drupal.ft.init();
    }
  };
}(jQuery));