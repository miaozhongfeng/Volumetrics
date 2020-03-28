<?php

require_once 'includes/database-access.php';

require __DIR__ . '/paypal/bootstrap.php';
use PayPal\Api\Amount;
use PayPal\Api\Details;
use PayPal\Api\Item;
use PayPal\Api\ItemList;
use PayPal\Api\Payer;
use PayPal\Api\Payment;
use PayPal\Api\RedirectUrls;
use PayPal\Api\Transaction;
use PayPal\Exception\PPConnectionException;

session_start();
if(!isset($_SESSION['orderData']))
{
    header("Location:e-store.php");
    exit();
}
    
if(!isset($_SESSION['active_user_id']))
{
    header("Location:login.php?redirectUrl=e-store.php");
    exit();
}

//if accessed from confirm order, shippingInfo id should be set
//if accessed from my orders existingOrder id should be set
if((!isset($_GET['existingOrderId']) || !ctype_digit($_GET['existingOrderId'])) && (!isset($_GET['shippingId']) || !ctype_digit($_GET['shippingId'])))
{
    header("Location:login.php?redirectUrl=e-store.php");
    exit();
}

$userId = $_SESSION['active_user_id'];
$shippingInfoId = isset($_GET['shippingId']) ? $_GET['shippingId'] : 0;
$existingOrderId = isset($_GET['existingOrderId']) ? $_GET['existingOrderId'] : 0;
$orderData = explode(",", $_SESSION['orderData']);


//get the data from session
$isMultiple = $orderData[0];
$isFromCart = $orderData[1];
$resultsArr = array();

if($isFromCart == 0)
{
    $productId = $orderData[2];
    $quantity = $orderData[3];
    $query = "SELECT name, price, shipping_price, shipping_price_type FROM products WHERE id = '$productId' LIMIT 1";
    $result = mysqli_query($config['mysql_con'], $query);
    if(!$result)
    {
        setErrorAndExit('error', 'default');
    }

    $row = mysqli_fetch_array($result, MYSQLI_ASSOC);
    $row['quantity'] = $quantity;
    $row['id'] = $productId;
    //free shipping
    if($row['shipping_price_type'] == '0')
        $row['shipping_price'] = 0;

    //check shipping range price
    if($row['shipping_price_type'] == '2')
    {
        $query = "SELECT price FROM shipping_prices 
                WHERE product_id = '$productId'
                AND 
                    (range_from <= '$quantity' AND range_to >= '$quantity')
                    OR
                        range_to = 
                        (
                            SELECT MAX(range_to) 
                            FROM shipping_prices
                            WHERE product_id = '$productId'
                        )
                LIMIT 1";

        $result = mysqli_query($config['mysql_con'], $query);
        if(!$result)
        {
            setErrorAndExit('error', 'default');
        }

        $row1 = mysqli_fetch_array($result, MYSQLI_NUM);

        $row['shipping_price'] = $row1[0];
    }
    array_push($resultsArr, $row);
    
}
else if($isFromCart == 1)
{
    $ids = array_slice($orderData, 2); 
    $idsString = join(",",$ids);

    $query = "SELECT c.quantity, p.id, p.name, p.price, p.shipping_price, p.shipping_price_type, sp.price AS shipping_range_price
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
    WHERE c.id IN ($idsString)
    GROUP BY p.id
    ORDER BY c.id DESC";

    $result = mysqli_query($config['mysql_con'], $query);
    if(!$result)
    {
        setErrorAndExit('error', 'default');
    }

    while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
    {
        if(isset($row['shipping_price_type']) && $row['shipping_price_type'] == '2')
            $row['shipping_price'] = $row['shipping_range_price'];
        array_push($resultsArr, $row);
    }

    //delete cart items
    $query = "DELETE FROM cart WHERE id IN ($idsString)";
    $result = mysqli_query($config['mysql_con'], $query);
    if($result)
    {
        unset($_SESSION["cartItems"]);
           
    }
}

//defaults
$baseUrl="http://68.169.52.12/antler/QA/paypal";
$currency = "USD";
$tax = 0.00;
$subtotal = 0;
$totalprice = 0;
$totalShipping = 0;

//create item objects
$itemObjArr = array();
foreach ($resultsArr as $res) 
{
    // ### Itemized information
    // (Optional) Lets you specify item wise information

    $item = new Item();
    $item->setName($res['name'])
    ->setCurrency($currency)
    ->setQuantity($res['quantity'])
    ->setPrice($res['price'])
    ->setSKU($res['id']);
    array_push($itemObjArr, $item);

    $subtotal += $res['quantity'] * $res['price'];
    $totalShipping += $res['shipping_price'];
}

// Calculate total ammount paypable
$totalprice = $subtotal + $totalShipping + $tax;
?>


<?php
$description = "description";
 
// ### Payer
// A resource representing a Payer that funds a payment
// For paypal account payments, set payment method
// to 'paypal'.
$payer = new Payer();
$payer->setPaymentMethod("paypal");
 

