<?php

$page = 'social-network';
$styles = '<link rel="stylesheet" type="text/css" href="css/slick.css"/>
			<link rel="stylesheet" type="text/css" href="css/jquery.fancybox.css">
			<link rel="stylesheet" type="text/css" href="css/sn-style.css">';
$scripts = '<script type="text/javascript" src="js/slick.min.js"></script>
			<script type="text/javascript" src="js/jquery.fancybox.js"></script>
			<script type="text/javascript" src="js/pages/profile.js"></script>
			';
$headerText	= 'NRA-TA SOCIAL NETWORK';	
session_start();

if(isset($_GET['id']) && ctype_digit($_GET['id']))
	$userId = $_GET['id'];
else if(isset($_SESSION['active_user_id']))
	$userId = $_SESSION['active_user_id'];
else
{
	header("Location:index.php");
	exit();
}

if(!isset($_SESSION['active_user_id']))
{
	$styles .= '<link rel="stylesheet" type="text/css" href="css/sn-style.css">';
	include_once('includes/header-template.php');
}	
else
	include_once('includes/header-sn-template.php');

include_once('includes/database-access.php');

//get user info
$query = "SELECT firstname, lastname, personalInfo, city, hunting_location, gun, profile_picture FROM registered_users WHERE id='$userId' LIMIT 1";
$result = mysqli_query($config['mysql_con'], $query);
if(!$result)
	showErrorAndExit('default');
if(mysqli_num_rows($result) == 0)
	showErrorAndExit('The user does not exsist.');

list($firstname, $lastname, $personalInfo, $city, $huntingLocation, $gun, $image) = mysqli_fetch_array($result, MYSQLI_NUM);
$imgPath = 'user_images/profile_pictures/thumbs/'.$image;

//get total scans count
$query = "SELECT COUNT(id) FROM scans WHERE user_id = '$userId'";
$result = mysqli_query($config['mysql_con'], $query);
$row = mysqli_fetch_array($result, MYSQLI_NUM);
$totalScans = $row[0];

//get user scans
$query = "SELECT s.id, sp.image
			FROM scans AS s
			LEFT JOIN scan_pictures AS sp
			ON s.id = sp.scan_id AND sp.is_cover = '1'
			WHERE s.user_id = '$userId'
			GROUP BY s.id
			ORDER BY s.id DESC
			LIMIT 10";
$result = mysqli_query($config['mysql_con'], $query);

$scansArr = array();
if($result)
{
	while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
	{
		array_push($scansArr, $row);
	}
}

//get total pictures count
$query = "SELECT COUNT(id) FROM user_pictures WHERE user_id = '$userId'";
$result = mysqli_query($config['mysql_con'], $query);
$row = mysqli_fetch_array($result, MYSQLI_NUM);
$totalPictures = $row[0];

//get first n pictures
$query = "SELECT name, description, image 
			FROM user_pictures 
			WHERE user_id = '$userId'
			ORDER BY id DESC
			LIMIT 10";		
$result = mysqli_query($config['mysql_con'], $query);

$picturesArr = array();
if($result)
{
	while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
	{
		array_push($picturesArr, $row);
	}
}

//check if this is my profile
$myProfile = isset($_SESSION['active_user_id']) && $_SESSION['active_user_id'] == $userId;
//check friendship if logged in and not myProfile 
$showFriendBtn = $showUnfriendBtn = $showAcceptBtn = $showPendingFriendship = false;
if(isset($_SESSION['active_user_id']) && !$myProfile)
{
	$myUserId = $_SESSION['active_user_id'];
	$query = "SELECT id FROM friends WHERE 
				(user1id = '$userId' AND user2id = '$myUserId')
				OR (user1id = '$myUserId' AND user2id = '$userId')
				LIMIT 1";
	$result = mysqli_query($config['mysql_con'], $query);
	if($result)
	{
		if(mysqli_num_rows($result) > 0)
		{
			$showUnfriendBtn = true;
			list($friendshipId) = mysqli_fetch_array($result, MYSQLI_NUM);
		}
		else
		{
			//not friends, check friend requests
			$query = "SELECT id, inviter_id, user_id FROM friend_requests WHERE
						(inviter_id = '$userId' AND user_id = '$myUserId')
						OR (inviter_id = '$myUserId' AND user_id = '$userId')
						LIMIT 1";
			$result = mysqli_query($config['mysql_con'], $query);
			if($result)
			{
				//friend request exists
				if(mysqli_num_rows($result) > 0)
				{
					list($requestId, $inviterId, $inviteeId) = mysqli_fetch_array($result, MYSQLI_NUM);
					if($myUserId == $inviteeId)
						$showAcceptBtn = true;
					else
						$showPendingFriendship = true;
				}

				//not friends, no requests
				else
				{
					$showFriendBtn = true;
				}
			}	
		}
			
	}
}

