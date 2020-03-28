<?php

$styles = '<link rel="stylesheet" type="text/css" href="css/scans.css">
			<link rel="stylesheet" type="text/css" href="css/sn-style.css">
			<link rel="stylesheet" type="text/css" href="css/slick.css"/>
			<link rel="stylesheet" type="text/css" href="css/jquery.fancybox.css">';
$scripts = '<script type="text/javascript" src="js/slick.min.js"></script>
			<script type="text/javascript" src="js/jquery.fancybox.js"></script>
			<script type="text/javascript" src="js/pages/my-trophyroom.js"></script>
			<script type="text/javascript" src="js/pages/my-pictures.js"></script>';




session_start();

if(!isset($_SESSION['active_user_id']))
	include_once('includes/header-template.php');
else
	include_once('includes/header-sn-template.php');

include_once('includes/database-access.php');

$myTrophyroom = false;
if(isset($_GET['id']) && ctype_digit($_GET['id']))
{
	$userId = $_GET['id'];
	if(isset($_SESSION['active_user_id']) && $_SESSION['active_user_id'] == $userId)
		$myTrophyroom = true;
}
else if(isset($_SESSION['active_user_id']))
{
	$userId = $_SESSION['active_user_id'];
	$myTrophyroom = true;
}
else
{
	header('Location:login.php?redirectUrl=my-trophyroom.php');
}

//get cms page description
$cmsDescription = '';
$query = "SELECT text FROM cms_content WHERE code = 'myTrophyroomDesc' LIMIT 1";
$result = mysqli_query($config['mysql_con'], $query);
if($result)
{
	$row = mysqli_fetch_array($result, MYSQLI_NUM);
	$cmsDescription = $row[0];
}

//get scan count
$query = "SELECT COUNT(id) FROM scans WHERE user_id = '$userId' AND included_in_trophyroom = '1'";
$result = mysqli_query($config['mysql_con'], $query);
$row = mysqli_fetch_array($result, MYSQLI_NUM);
$totalScans = $row[0];

//get scans
$query = "SELECT s.id, s.location, s.weapon, sp.image 
			FROM scans AS s
			LEFT JOIN scan_pictures AS sp
			ON s.id = sp.scan_id AND sp.is_cover = '1'
			WHERE s.user_id = '$userId' AND s.included_in_trophyroom = '1'
			GROUP BY s.id
			ORDER BY s.id DESC";

$result = mysqli_query($config['mysql_con'], $query);

$scansArr = array();
if($result)
{
	while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
	{
		array_push($scansArr, $row);
	}
}

$query = "SELECT name, description, image FROM user_pictures WHERE user_id = '$userId' AND included_in_trophyroom = '1'";		
$result = mysqli_query($config['mysql_con'], $query);

