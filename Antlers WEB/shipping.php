<?php

$styles = '<link rel="stylesheet" type="text/css" href="css/sn-style.css">
			<link rel="stylesheet" type="text/css" href="css/estore.css">';
$scripts = '<script type="text/javascript" src="js/jquery.extends.js"></script>';
$subPage = 'shipping';

include_once("includes/database-access.php");
include_once('includes/countries.php');
include_once("includes/header-estore-template.php");
$msg = array();
if(!isset($_SESSION['active_user_id']))
	header("Location:e-store.php");

$userId = $_SESSION['active_user_id'];

//if post save to db
if(isset($_POST['formSubmitted']) && $_POST['formSubmitted'] == 'true')
{

	//validate input
	$countryCode = $_POST['countryCode'];
	if(isset($_POST['street']) && !empty($_POST['street']))
		$street = strip_tags(mysqlclean($_POST['street'], 100, $config['mysql_con']));
	else
		$error = 'Please enter all required fields.';

	if(isset($_POST['apartment']) && !empty($_POST['apartment']))
		$apartment = strip_tags(mysqlclean($_POST['apartment'], 100, $config['mysql_con']));
	else
		$apartment = '';

	if(isset($_POST['city']) && !empty($_POST['city']))
		$city = strip_tags(mysqlclean($_POST['city'], 30, $config['mysql_con']));
	else
		$error = 'Please enter all required fields.';

	if(isset($_POST['state']) && !empty($_POST['state']))
		$state = strip_tags(mysqlclean($_POST['state'], 60, $config['mysql_con']));
	else
		$error = 'Please enter all required fields.';

	if(isset($_POST['zip']) && !empty($_POST['zip']))
		$zip = strip_tags(mysqlclean($_POST['zip'], 9, $config['mysql_con']));
	else
		$zip = '';

	$telNumber ='';
	$mobileNumber = '';
	if((!isset($_POST['mobileNumber']) || empty($_POST['mobileNumber'])) 
		&& ((!isset($_POST['telCountryCode']) || empty($_POST['telCountryCode']))
			|| (!isset($_POST['telCityCode']) || empty($_POST['telCityCode']))
			|| (!isset($_POST['telNumber']) || empty($_POST['telNumber']))))
	{
		$error = 'You must include a Telephone and/or Mobile number.';
	}
	else
	{
		if(isset($_POST['mobileNumber']) && !empty($_POST['mobileNumber']))
		{
			if(ctype_digit($_POST['mobileNumber']))
				$mobileNumber = $_POST['mobileNumber'];
			else
				$error = 'Please enter valid mobile number. Only digits are allowed.';
		}

		if((isset($_POST['telCountryCode']) && !empty($_POST['telCountryCode']) && ctype_digit($_POST['telCountryCode']))
			&& (isset($_POST['telCityCode']) && !empty($_POST['telCityCode']) && ctype_digit($_POST['telCityCode']))
			&& (isset($_POST['telNumber']) && !empty($_POST['telNumber'])) && ctype_digit($_POST['telNumber']))
		{
			$telNumber = $_POST['telCountryCode'].'-'.$_POST['telCityCode'].'-'.$_POST['telNumber'];
		}
	}

	//input is valid, save in db
	if(!isset($error))
	{
		//update exsisting row
		if(isset($_POST['shippingInfoId']) && !empty($_POST['shippingInfoId']) && ctype_digit($_POST['shippingInfoId']))
		{
			$shippingInfoId = $_POST['shippingInfoId'];
			$query = "UPDATE customer_shipping_info 
						SET country_code = '$countryCode', 
							address = '$street',
							apartment = '$apartment',
							city = '$city',
							state = '$state',
							zip = '$zip',
							telephone_number = '$telNumber',
							mobile_number = '$mobileNumber'
						WHERE id = '$shippingInfoId' LIMIT 1";
		}
		//insert new row
		else
		{
			$query = "INSERT INTO customer_shipping_info (customer_id, country_code, address, apartment, city, state, zip, telephone_number, mobile_number)
					VALUES('$userId', '$countryCode', '$street', '$apartment', '$city', '$state', '$zip', '$telNumber', '$mobileNumber')";
		}
		

		$result = mysqli_query($config['mysql_con'], $query) or die(mysqli_error($config['mysql_con']));
		if(!$result)
		{
			showErrorAndExit('default');
		}

		$_SESSION['success'] = 'Shipping information successfully saved.';
		header("Location:e-store.php");	
	}
	else
		$msg['warning'] = $error;
}