?>
<div class="sn-menu-container">
	<?php include_once('partials/sn-menu.php');?>
</div>
<input type="hidden" id="userId" value="<?php echo $userId;?>"/>
<input type="hidden" id="totalPictures" value="<?php echo $totalPictures;?>"/>
<input type="hidden" id="totalScans" value="<?php echo $totalScans;?>"/>
<div class="sn-main-container">
	<div class="sn-main-top">
		<div class="arrow-holder">
			<div class="sn-main-arrow" style="left:52px"></div>
			<div class="scores-text" style="left: 83px;">PROFILE PAGE</div>
		</div>
	</div>
	<div class="sn-scrolable-margin"></div>
	<div class="sn-scrolable">
		<div id="notificationContainer"></div>

		<div class="profile-content">
			<div class="image-wrapper">
				<img src="<?php echo $imgPath?>" alt="profile picture"/><br/>
				<a href="my-trophyroom.php?id=<?php echo $userId;?>" class="default-btn my-trophyroom-btn public-my-trophyroom-btn"><?php echo ucfirst($firstname)."'s" ?><br/>Trophy Room</a>

				<?php if($showFriendBtn):?>
					<br/><br/>
					<a href="#" class="default-btn default-btn-green" id="requestFriendshipBtn" data-id="<?php echo $userId?>">Send Friend Request</a>
				<?php endif;?>

				<?php if($showUnfriendBtn):?>
					<br/><br/>
					<a href="#" class="default-btn default-btn-red" id="unfriendBtn" data-id="<?php echo $friendshipId;?>" data-name="<?php echo "$firstname $lastname";?>">Unfriend</a>
				<?php endif;?>

				<?php if($showAcceptBtn):?>
					<div class="buttons-wrapper">
						<br/>
						<i>Pending Friendship Request</i><br/>
						<a href="#" class="green-link accept-friendship" data-val="true" data-requestid="<?php echo $requestId; ?>">Accept</a> &nbsp;&nbsp;
						<a href="#" class="red-link accept-friendship" data-val="false" data-requestid="<?php echo $requestId; ?>">Reject</a>
					</div>
				<?php endif;?>

				<?php if($showPendingFriendship):?>
					<br/><br/>
					<div class="buttons-wrapper"><i>Pending Friendship Request</i></div>
				<?php endif;?>
			</div>
			<div class="info-wrapper">
				<h1 class="font-bold"><?php echo $firstname . " " .$lastname?></h1>
				<div class="font-light"><?php echo $personalInfo;?></div>

				<div class="info-cols-wrapper">

					<?php if(!empty($city)): ?>
						<div class="info-col">
							<div class="angle-div default-angle-div">FROM</div>
							<div class="font-light info-col-text"><?php echo $city;?></div>
						</div>
					<?php endif;?>

					<?php if(!empty($huntingLocation)): ?>
						<div class="info-col">
							<div class="angle-div default-angle-div">HUNTS IN</div>
							<div class="font-light info-col-text"><?php echo $huntingLocation;?></div>
						</div>
					<?php endif;?>

					<?php if(!empty($gun)): ?>
						<div class="info-col">
							<div class="angle-div default-angle-div">USES</div>
							<div class="font-light info-col-text"><?php echo $gun;?></div>
						</div>
					<?php endif;?>

				</div>
			</div>

			<div class="clear"></div>

			<!-- scans -->

			<?php if(sizeof($scansArr) > 0) {?>
				<div class="profile-scans-wrapper">
					<div class="angle-div default-angle-div profile-section-title">SCANS</div>
					
					<div class="my-scans-row">
						<div class="scans-slider">
							<?php
							foreach ($scansArr as $scan) 
							{
								if(isset($scan['image']))
									$imgPath = 'user_images/scan_pictures/thumbs/'.$scan['image'];
								else
									$imgPath = 'user_images/scan_pictures/thumbs/default.jpg';

								?>

								<div>
									<div class="my-scans-item">
										<div class="my-scans-header-wrapper">
											<div class="my-scans-header-text font-semibold"></div>
										</div>
										<a href="scan.php?id=<?php echo $scan['id'];?>" class="scan-link">
											<img data-lazy="<?php echo $imgPath;?>" alt="scan picture">
										</a>
									</div>
								</div>
								

						<?php } //end for?>
						</div>
						
					</div>
				</div>
			<?php } //end if scans > 0 ?>

			<!-- pictures -->

			<?php if(sizeof($picturesArr) > 0) {?>
				<div class="profile-pictures-wrapper">
					<div class="angle-div default-angle-div profile-section-title">PICTURES</div>
					<div class="my-scans-row">
						<div class="pictures-slider">
							<?php
								foreach ($picturesArr as $pic) 
								{
									
									$thumbPath = 'user_images/user_pictures/thumbs/'.$pic['image'];
									$imgPath = 'user_images/user_pictures/'.$pic['image'];
									if(isset($pic['name']))
									{
										$picName = strlen($pic['name']) > 30 ? substr($pic['name'] , 0, 30).'...' : $pic['name'];
									}
									else
									{
										$picName = '';
									}
									$titleText = '';
									if(isset($pic['name']) && !empty($pic['name']) && isset($pic['description']) && !empty($pic['description']))
										$titleText = $pic['name'] . ' - ' . $pic['description'];
									else if(isset($pic['name']) && !empty($pic['name']))
										$titleText = $pic['name'];
									else if(isset($pic['description']) && !empty($pic['description']))
										$titleText = $pic['description'];
									?>

									<div>
										<div class="my-scans-item">
											<div class="my-scans-header-wrapper">
												<div class="my-scans-header-text font-semibold"><?php echo $picName;?></div>
											</div>
											<a class="fancybox-thumbs" data-fancybox-group="thumb" href="<?php echo $imgPath;?>" title="<?php echo $titleText;?>">
									            <img data-lazy="<?php echo $thumbPath;?>" alt="user picture"/>
									        </a>;
											<!-- <a href="#" class="scan-link" style="background-image:url(<?php echo $thumbPath;?>)"></a> -->
										</div>
									</div>
									
									

							<?php } //end for?>
						</div>
					</div>
				</div>
				<br/><br/>
			<?php } //end if pictures > 0 ?>
		</div>
	</div>