$picturesArr = array();
if($result)
{
	while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
	{
		array_push($picturesArr, $row);
	}
}
?>
<div class="my-trophyroom-wrapper">
	<div class="scans-welcome-wrapper">
		<div class="wood-background"></div>
		<div class="welcome-container">
			<div class="welcome-text-header">
				<div class="welcome-line"></div>
				<h1>
					Welcome to My Trophy Room
				</h1>
			</div>
			<div class="additional-info font-light">
				<?php if($myTrophyroom):?>
					<?php echo $cmsDescription;?>
				<?php endif;?>
			</div>
		</div>
	</div>
	<div class="sn-main-top"></div>
	<div id="notificationContainer"></div>

	<div class="my-trophyroom-content">

		<?php if($myTrophyroom):?>

		<div class="buttons-holder">
			<a href="new-scan.php" class="default-btn default-btn-green">NEW SCAN</a>
			<a href="#" id="addPicture" class="default-btn default-btn-green">NEW PICTURE</a>
		</div>

		<?php endif;?>

		<?php if($totalScans > 0) { ?>

		<input type="hidden" id="totalScans" value="<?php echo $totalScans;?>" />
		<input type="hidden" id="userId" value="<?php echo $userId;?>" />
		<div class="my-trophyroom-slider-wrapper">

			<?php
				$sliderClass = $totalScans > 3 ? "scans-slider" : "no-slider no-slider-".$totalScans.'-items'; 
			?>
			<div class="slider <?php echo $sliderClass;?>">
				<?php
					foreach ($scansArr as $scan) 
					{
						if(isset($scan['image']))
							$imgPath = 'user_images/scan_pictures/thumbs/'.$scan['image'];
						else
							$imgPath = 'user_images/scan_pictures/thumbs/default.jpg';

						if(!empty($scan['weapon']) && strlen($scan['weapon']) > 25)
							$scan['weapon'] = substr($scan['weapon'], 0, 25).'...';

						$locationName = '';
						if(!empty($scan['location']))
						{
							$locArr = explode(",", $scan['location']);
							if(sizeof($locArr) > 0)
							{
								$locationName = strlen($locArr[0]) > 25 ? substr($locArr[0], 0, 25).'...' : $locArr[0];
							}
						}
							
						?>

						<div class="item-holder">
							<div class="trophy-item">
								<div class="image-holder">
									<a href="scan.php?id=<?php echo $scan['id']?>" style="background-image:url(<?php echo $imgPath;?>)" class="scan-link"></a>
								</div>

								<div class="info-wrapper">
									<div class="spliter-arrow"></div>
									<div class="info-details-wrapper">

										<?php if(!empty($locationName)):?>
											<div class="info-row">
												<div class="angle-div default-angle-div">LOCATION</div>
												<div class="info-value"><?php echo $locationName;?></div>
											</div>
										<?php endif;?>

										<?php if(!empty($scan['weapon'])):?>
											<div class="info-row">
												<div class="angle-div default-angle-div">GUN</div>
												<div class="info-value"><?php echo $scan['weapon'];?></div>
											</div>
										<?php endif;?>
									</div>
								</div>
							</div>
						</div>
				<?php } // end while?>

			</div>
		</div>

	</div>
	<?php } //end if results > 0
	else echo '<h3 class="no-results-green">No Trophies</h3>';
	?>

	<?php if(sizeof($picturesArr) > 0) {?>
		<div class="trophyroom-category-header">
			<div class="trophyroom-category-text-wrapper">
				<div class="header-line"></div>
				<h1>My Pictures</h1>
			</div>
		</div>
		<div class="trophyroom-category-arrow"></div>


		<div class="my-trophyroom-pictures-slider-wrapper">
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
							<div class="picture-container">
								<div class="image-holder my-pictures-holder">
									<a class="fancybox-thumbs" data-fancybox-group="thumb" href="<?php echo $imgPath;?>" title="<?php echo $titleText;?>">
							            <img src="<?php echo $thumbPath;?>" alt="user picture"/>
							        </a>;
									<!-- <a href="#" class="scan-link" style="background-image:url(<?php echo $thumbPath;?>)"></a> -->
								</div>
							</div>
						</div>
				<?php } //end for?>
			</div>
		</div>
		<br/><br/>
	<?php } //end if pictures > 0 ?>


</div>

<!-- js templates -->
<div class="fullscreen-overlay edit-profile-overlay" data-uploading="false"></div>
<div id="scanThumbTemplate" style="display: none">
	<div class="item-holder slick-active">
		<div class="trophy-item">
			<div class="image-holder">
				<a href="scan.php?id={{scanId}}" style="background-image:url({{imgPath}})" class="scan-link"></a>
			</div>

			<div class="info-wrapper">
				<div class="spliter-arrow"></div>
				<div class="info-details-wrapper">
					<div class="info-row">
						<div class="angle-div default-angle-div">LOCATION</div>
						<div class="info-value">{{location}}</div>
					</div>

					<div class="info-row">
						<div class="angle-div default-angle-div">GUN</div>
						<div class="info-value">{{gun}}</div>
					</div>
				</div>
			</div>
		</div>
	</div>
</div>

<div id="loadingThumbTemplate" style="display: none">
	<div class="loadingThumb">
		<div class="trophy-item"></div>
	</div>
</div>

<?php include_once('includes/footer-template.php');?>