//get shipping info
$query = "SELECT * FROM customer_shipping_info WHERE customer_id = '$userId' LIMIT 1";
$result = mysqli_query($config['mysql_con'], $query);
if(!$result)
{
	showErrorAndExit('default');
}

list($id, $customer_id, $countryCode, $street, $apartment, $city, $state, $zip, $telephoneNumber, $mobileNumber) = mysqli_fetch_array($result, MYSQLI_NUM);
$telCountryCode = '';
$telCityCode = '';
$telNumber = '';
if(!empty($telephoneNumber))
{
	$telNumArr = explode("-", $telephoneNumber);
	if(sizeof($telNumArr) == 3)
	{
		$telCountryCode = $telNumArr[0];
		$telCityCode = $telNumArr[1];
		$telNumber = $telNumArr[2];
	}
}

$countries = countryArray();
?>


<div class="sn-main-container">
	<div class="sn-main-top">
		<div class="arrow-holder">
			<div class="sn-main-arrow" style="left:362px"></div>
		</div>
	</div>
	<div class="sn-scrolable-margin"></div>
	<div class="sn-scrolable">
		<div class="estore-content">
			<div class="shipping-container">
				<div class="estore-notification-container">
					<?php showNotification($msg);?>
				</div>
				<form id="shippingForm" action="" method="post">
					<input type="hidden" name="formSubmitted" value="true">
					<input type="hidden" name="shippingInfoId" value="<?php echo $id?>" />
					<table class="shipping-table">
						<tr>
							<td style="width:200px;">
								<div class="angle-div default-angle-div shipping-label">
									Country/Region
								</div>
							</td>
							<td>
								<select name="countryCode">
									<?php
										foreach($countries as $key => $value) {
											if($key == $countryCode)
										    	echo '<option value="'.$key.'" selected>'.$value.'</option>';
										    else
										    	echo '<option value="'.$key.'">'.$value.'</option>';
										}
									?>
								</select>
							</td>
						</tr>

						<tr>
							<td style="width:200px;">
								<div class="angle-div default-angle-div shipping-label">
									Street Address
								</div>
							</td>
							<td>
								<input type="text" placeholder="Street Address" style="margin-bottom:5px;" name="street" id="street" maxlength="100" value="<?php echo $street;?>"><br/>
								<input type="text" placeholder="Apartment, suite, unit. (optional)" name="apartment" id="apartment" maxlength="100" value="<?php echo $apartment;?>">
								<div class="validation-msg" id="streetField"></div>
							</td>
						</tr>

						<tr>
							<td style="width:200px;">
								<div class="angle-div default-angle-div shipping-label">
									City
								</div>
							</td>
							<td>
								<input type="text" name="city" id="city" maxlength="30" value="<?php echo $city;?>">
								<div class="validation-msg" id="cityField"></div>
							</td>
						</tr>

						<tr>
							<td style="width:200px;">
								<div class="angle-div default-angle-div shipping-label">
									State/Province/Region
								</div>
							</td>
							<td>
								<input type="text" name="state" id="state" maxlength="60" value="<?php echo $state;?>">
								<div class="validation-msg" id="stateField"></div>
							</td>
						</tr>
					</table>

					<table class="shipping-table" style="float:right">
						<tr>
							<td style="width:200px;">
								<div class="angle-div default-angle-div shipping-label">
									Zip/Postal Code
								</div>
							</td>
							<td>
								<input type="text" name="zip" id="zip" maxlength="9" value="<?php echo $zip;?>">
								<div class="validation-msg" id="zipField"></div>
							</td>
						</tr>

						<tr>
							<td style="width:200px;">
								<div class="angle-div default-angle-div shipping-label">
									Telephone
								</div>
							</td>
							<td>
								<input type="text" style="width:40px" name="telCountryCode" id="telCountryCode" class="onlyIntegers" maxlength="8" value="<?php echo $telCountryCode;?>">
								<input type="text" style="width:40px" name="telCityCode" id="telCityCode" class="onlyIntegers" maxlength="8" value="<?php echo $telCityCode;?>">
								<input type="text" style="width:137px" name="telNumber" id="telNumber" class="onlyIntegers" maxlength="36" value="<?php echo $telNumber;?>">
								
								<div style="font-size:13px;">Country code - City Code - Tel. Number</div>
								<div class="validation-msg" id="phoneField"></div>
							</td>
						</tr>

						<tr>
							<td style="width:200px;">
								<div class="angle-div default-angle-div shipping-label">
									Mobile
								</div>
							</td>
							<td>
								<input type="text" name="mobileNumber" id="mobileNumber" class="onlyIntegers" maxlength="16" value="<?php echo $mobileNumber;?>">
								<div class="validation-msg" id="mobileField"></div>
							</td>
						</tr>
					</table>

					<div class="clear"></div>
					<div class="inline-btns-holder" style="float:right">
						<div class="default-btn default-btn-green" id="saveBtn" style="width:260px;">SAVE AND CONTINUE SHOPPING</div>
						<a href="e-store.php" class="default-btn shipping-cancel-btn">CANCEL</a>
					</div>
				</form>

			
		</div>
		</div>
	</div>
