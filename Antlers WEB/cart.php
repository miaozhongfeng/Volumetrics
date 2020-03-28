<?php
$styles = '<link rel="stylesheet" type="text/css" href="css/sn-style.css">
			<link rel="stylesheet" type="text/css" href="css/estore.css">';
$scripts = '<script type="text/javascript" src="js/jquery.extends.js"></script>
			<script type="text/javascript" src="js/estore.js"></script>';
$subPage = 'cart';

include_once("includes/database-access.php");
include_once("includes/header-estore-template.php");

if(isset($_SESSION['active_user_id']))
	$userId = $_SESSION['active_user_id'];

else if(isset($_COOKIE['anonymous_id']))	
	$userId = $_COOKIE['anonymous_id'];
else
	showErrorAndExit('default');

//
$query = "SELECT c.id, c.quantity, p.*, sp.price
			FROM cart AS c
			INNER JOIN products AS p
			ON c.product_id = p.id
			LEFT JOIN shipping_prices AS sp
			ON sp.product_id = c.product_id AND p.shipping_price_type = '2'
				AND 
				(
					sp.range_from <= c.quantity AND sp.range_to >= c.quantity
					OR
					sp.range_to = 
					(
						SELECT MAX(range_to) 
						FROM shipping_prices
						WHERE product_id = c.product_id
					)
				)
			WHERE user_id = '$userId'
			GROUP BY p.id
			ORDER BY c.id DESC";

$result = mysqli_query($config['mysql_con'], $query);

if(!$result)
	showErrorAndExit('default');
$hasCartItems = mysqli_num_rows($result) > 0 ? true : false;

?>

<div class="sn-main-container">
	<div class="sn-main-top">
		<div class="arrow-holder">
			<div class="sn-main-arrow" style="left:52px"></div>
		</div>
	</div>
	<div class="sn-scrolable-margin"></div>
	<div class="sn-scrolable">
		<div class="estore-content cart-container">
			<div class="estore-notification-container"></div>

			<?php
			if($hasCartItems)
			{
			?>
			<table class="cart-table font-light">
				<tr class="table-header">
					<td style="width:430px;">
						Product Name & Details
					</td>
					<td>
						Quantity
					</td>
					<td>
						Price
					</td>
					<td>
						Shipping Details
					</td>
					<td></td>
				</tr>

				<?php
					$allProductsTotalPrice = 0;
					$allShippingTotalPrice = 0;
					$allTotalTotalPrice = 0;
					while(list($cartId, $cartQuantity, $productId, $name, $description, $category, $price, $shippingPrice, $shippingPriceType, $productQuantity, $coverImage, $shippingRangePrice) = mysqli_fetch_array($result, MYSQLI_NUM))
					{
						$imgPath = "user_images/products/thumbs/".$coverImage;
						if(strlen($description) > 200)
							$description = substr($description, 0, 200).'...';
						if ($shippingPriceType == 0)
							$shippingPrice = 0;
						else if($shippingPriceType == 2)
							$shippingPrice = $shippingRangePrice;

						if($productQuantity < $cartQuantity)
							$cartQuantity = $productQuantity;

						$notAvailableClass = $productQuantity == 0 ? ' not-available' : '';

						if($productQuantity > 0 && $cartQuantity > 0)
						{
							$totalPrice = $cartQuantity * $price + $shippingPrice;
							$allProductsTotalPrice += $cartQuantity * $price; //without shipping
							$allShippingTotalPrice += $shippingPrice;
							$allTotalTotalPrice += $totalPrice; //with shipping
						}
						
					?>
						<tr class="row_<?php echo $cartId;?> info-row<?php echo $notAvailableClass;?>" id="row_<?php echo $cartId;?>">
							<input type="hidden" class="quantityHdn" value="<?php echo $productQuantity;?>" />
							<input type="hidden" class="cartQuantityHdn" value="<?php echo $cartQuantity;?>" />
							<input type="hidden" class="shippingPriceTypeHdn" value="<?php echo $shippingPriceType;?>" />
							<input type="hidden" class="productIdHdn" value="<?php echo $productId;?>" />
							<input type="hidden" class="shippingPriceHdn" value="<?php echo $shippingPrice;?>" />
							<input type="hidden" class="priceHdn" value="<?php echo $price;?>" />
							<td>
								<a class="cart-img transitions" href="product-details.php?id=<?php echo $productId;?>" style="background:url(<?php echo $imgPath;?>)" title="details" target="_blank"></a>
								<div class="cart-description">
									<div class="font-bold"><?php echo $name;?></div>
									<div><?php echo $description;?></div>
								</div>
							</td>
							<td>
								<?php
								if($productQuantity > 0)
								{
									?>
										<input type="number" min="1" class="quantity-box" value="<?php echo $cartQuantity;?>"/> piece
										<div class="quantity-validation-msg"></div>
									<?php
								}
								else echo '<span class="green-text">OUT OF STOCK</span>';
								?>
							</td>
							<td>
								<span class="product-price">US $<?php echo $price;?></span>
								<span> /piece</span>
							</td>
							<td>
								<div class="cart-shipping-details">
									<?php
										if ($shippingPriceType == 0)
											echo '<div class="product-price" style="margin-top:0;">Free Shipping</div>';
										else
											echo '<div class="product-price" style="margin-top:0;">US $<span class="shippingPriceField">'.$shippingPrice.'</span></div>';
									?>
								</div>
								
							</td>
							<td>
								<div class="default-btn remove-cart-item" data-cart-id="<?php echo $cartId;?>">REMOVE</div>
								<?php if($productQuantity > 0)
								{
									?>
										<form action="preprocess-order.php" method="POST">
											<input type="hidden" class="cartIdHdn" name="cartId" value="<?php echo $cartId;?>" />
											<input type="submit" class="default-btn default-btn-green cart-buy-btn" value="BUY">
										</form>
									<?php
								}
								?>
								
							</td>
						</tr>
						<tr class="row_<?php echo $cartId;?> totals-row">
							<td colspan="5" class="totals-td">
								<?php 
									if($productQuantity > 0)
										echo 'Total: <span class="product-price"> US $ <span class="totalPriceField">'.$totalPrice.'</span></span>';
								?>
							</td>
						</tr>
					<?php
					}
				?>
				
				<tr>
					<td colspan="5" style="padding:0">
						<div class="cart-table-footer">
							<div style="float:right;">
								<span class="totals-label">Shipping: </span>
								<span class="font-bold">US $<span class="allShipingPricesField"><?php echo $allShippingTotalPrice?></span></span>
								<br/><br/>
								<span class="totals-label">Total: </span>
								<span class="product-price">US $<span class="allTotalField"><?php echo $allTotalTotalPrice;?></span></span>
								<br/><br/>
							</div>
							<div style="float:right; margin-right:20px;">
								<span class="totals-label">Subtotal: </span>
								<span class="font-bold">US $<span class="allPricesField"><?php echo $allProductsTotalPrice;?></span></span>
							</div>
							<div class="clear"></div>

							<div class="cart-btn-wrapper">
								<div class="default-btn default-btn-green" id="buyAllBtn">BUY ALL</div>
							</div>

							<div class="clear"></div>
						</div>
					</td>
				</tr>
			</table>
			<?php
			}
			//empty cart
			else
			{
				echo '<br /><br /><h4 class="text-center green-text">There are no products in your shopping cart</h4>';
			}
			?>
			<h4 class="text-center green-text empty-cart"></h4>
			<br/><br/>
		</div>
	</div>
