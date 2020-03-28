<?php
if(!isset($_GET['id']) || !ctype_digit($_GET['id']))
{
	header("Location:e-store.php");
	exit();
}

$styles = '<link rel="stylesheet" type="text/css" href="css/sn-style.css">
			<link rel="stylesheet" type="text/css" href="css/estore.css">';
$scripts = '<script type="text/javascript" src="js/pagination.js"></script>
			<script type="text/javascript" src="js/jquery.zoom.min.js"></script>
			<script type="text/javascript" src="js/jquery.extends.js"></script>
			<script type="text/javascript" src="js/estore.js"></script>';
$subPage = 'e-store';

include_once("includes/database-access.php");
include_once("includes/header-estore-template.php");


//get categories
$query = "SELECT * FROM product_categories";
$result = mysqli_query($config['mysql_con'], $query);
$categoriesArr = array();
if(!$result)
{
	showErrorAndExit('default');
}

while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
array_push($categoriesArr, $row);


//get product info
$id = $_GET['id'];
$query = "SELECT name, description, category_id, price, shipping_price, shipping_price_type, quantity, cover_image FROM products WHERE id = '$id' LIMIT 1";
$result = mysqli_query($config['mysql_con'], $query);
if(!$result)
{
	showErrorAndExit('default');
}
if(mysqli_num_rows($result) == 0)
{
	header("Location:e-store.php");
	exit();
}

list($name, $description, $productCategory, $price, $shippingPrice, $shippingPriceType, $quantity, $coverImage) = mysqli_fetch_array($result, MYSQLI_NUM);

//get shipping price ranges	
$shippingPriceRanges = array();
$shippingPriceRangesJson = '';
if($shippingPriceType ==  2)
{
	$query = "SELECT range_from, range_to, price FROM shipping_prices WHERE product_id = '$id'";
	$result = mysqli_query($config['mysql_con'], $query);
	if(!$result || mysqli_num_rows($result) < 1)
	{
		showErrorAndExit('default');
	}
	while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
	{
		array_push($shippingPriceRanges, $row);
	}

	//convert to json for easier js use
	$shippingPriceRangesJson = json_encode($shippingPriceRanges);
}

//get product images
$imagesArr = array();
if($coverImage != 'default.jpg')
{
	$query = "SELECT name FROM product_images WHERE product_id = '$id' ORDER BY is_cover_image DESC LIMIT 5";
	$result = mysqli_query($config['mysql_con'], $query);
	if(!$result)
	{
		showErrorAndExit('default');
	}

	while($row = mysqli_fetch_array($result, MYSQLI_NUM))
	{
		array_push($imagesArr, $row[0]);
	}
}

//get wishlist count for product (if user is logged)
$wishlistCount = 0;
if(isset($_SESSION['active_user_id']))
{
	$query = "SELECT COUNT(id) FROM wishlist WHERE product_id = '$id'";
	$result = mysqli_query($config['mysql_con'], $query);
	if(!$result)
	{
		showErrorAndExit('default');
	}
	$row = mysqli_fetch_array($result, MYSQLI_NUM);
	$wishlistCount = $row[0];
}

//TODO 
//get orders count for product
$ordersCount = 0;
$query = "SELECT COUNT(id) FROM orders WHERE product_id = '$id' AND status = '1'";
$result = mysqli_query($config['mysql_con'], $query);
if(!$result)
{
	showErrorAndExit('default');
}
$row = mysqli_fetch_array($result, MYSQLI_NUM);
$ordersCount = $row[0];


//calculate total price
switch ($shippingPriceType) {
	case '0':
		$totalPrice = $price;
		break;
	
	case '1':
		$totalPrice = $price + $shippingPrice;
		break;

	case '2':
		$totalPrice = $price + $shippingPriceRanges[0]['price'];
	break;

	default:
		showErrorAndExit('default');
		break;
}
?>