</div>

<?php include_once("includes/footer-template.php");?>
<script>
$('.onlyIntegers').ForceIntegersOnly();
$('#saveBtn').click(function(){
	if(validateInput())
		$('#shippingForm').submit();
})
function validateInput(){
	var valid = true;

	$('.validation-msg').hide();

	if($('#street').val().trim() == '')
	{
		$('#streetField').text('Please enter your street address.').show();
		valid = false;
	}
	if($('#city').val().trim() == '')
	{
		$('#cityField').text('Please enter your city.').show();
		valid = false;
	}
	if($('#state').val().trim() == '')
	{
		$('#stateField').text('Please enter your state/province/region.').show();
		valid = false;
	}
	//validate phone numbers
	if($('#mobileNumber').val().trim() == '' && $('#telCountryCode').val().trim() == '' && $('#telCityCode').val().trim() == '' && $('#telNumber').val().trim() == '')
	{
		$('#phoneField').text('You must include a Telephone and/or Mobile number.').show();
		valid = false;
	}
	else
	{
		//mobile field
		if($('#mobileNumber').val().trim() != '')
		{
			if(!parseInt($('#mobileNumber').val()))
			{
				$('#mobileField').html('Please enter valid mobile number. Only digits are allowed').show();
				valid = false;
			}
			else
			{
				$('#mobileNumber').val(parseInt($('#mobileNumber').val()));

				if($('#mobileNumber').val().trim().length < 5)
				{
					$('#mobileField').html('Please enter between 5 - 16 digits').show();
					valid = false;
				}
			}
		}

		//telephone fields
		if($('#telCountryCode').val().trim() != '' || $('#telCityCode').val().trim() != '' || $('#telNumber').val().trim() != '')
		{
			if($('#telNumber').val().trim() == '')
			{
				$('#phoneField').text('Please enter telephone number.').show();
				valid = false;
			}
			else
			{
				if(!parseInt($('#telNumber').val()))
				{
					$('#phoneField').text('Please enter valid phone number. Only digits are allowed').show();
					valid = false;
				}
				else
					$('#telNumber').val(parseInt($('#telNumber').val()));
			}

			if($('#telCityCode').val().trim() == '')
			{
				$('#phoneField').text('Please enter city code.').show();
				valid = false;
			}
			else
			{
				if(!parseInt($('#telCityCode').val()))
				{
					$('#phoneField').text('Please enter valid phone number. Only digits are allowed').show();
					valid = false;
				}
				else
					$('#telCityCode').val(parseInt($('#telCityCode').val()));
			}

			if($('#telCountryCode').val().trim() == '')
			{
				$('#phoneField').text('Please enter country code.').show();
				valid = false;
			}
			else
			{
				if(!parseInt($('#telCountryCode').val()))
				{
					$('#phoneField').text('Please enter valid phone number. Only digits are allowed').show();
					valid = false;
				}
				else
					$('#telCountryCode').val(parseInt($('#telCountryCode').val()));
			}
		}
	}
	return valid;
}

</script>