</div>
<form id="orderMultiple" method="post" action="preprocess-order.php">
	<input type="hidden" name="cartIds" id="cartIds" />
</form>
<div class="fullscreen-overlay">
	<div class="overlay-text-wrapper">
		<div class="overlay-text">Updating Cart</div>
	</div>
</div>
<?php include_once("includes/footer-template.php");?>
<script type="text/javascript">

$('.quantity-box').ForceIntegersOnly();

//1.validate input
//2.get shipping prices async if ranges set
//3.update cart quantity async
//4.update displayed prices
$('.quantity-box').blur(function(){

	if($(this).val() == $('.cartQuantityHdn').val())
		return;

	var parentRow = $(this).parents('.info-row');
	var quantity = parseInt($(this).val().trim());
	var totalQuantity = parentRow.find('.quantityHdn').val();
	var productId = parentRow.find('.productIdHdn').val();
	var shippingPriceType = parentRow.find('.shippingPriceTypeHdn').val();
	var productPrice = parseFloat(parentRow.find('.priceHdn').val());
	var cartId = parentRow.find('.cartIdHdn').val();

	if(validateQuantity(quantity, totalQuantity, $(this)))
	{
		showOverlay();
		if(shippingPriceType == 2)
		{
			$.ajax({
		        type: "POST",
		    	dataType: 'json',
		        url: "partials/estore/cart.php",
		        data: {productId :productId, cartQuantity: quantity}, 
		        success: function(data){
		        	if(parseFloat(data))
		        	{
		        		updateCartQuantityAsync(cartId, quantity, parseFloat(data), parentRow);
		        	}
		        	else
		        	{
		        		parentRow.find('.quantity-box').val($('.cartQuantityHdn').val());
		        		showNotification('default','error','.estore-notification-container');
		        		hideOverlay();
		        	}
		        },
		        error: function(){
		        	parentRow.find('.quantity-box').val($('.cartQuantityHdn').val());
		        	showNotification('default','error','.estore-notification-container');
		        	hideOverlay();
		        },
		    });
		}
		else
		{
			var shippingPrice = 0;
			if(shippingPriceType == 1)
				shippingPrice = parseFloat(parentRow.find('.shippingPriceHdn').val());

			updateCartQuantityAsync(cartId, quantity, shippingPrice, parentRow);
		}
	}
})