<div class="sn-main-container">
	<div class="sn-main-top">
		<div class="arrow-holder">
			<div class="sn-main-arrow" style="left:675px"></div>
		</div>
	</div>
	<div class="sn-scrolable-margin"></div>
	<div class="sn-scrolable">
		<div class="estore-content details-container">
			<div class="estore-notification-container"></div>
			
			<input type="hidden" id="productId" value="<?php echo $id;?>"/>
			<div class="sidebar-menu">
				<div class="angle-div default-angle-div">CHOOSE CATEGORY</div>
				<a href="e-store.php">All</a>
				<?php
					foreach ($categoriesArr as $category)
					{
						$activeClass = $category['id'] == $productCategory ? 'category-active':'';
						echo '<a href="e-store.php?category='.$category['id'].'" data-cat-id="'.$category['id'].'" class="'.$activeClass.'">'.$category['name'].'</a>';
					}
				?>
			</div>
			<div class="item-details-wrapper">
				<div class="images-container">
					<?php
						if(sizeof($imagesArr) > 0)
						{
							?>
								<div class="big-img">
									<span class='zoom' id='zoomDiv'>
										<img src="user_images/products/<?php echo $imagesArr[0];?>" width="320">
									</span>
									<div>
										<img src="images/estore-zoom-icon.png" />
										<span>Mouse over to zoom in</span>
									</div>		
								</div>
								<div class="product-thumbnails">
									<?php
										$isFirstBullet = true;
										foreach ($imagesArr as $imgName) {
											$activeThumbClass = $isFirstBullet ? ' thumb-img-active' : '';
											$bulletPath = 'user_images/products/bullets/'.$imgName;
											$imagePath = 'user_images/products/'.$imgName;
											echo '<div class="thumb-img'.$activeThumbClass.'" style="background-image:url('.$bulletPath.')" data-img="'.$imagePath.'"></div>';
											$isFirstBullet = false;
										}
									?>

								</div>
							<?php
						}
						else echo '<img src="user_images/products/default.jpg" class="default-product-img">';
					?>
					
				</div>
				<div class="product-info-container font-light">
					<input type="hidden" value='<?php echo $shippingPriceRangesJson;?>' id="shippingPriceRangesJson" />
					<input type="hidden" value="<?php echo $price;?>" id="priceHdn" />
					<input type="hidden" value="<?php echo $shippingPrice;?>" id="shippingPriceHdn" />
					<input type="hidden" value="<?php echo $shippingPriceType;?>" id="shippingPriceTypeHdn" />
					<input type="hidden" id="totalQuantity" value="<?php echo $quantity;?>"/>
					<div class="product-name">
						<?php echo $name;?>
					</div>
					<div style="text-align:right;padding-right:100px;">
						<span style="color:#204b29;">|&nbsp;&nbsp;&nbsp;&nbsp;</span>
						<span class="product-price" style="font-family: 'Source Sans Pro', sans-serif; font-weight:600;"><?php echo $ordersCount;?></span>
						<span class="grey-text"> orders</span>
					</div>
					<table class="grey-text info-table">
						<tr>
							<td class="info-label">Price</td>
							<td>
								<span class="product-price">US $<?php echo $price;?> </span>/piece
								<br /><br />
							</td>
						</tr>
						<tr class="grey-box">
							<td class="info-label">Description</td>
							<td>
								<span class="font-medium" style="color:#fff;"><?php echo $description?></span>
							</td>
						</tr>
						<?php
						if($quantity > 0)
						{
							?>
							<tr class="grey-box">
								<td class="info-label">Shipping</td>
								<td>
									<?php 
										if($shippingPriceType == 0)
											echo '<span class="font-bold" style="color:#fff;"> Free Shipping</span>';
										else if ($shippingPriceType == 1)
											echo '<span class="font-bold" style="color:#fff;"> US $'.$shippingPrice.'</span></span>';
										else if($shippingPriceType == 2)
										{
											echo '<span class="font-bold" style="color:#fff;"> US $<span id="shippingPriceField">'.$shippingPriceRanges[0]['price'].'</span></span>';
											echo '<a href="#" id="allShippingPricesLink" class="green-link">All shipping prices</a>';
											$rangesHtml='<div class="clear" id="shippingRangesContainer">';
											$rangesHtml.='<div class="row">
												<div class="cell font-medium">Quantity</div>
												<div class="cell font-medium">Price</div>
												</div>';

											$rangeCounter = 0;
											foreach ($shippingPriceRanges as $priceRange) {
												$selectedRangeClass = $rangeCounter == 0 ? ' cell-active' : '';
												$rangesHtml.='<div class="row" id="row_'.$rangeCounter.'">
												<div class="cell'.$selectedRangeClass.'">'.$priceRange['range_from'].' - '.$priceRange['range_to'].'</div>
												<div class="cell'.$selectedRangeClass.'"> $ '.$priceRange['price'].'</div>
												</div>';
												$rangeCounter++;
											}
											$rangesHtml.='</div>';
											echo $rangesHtml;
										}
									?>
									
								</td>
							</tr>
							<form action="preprocess-order.php" method="POST" id="buyForm">
								<input type="hidden" name="productId" value="<?php echo $id;?>"/>
								<tr class="grey-box">
									<td class="info-label">Quantity</td>
									<td>
										
										<input type="number" min="1" class="quantity-box" value="1" name="quantity" /> piece
										<div class="quantity-validation-msg"></div>
									</td>
								</tr>
								<tr class="grey-box">
									<td class="info-label">Total Price</td>
									<td>
										<span class="font-bold" style="color:#fff;"> US $<span id="totalPriceField"><?php echo $totalPrice;?></span></span>
									</td>
								</tr>
								<tr class="grey-box">
									<td class="info-label"></td>
									<td>
										<div class="details-btn-wrapper">
											<div>
												<input type="submit" class="default-btn default-btn-green font-bold" id="buyBtn" value="BUY NOW" style="line-height:0; font-family: 'Source Sans Pro', sans-serif; font-weight:600;">
											</div>
											<div class="default-btn default-btn-green add-to-cart-btn" id="cartBtn">ADD TO CART</div>
										<div class="cart-btn-wrapper">
									</td>
								</tr>
							</form>
							<?php
							if(isset($_SESSION['active_user_id']))
							{
								?>
								<tr class="grey-box">
									<td class="info-label"></td>
									<td>
										<img src="images/heart-icon.png" height="12"/>
										<a href="#" class="wishlist-link" id="wishlistBtn">Add to Wish List (<span id="wishlistCount"><?php echo $wishlistCount;?></span> Ads)</a>
									</td>
								</tr>
							<?php
							}
						?>
						
					<?php
					}//end if quantity > 0
					else
					{
						echo '<tr class="grey-box"><td colspan="2" class="font-medium" style="color:#27a43e;">OUT OF STOCK</td></tr>';
					}
					?>
						
						
					</table>
				</div>
				<div class="clear"></div>
			</div>
			<div class="clear"></div>
			<br/><br/>
		</div>
	</div>
