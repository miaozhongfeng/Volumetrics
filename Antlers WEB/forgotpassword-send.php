<?php
ob_start();	
	include_once('includes/database-access.php');
	include_once('includes/functions.php');
	
	date_default_timezone_set("UTC"); 
	session_start();

	// Here we get all the information from the fields and send email with password.
	$emailForm = $_POST['email'];
	
	//gets the info for user which is entered in the fields in the form
	$queryGetUserForgotPass = "SELECT * 
							   FROM registered_users 
							   WHERE email = '$emailForm'";
	
	if ($resultGetUserForgotPass = @ mysqli_query ($config['mysql_con'],$queryGetUserForgotPass)) {	
		$num_rowsGetUserForgotPass = @ mysqli_num_rows($resultGetUserForgotPass);	
		
		if($num_rowsGetUserForgotPass > 0) {
			while ($rowGetUserForgotPass = @ mysqli_fetch_array($resultGetUserForgotPass)) {
				$UserForgotPassID = $rowGetUserForgotPass['id'];
				$UserForgotPassSalt = $rowGetUserForgotPass['salt'];
				$UserForgotPassUserName = $rowGetUserForgotPass['username'];
				$UserForgotPassFirstName = $rowGetUserForgotPass['firstname'];
				$UserForgotPassLastName = $rowGetUserForgotPass['lastname'];
				$UserForgotPassDeleted = $rowGetUserForgotPass['isDeleted'];
				$UserForgotPassVerified = $rowGetUserForgotPass['isVerified'];
			}
			
			if($UserForgotPassDeleted == 1) {
$output = ob_get_contents();
ob_end_clean();		
				$notificationText = "Account does no longer exist!";
				$notificationType = "error";
				$notificationThirdClass = "failure-class";
				echo customNotifications($notificationText, $notificationType, $notificationThirdClass);
			} else if($UserForgotPassVerified == 0) {
$output = ob_get_contents();
ob_end_clean();		
				$notificationText = "Account is not verified!";
				$notificationType = "warning";
				$notificationThirdClass = "failure-class";
				echo customNotifications($notificationText, $notificationType, $notificationThirdClass);
			} else {
				$actual_link = rtrim("http://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]");
				$actual_link = rtrim($actual_link, "/");				
				$resetURL = str_replace("forgotpassword-send.php","reset.php",$actual_link)."?id=$UserForgotPassSalt&type=$UserForgotPassID"; //id is salt and type is id, I want to hide that from customer
				
				$subjectMsg = "NRA-TA Reset Password";				
				$contentMsg = "Dear $UserForgotPassFirstName $UserForgotPassLastName, <br><br>
				Username for getting into your account is: $UserForgotPassUserName<br><br>
				If you forgot your password, reset it by clicking <a href='$resetURL'>here</a>.<br><br>
				Regards<br>
				NRA-TA team.<br>";
				$fromNameMsg = "NRA-TA";
				$SendToClient = SendMail( $emailForm, $contentMsg, $subjectMsg, $fromNameMsg );
$output = ob_get_contents();
ob_end_clean();		
				$notificationText = "Link with credentials was sent to your email!";
				$notificationType = "success";
				$notificationThirdClass = "success-class";
				echo customNotifications($notificationText, $notificationType, $notificationThirdClass);			
			}			
		} else {
$output = ob_get_contents();
ob_end_clean();	
			$notificationText = "Account does not exist! Try with another one!";
			$notificationType = "error";
			$notificationThirdClass = "failure-class";
			echo customNotifications($notificationText, $notificationType, $notificationThirdClass);
		}
	}	
?>