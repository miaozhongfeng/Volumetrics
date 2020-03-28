<?php
$styles = '<link rel="stylesheet" type="text/css" href="css/sn-style.css">
			<link rel="stylesheet" type="text/css" href="css/estore.css">';
$scripts = '<script type="text/javascript" src="js/pagination.js"></script>
			<script type="text/javascript" src="js/estore.js"></script>';
$subPage = 'wishlist';

include_once("includes/database-access.php");
include_once("includes/header-estore-template.php");

if(!isset($_SESSION['active_user_id']))
	header("Location:e-store.php");

?>


<div class="sn-main-container">
	<div class="sn-main-top">
		<div class="arrow-holder">
			<div class="sn-main-arrow" style="left:212px"></div>
		</div>
	</div>
	<div class="sn-scrolable-margin"></div>
	<div class="sn-scrolable">
		<div class="estore-content cart-container wishlist-container">
			<div class="estore-notification-container"></div>

			<table class="cart-table font-light" id="wishlistTable" style="display:none;">
				<tr class="table-header">
				<td style="width:530px;">
					Product Name & Details
				</td>
				<td style="width:135px;">
					Price
				</td>
				<td>
					Shipping Details
				</td>
				<td></td>
			</tr>

				<!-- ajax loaded content -->
			</table>

			<div class="pagination"></div>

			<br/><br/>
		</div>
	</div>
</div>

<?php include_once("includes/footer-template.php");?>

<script type="text/javascript">
loadContent(1);

$('.pagination').on('click', 'a', function(e){
	e.preventDefault();
	loadContent($(this).data('page'), $('#categoryIdHdn').val());
});

//load content
function loadContent(page){
	//showLoader();
	$('.wishlist-item-row').remove();
	$('#wishlistTable').append('<tr class="loader-row"><td colspan="4"><div id="ajaxLoaderDiv"></div></td></tr>');
	$.ajax({
        url: 'partials/estore/wishlist.php',
        type: 'POST',
        dataType: 'json',
        data: {page: page},
        success: function (obj) {        
        	if(obj)
			{					
				if(obj.data.error)
				{
					showNotification('default','error', '.estore-notification-container');
					exit();
				}

				var html = '';

				if(obj.results.length > 0)
				{
					$('#wishlistTable').show();
					$('.loader-row').remove();
					$('#wishlistTable').append(obj.results);
					var isLast = obj.data.totalResults > obj.data.page * obj.data.resultsPerPage ? false : true;
					//pagination.js
					$('.pagination').show();
					$('.pagination').html(pagination(obj.data.page, obj.data.totalPages, isLast));
				}
				else
				{
					$('#wishlistTable').remove();
					$('.wishlist-container').append('<br /><br /><h4 class="text-center green-text">There are no products in your wish list</h4>');
					$('.pagination').hide();
				}
				//$('#ajaxContent').html(html);
			}			
			else
				showNotification('default','error', '.estore-notification-container');
        },
        error: function (obj) {
        	//alert(obj.responseText);
        	showNotification('default','error', '.estore-notification-container');
        }
    });
}

//remove wishlist item
$('#wishlistTable').on('click', '.remove-wishlist-item', function(){
	var wishlistId = $(this).data('id');
	var parentRow = $(this).closest('tr');
	$.ajax({
		url: 'partials/estore/wishlist.php',
        type: 'POST',
        dataType: 'json',
        data: {removedWishlistId: wishlistId},
        success: function (data) {
        	if(data && data == true)
        	{
        		removeTableRow(parentRow, 'Item successfully removed from wishlist.');
        	}
        	else
        	{
        		showNotification('default','error', '.estore-notification-container');
        	}
        },
        error:function(){
        	showNotification('default','error', '.estore-notification-container');
        }
	});
})

//add to wishlist item to cart
$('#wishlistTable').on('click', '.add-to-cart-btn', function(){
	var wishlistId = $(this).data('id');
	var productId = $(this).data('productId');
	var parentRow = $(this).closest('tr');
	$.ajax({
		url: 'partials/estore/wishlist.php',
        type: 'POST',
        dataType: 'json',
        data: {toCartWishlistId: wishlistId, productId: productId},
        success: function (data) {
        	if(data && (data == 'new' || data == 'exsisting'))
        	{
        		removeTableRow(parentRow, 'Item successfully added to your shopping cart.');
        		if(data == 'new')
        		{
        			$('.menu-cart-number').text(parseInt($('.menu-cart-number').text())+1);
        			flushCartNumber();
        		}
        		
        	}
        	else
        	{
        		showNotification('default','error', '.estore-notification-container');
        	}
        },
        error:function(){
        	showNotification('default','error', '.estore-notification-container');
        }
	});
});

function removeTableRow(row, notification){
	row.toggle('200', function(){
		row.remove();
		showNotification(notification,'success', '.estore-notification-container');
		if($('#wishlistTable').find('.wishlist-item-row').length < 1)
		{
			var goToPage = 1;
			var currentPage = $('.page-active').text().split(' ').pop();
			if(parseInt(currentPage) && parseInt(currentPage) > 1)
				var goToPage = parseInt(currentPage) - 1;


			loadContent(goToPage);
		}
	});
}
</script>

