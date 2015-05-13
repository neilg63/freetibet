(function ($){
    $(document).ready(function (){
        $(".report-table").delegate("a.spamlink", "click", function (e){
            var element, href, original, processingEl, request, row;
            e.preventDefault();
            element = $(this);
            row = element.closest("tr");
            processingEl = $("<span>").html("processing...");
            href = element.attr("href");
            original = element.replaceWith(processingEl);

            request = $.ajax(
                {
                    url: href,
                    data: {
                        ajax: true
                    },
                    success: function (response){
                        var spamField, isSpam;
                        spamField = row.find("td.spam");
                        isSpam = response.spam == "1" ? "Yes" : "No";
                        processingEl.replaceWith(response.link);
                        spamField.html(isSpam).addClass("recently-changed");
                        setTimeout(function (){
                            spamField.removeClass("recently-changed");
                        }, 300);
                        row.removeClass("error");
                    },
                    error: function (response){
                        var errorDiv;
                        processingEl.replaceWith(original);
                        errorDiv = $(".ajax-errors");
                        $("<p>").addClass("error").html("Error changing spam status").appendTo(errorDiv);
                        row.addClass("error");
                    },
                    dataType: "json"
                }
            );
        });
    });
})(jQuery);