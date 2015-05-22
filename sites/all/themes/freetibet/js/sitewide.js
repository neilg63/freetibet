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
	
	
	Drupal.p3 = {
			
		resizeIframes: function() {
			var fc = Drupal.settings.p3.iframes;
			if (fc.length>0) {
				fc.fitIframes();
			}
		},

		addTouchSupport: function() {
			document.addEventListener("touchstart", function(){
			}, true); 
			//$('body').bind('touchstart', function() {});

			//See more at: http://danlobo.co.uk/article/touching-hover-states#sthash.7tSUuAPn.dpuf
		},
		
		highlightPlatform3: function() {
			var ts = $('h1,h2,h3,h4,h5,h6,p,li,a,em,strong'),u3 = '\uF733' . toString(16), numT = ts.length,i=0,j=0,k=0,m=false,it,cs,tx,ws;
			for (;i<numT;i++) {
				it = ts.eq(i);
				cs = it.contents();
				ih = '';
				for (j=0;j<cs.length;j++) {
					if (cs[j] instanceof Text) {
						if (cs[j].textContent) {
							m = false;
							tx = cs[j].textContent;
							ws = tx.split(/\b/);
							for (k=0;k<ws.length;k++) {
								if (ws[k].length>6 && /\platform3\b/i.test(ws[k])) {
									ws[k] = ws[k].replace('3', u3 );
									m = true;
								}
							}
							if (m) {
								cs[j].textContent = ws.join('');
							}
						}
					}
				}
			}
			var codes = {
				p: '\uE029',
				l: '\uE01C',
				a: '\uE000',
				t: '\uE02F',
				f: '\uE012',
				o: '\uE021',
				r: '\uE02C',
				m: '\uE01E'	
			},
		animLink = $('a.animtext-link'),lt,cd;
			for (t in codes) {
				lt = animLink.find('.char-' + t);
				if (lt.length>0) {
					lt.html( codes[t].toString(16));
				}
			}
			animLink.find('.char-3').html(u3);
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

			
		init: function() {
			Drupal.settings.p3 = {};
			Drupal.settings.p3.iframes = $('.file-video, .file-audio-soundcloud');
			this.resizeIframes();
			//this.highlightPlatform3();
			this.shareThisLabelClick();
			this.addTouchSupport();
			$(window).on('resize',Drupal.p3.resizeIframes);
		}
	};
	
  Drupal.behaviors.p3 = {
    attach : function(context) {
    	// now initialize
    	Drupal.p3.init();
    }
  };
}(jQuery));
if (Drupal.gmap) {
	Drupal.gmap.addHandler('gmap',function(elem) {
		var obj = this,
		styles = [
	      {
	        stylers: [
	          { hue: "#000000" }
	        ]
	      },
	      {
	          featureType: "landscape",
	          elementType: "geometry.fill",
	          stylers: [
	            { visibility: "on" },
	            { color: "#b8b8b8" }
	          ]
	        },
	        {
	            elementType: "geometry.fill",
	            stylers: [
	              { color: "#b8b8b8" }
	            ]
	          },
	            { featureType: "landscape", 
	          elementType: "geometry.fill", 
	          stylers: [ 
	            { color: "#b8b8b8" } 
	          ] 
	          },
	          	{
	            featureType: "landscape.man_made",
	            elementType: "geometry.fill",
	            stylers: [
	              { color: "#e2e2e2" },
	              { visibility: "on" }
	            ]
	          },
	          {
	            featureType: "road",
	            elementType: "geometry.fill",
	            stylers: [
	              { visibility: "on" },
	              { saturation: -100 }
	            ]
	          },
	          {
	            featureType: "road",
	            elementType: "geometry.stroke",
	            stylers: [
	              { visibility: "off" }
	            ]
	          },
	          {
	          featureType: "road",
	              elementType: "labels",
	              stylers: [
	                { visibility: "off" }
	              ]
	            },
	          /*{
	            featureType: "road.highway",
	            stylers: [
	              { color: "#EFEDBF" },
	              { visibility: "simplified" }
	            ]
	          },
	          {
	            featureType: "road.arterial",
	            stylers: [
	              { visibility: "simplified" },
	              { color: "#eeeeee" }
	            ]
	          },
	            {
	            featureType: "road.arterial",
	            elementType: "labels.text.fill",
	            stylers: [
	              { color: "#585858" },
	              { visibility: "on" }
	            ]
	          },*/
	        	{
	            elementType: "labels.text.stroke",
	            stylers: [
	              { visibility: "off" }
	            ]
	          },
	          {
	            featureType: "water",
	            stylers: [
	              { color: "#606060" },
	              { visibility: "simplified" }
	            ]
	          },{
	            featureType: "road.highway",
	            elementType: "labels.text",
	            stylers: [
	              { visibility: "on" }
	            ]
	          },{
	            featureType: "road.local",
	            elementType: "labels.text",
	            stylers: [
	              { visibility: "off" }
	            ]
	          },{
	            featureType: "road.arterial",
	            elementType: "labels.text",
	            stylers: [
	              { visibility: "on" }
	            ]
	          },{
	            featureType: "road",
	            elementType: "labels.text.stroke",
	            stylers: [
	              { visibility: "off" }
	            ]
	          },
	           {
	          featureType: "road.highway",
	          elementType: "geometry.fill", 
	          stylers: [
	            { color: "#eeeeee" }
	          ]
	          },
	          {
	          featureType: "road.highway",
	          elementType: "geometry.stroke",
	          stylers: [
	            { color: "#eeeeee" }
	          ]
	          },
	          {
	          featureType: "road.arterial",
	          elementType: "geometry.fill",
	          stylers: [
	            { color: "#eeeeee" }
	          ]
	          },
	          {
	          featureType: "transit",
	          elementType: "geometry.stroke",
	          stylers: [
	            { color: "#444444" }
	          ]
	          },
	          {
	          featureType: "transit",
	          elementType: "labels.icon",
	              stylers: [
	                { saturation: -100 }
	              ]
	              },
	          {
	          featureType: "road.arterial",
	          elementType: "geometry.stroke",
	          stylers: [
	            { color: "#e9e9e9" }
	          ]
	          },
	          {
	          featureType: "road.local",
	          elementType: "geometry.stroke",
	          stylers: [
	            { visibility: "off" }
	          ]
	          },
	          {
	          featureType: "road.local",
	          elementType: "geometry.fill",
	          stylers: [
	            { color: "#e6e6e6" }
	          ]
	          },
	         {
	           featureType: "poi",
	            elementType: "labels.icon",
	            stylers: [
	              { visibility: "off" }
	            ]
	          },
	          {
	              featureType: "poi.business",
	               elementType: "labels",
	               stylers: [
	                 { visibility: "off" }
	               ]
	             },
	          {
	          featureType: "poi.medical",
	          elementType: "labels",
	          stylers: [
	            { visibility: "off" }
	          ]
	          },
	          {
	          featureType: "administrative",
	          elementType: "labels",
	          stylers: [
	            { visibility: "off" }
	          ]
	          }
	      ];

		obj.bind("init", function() {
			var opts = obj.opts;
			var mapOptions = {
				styles: styles,
				minZoom: 9,
				maxZoom: 19,
				mapTypeControl: false
			};
			obj.map.setOptions(mapOptions);
		});
	});

}