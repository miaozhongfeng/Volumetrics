<?php
$page = 'social-network';
$styles = '<link rel="stylesheet" type="text/css" href="css/sn-style.css">';
$scripts = '<script type="text/javascript" src="js/pages/my-scans.js"></script>';
$headerText	= 'NRA-TA SOCIAL NETWORK';	
session_start();
include_once('includes/header-sn-template.php');
include_once('includes/database-access.php');

$userId = $_SESSION['active_user_id'];
$query = "SELECT s.id, s.included_in_trophyroom, sp.image 
			FROM scans AS s
			LEFT JOIN scan_pictures AS sp
			ON s.id = sp.scan_id AND sp.is_cover = '1'
			WHERE s.user_id = $userId
			GROUP BY s.id
			ORDER BY s.id DESC
			LIMIT 12";
$result = mysqli_query($config['mysql_con'], $query);

$loadedScans = mysqli_num_rows($result);
?>
<div class="sn-menu-container">
	<?php include_once('partials/sn-menu.php');?>
</div>
<div class="sn-main-container">
	<div class="sn-main-top">
		<div class="arrow-holder">
			<div class="sn-main-arrow" style="left:52px"></div>
			<div class="scores-text" style="left: 100px;">MY SCANS</div>
		</div>
	</div>
	<div class="sn-scrolable-margin"></div>
	<div class="sn-scrolable">
		<div id="notificationContainer"><?php showSessionNotification();?></div>

		<div class="my-scans-content">
			<div class="my-scans-row" style="text-align: left; padding-left: 50px; margin-bottom: 0px;">
				<a href="new-scan.php" class="default-btn default-btn-green">NEW SCAN</a>
			</div>
			<?php if(mysqli_num_rows($result) > 0 ){?>
			<div class="my-scans-row">
				<?php

					$counter = 0;
					while(list($scanId, $isIncluded, $image) = mysqli_fetch_array($result, MYSQLI_NUM))
					{
						if(isset($image))
							$imgPath = 'user_images/scan_pictures/thumbs/'.$image;
						else
							$imgPath = 'user_images/scan_pictures/thumbs/default.jpg';

						$checkedAttr = $isIncluded ? "checked" : "";
					?>

					<?php if($counter != 0 && $counter % 3 == 0) :?>
						</div>
						<div class="row-spliter"></div>
						<div class="row-spliter-arrow"></div>
						<div class="my-scans-row">
					<?php endif;?>

					<div class="my-scans-item">
						<div class="my-scans-header-wrapper">
							<div class="my-scans-header-text font-semibold">ADD TO TROPHY ROOM</div>
							<div class="checkbox-wrapper">
								<input type="checkbox" id="<?php echo $scanId;?>" class="css-checkbox include-scan" <?php echo $checkedAttr;?>/>
								<label for="<?php echo $scanId;?>" class="css-label"></label>
							</div>
						</div>
						<div class="link-wrapper">
							<a href="scan.php?id=<?php echo $scanId;?>" class="scan-link" style="background-image:url(<?php echo $imgPath;?>)"></a>
						</div>

						<div class="my-scans-buttons-wrapper">
							<a href="edit-scan.php?id=<?php echo $scanId;?>" class="default-btn default-btn-green">EDIT</a>
							<a href="#" class="default-btn default-btn-red delete-scan-btn" data-id="<?php echo $scanId;?>">DELETE</a>
						</div>
					</div>

					<?php $counter++; } ?>
				
			</div><!-- last scan row -->

			

		<?php } else echo '<h3 class="no-results-green">No scans</h3>'; ?>

		</div>
		<?php if($loadedScans == 12):?>
			<p class="load-more-btn-holder">
				<a href="#" id="loadMore" class="default-btn default-btn-green">LOAD MORE</a>
				<div id="ajaxLoaderDiv" style="display:none;"></div>
			</p>
			<br/><br/>
		<?php endif;?>
	</div>
</div>
<div id="scanThumbTemplate" style="display: none;">
	<div class="my-scans-item">
		<div class="my-scans-header-wrapper">
			<div class="my-scans-header-text font-semibold">ADD TO TROPHY ROOM</div>
			<div class="checkbox-wrapper">
				<input type="checkbox" id="{{scanId}}" class="css-checkbox include-scan" {{checkedattr}}/>
				<label for="{{scanId}}" class="css-label"></label>
			</div>
		</div>
		<div class="link-wrapper">
			<a href="scan.php?id={{scanId}}" class="scan-link" style="background-image:url({{imgPath}})"></a>
		</div>

		<div class="my-scans-buttons-wrapper">
			<a href="edit-scan.php?id={{scanId}}" class="default-btn default-btn-green">EDIT</a>
			<a href="#" class="default-btn default-btn-red delete-scan-btn" data-id="{{scanId}}">DELETE</a>
		</div>
	</div>
</div>
<?php include_once('includes/footer-template.php');?>