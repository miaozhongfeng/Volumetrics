<?php
ob_start();	
	include_once('includes/database-access.php');
	include_once('includes/functions.php');

	date_default_timezone_set("UTC"); 
	session_start();

	// Here we get all the information from the fields and store them into database.
	$usernameForm = $_POST['username'];
	$passwordForm = $_POST['password'];
	$password_hash = hash('sha256', $passwordForm);
	
	$queryCheckUserLogin = "SELECT * 
							FROM registered_users 
							WHERE username = '$usernameForm' AND password = '$password_hash' LIMIT 1";
	
	if ($resultCheckUserLogin = @ mysqli_query ($config['mysql_con'],$queryCheckUserLogin)) 
	{	
		$num_rowsCheckUserLogin = @ mysqli_num_rows($resultCheckUserLogin);	

		if($num_rowsCheckUserLogin > 0) 
		{
			while ($rowCheckUserLogin = @ mysqli_fetch_array($resultCheckUserLogin)) 
			{
				$UserHashPassID = $rowCheckUserLogin['id'];
				$UserHashPassUsername = $rowCheckUserLogin['username'];
				$UserHashPassFirstName = $rowCheckUserLogin['firstname'];
				$UserHashPassLastName = $rowCheckUserLogin['lastname'];
				$UserHashPassEmail = $rowCheckUserLogin['email'];
				$UserHashPersonalInfo = $rowCheckPersonalInfo['personalInfo'];
				$UserHashDateCreated = $rowCheckUserLogin['dateCreated'];
				$UserHashPassDeleted = $rowCheckUserLogin['isDeleted'];
				$UserHashPassVerified = $rowCheckUserLogin['isVerified'];
				$UserProfilePicture = $rowCheckUserLogin['profile_picture'];
			}
			if($UserHashPassDeleted == 1) {
				session_destroy();
				$output = ob_get_contents();
				ob_end_clean();		
				$notificationText = "Account does no longer exist!";
				$notificationType = "error";
				$notificationThirdClass = "failure-class";
				echo customNotifications($notificationText, $notificationType, $notificationThirdClass);
			} 
			else if($UserHashPassVerified == 0) 
			{
				session_destroy();
				$output = ob_get_contents();
				ob_end_clean();		
				$notificationText = "Account is not verified! Check your email!";
				$notificationType = "warning";
				$notificationThirdClass = "failure-class";
				echo customNotifications($notificationText, $notificationType, $notificationThirdClass);
			}

			//success
			else 
			{
				$_SESSION["active_user_id"] = $UserHashPassID;
				$_SESSION["active_user_fullname"] = $UserHashPassFirstName." ".$UserHashPassLastName;
				$_SESSION["active_user_datecreated"] = formatDate($UserHashDateCreated);
				$_SESSION["active_user_picture"] = $UserProfilePicture;

				transferCartItems($UserHashPassID);
				unset($_SESSION["cartItems"]);

				$output = ob_get_contents();
				ob_end_clean();			
				$notificationText = "Successfull log in!";
				$notificationType = "success";
				$notificationThirdClass = "success-class";
				echo customNotifications($notificationText, $notificationType, $notificationThirdClass);
			}
		} 
		else
		{
			session_destroy();
			$output = ob_get_contents();
			ob_end_clean();	
			$notificationText = "Incorrect username or password!";
			$notificationType = "error";
			$notificationThirdClass = "failure-class";
			echo customNotifications($notificationText, $notificationType, $notificationThirdClass);
		}
	}

function transferCartItems($userId){
	if(isset($_COOKIE['anonymous_id']))	
	{
		global $config;
		$anonymousId = $_COOKIE['anonymous_id'];

		//transfer cart items from anonymous id to logged id (except for the duplicates)
		//first nested select is only workaround
		$query = "UPDATE cart 
					SET user_id = '$userId', is_user_registered = '1'
					WHERE user_id = '$anonymousId' 
						AND is_user_registered = '0'
						AND product_id NOT IN
						(
							SELECT product_id FROM
							(
								SELECT product_id
								FROM cart AS c
								WHERE user_id = '$userId' AND is_user_registered = '1'
							) AS tmptable
							
						)";

		$result = mysqli_query($config['mysql_con'], $query);

		//remove duplicates
		if($result)
		{
			$query = "DELETE FROM cart WHERE user_id = '$anonymousId' AND is_user_registered = '0'";
			$result = mysqli_query($config['mysql_con'], $query);
		}
	}
}
?>