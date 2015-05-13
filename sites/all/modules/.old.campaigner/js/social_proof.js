(function ($){
    $.fn.campaignerSocialProof = function (options) {
        var currentProof = {}, setProof, settings;
        // The URL gives the URL to get social proof from
        // Action provides the action number (to be passed as GET to the URL)
        // The tableid provides the id desired for the table
        // row-class provides the class to be applied to each row
        settings = $.extend({
            'url': '',
            'action': 0,
            'tableid': 'signature_block',
            'row-class': "individual_signature",
            'refresh-rate': 30000,
            fields: ['name', "region_title"]
        }, options);
        setProof = function (el, proof){
            var  odd, proofIndex = {};
            odd = true;
            proof.sort(function (a, b){
                return a.created - b.created;
            });
            $.each(proof, function (idx, value){
                var element, html;
                if (typeof currentProof[value.id] === "undefined"){
                    html = '';
                    for (var i= 0, fieldLength = settings.fields.length; i < fieldLength; i++){
                        var fieldValue;
                        fieldValue = value[settings.fields[i]];
                        if (fieldValue === null){
                            fieldValue = '';
                        }
                        html += "<div class='sig_field " + settings.fields[i] + "'>" + fieldValue + "</div>";
                    }
                    element = $('<div>').html(html).addClass(settings['row-class']).data("id", proof.id).addClass(odd ? "odd" : "even").hide().prependTo(el);
                    element.slideDown(400);
                }
                else {
                    element = $(currentProof[value.id]);
                }
                proofIndex[value.id] = element;
            });
            $.each(currentProof, function (idx, value){
                if (typeof proofIndex[idx] === "undefined"){
                    $(value).slideUp("normal", function (){this.remove()});
                }
            });
            currentProof = proofIndex;
        };
        return this.each(function (idx, element){
            var existing, interval, proof, table;
            if ($(element).data("social-proof-active")){
                return;
            }
            $(element).data("social-proof-active", true);
            $('#' + settings.tableid).remove();
            table = $("<div>").attr("id", settings.tableid).appendTo($(element));
            proof = $(element).data("proof");
            existing = $(element).find("." + settings['row-class']);
            $.each(existing, function (idx, value){
                currentProof[$(value).data("id")] = $(value);
            });
            $(element).data("proof", "");
            if (typeof proof !== "undefined"){
                setProof($(table), proof);
            }
            interval = setInterval(function (){
                $.getJSON(settings.url + "/" + settings.action, function (response){
                    if (typeof response !== "undefined"){
                        setProof(table, response);
                    }
                });
            }, settings['refresh-rate']);
            $(element).data("interval", interval);

        });
    };
})(jQuery);