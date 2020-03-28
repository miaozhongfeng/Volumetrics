<?php
ob_start();	
	include_once('includes/database-access.php');
	include_once('includes/functions.php');
	
	date_default_timezone_set("UTC"); 
	session_start();

	// Here we get all the information from the fields and store them into database.
	$usernameForm = $_POST['username'];
	$passwordForm = $_POST['password'];
	$confirm_passwordForm = $_POST['confirm_password'];
	$firstnameForm = $_POST['firstname'];
	$lastnameForm = $_POST['lastname'];
	$emailForm = $_POST['email'];
	
	if(isset($_GET['type'])) {
		if(isset($_GET['id'])) {
			$salt = $_GET['id'];
			
			//gets the info for user which is connected with the email that is sent
			$queryGetUserHashPass = "SELECT * 
									FROM registered_users 
									WHERE salt = '$salt'";
			
			if ($resultGetUserHashPass = @ mysqli_query ($config['mysql_con'],$queryGetUserHashPass)) {	
				$num_rowsGetUserHashPass = @ mysqli_num_rows($resultGetUserHashPass);	

				while ($rowGetUserHashPass = @ mysqli_fetch_array($resultGetUserHashPass)) {
					$UserHashPassDeleted = $rowGetUserHashPass['isDeleted'];
					$UserHashPassVerified = $rowGetUserHashPass['isVerified'];
				}
			}
			
			//header("Location: fff.php?rows=$num_rowsGetUserHashPass&deleted=$UserHashPassDeleted&verified=$UserHashPassVerified");
			//die();	
			
			if(($num_rowsGetUserHashPass == 0) || ($UserHashPassDeleted == 1)){
						
				$notificationText = "Your account was not found in the system!";
				$notificationType = "error";
				$notificationThirdClass = "notification-session";
				$_SESSION["register_notification"] = customNotifications($notificationText, $notificationType, $notificationThirdClass);
			
				header('Location: register.php');
				die();		
			}
						
			//confirm the user if he is pending
			if($_GET['type'] == "confirm") 
			{
				if($UserHashPassVerified == 1){
							
					$notificationText = "Your account has already been verified!";
					$notificationType = "warning";
					$notificationThirdClass = "notification-session";
					$_SESSION["login_notification"] = customNotifications($notificationText, $notificationType, $notificationThirdClass);
				
					header('Location: login.php');
					die();		
				}
			
				$queryConfirmUser = "UPDATE registered_users
									SET isVerified = 1
									WHERE salt = '$salt'";									
				
				if ($resultConfirmUser = @ mysqli_query ($config['mysql_con'],$queryConfirmUser)) {
							
					$notificationText = "Your account has been verified!";
					$notificationType = "success";
					$notificationThirdClass = "notification-session";
					$_SESSION["login_notification"] = customNotifications($notificationText, $notificationType, $notificationThirdClass);
				
					header('Location: login.php');
					die();				
				} else {
					dberror($config['mysql_con']);
					
					$notificationText = "Some error occurred!";
					$notificationType = "error";
					$notificationThirdClass = "notification-session";
					$_SESSION["register_notification"] = customNotifications($notificationText, $notificationType, $notificationThirdClass);
					
					header('Location: register.php');
					die();
				}	
			} else if($_GET['type'] == "cancel") { //cancel user if he is pending
			
				if($UserHashPassVerified == 1){
							
					$notificationText = "Your account is already verified and can not be deleted!";
					$notificationType = "warning";
					$notificationThirdClass = "notification-session";
					$_SESSION["login_notification"] = customNotifications($notificationText, $notificationType, $notificationThirdClass);
				
					header('Location: login.php');
					die();		
				}			
			 
				$queryDeleteUser = "DELETE FROM registered_users
									WHERE salt = '$salt'";

				if ($resultDeleteUser = @ mysqli_query ($config['mysql_con'],$queryDeleteUser)) {
				
					$notificationText = "Your account has been cancelled!";
					$notificationType = "warning";
					$notificationThirdClass = "notification-session";
					$_SESSION["register_notification"] = customNotifications($notificationText, $notificationType, $notificationThirdClass);

					header('Location: register.php');
					die();				
				} else {
					dberror($config['mysql_con']);
					
					$notificationText = "Some error occurred!";
					$notificationType = "error";
					$notificationThirdClass = "notification-session";
					$_SESSION["register_notification"] = customNotifications($notificationText, $notificationType, $notificationThirdClass);
					
					header('Location: register.php');
					die();
				}	
			} else {

				$notificationText = "Some error occurred!";
				$notificationType = "error";
				$notificationThirdClass = "notification-session";
				$_SESSION["register_notification"] = customNotifications($notificationText, $notificationType, $notificationThirdClass);
			
				header('Location: register.php');
				die();
			}
		} else {
					
			$notificationText = "Some error occurred!";
			$notificationType = "error";
			$notificationThirdClass = "notification-session";
			$_SESSION["register_notification"] = customNotifications($notificationText, $notificationType, $notificationThirdClass);
							
			header('Location: register.php');
			die();
		}
	}
	
	if(empty($usernameForm) || $usernameForm == "" || empty($passwordForm) || $passwordForm == "" || empty($firstnameForm) || $firstnameForm == "" || empty($lastnameForm) || $lastnameForm == "" || empty($emailForm) || $emailForm == "" || ($passwordForm != $confirm_passwordForm)) {
		header('Location: register.php');
		die();
	}
	
	$queryGetUsername = "SELECT * 
						FROM registered_users 
						WHERE username = '$usernameForm' AND isDeleted = 0";
	
	//check if username already exist in database
	if ($resultGetUsername = @ mysqli_query ($config['mysql_con'],$queryGetUsername)) {	
		$num_rowsGetUsername = mysqli_num_rows($resultGetUsername);	
		if($num_rowsGetUsername > 0) {
$output = ob_get_contents();
ob_end_clean();
				
			$notificationText = "Username already exist! Try with another one!";
			$notificationType = "error";
			$notificationThirdClass = "failure-class";
			echo customNotifications($notificationText, $notificationType, $notificationThirdClass);

		} else {
		
			$queryGetEmail = "SELECT * 
							  FROM registered_users 
							  WHERE email = '$emailForm' AND isDeleted = 0";
		
			//check if email already exist in database
			if ($resultGetEmail = @ mysqli_query ($config['mysql_con'],$queryGetEmail)) {	
				$num_rowsGetEmail = mysqli_num_rows($resultGetEmail);	
				if($num_rowsGetEmail > 0) {
$output = ob_get_contents();
ob_end_clean();
				
					$notificationText = "Email is already used! Try with another one!";
					$notificationType = "error";
					$notificationThirdClass = "failure-class";
					echo customNotifications($notificationText, $notificationType, $notificationThirdClass);

				} else {		
		
					$actual_link = rtrim("http://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]");
					$actual_link = rtrim($actual_link, "/");
					$password_hash = hash('sha256', $passwordForm);
					$digits = 3;
					$salt = rand(pow(10, $digits-1), pow(10, $digits)-1);
					$salt = hash('sha256', $password_hash.$salt);
					$date_nowUTC = date('Y-m-d H:i:s');		
					$confirmationURL = $actual_link."?type=confirm&id=$salt";
					$cancelURL = $actual_link."?type=cancel&id=$salt";
					
					$subjectMsg = "NRA-TA Registration";
					$contentMsg = "Dear $firstnameForm $lastnameForm, <br><br>
					Thank you for registering with NRA-TA team.<br><br>
					To confirm your registration please click <a href='$confirmationURL'>here</a>.<br>
					If this was not you, cancel the registration <a href='$cancelURL'>here</a>.<br><br>
					Regards<br>
					NRA-TA team.<br>";
					$fromNameMsg = "NRA-TA";
					
					$queryInsertUser = "INSERT INTO registered_users (username, password, firstname, lastname, email, isDeleted, isVerified, dateCreated, dateLastLogin, salt) VALUES ('$usernameForm', '$password_hash', '$firstnameForm', '$lastnameForm', '$emailForm', '0', '0', '$date_nowUTC', '$date_nowUTC', '$salt');";
					
					//check if the new user is successfully inserted into database
					if ($resultInsertUser = @ mysqli_query ($config['mysql_con'],$queryInsertUser)) {	
						$SendToClient = SendMail( $emailForm, $contentMsg, $subjectMsg, $fromNameMsg );
$output = ob_get_contents();
ob_end_clean();
							
						$notificationText = "You have successfully registered with us! Please check your email to confirm the account!";
						$notificationType = "success";
						$notificationThirdClass = "success-class";
						echo customNotifications($notificationText, $notificationType, $notificationThirdClass);
							
					} else {
						dberror($config['mysql_con']);
$output = ob_get_contents();
ob_end_clean();	

						$notificationText = "Some error occurred while registration! Please try again later!";
						$notificationType = "error";
						$notificationThirdClass = "failure-class";
						echo customNotifications($notificationText, $notificationType, $notificationThirdClass);
					}
				}
			}
		}
	} else {
		dberror($config['mysql_con']);
	} 
?>