</div>

<!-- js templates -->
<div id="scanThumbTemplate" style="display: none">
	<div>
		<div class="my-scans-item">
			<div class="my-scans-header-wrapper">
				<div class="my-scans-header-text font-semibold">SCAN #</div>
			</div>
			<a href="scan.php?id={{scanId}}" class="scan-link">
				<img data-lazy="{{imgPath}}" alt="scan picture">
			</a>
		</div>
	</div>
</div>

<div id="imgThumbTemplate" style="display: none">
	<div>
		<div class="my-scans-item">
			<div class="my-scans-header-wrapper">
				<div class="my-scans-header-text font-semibold">{{imgName}}</div>
			</div>
			<a class="fancybox-thumbs" data-fancybox-group="thumb" href="{{imgPath}}" title="{{titleText}}">
	            <img data-lazy="{{thumbPath}}" alt="user picture"/>
	        </a>
		</div>
	</div>
</div>

<div id="loadingThumbTemplate" style="display: none">
	<div>
		<div class="my-scans-item">
			<div class="my-scans-header-wrapper">
				<div class="my-scans-header-text font-semibold"></div>
			</div>
			<a href="#">
				<div class="loading-thumb"></div>
			</a>
		</div>
	</div>
</div>
<?php include_once('includes/footer-template.php');?>