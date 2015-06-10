(function($) {
	
	Drupal.svgimg = {
		
		supported: function() {
			return document.implementation.hasFeature("http://www.w3.org/TR/SVG11/feature#Image", "1.1");
		},
			
		init: function() {
			var s = Drupal.settings.svgimg;
			s.imgs = $('img.svg-img');
			s.numImgs = s.imgs.length;
			if (s.numImgs > 0) {
				s.supported = Drupal.svgimg.supported();
				if (!s.supported) {
					var i=0,im,png;
					for (;i<s.numImgs;i++) {
						im = s.imgs.eq(i);
						png = im.attr('data-src');
						if (png) {
							im.attr('data-src',im.attr('src'));
							im.attr('src',png);
						}
					}
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
