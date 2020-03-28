<?php
$styles = '<link rel="stylesheet" type="text/css" href="css/scans.css">
		   <link rel="stylesheet" type="text/css" href="css/jquery.minical.css">';
$scripts = '<script src="https://maps.googleapis.com/maps/api/js?v=3.exp&libraries=places"></script>
			<script src="threeJS/three.min.js"></script>
			<script src="threeJS/STLLoader.js"></script>
			<script src="threeJS/TrackballControls.js"></script>
			<script src="threeJS/Detector.js"></script>
			<script type="text/javascript" src="threeJS/display_stl.js"></script>
			<script src="js/jquery.minical.js"></script>
			<script type="text/javascript" src="js/so-widget.js"></script>
			<script type="text/javascript" src="js/pages/new-scan.js"></script>';




session_start();
include_once('includes/header-sn-template.php');
include_once('includes/database-access.php');

//get categories
$query = "SELECT * FROM trophy_categories ORDER BY orderNum";
$result = mysqli_query($config['mysql_con'], $query);
$categoriesArr = array();
if(!$result || mysqli_num_rows($result) == 0)
{
	showErrorAndExit('default');
}

while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
array_push($categoriesArr, $row);
?>

<div class="scans-wrapper new-scan-wrapper">
	<div class="scans-welcome-wrapper">
		<div class="wood-background"></div>
		<div class="welcome-container">
			<div class="welcome-text-header">
				<div class="welcome-line"></div>
				<h1>
					Upload a new Scan
				</h1>
			</div>
			<div class="additional-info font-light">
				After scanning your trophy, enter as many of the data fields as you can for customizable record keeping. 
			</div>
		</div>
	</div>
	<div class="sn-main-top"></div>
	<div id="notificationContainer"></div>
	<div class="new-scan-content">

		<div class="scan-wrapper content-col">
			<form id="scanForm" method="post" enctype="multipart/form-data">
				<input type="file" name="uploadFile" id="scanFile" style="display: none;" />
			</form>
			<input type="hidden" id="scanPath">
			
			<div class="scan-box-bg">
				<div class="scan-box scan-box-loading" id="scan-box">
					<img src="images/add-scans-default-thumb.jpg">
					<div id="uploadScanBtn">UPLOAD A NEW SCAN</div>
				</div>
			</div>
			
			<div class="scan-hidden-buttons">
				<a href="#" id="fullScreenBtn" class="default-btn default-btn-green">FULLSCREEN</a>
				<a href="#" id="screenshotBtn" class="default-btn default-btn-green">TAKE SCREENSHOT</a>
			</div>

			<div class="main-cover-wrapper" style="display: none;">
				<div class="default-angle-div angle-div">COVER IMAGE</div>
				<div class="main-cover-box">
					<img id="mainCoverImg" src="user_images/scan_pictures/thumbs/<?php echo $coverImgSrc;?>" />
				</div>
			</div>
		</div>

		<div class="scan-info-wrapper content-col">
			<form id="scanInfoForm" action="partials/scans/save_scan_info.php" method="post">
				<input type="hidden" name="locationName" id="locationName" />
				<input type="hidden" name="lng" id="lng" />
				<input type="hidden" name="lat" id="lat" />
				<input type="hidden" name="scanId" id="scanId" />


				<div class="default-angle-div angle-div">ADD DESCRIPTION</div>
				<textarea name="description" id="description"></textarea>
				
				<div class="scan-pictures-wrapper">
					<div class="scan-picture-thumb" id="addPictureBtn">NEW PICTURE</div>
				</div>

				<div class="default-angle-div angle-div">PIN IT ON THE MAP</div>
				<input type="text" id="mapAutocomplete" />
				<div id="map"></div>

				<div class="so-container">
					<div class="default-angle-div angle-div">CHOOSE CATEGORY</div>
					<div class="so-widget">	
						<?php
							if(sizeof($categoriesArr ) > 0)
							{
								echo '<input type="hidden" name="category" id="category" value="">';
								echo '<div class="so-header">Select Category</div>';
								echo '<ul>';
								
								foreach ($categoriesArr as $category)
								{
									echo '<li data-val="'.$category['id'].'">'.$category['name'].'</li>';
								}

								echo '</ul>';
							}
							
						?>
					</div>
				</div>

				<div class="two-cols-wrapper">
					<div class="default-angle-div angle-div two-cols-item scan-label">WEAPON USED</div>
					<input type="text" name="weapon" id="weapon" class="two-cols-item" maxlength="60"/>
				</div>

				<div class="two-cols-wrapper">
					<div class="default-angle-div angle-div two-cols-item scan-label">DATE</div>
					<input type="text" name="date" id="date" class="two-cols-item" />
				</div>
				
				<br/><br/>
				<button class="default-btn">SAVE</button>
			</form>
		</div>
	</div>
	<div class="clear"></div>
</div>

<!-- hidden -->

<form id="scanPictureForm">
	<input type="file" name="uploadFile" id="pictureFile" accept="image/png, image/jpeg, image/jpg, image/gif" style="display: none;"/>
</form>

<div id="thumbTemplate" style="display: none">
	<div class="scan-picture-thumb">
		{{imgSrc}}
		<a href="#" class="delete-picture-btn" data-id="{{imgId}}" title="Delete Picture">x</a>
		<a href="#" class="scan-picture-cover {{isCoverClass}}" data-id="{{imgId}}" title="{{coverText}}"></a>
	</div>
</div>

<div id="screenshotTemplate" style="display: none">
	<div class="scan-picture-thumb">
		{{imgSrc}}
	</div>
</div>

<div class="fullscreen-overlay progress-overlay">
	<div class="upload-progress-wrapper">
		<div class="upload-progress-label">Uploading scan</div>
		<div class="upload-progress-background">
			<div class="upload-progress-percent"></div>
		</div>
	</div>
</div>

<div class="fullscreen-overlay screenshot-overlay">
	<div class="screenshot-wrapper">
		<div class="image-holder"></div>
		<div>
			<a href="#" class="default-btn default-btn-green" id="uploadScreenshotBtn">UPLOAD</a>
			<a href="#" class="default-btn default-btn-red" id="deleteScreenshotBtn">DISCARD</a>
		</div>
	</div>
</div>

<?php include_once('includes/footer-template.php');?>