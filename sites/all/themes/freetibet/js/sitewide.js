(function($) {

	window.viewportSize = {};

		window.viewportSize.getHeight = function () {
			return getSize("Height");
		};

		window.viewportSize.getWidth = function () {
			return getSize("Width");
		};

		var getSize = function (Name) {
			var size;
			var name = Name.toLowerCase();
			var document = window.document;
			var documentElement = document.documentElement;
			if (window["inner" + Name] === undefined) {
				// IE6 & IE7 don't have window.innerWidth or innerHeight
				size = documentElement["client" + Name];
			}
			else if (window["inner" + Name] != documentElement["client" + Name]) {
				// WebKit doesn't include scrollbars while calculating viewport size so we have to get fancy

				// Insert markup to test if a media query will match document.doumentElement["client" + Name]
				var bodyElement = document.createElement("body");
				bodyElement.id = "vpw-test-b";
				bodyElement.style.cssText = "overflow:scroll";
				var divElement = document.createElement("div");
				divElement.id = "vpw-test-d";
				divElement.style.cssText = "position:absolute;top:-1000px";
				// Getting specific on the CSS selector so it won't get overridden easily
				divElement.innerHTML = "<style>@media(" + name + ":" + documentElement["client" + Name] + "px){body#vpw-test-b div#vpw-test-d{" + name + ":7px!important}}</style>";
				bodyElement.appendChild(divElement);
				documentElement.insertBefore(bodyElement, document.head);

				if (divElement["offset" + Name] == 7) {
					// Media query matches document.documentElement["client" + Name]
					size = documentElement["client" + Name];
				}
				else {
					// Media query didn't match, use window["inner" + Name]
					size = window["inner" + Name];
				}
				// Cleanup
				documentElement.removeChild(bodyElement);
			}
			else {
				// Default to use window["inner" + Name]
				size = window["inner" + Name];
			}
			return size;
		};

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
			var s = Drupal.settings.ft, fc = s.iframes;
			s.width = window.viewportSize.getWidth();
			if (fc.length>0) {
				fc.fitIframes();
			}
			if (s.width >= s.desktopWidth) {
				s.header.removeAttr('style');
			  s.b.removeClass('menu-expanded');
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
				var s = Drupal.settings.ft,h;
				if (s.width <= s.desktopWidth) {	
					if (s.b.hasClass('menu-expanded')) {
						h = s.logo.height();
						s.header.css('height', h + 'px');
						s.b.removeClass('menu-expanded');
					} else {
						h = (s.menuItems.first().height() * s.numMenuItems) + (s.sbox.height() * 1.5) + s.logo.height();
						s.header.css('height', h + 'px');
						s.b.addClass('menu-expanded');
					}
				}
			});
		},
			
		init: function() {
			Drupal.settings.ft = {};
			var s = Drupal.settings.ft;
			s.desktopWidth = 800;
			s.width = window.viewportSize.getWidth();
			s.iframes = $('.file-video, .file-audio-soundcloud');
			s.b = $('body');
			s.sbox =  $('#block-search-form');
			s.header = $('#header');
			s.menuItems = s.header.find('.region-header nav li');
			s.numMenuItems = s.menuItems.length;
			s.logo = $('#logo');
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