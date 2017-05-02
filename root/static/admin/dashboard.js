$('#myModal').on('show', function() {
    var id = $(this).data('id'), removeBtn = $(this).find('.danger');
})

$('.confirm-delete').on('click', function(e) {
    e.preventDefault();

    var id = $(this).data('id');
    $('#myModal').data('id', id).modal('show');
});

$('#btnYes').click(function() {
    // handle deletion here
    var $modalDiv = $('#myModal');
    var id = $('#myModal').data('id');

    $modalDiv.addClass('loading');
    $.post('posts/delete/' + id).then(function() {
        $('#post-'+id).remove();
        $modalDiv.modal('hide').removeClass('loading');
    });
});
