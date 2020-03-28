<?php
	session_start();

	if(isset($_SESSION["active_user_id"]))
	{
		include_once('includes/database-access.php');
		$id = $_SESSION["active_user_id"];
		$query = "DELETE FROM online_users WHERE user_id = '$id' LIMIT 1";
		$result = mysqli_query($config['mysql_con'], $query);
	}
	//unset($_SESSION["active_user_firstname"]);
	unset($_SESSION["active_user_fullname"]);
	unset($_SESSION["active_user_id"]);
	unset($_SESSION["active_user_picture"]);
	unset($_SESSION["active_user_datecreated"]);
	unset($_SESSION["cartItems"]);
	unset($_SESSION["orderData"]);
	session_destroy();
	header("Location:index.php");
?>