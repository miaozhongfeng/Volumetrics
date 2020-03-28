<?php
$page = 'home';
$styles = '<link rel="stylesheet" type="text/css" href="css/forgotpassword.css">';
$scripts = '<script src="http://ajax.aspnetcdn.com/ajax/jquery.validate/1.13.0/jquery.validate.js"></script>';
$scripts .= '
<script>
	function submitResetPasswordForm() {
		$("#resetpassword-form").submit();	
	}
  
  	$.validator.setDefaults({
		submitHandler: function() {					
			$.post("reset-send.php", $("#resetpassword-form").serialize(),  function(response) {
				$("#success").animate({opacity:1});
				$("#success").html(response);
				$("#success").delay( 4000 ).animate({opacity:0});
				if((response).indexOf("success-class") > 0) {
					$("#password").val("");
					$("#confirm_password").val("");
				}
			});
			return false;
		}
	}); 
  
	$().ready(function() {
		$(".notification-session").animate({opacity:1});
		$(".notification-session").delay(4000).animate({opacity:0});	
		
		// validate resetpassword form on keyup and submit
		$("#resetpassword-form").validate({
			rules: {
				password: {
					required: true,
					minlength: 5,
					maxlength: 30
				},
				confirm_password: {
					required: true,
					minlength: 6,
					equalTo: "#password"
				}
			},
			messages: {	
				password: {
					required: "Please provide a new password",
					minlength: "Your password must be at least 6 characters long",
					maxlength: "Password can not consist more than 30 characters"
				},
				confirm_password: {
					required: "Please provide a new password",
					minlength: "Your password must be at least 6 characters long",
					equalTo: "Please enter the same password as above"
				}
			}
		});
	});
</script>';

$hiddenID = "";
if(isset($_GET['type'])) { 		//this is id, I want to hide that from customer
	$hiddenID = $_GET['type'];
}
$hiddenSalt = "";
if(isset($_GET['id'])) {		//this is salt, I want to hide that from customer
	$hiddenSalt = $_GET['id'];
}

include_once('includes/header-template.php');
?>
<div class="background-wood-cover">
	<div class="background-wood-cover-content">
		<div class="contact-header">
			<div class="contact-header-content">
				&nbsp
			</div>		 
		</div>
		<div class="contact-right-screen">	
			<div class="contact-right-screen-content">			
				<div class="overlay" onClick="style.pointerEvents='none'"></div>
				<div class="contact-iframe">
					<div class="styling-up"></div>
					<div class="contact-grid">
						<div id="success"></div>
						<div class="contact-grid-middle-top">
							<div class="styled-field "><span class="white-text-whitneysemibold">RESET PASSWORD</span></div>	
						</div>
						<div class="contact-us-form">
							<form name="resetpassword-form" class="resetpassword-form" id="resetpassword-form" method="post">
								<div class="contact-grid-middle">
									<div class="middle-grid-content">
										<div class="middle-grid-content-section">
											<span class="white-text-whitneysemibold">NEW PASSWORD</span>
											<span class="green-text-whitneysemibold">REQUIRED</span><br>						
											<input type="password" id="password" name="password" value=""/><br>
										</div>
										<div class="middle-grid-content-section">
											<span class="white-text-whitneysemibold">CONFIRM NEW PASSWORD</span>
											<span class="green-text-whitneysemibold">REQUIRED</span><br>						
											<input type="password" id="confirm_password" name="confirm_password" value=""/><br>
										</div>
										<a href="javascript:void(0)" onclick="submitResetPasswordForm()" style="text-decoration: none;">
											<div class="green-btn-submit"><span class="green-btn-text-whitneymedium">RESET</span></div>
										</a>
										<span class="white-text-whitneymedium"><a href="login.php">Log in to your account</a></span>
									</div>
								</div>
								<input type="hidden" name="hiddenid" id="hiddenid" value="<?php echo $hiddenSalt; ?>">
								<input type="hidden" name="hiddentype" id="hiddentype" value="<?php echo $hiddenID; ?>">
							</form>
						</div>
						<div class="clear"></div>
					</div>					
				</div>
			</div>
		</div>
	</div>
</div>

<?php include_once('includes/footer-template.php')?>