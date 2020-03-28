<?php
$page = 'contact';
$styles = '<link rel="stylesheet" type="text/css" href="css/contact.css">';
$scripts = '<script src="https://maps.googleapis.com/maps/api/js"></script>';
$scripts .= '<script src="http://ajax.aspnetcdn.com/ajax/jquery.validate/1.13.0/jquery.validate.js"></script>';
$scripts .= "
<script>
	function initialize() {
		var mapCanvas = document.getElementById('map_canvas');
		var mapOptions = {
			center: new google.maps.LatLng(38.863061,-77.336036),
			zoom: 8,
			mapTypeId: google.maps.MapTypeId.ROADMAP
		}
		var map = new google.maps.Map(mapCanvas, mapOptions)		
		
		var myLatlng = new google.maps.LatLng(38.863061,-77.336036);			
		var marker = new google.maps.Marker({
			position: myLatlng,
			map: map,
			title: 'NRA-TA Contact'
		});	
	}  
	google.maps.event.addDomListener(window, 'load', initialize);
</script>";
$scripts .= '
<script>
	function submitContactUsForm() {
		$("#contact-us-form").submit();
	}
	
	$.validator.setDefaults({
		submitHandler: function() {					
			$.post("contact-send.php", $("#contact-us-form").serialize(),  function(response) {
				$("#success").animate({opacity:1});
				$("#success").html(response);
				$("#success").delay( 4000 ).animate({opacity:0});
				if((response).indexOf("success-class") > 0) {
					$("#name").val("");
					$("#email").val("");
					$("#enquiry").val("");
					$("#message").val("");
				}
			});
			return false;
		}
	});
  
	$().ready(function() {
		// validate signup form on keyup and submit
		$("#contact-us-form").validate({
			rules: {
				name: "required",
				email: {
					required: true,
					email: true
				},
				enquiry: "required",
				message: "required"
			},
			messages: {
				name: "Please enter your name",
				email: "Please enter a valid email address",
				enquiry: "Please enter enquiry",
				message: "Please enter your message"
			}
		});
	});
</script>';
session_start();

if(!isset($_SESSION['active_user_id']))
	include_once('includes/header-template.php');
else
	include_once('includes/header-sn-template.php');

?>	
	<div class="background-wood-cover">
		<div class="background-wood-cover-content">
			<div class="contact-header">
				<div class="contact-header-content">
					<hr class="contact-header-line"> &nbsp Contact Us &nbsp <hr class="contact-header-line">
				</div>
			</div>
			<div class="contact-right-screen">	
				<div class="contact-right-screen-content">	
					<div class="overlay" onClick="style.pointerEvents='none'"></div>
					<div class="contact-iframe">
						<div class="styling-up"></div>
						<div class="contact-grid">
							<div class="contact-grid-content" style="text-align: center">
							<div id="success"></div>								
								<!-- <div class="contact-grid-left">
									<div class="styled-field "><span class="white-text-whitneysemibold">CONTACT</span></div>
									<div class="left-grid-content">
										<div class="white-text-whitneysemibold">NATIONAL RIFLE ASSOCIATION  TROPHY ANALYSIS<br>sed do eiusmod tempor<br>incididunt ut labore et dolor</div>
										<div class="left-grid-content-section">
											<img class="telephone-symbol tiny-symbol" src="./images/telephone-symbol.png"/><span class="green-text-whitneysemibold">&nbsp +46 (0)31 24 15 0000</span><br>
											<img class="email-symbol tiny-symbol" src="./images/email-symbol.png"/><span class="green-text-whitneysemibold">&nbsp office@nrata.com</span>
										</div>
										<div class="left-grid-content-section">
											<img class="twitter-symbol tiny-symbol" src="./images/twitter-symbol.png"/><div class="green-text-whitneysemibold">&nbsp @NationalRifleAssociation</div><br>
											<img class="facebook-symbol tiny-symbol" src="./images/facebook-symbol.png"/><div class="green-text-whitneysemibold">&nbsp /NationalRifleAssociation</div>
										</div>		
										<div id="map_canvas"></div>
									</div>
								</div> -->
								<div class="contact-grid-right" style="display: inline-block; float: none; text-align: left;">
									<form name="contact-us-form" class="contact-us-form" id="contact-us-form" method="post">
										<div class="styled-field "><span class="white-text-whitneysemibold">GENERAL ENQUIRIES</span></div>
										<div class="right-grid-content">
											<div class="right-grid-content-section">
												<div class="white-text-whitneysemibold"><label for="name">NAME</label></div>
												<input type="text" id="name" name="name" value=""/>
											</div>
											<div class="right-grid-content-section">
												<div class="white-text-whitneysemibold"><label for="email">E-MAIL</label></div>
												<input type="text" id="email" name="email" value=""/>
											</div>
											<div class="right-grid-content-section">
												<div class="white-text-whitneysemibold"><label for="enquiry">NATURE OF ENQUIRY</label></div>
												<input type="text" id="enquiry" name="enquiry" value=""/>
											</div>
											<div class="right-grid-content-section">
												<div class="white-text-whitneysemibold"><label for="message">MESSAGE</label></div>
												<textarea id="message" name="message" required></textarea><br>
											</div>
											<a href="javascript:void(0)" onclick="submitContactUsForm()" style="text-decoration: none;">
												<div class="green-btn-submit"><span class="green-btn-text-whitneymedium">SUBMIT</span></div>
											</a>
										</div>				
									</form>
								</div>	
								<div class="clear"></div>
							</div>
						</div>
					</div>
				</div>
			</div>
		</div>
	</div>
<?php include_once('includes/footer-template.php');?>