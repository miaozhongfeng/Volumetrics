<?php


$styles = '<link rel="stylesheet" type="text/css" href="css/sn-style.css">
			<link rel="stylesheet" type="text/css" href="css/estore.css">';
$scripts = '<script type="text/javascript" src="js/pagination.js"></script>';
$subPage = 'orders';

//include_once("includes/database-access.php");
include_once("includes/header-estore-template.php");

if(!isset($_SESSION['active_user_id']))
{
	header('Location:login.php?redirectUrl=orders.php');
	exit();
}
?>


<div class="sn-main-container">
	<div class="sn-main-top">
		<div class="arrow-holder">
			<div class="sn-main-arrow" style="left:522px"></div>
		</div>
	</div>
	<div class="sn-scrolable-margin"></div>
	<div class="sn-scrolable">
		<div class="estore-content orders-container">
			<div class="estore-notification-container"><?php showSessionNotification();?></div>
			<table class="orders-table">
				<tr class="orders-table-head">
					<td style="width:350px;">Product</td>
					<td>Price</td>
					<td>Quantity</td>
					<td>Shipping</td>
					<td>Status</td>
					<td>Action</td>
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

function loadContent(page){
	$('.orders-row').remove();
	$('.orders-table').append('<tr class="loader-row"><td colspan="6"><div id="ajaxLoaderDiv"></div></td></tr>');
	$.ajax({
        url: 'partials/estore/orders.php',
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
					$('.orders-table').show();
					$('.loader-row').remove();
					$('.orders-table').append(obj.results);
					var isLast = obj.data.totalResults > obj.data.page * obj.data.resultsPerPage ? false : true;
					//pagination.js
					$('.pagination').show();
					$('.pagination').html(pagination(obj.data.page, obj.data.totalPages, isLast));
				}
				else
				{
					$('.orders-table').remove();
					$('.orders-container').append('<br /><br /><h4 class="text-center green-text">No orders</h4>');
					$('.pagination').hide();
				}
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

//cancel order
$('.orders-table').on('click', '.cancel-order-btn', function(){
	var orderId = $(this).data('orderId');
	var thisBtn = $(this);
	$.ajax({
        url: 'partials/estore/orders.php',
        type: 'POST',
        dataType: 'json',
        data: {cancelledOrderId: orderId},
        success: function (data) {
        	if(data == true)
        	{
        		thisBtn.parents('tr').find('.status-text').text('Cancelled')
        		thisBtn.parents('.btnCell').html('<p>Order cancelled</p>');
        	}
        	else
        		showNotification('default','error', '.estore-notification-container');	
        },
        error: function(data){
        	alert(data.responseText);
			showNotification('default','error', '.estore-notification-container');
        }
    });  
});

//show order details
$('.orders-table').on('click', '#detailsLink', function(e){
	e.preventDefault();
	var shippingId = $(this).data('shippingId');
	var thisRow = $(this).parents('tr');
	if(thisRow.next('tr').hasClass('details-row'))
		return;

	$('.details-row').remove();
	$('<tr class="orders-row details-row"><td colspan="6" class="l-border r-border"><div id="ajaxLoaderDiv"></div></td></tr>').insertAfter(thisRow);

	$.ajax({
        url: 'partials/estore/orders.php',
        type: 'POST',
        data: {shippingId: shippingId},
        success: function (data) {
        	if(data.trim() != '' && data != 'false')
        	{
        		$('.details-row td').html(data);
        	}
        	else
        		showNotification('default','error', '.estore-notification-container');	
        },
        error: function(data){
        	//alert(data.responseText);
			showNotification('default','error', '.estore-notification-container');
        }
    });  
});

$('.orders-table').on('click', '.close-link', function(e){
	e.preventDefault();
	$('.details-row').remove();
});

//checkout 
$('.orders-table').on('click', '.checkout-btn', function(e){
	e.preventDefault();
	var orderId = $(this).data('orderId');

	$.ajax({
        url: 'partials/estore/orders.php',
        type: 'POST',
        dataType: 'json',
        data: {checkoutOrderId: orderId},
        success: function (data) {
        	if(data == true)
        	{
        		window.location.href = 'express-checkout.php?existingOrderId='+orderId;
        	}
        	else
        		showNotification('default','error', '.estore-notification-container');	
        },
        error: function(data){
        	//alert(data.responseText);
			showNotification('default','error', '.estore-notification-container');
        }
    });  
});
</script>