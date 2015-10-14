(function($) {

  if (!Date.now) {
      Date.now = function() { return new Date().getTime(); }
  }

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
	   var items = this, numItems = items.length,item, i=0,w,h,fr,fw,fh,ar,isPc;
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
           isPc = /%$/.test(fw);
           if (isPc) {
             fw = fr.width();
           }
				   if (fw) {
					   fh = fr.attr('height');
             isPc = /%$/.test(fh);
             if (isPc) {
               if (fh == '100%') {
                 ar = 16/9;
               } else {
                 ar = fw * ( parseInt(fh) / 100 );
               }
             }
					   else if ($.isNumeric(fw) && $.isNumeric(fh)) {
               fw = parseInt(fw);
               fh = parseInt(fh);
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
              //selList.append('<li class="label">'+$.trim(lbl.text())+'</li>');
              //catList.append('<li class="label">Category</li>');
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
    	
		resizeHandler: function(init) {
			var s = Drupal.settings.ft;
			s.width = window.viewportSize.getWidth();
			if (s.numIfs>0) {
				s.iframes.fitIframes();
			}
      if (s.numSlideCaptions>0) {
        Drupal.ft.resizeSlideCaptions();
      }
			if (s.width >= s.desktopWidth) {
				s.header.removeAttr('style');
			  s.b.removeClass('menu-expanded');
			}
      if (!init) {
        init = false;
      }
			s.header.removeAttr('style');
      if (s.width >= s.desktopWidth && (s.footerMenu.hasClass('mobile-width') || init===true)) {
        s.footerMenu.find('br').remove();
        s.footerMenu.removeClass('mobile-width');
         Drupal.ft.toggleViewsFilter('horizontal');
         $.event.trigger({type:'widthModeSwitch',mode:'desktop'});
         s.widthMode = 'desktop';
      } else if (s.width < s.desktopWidth && s.footerMenu.hasClass('mobile-width') == false) {
        s.footerMenu.find('.facebook').before('<br />');
        s.footerMenu.addClass('mobile-width');
        Drupal.ft.toggleViewsFilter('default');
        $.event.trigger({type:'widthModeSwitch',mode:'mobile'});
        s.widthMode = 'mobile';
      }
		},
    
    scrollHandler: function() {
      var s = Drupal.settings.ft, sTop = $(window).scrollTop(),ht=$(window).height() / 32, downPush = s.lastTop < sTop - (ht*8);
      if (downPush || s.lastTop > sTop + (ht*8)) {
        s.lastTop = sTop;
        if (downPush && sTop > s.header.height()) {
          Drupal.ft.toggleMenu('contract');
        }
      }
      if (sTop > (ht*2) && s.b.hasClass('header-contracted') == false) {
        s.b.addClass('header-contracted');
      } else if (sTop <= (ht*2) && s.b.hasClass('header-contracted')) {
        s.b.removeClass('header-contracted');
      }
      if (s.hasPinnedSection) {
        var itemTop=0,isFixed = s.pinnedSection.hasClass('fixed'),retracted=s.pinnedSection.hasClass('retracted');
        sTop += (ht*2);
        if (isFixed) {
          itemTop = s.pinnedSection.next().position().top; 
        } else {
          itemTop = s.pinnedSection.position().top;
        }
        if (sTop  > itemTop && isFixed == false && retracted == false) {
            s.pinnedSection.addClass('fixed');
        } else if (sTop  <= itemTop && (isFixed || retracted)) {
          s.pinnedSection.removeClass('fixed retracted');
        }
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
    
    toggleMenu: function(mode) {
			var s = Drupal.settings.ft,h,expanded;
      switch (mode) {
        case 'contract':
          expanded = true;
          break;
        case 'expand':
          expanded = false;
          break;
        default:
          expanded = s.b.hasClass('menu-expanded');
          break;
      }
			if (!expanded || s.header.attr('style') == undefined) {
        var liw = s.menuItems.first().width(),numRows = s.numMenuItems, cssAttrs = {'max-height':'auto'};
        if (liw < s.width/2) {
          numRows /= 2;
          if (s.b.hasClass('header-contracted')) {
            numRows += 0.5;
          }
        }
				h = (s.menuItems.first().outerHeight() * numRows) + (s.sbox.outerHeight() * 2) + s.logo.height();
        cssAttrs.height = h + 'px';
				s.header.css(cssAttrs);
			}
			if (expanded) {
				h = s.logo.height();
				s.header.css('height', h + 'px');
        setTimeout(function(){
          Drupal.settings.ft.header.removeAttr('style');
        },500);
				s.b.removeClass('menu-expanded');
			} else {
				s.b.addClass('menu-expanded');
			}
    },

		menuToggle: function() {
			$('.menu-toggle').on('click', function(e){
				e.preventDefault();
				e.stopImmediatePropagation();
        if (Drupal.settings.ft.width <= Drupal.settings.ft.desktopWidth) {
				  Drupal.ft.toggleMenu();
        }
			});
		},
    
    arrangeBoxes: function() {
      var s = Drupal.settings.ft,i=0,j=0,nb=0,boxes,sect,n2,n3,r2,r3;
      for (;i<s.numPageSections;i++) {
        sect = s.pageSections.eq(i);
        boxes = sect.find('.field > figure,> figure,> .file,> .key-point');
        nb = boxes.length;
        if (nb>0) {
          sect.addClass('num-boxes-' + nb);
          n2 = Math.floor(nb / 2) * 2;
          r2 = nb % 2;
          n3 = Math.floor(nb / 3) * 3;
          r3 = nb % 3;
          for (j=0;j<nb;j++) {
            if (j >= n2 && r2 > 0) {
              boxes.eq(j).addClass('row-2-' + r2);
            }
            if (j >= n3 && r3 > 0) {
              boxes.eq(j).addClass('row-3-' + r3);
            }
          }
        }
      }
    },
    
    processForm: function(self,e) {
      var ins = self.find('input.required'),j=0,valid=true,it,vl;
       for (;j< ins.length;j++) {
         it = ins.eq(j);
         it.parent().find('span.error').remove();
         it.removeClass('error');
         if (it.hasClass('form-text')) {
           vl = it.val();
           if ($.trim(vl).length < 1) {
             valid = false;
             msg = 'This field is required';
           } else if (/email/.test(it.attr('id'))) {
             if (/[^@]@\w+/.test(vl) == false) {
               valid = false;
               msg = 'Please enter a valid email address';
             }
           }
           if (!valid) {
             it.addClass('error');
             it.after('<span class="error">'+msg+'</span>');
             e.preventDefault();
           }
         }
       }
    },
    
    sizeSlideCaption: function(self) {
      var s = Drupal.settings.ft, w = self.width(),par,pw,cp,lo;
      if (w > 0) {
        par = self.parent();
        pw = par.width();
        cp = par.find('.field-name-field-text');
        if (cp.length>0) {
          if (s.widthMode == 'mobile') {
            cp.removeAttr('style');
          } else {
            lo = ((pw-w)/2) + 1;
            w -= (parseInt(cp.css('padding-left')) * 2)
            cp.css({width:w + 'px', left: lo + 'px' });
          }
          if (self.hasClass('caption-resized') == false) {
            self.addClass('caption-resized');
          }
        }
      }
    },
    
    resizeSlideCaptions: function() {
      var s = Drupal.settings.ft, i=0,im;
      for (;i<s.numSlideCaptions;i++) {
        im = s.slideCaptions.eq(i).parent().find('img');
        if (im.hasClass('caption-resized')) {
          Drupal.ft.sizeSlideCaption(im);
        }
      }
    },
    
    manageSlideCaptions: function() {
      var s = Drupal.settings.ft, i=0, cp, im,pc;
      for (;i<s.numSlideCaptions;i++) {
        cp = s.slideCaptions.eq(i);
        pc = cp.parent().find('picture');
        im = pc.find('img');
        if (pc.parent().prop('tagName').toLowerCase()=='a') {
          pc.unwrap();
        }
       pc.append(cp);
        im.on('load',function(){
          Drupal.ft.sizeSlideCaption($(this))
        })
        
      }
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
        s.numIfs = s.iframes.length;
  			s.b = $('body');
  			s.sbox =  $('#block-search-form');
  			s.header = $('#header');
  			s.menuItems = s.header.find('.region-header nav li');
  			s.numMenuItems = s.menuItems.length;
  			s.logo = $('#logo');
        s.content = $('#content');
        s.footerMenu = $('#block-menu-menu-footer-menu ul');
        s.currTermFilter = 0;
        s.maxTopicItems = 5;
        s.pinnedSection = $('.top-section-pinned .field-name-top');
        s.hasPinnedSection = s.pinnedSection.length > 0;
        s.slideCaptions = $('#content .slides li > .field-name-field-text');
        s.numSlideCaptions = s.slideCaptions.length;
        if (s.numSlideCaptions > 0) {
          Drupal.ft.manageSlideCaptions();
        }
        if (s.hasPinnedSection) {
          s.pinnedSectionClose = $('<button class="close" title="close" aria-label="Close overlay"><strong>&times;</strong></button>');
          s.pinnedSection.find('div.page-section').append(s.pinnedSectionClose);
          s.pinnedSectionClose.on('click',function(e){
            var s = Drupal.settings.ft;
            s.pinnedSection.removeClass('fixed').addClass('retracted');
          });
        }
        s.lastTop = 0;
        s.widthMode = 'desktop';
        s.pageSections = s.content.find('.page-section');
        s.numPageSections = s.pageSections.length;
        if (s.numPageSections>0) {
          Drupal.ft.arrangeBoxes();
        }
        
        s.pfs = $('form');
        s.npfs = s.pfs.length;
        if (s.npfs > 0) {
          var ts = Date.now(),i=0,f,id,ac;
          for (;i< s.npfs;i++) {
            f = s.pfs.eq(i), id = f.attr('id');
            if (/-search-/.test(id) == false) {
              ac = f.attr('action');
              if (ac.indexOf('?') >= 0) {
                ac += '&';
              } else {
                ac += '?';
              }
              ac = ac.replace(/[&?]t=\d+/,'');
              f.attr('action', ac + 't=' +ts );
              if (/-mailchimp-/.test(id) == false) {
                f.on('submit',function(e){
                  Drupal.ft.processForm($(this),e);
                });
              }
            }
          }
        }
        var fl = $('#content .file > a'),nl= fl.length,i=0,it,ch,tg;
        for (;i< nl;i++) {
          it = fl.eq(i);
          if (/^\/(field-collection|files?)\//.test(it.attr('href'))) {
            ch = it.children().first();
            tg = ch.prop('tagName').toLowerCase();
            if (tg == 'picture' || tg == 'img') {
              ch.unwrap();
            }
          }
        }  
  			$(window).on('resize orientationchange',Drupal.ft.resizeHandler);
        $(document).on('scroll',Drupal.ft.scrollHandler);
  			this.shareThisLabelClick();
  			this.addTouchSupport();
        this.menuToggle();
      }
      
      s.viewsFilterForm = $('.view-filters form');
      s.hasViewsFilter = false;
      if (s.viewsFilterForm.length>0) {
        s.filterSelectors = s.viewsFilterForm.find('select');
        s.numFilterSels = s.filterSelectors.length;
        s.hasViewsFilter = s.numFilterSels>0;
      }
			this.resizeHandler(true);
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