$itemList = new ItemList();
$itemList->setItems($itemObjArr);
 
// ### Additional payment details
// Use this optional field to set additional
// payment information such as tax, shipping
// charges etc.
$details = new Details();
$details->setShipping($totalShipping)
	->setTax($tax)
	->setSubtotal($subtotal);
 
// ### Amount
// Lets you specify a payment amount.
// You can also specify additional details
// such as shipping, tax.
$amount = new Amount();
$amount->setCurrency($currency)
	->setTotal($totalprice)
	->setDetails($details);
 
// ### Transaction
// A transaction defines the contract of a
// payment - what is the payment for and who
// is fulfilling it. 
$transaction = new Transaction();
$transaction->setAmount($amount)
	->setItemList($itemList)
	->setDescription($description);
 
// ### Redirect urls
// Set the urls that the buyer must be redirected to after 
// payment approval/ cancellation.
$redirectUrls = new RedirectUrls();
$redirectUrls->setReturnUrl("$baseUrl/execute-payment.php?approved=true")
	->setCancelUrl("$baseUrl/execute-payment.php?approved=false");
 
// ### Payment
// A Payment Resource; create one using
// the above types and intent set to 'sale'
$payment = new Payment();
$payment->setIntent("sale")
	->setPayer($payer)
	->setRedirectUrls($redirectUrls)
	->setTransactions(array($transaction));
 
// ### Create Payment
// Create a payment by calling the 'create' method
// passing it a valid apiContext.
// The return object contains the state and the
// url to which the buyer must be redirected to
// for payment approval
try 
{
	$payment->create($apiContext);
} 
catch (PPConnectionException $ex) 
{
    createFailureLog($ex->getMessage(), $userId);
    setErrorAndExit('error', 'default');
	//var_dump($ex->getData());	
}
 
// ### Get redirect url
// The API response provides the url that you must redirect
// the buyer to. Retrieve the url from the $payment->getLinks()
// method
foreach($payment->getLinks() as $link) {
	if($link->getRel() == 'approval_url') {
		$redirectUrl = $link->getHref(); 
		break;
	}
}
$payment_id = $payment->getId();
 
/////////////////////
//   insert in db
/////////////////////

// create new transaction entry in db
$hash = md5($payment_id);
$_SESSION['paymentId'] = $hash;
$query = "INSERT INTO paypal_transactions (payment_id, hash, complete) VALUES('$payment_id', '$hash', 0)";
$result = mysqli_query($config['mysql_con'], $query);
if(!$result)
{
    setErrorAndExit('error', 'default');
}

$transactionId = mysqli_insert_id($config['mysql_con']);

// create new order/orders
if($existingOrderId == 0)
{
    date_default_timezone_set("UTC"); 
    $dateNowUTC = date("Y-m-d H:i:s", time());  

    $ordersQueryString = '';
    foreach ($resultsArr as $item) 
    {
        $ordersQueryString.='('.$userId.','.$item['id'].','.$transactionId.','.$item['price'].','.$item['shipping_price'].','.$item['quantity'].','.$shippingInfoId.','."'$dateNowUTC'".'),';
    }
    $ordersQueryString = substr($ordersQueryString, 0, -1);
    $query = sprintf("INSERT INTO orders (user_id, product_id, transaction_id, price, shipping_price, quantity, shipping_info_id, date) VALUES %s", $ordersQueryString);
    $result = mysqli_query($config['mysql_con'], $query);
    if(!$result)
    {
        setErrorAndExit('error', 'default');
    }
}
//update existing order
else
{
    $query = "UPDATE orders SET transaction_id = '$transactionId' WHERE id = '$existingOrderId' LIMIT 1";
    $result = mysqli_query($config['mysql_con'], $query);
    if(!$result)
    {
        setErrorAndExit('error', 'default');
    }
}

// everything is ok, redirect to PayPal
if(isset($redirectUrl)) {
    unset($_SESSION['orderData']);
	header("Location: $redirectUrl");
	exit;
}

?>

<?php
function setErrorAndExit($type, $message){
    global $existingOrderId;

    if($existingOrderId == 0)
        $redirectUrl = 'confirm-order.php';
    else
        $redirectUrl = 'orders.php';

    $_SESSION[$type] = $message;
    header('Location:'. $redirectUrl );
    exit();
}

function createFailureLog($exceptionMsg, $userId){
    //echo "Exception: " . $ex->getMessage() . PHP_EOL;
    $logText = "";
    $dt = new DateTime();
    $logText .= "Date: ". $dt->format('Y-m-d H:i:s').PHP_EOL;
    $logText .= "Exception: ". $exceptionMsg.PHP_EOL;
    $logText .= "User ID: ". $userId.PHP_EOL;
    $logText .= "========================".PHP_EOL;
    file_put_contents("paypal/paypal_log.txt", $logText, FILE_APPEND);
}
?>