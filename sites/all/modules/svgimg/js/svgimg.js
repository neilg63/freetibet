(function($) {
	
	Drupal.svgimg = {
		
		supported: function() {
			return document.implementation.hasFeature("http://www.w3.org/TR/SVG11/feature#Image", "1.1");
		},
		
		replaceObjects: function() {
			var s = Drupal.settings.svgimg,i=0,wr,png,im;
			for (;i<s.numObjs;i++) {
				wr = s.objs.eq(i);
				png = wr.attr('data-src');
				if (png) {
					if (typeof decodeURIComponent == 'function') {
						png = decodeURIComponent(png);
					}
					im = new Image();
					im.src = png;
					im = $(im);
					im.addClass('png-alternative');
					wr.html( im );
				}
			}
		},
		
		replaceImg: function() {
			var s = Drupal.settings.svgimg,i=0,im,png;
			for (;i<s.numImgs;i++) {
				im = s.imgs.eq(i);
				png = im.attr('data-src');
				if (png) {
					if (typeof decodeURIComponent == 'function') {
						png = decodeURIComponent(png);
					}
					im.attr('data-src',im.attr('src'));
					im.attr('src',png);
				}
			}
		},
			
		init: function() {
			var s = Drupal.settings.svgimg;
			s.imgs = $('img.svg-img');
			s.numImgs = s.imgs.length;
			s.supported = Drupal.svgimg.supported();
			
			if (s.numImgs > 0) {
				s.supported = Drupal.svgimg.supported();
				if (!s.supported) {
					Drupal.svgimg.replaceImg();
				}
			}
			
			if (!s.supported) {
				s.objs = $('.svg-object');
				s.numObjs = s.objs.length;
				if (s.numObjs > 0) {
					Drupal.svgimg.replaceObjects();
				}
			}
		}
	};
	
  Drupal.behaviors.svgimg = {
    attach : function(context) {
    	if (!Drupal.settings.svgimg) {
    		Drupal.settings.svgimg = {};
    	}
    	Drupal.svgimg.init();
    }
  };
}(jQuery));