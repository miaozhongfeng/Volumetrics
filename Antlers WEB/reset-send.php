<?php
ob_start();	
	include_once('includes/database-access.php');
	include_once('includes/functions.php');
	
	date_default_timezone_set("UTC"); 
	session_start();
	
	// Here we get all the information from the fields needed for reset password.
	$hiddenIDForm = $_POST['hiddentype'];
	$hiddenSaltForm = $_POST['hiddenid'];
	$passwordForm = $_POST['password'];
	$confirm_passwordForm = $_POST['confirm_password'];
	
	//gets the info for user which is connected with the email that is sent
	$queryGetUserForReset = "SELECT * 
							 FROM registered_users 
							 WHERE id = '$hiddenIDForm' AND salt = '$hiddenSaltForm'";	

	if ($resultGetUserForReset = @ mysqli_query ($config['mysql_con'],$queryGetUserForReset)) {	
		$num_rowsGetUserForReset = @ mysqli_num_rows($resultGetUserForReset);	

		if($num_rowsGetUserForReset > 0) {
			while ($rowGetUserForReset = @ mysqli_fetch_array($resultGetUserForReset)) {
				$UserForResetUsername = $rowGetUserForReset['username'];
				$UserForResetEmail = $rowGetUserForReset['email'];			
				$UserForResetFirstName = $rowGetUserForReset['firstname'];
				$UserForResetLastName = $rowGetUserForReset['lastname'];
				$UserForResetDeleted = $rowGetUserForReset['isDeleted'];
				$UserForResetVerified = $rowGetUserForReset['isVerified'];
			}
			
			if($UserForResetDeleted == 1) {
$output = ob_get_contents();
ob_end_clean();		
				$notificationText = "Account does no longer exist!";
				$notificationType = "error";
				$notificationThirdClass = "failure-class";
				echo customNotifications($notificationText, $notificationType, $notificationThirdClass);
			} else if($UserForResetVerified == 0) {
$output = ob_get_contents();
ob_end_clean();		
				$notificationText = "Account is not verified!";
				$notificationType = "warning";
				$notificationThirdClass = "failure-class";
				echo customNotifications($notificationText, $notificationType, $notificationThirdClass);
			} else {
			
				$password_hash = hash('sha256', $passwordForm);
				$digits = 3;
				$salt = rand(pow(10, $digits-1), pow(10, $digits)-1);
				$salt = hash('sha256', $password_hash.$salt);

				$queryResetUser = "UPDATE registered_users
								   SET password = '$password_hash', salt = '$salt'
								   WHERE id = '$hiddenIDForm' AND salt = '$hiddenSaltForm'";	

				$resultResetUser = @ mysqli_query ($config['mysql_con'],$queryResetUser);				
$output = ob_get_contents();
ob_end_clean();		
				$notificationText = "Your password is successfully reset!";
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