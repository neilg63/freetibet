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

	$.fn.fitIframes = function(w,h) {
	   var items = this, numItems = items.length,item, i=0,w,h,fr,fw,fh,ar;
     if (!w) {
       w = 16;
     }
     if (!h) {
       h = 9.5;
     }
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
					   ar = h/w;
				   }
				   h = w / ar;
				   fr.css({width:'100%',height: h + 'px' });
			   }
			   
		   }
	   }
	   return this;
	}
	
	
	Drupal.ft = {
    
    handleFilterMenu: function(self) {
      self.find('li.term').on('click',function(e){
        var s = Drupal.settings.ft, it = $(this), par = it.parent(), refId = par.attr('id').replace(/-(sub)?menu$/,''), refSel = $('#'+refId),tid;
        if (refSel.length > 0) {
          e.stopImmediatePropagation();
          s.currTermFilter = it.attr('data-opt');
          refSel.val(s.currTermFilter);
          //par.parent().find('.selected').removeClass('selected');
          it.addClass('loading');
          s.viewsFilterForm.find('.form-submit').trigger('click');
        }
      });
    },
		
    toggleViewsFilter: function(mode) {
      var s = Drupal.settings.ft, i=0, j=0,sel,numOpts=0,opts,opt;
      if (s.hasViewsFilter) {
        if (mode == 'horizontal') {
          if (s.viewsFilterForm.hasClass('has-filter-menu')) {
            s.viewsFilterForm.find('ul.filter-menu').removeClass('hide');
            s.viewsFilterForm.find('.form-item,label').addClass('hide');
          } else {
            var selList,catList,item,txt,lbl;
            for (; i < s.numFilterSels;i++) {
              sel = s.filterSelectors.eq(i);
              lbl = sel.parent().parent().parent().find('label');
              selList = $('<ul class="filter-menu"></ul>');
              catList = selList.clone();
              selList.attr('id',sel.attr('id') + '-menu').addClass('large');
              catList.attr('id',sel.attr('id') + '-submenu');
              selList.append('<li class="label">'+$.trim(lbl.text())+'</li>');
              catList.append('<li class="label">Category</li>');
              if (sel.hasClass('hide') == false) {
                opts = sel.find('option');
                numOpts = opts.length;
                if (opts.length>0) {
                  for (j=0;j<numOpts;j++) {
                    opt = opts.eq(j);
                    txt = opt.html();
                    if (/-\s*any/i.test(txt)) {
                      txt = 'All';
                    }
                    item = $('<li data-opt="'+opt.attr('value')+'" class="term">'+txt+'</li>');
                    if (
                      (j == 0 && s.currTermFilter < 1)
                      || (opt.attr('value') == s.currTermFilter)
                    ) {
                      item.addClass('selected');
                    }
                    if (j< s.maxTopicItems) {
                      selList.append(item);
                    } else {
                      catList.append(item);
                    }
                    
                  }
                }
                sel.parent().addClass('hide');
                lbl.addClass('hide');
                lbl.after(selList);
                if (numOpts > s.maxTopicItems) {
                  selList.after(catList);
                }
                Drupal.ft.handleFilterMenu(s.viewsFilterForm);
              }
            }
            s.viewsFilterForm.addClass('has-filter-menu');
          }
          s.viewsFilterForm.find('.form-submit').addClass('hide');
        } else {
          s.viewsFilterForm.find('label,.form-item,.form-submit').removeClass('hide');
           s.viewsFilterForm.find('ul.filter-menu').addClass('hide');
        }
      } 
    },
    	
		resizeHandler: function() {
			var s = Drupal.settings.ft, fc = s.iframes;
			s.width = window.viewportSize.getWidth();
			if (fc.length>0) {
				fc.fitIframes();
			}
			if (s.width >= s.desktopWidth) {
				s.header.removeAttr('style');
			  s.b.removeClass('menu-expanded');
			}
			s.header.removeAttr('style');
      if (s.width >= s.desktopWidth ) {
        s.footerMenu.find('br').remove();
        s.footerMenu.removeClass('mobile-width');
         Drupal.ft.toggleViewsFilter('horizontal');
      } else if (s.footerMenu.hasClass('mobile-width') == false) {
        s.footerMenu.find('.facebook').before('<br />');
        s.footerMenu.addClass('mobile-width');
        Drupal.ft.toggleViewsFilter('default');
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
					var expanded = s.b.hasClass('menu-expanded');
					if (!expanded || s.header.attr('style') == undefined) {
						h = (s.menuItems.first().outerHeight() * s.numMenuItems) + (s.sbox.outerHeight() * 1.5) + s.logo.height();
						s.header.css('height', h + 'px');
					}
					if (expanded) {
						h = s.logo.height();
						s.header.css('height', h + 'px');
						s.b.removeClass('menu-expanded');
					} else {
						s.b.addClass('menu-expanded');
					}
				}
			});
		},
			
		init: function(mode) {	
      if (mode == 'full') {
  			Drupal.settings.ft = {};
      }
  		var s = Drupal.settings.ft;
      if (mode == 'full') {
  			s.desktopWidth = 800;
  			s.width = window.viewportSize.getWidth();
  			s.iframes = $('.file-video, .file-audio-soundcloud,.field-name-field-iframe');
  			s.b = $('body');
  			s.sbox =  $('#block-search-form');
  			s.header = $('#header');
  			s.menuItems = s.header.find('.region-header nav li');
  			s.numMenuItems = s.menuItems.length;
  			s.logo = $('#logo');
        s.footerMenu = $('#block-menu-menu-footer-menu ul');
        s.currTermFilter = 0;
        s.maxTopicItems = 5;
      }
      
      s.viewsFilterForm = $('.view-filters form');
      s.hasViewsFilter = false;
      if (s.viewsFilterForm.length>0) {
        s.filterSelectors = s.viewsFilterForm.find('select');
        s.numFilterSels = s.filterSelectors.length;
        s.hasViewsFilter = s.numFilterSels>0;
      }
			this.resizeHandler();
			this.shareThisLabelClick();
			this.addTouchSupport();
			this.menuToggle();
			$(window).on('resize orientationchange',Drupal.ft.resizeHandler);
		}
	};
	
  Drupal.behaviors.ft = {
    attach : function(context) {
    	// now initialize
      var mode = context instanceof HTMLDocument? 'full' : 'partial';
      Drupal.ft.init(mode);
    }
  };
}(jQuery));