$('.remove-cart-item').click(function(){
	var cartId = $(this).data('cartId');
	$.ajax({
        type: "POST",
    	dataType: 'json',
        url: "partials/estore/cart.php",
        data: {removedCartId :cartId}, 
        success: function(data){
        	if(data == true)
        	{
        		var affectedRows = $('.row_'+cartId);	        		
        		cartItemsNum = parseInt($('.menu-cart-number').text())-1;
        		//no more items
        		if(cartItemsNum == 0)
        		{
        			$('.cart-table').toggle();
        			$('.empty-cart').text('There are no products in your shopping cart');
        		}
        		affectedRows.toggle('0',function(){
        			affectedRows.remove();
        			calculateTotalPrices();
        		});
        		
        		$('.menu-cart-number').text(cartItemsNum);
				showNotification('Product successfully removed from cart.','success','.estore-notification-container');
				flushCartNumber();
        	}
        	else
        	{
        		showNotification('default','error','.estore-notification-container');
        	}
        },
        error: function(){
        	showNotification('default','error','.estore-notification-container');
        }
    });
});

function validateQuantity(quantity, totalQuantity, quantityBox){
	
	parentRow = quantityBox.parents('.info-row');
	parentRow.find('.quantity-validation-msg').text('');
	if(!parseInt(quantity))
	{
		parentRow.find('.quantity-validation-msg').text('Value must be a valid integer.');
		return false;
	}
	if(quantity == '')
	{
		parentRow.find('.quantity-validation-msg').text('Plaease enter quantity.');
		return false;
	}
	
	if(parseInt(quantity) > parseInt(totalQuantity))
	{
		parentRow.find('.quantity-validation-msg').text('Purchases are limited to '+totalQuantity+' pieces');
		quantityBox.val($('.cartQuantityHdn').val());
		return false;
	}
	return true;
}

function updateCartQuantityAsync(cartId, quantity, shippingPrice, parentRow){
	$.ajax({
        type: "POST",
    	dataType: 'json',
        url: "partials/estore/cart.php",
        data: {cartId :cartId, cartQuantity: quantity}, 
        success: function(data){
        	if(data == true)
        	{
        		var productPrice = parseFloat(parentRow.find('.priceHdn').val());
        		updatePrices(shippingPrice, productPrice, quantity, parentRow);
        		$('.cartQuantityHdn').val(quantity);
        	}
        	else
        	{
        		parentRow.find('.quantity-box').val($('.cartQuantityHdn').val());
        		showNotification('default','error','.estore-notification-container');
        	}
        },
        error: function(){
        	parentRow.find('.quantity-box').val($('.cartQuantityHdn').val());
        	showNotification('default','error','.estore-notification-container');
        },
        complete:function(){
        	hideOverlay();
        }
    });
}

function updatePrices(shippingPrice, productPrice, quantity, parentRow){
	var productTotal = (productPrice * quantity) + shippingPrice;
	parentRow.find('.shippingPriceField').text(shippingPrice);
	parentRow.next('.totals-row').find('.totalPriceField').text(productTotal);
	calculateTotalPrices();
}

function calculateTotalPrices(){
	var totalShipping = 0;
	var subtotalPrice = 0;
	var totalPrice = 0;

	$('.info-row').each(function(){
		if(!($(this).hasClass('not-available')))
		{
			if(parseFloat($(this).find('.shippingPriceField').text()))
				totalShipping += parseFloat($(this).find('.shippingPriceField').text());

			if(parseFloat($(this).find('.priceHdn').val()) && parseInt($(this).find('.quantity-box').val()))
				subtotalPrice += parseFloat($(this).find('.priceHdn').val()) * parseInt($(this).find('.quantity-box').val());
		}
	});
	
	totalPrice = subtotalPrice + totalShipping;

	$('.allShipingPricesField').text(totalShipping.toFixed(2));
	$('.allTotalField').text(totalPrice.toFixed(2));
	$('.allPricesField').text(subtotalPrice.toFixed(2));
}

function showOverlay(){
	$('body').css('overflow', 'hidden').scrollTop(0);
	var centerHeight = ($(window).height() - 80)/2;
	$('.overlay-text-wrapper').css('margin-top', centerHeight+'px');
	$('.fullscreen-overlay').css('opacity', '0').fadeTo(200,1);
}
function hideOverlay(){
	$('.fullscreen-overlay').fadeTo(200,0,function(){
		$('.fullscreen-overlay').hide();
	});
	$('body').css('overflow', 'auto');
}

$('#buyAllBtn').click(function(){
	idsString='';
	$('.cartIdHdn').each(function(){
		idsString += $(this).val() + ',';
	});
	idsString = idsString.substr(0, idsString.length - 1);
	$('#cartIds').val(idsString);
	$('#orderMultiple').submit();
})
</script>