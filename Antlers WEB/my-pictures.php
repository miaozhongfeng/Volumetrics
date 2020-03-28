<?php
$page = 'social-network';
$styles = '<link rel="stylesheet" type="text/css" href="css/jquery.fancybox.css">
			<link rel="stylesheet" type="text/css" href="css/sn-style.css">';
$scripts = '<script type="text/javascript" src="js/jquery.fancybox.js"></script>
			<script type="text/javascript" src="js/pages/my-pictures.js"></script>';
$headerText	= 'NRA-TA SOCIAL NETWORK';	
session_start();
include_once('includes/header-sn-template.php');
include_once('includes/database-access.php');

$userId = $_SESSION['active_user_id'];
$query = "SELECT id, name, description, image, included_in_trophyroom
			FROM user_pictures AS s
			WHERE user_id = '$userId'
			ORDER BY id DESC
			LIMIT 12";
$result = mysqli_query($config['mysql_con'], $query);

$loadedPics = mysqli_num_rows($result);
?>
<div class="sn-menu-container">
	<?php include_once('partials/sn-menu.php');?>
</div>
<div class="sn-main-container">
	<div class="sn-main-top">
		<div class="arrow-holder">
			<div class="sn-main-arrow" style="left:52px"></div>
			<div class="scores-text" style="left: 85px;">MY PICTURES</div>
		</div>
	</div>
	<div class="sn-scrolable-margin"></div>
	<div class="sn-scrolable">
		<div id="notificationContainer"><?php showSessionNotification();?></div>

		<div class="my-scans-content my-pictures-content">
			<div class="my-scans-row" style="text-align: left; padding-left: 50px; margin-bottom: 0px;">
				<a href="new-scan.php" class="default-btn default-btn-green" id="addPicture">NEW PICTURE</a>
			</div>
			<?php if(mysqli_num_rows($result) > 0 ){?>
			<div class="my-scans-row">
				<?php

					$counter = 0;
					while(list($picId, $name, $desc, $image, $isIncluded) = mysqli_fetch_array($result, MYSQLI_NUM))
					{
						$thumbPath = 'user_images/user_pictures/thumbs/'.$image;
						$imgPath = 'user_images/user_pictures/'.$image;

						$checkedAttr = $isIncluded ? "checked" : "";

						if(isset($name))
						{
							$picName = strlen($name) > 30 ? substr($name , 0, 30).'...' : $name;
						}
						else
						{
							$picName = '';
						}
						$titleText = '';
						if(isset($name) && !empty($name) && isset($desc) && !empty($desc))
							$titleText = $name . ' - ' . $desc;
						else if(isset($name) && !empty($name))
							$titleText = $name;
						else if(isset($desc) && !empty($desc))
							$titleText = $desc;
					?>

					<?php if($counter != 0 && $counter % 3 == 0) :?>
						</div>
						<div class="row-spliter"></div>
						<div class="row-spliter-arrow"></div>
						<div class="my-scans-row">
					<?php endif;?>

					<div class="my-scans-item" id="pic_<?php echo $picId;?>">
						<div class="my-scans-header-wrapper">
							<div class="my-scans-header-text font-semibold">ADD TO TROPHY ROOM</div>
							<div class="checkbox-wrapper">
								<input type="checkbox" id="<?php echo $picId;?>" class="css-checkbox include-scan" <?php echo $checkedAttr;?>/>
								<label for="<?php echo $picId;?>" class="css-label"></label>
							</div>
						</div>
						<div class="link-wrapper">
							<a href="<?php echo $imgPath;?>" class="fancybox-thumbs scan-link" data-fancybox-group="thumb" title="<?php echo $titleText;?>">
								<img src="<?php echo $thumbPath?>">
							</a>
						</div>

						<div class="my-scans-buttons-wrapper">
							<a href="#" class="default-btn default-btn-green edit-pic-btn" data-id="<?php echo $picId;?>">EDIT</a>
							<a href="#" class="default-btn default-btn-red delete-pic-btn" data-id="<?php echo $picId;?>">DELETE</a>
						</div>
					</div>

					<?php $counter++; } ?>
				
			</div><!-- last scan row -->

		<?php } else echo '<h3 class="no-results-green">No pictures</h3>'; ?>

		</div>

		<?php if($loadedPics == 12):?>
			<p class="load-more-btn-holder">
				<a href="#" id="loadMore" class="default-btn default-btn-green">LOAD MORE</a>
				<div id="ajaxLoaderDiv" style="display:none;"></div>
			</p>
			<br/><br/>
		<?php endif;?>

	</div>
</div>

<div id="picThumbTemplate" style="display: none">
	<div class="my-scans-item" id="pic_{{picId}}">
		<div class="my-scans-header-wrapper">
			<div class="my-scans-header-text font-semibold">ADD TO TROPHY ROOM</div>
			<div class="checkbox-wrapper">
				<input type="checkbox" id="{{picId}}" class="css-checkbox include-scan" {{checkedattr}}/>
				<label for="{{picId}}" class="css-label"></label>
			</div>
		</div>
		<div class="link-wrapper">
			<a href="{{imgPath}}" class="fancybox-thumbs scan-link" data-fancybox-group="thumb" title="{{titleText}}">
				<img src="{{thumbPath}}">
			</a>
		</div>

		<div class="my-scans-buttons-wrapper">
			<a href="#" class="default-btn default-btn-green edit-pic-btn" data-id="{{picId}}">EDIT</a>
			<a href="#" class="default-btn default-btn-red delete-pic-btn" data-id="{{picId}}">DELETE</a>
		</div>
	</div>

</div>

<div class="fullscreen-overlay edit-profile-overlay" data-uploading="false"></div>
<?php include_once('includes/footer-template.php');?>