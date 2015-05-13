(function ($){
  $(document).ready(function (){
    $(".report-table").delegate("a.sendlink", "click", function (e){
      var element, errorFunction, href, original, processingEl, request, row;
      e.preventDefault();
      element = $(this);
      row = element.closest("tr");
      processingEl = $("<span>").html("processing...");
      href = element.attr("href");
      original = element.replaceWith(processingEl);
      errorFunction = function (){
        var errorDiv;
        processingEl.replaceWith(original);
        errorDiv = $(".ajax-errors");
        $("<p>").addClass("error").html("Error sending mail").appendTo(errorDiv);
        row.addClass("error");
      };
      request = $.ajax(
        {
          url: href,
          data: {
            ajax: true
          },
          success: function (response){
            if (response === "ok"){
              processingEl.replaceWith("mail sent");
              row.removeClass("error");
            }
            else {
              errorFunction();
            }
          },
          error: function (response){
            errorFunction();
          },
          dataType: "json"
        }
      );
    });
  });
})(jQuery);