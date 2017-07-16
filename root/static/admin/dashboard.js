$('#myModal').on('show', function() {
    var id = $(this).data('id'), removeBtn = $(this).find('.danger');
})

$('.confirm-delete').on('click', function(e) {
    e.preventDefault();

    var id = $(this).data('id');
    var path = $(this).data('path');
    $('#myModal').data('path', path);
    $('#myModal').data('id', id).modal('show');
});

$('#btnYes').click(function() {
    // handle deletion here
    var $modalDiv = $('#myModal');
    var id = $modalDiv.data('id');
    var path = $modalDiv.data('path');

    $modalDiv.addClass('loading');
    $.post(path + id).then(function() {
        $('#row-id-'+id).remove();
        $modalDiv.modal('hide').removeClass('loading');
    });
});
