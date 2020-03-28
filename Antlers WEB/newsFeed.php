<?php
session_start();
include_once('includes/database-access.php');
$userId = $_SESSION['active_user_id'];
$query =   "SELECT 0 AS type, s.id, sp.image AS coverImage, 
				u.id AS fId, u.firstname AS fFirstname, u.lastname AS fLastname, u.profile_picture AS fImage,
				null AS fofId, null AS fofFirstname, null AS fofLastname, null AS fofImage,
				s.date_uploaded AS dateUploaded, null AS myFriendId
			FROM scans AS s
			LEFT JOIN scan_pictures AS sp
			ON s.id = sp.scan_id AND sp.is_cover = '1'
			INNER JOIN friends AS f
			ON(f.user1id = '$userId' && f.user2id = s.user_id) || (f.user1id = s.user_id && f.user2id = '$userId')
			INNER JOIN registered_users AS u
			ON (f.user1id = u.id OR f.user2id = u.id) AND u.id <> '$userId'
		UNION ALL(
			SELECT 1 AS type, up.id, up.image AS coverImage,
				u.id AS fId, u.firstname AS fFirstname, u.lastname AS fLastname, u.profile_picture AS fImage,
				null AS fofId, null AS fofFirstname, null AS fofLastname, null AS fofImage,
				up.date_uploaded AS dateUploaded, null AS myFriendId
			FROM user_pictures AS up
			INNER JOIN friends AS f
			ON(f.user1id = '$userId' && f.user2id = up.user_id) || (f.user1id = up.user_id && f.user2id = '$userId')
			INNER JOIN registered_users AS u
			ON (f.user1id = u.id OR f.user2id = u.id) AND u.id <> '$userId'
			)
		UNION ALL(
			SELECT 2 AS type, null AS id, null AS coverImage,
				f2.user1id AS fId, u1.firstname AS fFirstname, u1.lastname AS fLastname, u1.profile_picture AS fImage,
				f2.user2id AS fofId, u2.firstname AS fofFirstname, u2.lastname AS fofLastname, u2.profile_picture AS fofImage,
				f2.date_accepted AS dateUploaded,
				IF(f2.user1id IN (f.user1id, f.user2id) OR f2.user1id IN (f.user1id, f.user2id), f2.user1id, f2.user2id) AS myFriendId
			FROM friends AS f
			INNER JOIN friends AS f2
			ON '$userId' IN(f.user1id, f.user2id) 
				AND ((f.user1id IN(f2.user1id, f2.user2id) || f.user2id  IN(f2.user1id, f2.user2id)))
			INNER JOIN registered_users AS u1
			ON u1.id = f2.user1id
			INNER JOIN registered_users AS u2
			ON u2.id = f2.user2id
			GROUP BY f2.user1id, f2.user2id
			)
		
		ORDER BY dateUploaded DESC LIMIT 10
		";

$result = mysqli_query($config['mysql_con'], $query) or die(mysqli_error($config['mysql_con']));
while (list($type, $id, $coverImage, $fId, $fFirstname, $fLastname, $fImage,
		$fofId, $fofFirstname, $fofLastname, $fofImage, $dateUploaded, $myFriendId) = mysqli_fetch_array($result, MYSQLI_NUM))
{
	//scans
	if($type == 0)
	{
		echo "$fFirstname $fLastname has uploaded new scan<br>";
	}
	//pictures
	else if($type == 1)
	{
		echo "$fFirstname $fLastname has uploaded new picture<br>";
	}
	//friends
	else
	{
		$myFriendship = false;
		if($fId == $userId)
		{
			$user1Id = 0; $user1Firstname = $$user1Lastname = ''; $user1Image = $fImage;
			$user2Id = $fofId; $user2Firstname = $fofFirstname; $user2Lastname = $fofLastname; $user2Image = $fofImage;
			$myFriendship = true;
		}
		else if($fofId == $userId)
		{
			$user1Id = 0; $user1Firstname = $user1Lastname = ''; $user1Image = $fofImage;
			$user2Id = $fId; $user2Firstname = $fFirstname; $user2Lastname = $fLastname; $user2Image = $fImage;
			$myFriendship = true;
		}
		else if($fId == $myFriendId)
		{
			$user1Id = $fId; $user1Firstname = $fFirstname; $user1Lastname = $fLastname; $user1Image = $fImage;
			$user2Id = $fofId; $user2Firstname = $fofFirstname; $user2Lastname = $fofLastname; $user2Image = $fofImage;
		}
		else
		{
			$user1Id = $fofId; $user1Firstname = $fofFirstname; $user1Lastname = $fofLastname; $user1Image = $fofImage;
			$user2Id = $fId; $user2Firstname = $fFirstname; $user2Lastname = $fLastname; $user2Image = $fImage;			
		}

		echo $dateUploaded;
		if($myFriendship)
		{
			echo "You became friend with $user2Firstname $user2Lastname<br>";
		}
		else
		{
			echo "$user1Firstname $user1Lastname became friend with $user2Firstname $user2Lastname<br>";
		}
	}

}


?>