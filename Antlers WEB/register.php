<?php
$page = 'home';
$styles = '<link rel="stylesheet" type="text/css" href="css/register.css">';
$scripts = '<script src="http://ajax.aspnetcdn.com/ajax/jquery.validate/1.13.0/jquery.validate.js"></script>';
$scripts .= '
<script>
	function submitRegisterForm() {
		$("#register-form").submit();
	}
	
	$.validator.setDefaults({
		submitHandler: function() {	
			$(".ajaxLoader").show();	
			$("#submitLink").hide();
					
			$.post("register-add.php", $("#register-form").serialize(),  function(response) {
				//if($("#success").css("opacity") == 0) {
					$("#success").animate({opacity:1});
					$("#success").html(response);
					$("#success").delay( 4000 ).animate({opacity:0});
					if((response).indexOf("success-class") > 0) {
						$("#firstname").val("");
						$("#lastname").val("");
						$("#username").val("");
						$("#password").val("");
						$("#confirm_password").val("");
						$("#email").val("");
					}
					$(".ajaxLoader").hide();
					$("#submitLink").show();

				//}
			});
			return false;
		}
	});
  
	$().ready(function() {
		$(".notification-session").animate({opacity:0});
		$(".notification-session").delay(4000).animate({opacity:0});
	
		// validate register form on keyup and submit
		$("#register-form").validate({
			rules: {
				firstname: {
					required: true,
					maxlength: 30
				},
				lastname: {
					required: true,
					maxlength: 30
				},
				username: {
					required: true,
					minlength: 2,
					maxlength: 30
				},
				password: {
					required: true,
					minlength: 6,
					maxlength: 30
				},
				confirm_password: {
					required: true,
					minlength: 6,
					equalTo: "#password"
				},
				email: {
					required: true,
					email: true
				}
			},
			messages: {
				firstname: {
					required: "Please enter your first name",
					maxlength: "First name can not consist more than 30 characters"
				},				
				lastname: {
					required: "Please enter your last name",
					maxlength: "Last name can not consist more than 30 characters"
				},				
				username: {
					required: "Please enter a username",
					minlength: "Your username must be at least 2 characters long",
					maxlength: "Username can not consist more than 30 characters"
				},
				password: {
					required: "Please provide a password",
					minlength: "Your password must be at least 6 characters long",
					maxlength: "Password can not consist more than 30 characters"
				},
				confirm_password: {
					required: "Please provide a password",
					minlength: "Your password must be at least 6 characters long",
					equalTo: "Please enter the same password as above"
				},
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
							<div id="success">
							<?php
								session_start();
								if(isset($_SESSION["register_notification"]))
									echo $_SESSION["register_notification"];
								session_destroy();
							?>
							</div>
							<div class="contact-grid-middle-top">
								<div class="styled-field "><span class="white-text-whitneysemibold">REGISTER</span></div>	
							</div><br>
							<div class="contact-us-form">
								<form name="register-form" class="register-form" id="register-form" method="post">
									<div class="contact-grid-left">
										<div class="left-grid-content">
											<div class="left-grid-content-section">
												<span class="white-text-whitneysemibold"><label for="username">USERNAME</label></span>
												<span class="green-text-whitneysemibold">REQUIRED</span><br>						
												<input type="text" id="username" name="username" value=""/><br>
											</div>
											<div class="left-grid-content-section">
												<span class="white-text-whitneysemibold"><label for="password">PASSWORD</label></span>
												<span class="green-text-whitneysemibold">REQUIRED</span><br>						
												<input type="password" id="password" name="password" value=""/><br>
											</div>
											<div class="left-grid-content-section">
												<span class="white-text-whitneysemibold"><label for="confirm_password">CONFIRM PASSWORD</label></span>
												<span class="green-text-whitneysemibold">REQUIRED</span><br>						
												<input type="password" id="confirm_password" name="confirm_password" value=""/><br>
											</div>	
										</div>
									</div>
									<div class="contact-grid-right">
										<div class="right-grid-content">
											<div class="right-grid-content-section">
												<span class="white-text-whitneysemibold"><label for="firstname">FIRST NAME</label></span>
												<span class="green-text-whitneysemibold">REQUIRED</span><br>						
												<input type="text" id="firstname" name="firstname" value=""/><br>
											</div>
											<div class="right-grid-content-section">
												<span class="white-text-whitneysemibold"><label for="lastname">LAST NAME</label></span>
												<span class="green-text-whitneysemibold">REQUIRED</span><br>						
												<input type="text" id="lastname" name="lastname" value=""/><br>
											</div>
											<div class="right-grid-content-section">
												<span class="white-text-whitneysemibold"><label for="email">E-MAIL</label></span>
												<span class="green-text-whitneysemibold">REQUIRED</span><br>						
												<input type="text" id="email" name="email" value=""/><br>
											</div>
											<div  style="min-height:50px;">
												<div class="ajaxLoader">
												</div>
												<a href="javascript:void(0)" onclick="submitRegisterForm()" style="text-decoration: none;" id="submitLink">
													<div class="green-btn-submit"><span class="green-btn-text-whitneymedium">SUBMIT</span></div>
												</a>
											</div>
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