</div>

<?php include_once("includes/footer-template.php");?>

<script>
$(document).ready(function(){
	$('.quantity-box').ForceIntegersOnly();

	$('#zoomDiv').zoom();

	//load product images on page load to get better on thumb click responses
	setTimeout(function(){
		$('.thumb-img').each(function(){
		var imgUrl = $(this).attr('data-img');
		var img = $("<img />").attr('src', imgUrl).attr('width','320')
		    .load(function() {});
		});
	},3000);
	
	$('.thumb-img').click(function(){
		$('.thumb-img-active').removeClass('thumb-img-active');
		$(this).addClass('thumb-img-active');

		var imgUrl = $(this).attr('data-img');
		var img = $("<img />").attr('src', imgUrl).attr('width','320')
		    .load(function() {
		        if (!this.complete || typeof this.naturalWidth == "undefined" || this.naturalWidth == 0) {
		            alert('broken image!');
		        } else {
		        	//alert(this.naturalHeight);
		        	$('#zoomDiv').html(img);
		           
		        	var el = $('#zoomDiv'), curHeight = el.height(), autoHeight = el.css('height', 'auto').height();
					el.height(curHeight).animate({height: autoHeight}, 300, function(){$('#zoomDiv').zoom();});
		        }
		    });
	});
});


$('#cartBtn').click(function(){
	if(!validateQuantity())
	{
		return;
	}

	var productId = $('#productId').val();
	var quantity = $('.quantity-box').val().trim();
	$.ajax({
        type: "POST",
    	dataType: 'json',
        url: "partials/estore/cart.php",
        data: {addedId :productId, quantity: quantity}, 
        success: function(data){
            if(data)
            {
            	//new cart item
				if(data == "new")
				{
					$('.menu-cart-number').text(parseInt($('.menu-cart-number').text())+1);
					showNotification('Product added to cart.','success','.estore-notification-container');
					flushCartNumber();
				}
				//edited cart item
            	else if(data == true)
            	{
            		showNotification('Product added to cart.','success','.estore-notification-container');
        			flushCartNumber();
            	}
            	else
            		showNotification('default','error','.estore-notification-container');
            }
            else
            {
            	showNotification('default','error','.estore-notification-container');
            }
				
        },
        error: function(data){
        	showNotification('default','error','.estore-notification-container');
        	//alert(data.responseText);
        }
    });
});

