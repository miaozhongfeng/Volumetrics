<?php
session_start();
if(isset($_SESSION['active_user_id']))
{
	header("Location:index.php");
	exit();
}

$page = 'home';
$styles = '<link rel="stylesheet" type="text/css" href="css/forgotpassword.css">';
$scripts = '<script src="http://ajax.aspnetcdn.com/ajax/jquery.validate/1.13.0/jquery.validate.js"></script>';
$scripts .= '
<script>
	function submitForgotPasswordForm() {
		$("#forgotpassword-form").submit();	
	}
  
  	$.validator.setDefaults({
		submitHandler: function() {					
			$.post("forgotpassword-send.php", $("#forgotpassword-form").serialize(),  function(response) {
				$("#success").animate({opacity:1});
				$("#success").html(response);
				$("#success").delay( 4000 ).animate({opacity:0});
				if((response).indexOf("success-class") > 0) {
					$("#email").val("");
				}
			});
			return false;
		}
	}); 
  
	$().ready(function() {
		$(".notification-session").animate({opacity:1});
		$(".notification-session").delay(4000).animate({opacity:0});	
		
		// validate forgotpassword form on keyup and submit
		$("#forgotpassword-form").validate({
			rules: {
				email: {
					required: true,
					email: true
				}
			},
			messages: {	
				email: "Please enter a valid email address"
			}
		});
	});
</script>';

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
							<div class="styled-field "><span class="white-text-whitneysemibold">FORGOT USERNAME/PASSWORD</span></div>	
						</div>
						<div class="contact-us-form">
							<form name="forgotpassword-form" class="forgotpassword-form" id="forgotpassword-form" method="post">
								<div class="contact-grid-middle">
									<div class="middle-grid-content">
										<div class="middle-grid-content-section">
											<span class="white-text-whitneysemibold">EMAIL</span>
											<span class="green-text-whitneysemibold">REQUIRED</span><br>						
											<input type="text" id="email" name="email" value=""/><br>
										</div>
										<a href="javascript:void(0)" onclick="submitForgotPasswordForm()" style="text-decoration: none;">
											<div class="green-btn-submit"><span class="green-btn-text-whitneymedium">SEND</span></div>
										</a>
										<span class="white-text-whitneymedium"><a href="login.php">Log in to your account</a></span>
									</div>
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

<?php include_once('includes/footer-template.php')?>