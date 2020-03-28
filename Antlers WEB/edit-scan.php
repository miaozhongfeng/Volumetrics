<?php
if(!isset($_GET['id']) || !ctype_digit($_GET['id']))
{
	header('Location:my-scans.php');
	exit();
}

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

//get cms page description
$cmsDescription = '';
$query = "SELECT text FROM cms_content WHERE code = 'editScanDesc' LIMIT 1";
$result = mysqli_query($config['mysql_con'], $query);
if($result)
{
	$row = mysqli_fetch_array($result, MYSQLI_NUM);
	$cmsDescription = $row[0];
}

$scanId = $_GET['id'];
$userId = $_SESSION['active_user_id'];

//get scan data
$query = "SELECT category_id, latitude, longitude, location, date, description, weapon, scan FROM scans WHERE id = '$scanId' AND user_id = '$userId' LIMIT 1";
$result = mysqli_query($config['mysql_con'], $query);
if(!$result || mysqli_num_rows($result) == 0)
{
	showErrorAndExit('default');
}

list($categoryId, $lat, $lng, $locationName, $date, $description, $weapon, $scan) = mysqli_fetch_array($result, MYSQLI_NUM);
$formatedDate = formatDate($date);
$scanPath = 'user_images/user_scans/'.$scan;


//get scan pictures
$query = "SELECT id, image, is_cover FROM scan_pictures WHERE scan_id = '$scanId' ORDER BY is_cover DESC, id DESC LIMIT 10";
$result = mysqli_query($config['mysql_con'], $query);
$imagesArr = array();

while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
{
	$imagesArr[] = $row;
	if($row['is_cover'])
		$coverImgSrc = $row['image'];
}

//get categories
$query = "SELECT * FROM trophy_categories ORDER BY orderNum";
$result = mysqli_query($config['mysql_con'], $query);

if(!$result || mysqli_num_rows($result) == 0)
{
	showErrorAndExit('default');
}

$categoriesArr = array();
while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
{
	$categoriesArr[] = $row;
}

$selectedCatName='';
foreach ($categoriesArr as $key => $val) 
{
	if ($val['id'] === $categoryId) 
	{
        $selectedCatName = $val['name'];
        break;
	}
}

?>

<div class="scans-wrapper new-scan-wrapper edit-scan-wrapper">
	<div class="scans-welcome-wrapper">
		<div class="wood-background"></div>
		<div class="welcome-container">
			<div class="welcome-text-header">
				<div class="welcome-line"></div>
				<h1>
					Edit Scan
				</h1>
			</div>
			<div class="additional-info font-light">
				<?php echo $cmsDescription; ?>
			</div>
		</div>
	</div>
	<div class="sn-main-top"></div>
	<div id="notificationContainer"></div>
	<div class="new-scan-content">

		<div class="scan-wrapper content-col">
			<input type="hidden" id="scanPath" value="<?php echo $scanPath;?>">

			<div class="scan-box-bg">
				<div class="scan-box scan-box-loading" id="scan-box"></div>
			</div>

			<div class="scan-hidden-buttons">
				<a href="#" id="fullScreenBtn" class="default-btn default-btn-green">FULLSCREEN</a>
				<a href="#" id="screenshotBtn" class="default-btn default-btn-green">TAKE SCREENSHOT</a>
			</div>
			
			<?php if (isset($coverImgSrc)):?>
				<div class="main-cover-wrapper">
					<div class="default-angle-div angle-div">COVER IMAGE</div>
					<div class="main-cover-box">
						<img id="mainCoverImg" src="user_images/scan_pictures/thumbs/<?php echo $coverImgSrc;?>" />
					</div>
				</div>
			<?php endif;?>
		</div>

		<div class="scan-info-wrapper content-col">
			<form id="editScanInfoForm" action="partials/scans/edit_scan_info.php" method="post">
				<input type="hidden" name="locationName" id="locationName" value="<?php echo $locationName?>"/>
				<input type="hidden" name="lng" id="lng" value="<?php echo $lng;?>"/>
				<input type="hidden" name="lat" id="lat" value="<?php echo $lat;?>"/>
				<input type="hidden" name="scanId" id="scanId" value="<?php echo $scanId;?>"/>


				<div class="default-angle-div angle-div">ADD DESCRIPTION</div>
				<textarea name="description" id="description"><?php echo $description;?></textarea>
				
				<div class="scan-pictures-wrapper">
					<?php
						foreach ($imagesArr as $image)
						{
							$imgPath='user_images/scan_pictures/bullets/'.$image['image'];
							if($image['is_cover'])
							{
								$isCoverHtml = '<a href="#" class="scan-picture-cover scan-picture-cover-active" data-id="'.$image['id'].'" title="Cover image"></a>';
							}
							else
							{
								$isCoverHtml = '<a href="#" class="scan-picture-cover" data-id="'.$image['id'].'" title="Set as cover image"></a>';
							}
							echo '<div class="scan-picture-thumb">
									<img src="'.$imgPath.'"/>
									<a href="#" class="delete-picture-btn" data-id="'.$image['id'].'" title="Delete Picture">x</a>
									'.$isCoverHtml.'
								</div>';
						}
					?>
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
								if($selectedCatName != '')
								{
									echo '<input type="hidden" name="category" value="'.$categoryId.'">';
									echo '<div class="so-header">'.$selectedCatName.'</div>';
								}
								else
								{
									echo '<input type="hidden" name="category" value="">';
									echo '<div class="so-header">Select Category</div>';
								}
								
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
					<input type="text" name="weapon" id="weapon" class="two-cols-item" maxlength="60" value="<?php echo $weapon;?>"/>
				</div>

				<div class="two-cols-wrapper">
					<div class="default-angle-div angle-div two-cols-item scan-label">DATE</div>
					<input type="text" name="date" id="date" class="two-cols-item" value="<?php echo $formatedDate;?>"/>
				</div>
				
				<br/><br/>
				<button class="default-btn">SAVE</button>
			</form>
		</div>
	</div>
	<div class="clear"></div>
</div>

<!-- hidden -->
<div class="fullscreen-overlay screenshot-overlay">
	<div class="screenshot-wrapper">
		<div class="image-holder"></div>
		<div>
			<a href="#" class="default-btn default-btn-green" id="uploadScreenshotBtn">UPLOAD</a>
			<a href="#" class="default-btn default-btn-red" id="deleteScreenshotBtn">DISCARD</a>
		</div>
	</div>
</div>


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

<?php include_once('includes/footer-template.php');?>