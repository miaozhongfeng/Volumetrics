<?php
$styles = '<link rel="stylesheet" type="text/css" href="css/sn-style.css">
			<link rel="stylesheet" type="text/css" href="css/estore.css">';
$subPage = 'e-store';

include_once("includes/header-estore-template.php");


$paymentMessage = "";
switch ($_SESSION['paymentStatus']) {
	case 'success':
		$paymentMessage = "success message";
		break;
	
	case 'cancelled':
		$paymentMessage = "cancelled message";
		break;

	default:
		$paymentMessage = "error message";
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
		<div class="estore-content">
			<div class="estore-notification-container"><?php showSessionNotification();?></div>
			
			<div class="payment-completed-message">
				<?php echo $paymentMessage;?> 
			</div>

			
		</div>
		<br/><br/>
	</div>
</div>

<?php include_once("includes/footer-template.php");?>