$('.quantity-box').bind('change', function (){
	if(validateQuantity())
		updatePrice();
});

$('#wishlistBtn').click(function(e){
	e.preventDefault();
	var productId = $('#productId').val();

	$.ajax({
		type:'POST',
		dataType:'json',
		url:'partials/estore/wishlist.php',
		data:{addedId: productId},
		success:function(data){
			if(data == 'new')
			{
				$('#wishlistCount').text(parseInt($('#wishlistCount').text()) + 1);
				showNotification('Successfully added to wish list.','success','.estore-notification-container');
			}
			else if(data == 'exsists')
			{
				showNotification('This product is already in your wish list.','warning','.estore-notification-container');
			}
			else
			{
				showNotification('default','error','.estore-notification-container');
			}
		},
		error:function(data){
			showNotification('default','error','.estore-notification-container');
		}
	})
});
function validateQuantity(){
	$('.quantity-validation-msg').text('');
	var quantity = $('.quantity-box').val().trim();
	var totalQuantity = $('#totalQuantity').val();
	if(!parseInt(quantity))
	{
		$('.quantity-validation-msg').text('Value must be a valid integer.');
		return false;
	}
	if(quantity == '')
	{
		$('.quantity-validation-msg').text('Plaease enter quantity.');
		return false;
	}
	
	if(parseInt(quantity) > parseInt(totalQuantity))
	{
		$('.quantity-validation-msg').text('Purchases are limited to '+totalQuantity+' pieces');
		$('.quantity-box').val(totalQuantity);
		updatePrice();
		return false;
	}
	return true;
}

function updatePrice(){
	var totalPrice;
	var productPrice = parseFloat($('#priceHdn').val());
	var quantity = parseInt($('.quantity-box').val().trim());
	var rangeCounter = 0;
	var shippingPrice = parseFloat(calculateShippingPrice(quantity));
	if(!parseFloat(shippingPrice))
		shippingPrice = 0;
				
	$('#shippingPriceField').text(shippingPrice);
	totalPrice = (productPrice * quantity) + shippingPrice;
	$('#totalPriceField').text(totalPrice.toFixed(2));
}

function calculateShippingPrice(quantity){
	var priceType = $('#shippingPriceTypeHdn').val();
	var shippingPrice = 0;

	if(priceType == '0')
		return 0;

	if(priceType == '1')
		return $('#shippingPriceHdn').val(); 

	if(priceType == '2')
	{
		if($('#shippingPriceRangesJson').val().trim() != '')
		{
			var shippingPriceRangesArr = JSON.parse($('#shippingPriceRangesJson').val());

			if(shippingPriceRangesArr.length != 0)
			{
				var rangeCounter = 0;
				$.each(shippingPriceRangesArr, function(key,value){
		            if(parseInt(quantity) >= parseInt(value.range_from) && parseInt(quantity) <= parseInt(value.range_to))
		            {
		            	$('.cell-active').removeClass('cell-active');
            			$('#row_'+rangeCounter+' .cell').addClass('cell-active');
            			shippingPrice = value.price;
		            	
		            }
		            rangeCounter++;
		        });
		        //quantity is bigger than any of the ranges
		        //set last shipping price as shipping price
		        if(shippingPrice == 0)
		        {
		        	$('.cell-active').removeClass('cell-active');
		        	$('#row_'+(rangeCounter-1)+' .cell').addClass('cell-active');
		        	shippingPrice = shippingPriceRangesArr[shippingPriceRangesArr.length - 1].price;
		        }

		        return shippingPrice;
			}
		}
	}
}
$('#allShippingPricesLink').click(function(e){
	e.preventDefault();
	$('#shippingRangesContainer').toggle('500');
});

$('#buyForm').submit(function(e){
	if(!validateQuantity())
		e.preventDefault();
});
</script>