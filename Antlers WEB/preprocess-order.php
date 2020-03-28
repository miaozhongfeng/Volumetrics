<?php
# store order data in a session 

# session variable format : 
# isMultipleOrder,isFromCart,data
# possible cases:
# 0,1,id (in case of single cart item) 
# 1,1,id1,id2,id3... (in case of multiple cart items)
# 0,0,product_id,product_quantity (in case of direct order)

$isFromCart;
$isMultiple;
$data;
$invalid = false;
//single cart item
if(isset($_POST['cartId']))
{
	if(ctype_digit($_POST['cartId']))
	{
		$isMultiple = 0;
		$isFromCart = 1;
		$data = $_POST['cartId'];
	}
	else
		$invalid = true;
	
}

//multiple cart items
else if(isset($_POST['cartIds']))
{
	$isFromCart = 1;
	$data = $_POST['cartIds'];

	$idsArr = explode(",", $_POST['cartIds']);
	if(sizeof($idsArr) > 1)
		$isMultiple = 1;
	else if(sizeof($idsArr) == 1)
		$isMultiple = 0;
	else
		$invalid = true;
}

//single product item
else if(isset($_POST['productId']) && isset($_POST['quantity']))
{
	if(ctype_digit($_POST['productId']) && ctype_digit($_POST['quantity']) && $_POST['quantity'] > 0)
	{
		$isMultiple = 0;
		$isFromCart = 0;
		$data = $_POST['productId'].','.$_POST['quantity'];
	}
	else
		$invalid = true;
	
}

else
{
	header("Location:e-store.php");
	exit();
}

if($invalid)
{
	header("Location:e-store.php");
	exit();
}
session_start();

$_SESSION['orderData'] = $isMultiple.','.$isFromCart.','.$data;

header("Location:confirm-order.php");
?>