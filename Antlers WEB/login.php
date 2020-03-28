<?php
session_start();
if(isset($_SESSION['active_user_id']))
{
	header("Location:social-network.php");
	exit();
}

$page = 'home';
$styles = '<link rel="stylesheet" type="text/css" href="css/login.css">';
$scripts = '<script src="http://ajax.aspnetcdn.com/ajax/jquery.validate/1.13.0/jquery.validate.js"></script>';
$scripts .= '
<script>
	function submitLoginForm() {
		$("#login-form").submit();	
	}
  
  	$.validator.setDefaults({
		submitHandler: function() {					
			$.post("login-enter.php", $("#login-form").serialize(),  function(response) {
				if((response).indexOf("success-class") > 0) 
				{
					var redirectUrl = getUrlParameter("redirectUrl");
					if(redirectUrl == null || redirectUrl == "" || redirectUrl == "undefined" || redirectUrl.indexOf(".php") == -1)
						window.location.href = "social-network.php";
					else
						window.location.href = redirectUrl;
				} 
				else 
				{
					$("#success").animate({opacity:1});
					$("#success").html(response);
					$("#success").delay( 4000 ).animate({opacity:0});
				}
			});
			return false;
		}
	}); 
  
	$().ready(function() {
		$(".notification-session").animate({opacity:1});
		$(".notification-session").delay(4000).animate({opacity:0});	
		
		// validate login form on keyup and submit
		$("#login-form").validate({
			rules: {
				username: {
					required: true
				},
				password: {
					required: true
				}
			},
			messages: {	
				username: {
					required: "Please enter a username"
				},
				password: {
					required: "Please enter a password"
				}
			}
		});
	});
  
	function getUrlParameter(name){
	    var results = new RegExp("[\?&]" + name + "=([^&]*)").exec(window.location.href);
	    if (results==null){
	       return null;
	    }
	    else{
	       return results[1] || 0;
	    }
	}
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
							if(isset($_SESSION["login_notification"]))
								echo $_SESSION["login_notification"];
						?>
						</div>
						<div class="contact-grid-middle-top">
							<div class="styled-field "><span class="white-text-whitneysemibold">LOGIN</span></div>	
						</div>
						<div class="contact-us-form">
							<form name="login-form" class="login-form" id="login-form" method="post">
								<div class="contact-grid-middle">
									<div class="middle-grid-content">
										<div class="middle-grid-content-section">
											<span class="white-text-whitneysemibold">USERNAME</span>
											<span class="green-text-whitneysemibold">REQUIRED</span><br>						
											<input type="text" id="username" name="username" value=""/><br>
										</div>
										<div class="middle-grid-content-section">
											<span class="white-text-whitneysemibold">PASSWORD</span>
											<span class="green-text-whitneysemibold">REQUIRED</span><br>						
											<input type="password" id="password" name="password" value=""/><br>
										</div>
										<a href="javascript:void(0)" onclick="submitLoginForm()" style="text-decoration: none;">
											<div class="green-btn-submit"><span class="green-btn-text-whitneymedium">SUBMIT</span></div>
										</a>								
										<span class="white-text-whitneymedium"><a href="forgotpassword.php">Forgotten your username/password?</a></span>
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

<?php include_once('includes/footer-template.php');?>
