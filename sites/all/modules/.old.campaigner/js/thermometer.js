(function ($){
    $.fn.campaignerThermometer = function (options) {
        var settings;
        settings = $.extend({
            'showNumericCount':true
        }, options);

        return this.each(function (idx, element){
            var count, target, percentage, thermometer;
            element = $(element);
            if (element.data("thermometer-active") === true){
                return;
            }
            element.data("thermometer-active", true);
            element.addClass("clearfix");
            element.empty();
            thermometer = $("<div>").addClass("thermometer-holder").appendTo(element);
            target = element.data("target");
            count = element.data("count");
            percentage = (100 * count) / target;
            $("<div>").addClass("counter").css("width", percentage + "%").css("display", "block").appendTo(thermometer);
            $(".counter").css("width",percentage / 2 + "%").animate({ width: percentage + "%" }, 1000 );
            if (settings.showNumericCount === true){
                $("<p>").html("<strong>" + count + "</strong> signed to date").appendTo(thermometer);
            }
        });

    };
})(jQuery);