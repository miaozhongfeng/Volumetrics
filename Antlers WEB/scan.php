<?php
if(!isset($_GET['id']) || !ctype_digit($_GET['id']))
{
	header('Location:index.php');
	exit();
}

$styles = '<link rel="stylesheet" type="text/css" href="css/scans.css">
			<link rel="stylesheet" type="text/css" href="css/jquery.fancybox.css">';
$scripts = '<script src="https://maps.googleapis.com/maps/api/js?v=3.exp&libraries=places"></script>
			<script src="threeJS/three.min.js"></script>
			<script src="threeJS/STLLoader.js"></script>
			<script src="threeJS/TrackballControls.js"></script>
			<script src="threeJS/Detector.js"></script>
			<script type="text/javascript" src="threeJS/display_stl.js"></script>
			<script type="text/javascript" src="js/jquery.fancybox.js"></script>
			<script type="text/javascript" src="js/pages/view-scan.js"></script>
			';

session_start();

include_once('includes/database-access.php');
include_once('includes/functions.php');

//get cms page description
$cmsDescription = '';
$query = "SELECT text FROM cms_content WHERE code = 'scanDetailsDesc' LIMIT 1";
$result = mysqli_query($config['mysql_con'], $query);
if($result)
{
	$row = mysqli_fetch_array($result, MYSQLI_NUM);
	$cmsDescription = $row[0];
}

$scanId = $_GET['id'];

//get scan data
$query = "SELECT s.id, s.latitude, s.longitude, s.location, s.date, s.description, s.weapon, s.scan, s.user_id, tc.name, u.firstname, u.lastname
			FROM scans AS s
			LEFT OUTER JOIN trophy_categories AS tc
			ON tc.id = s.category_id
			INNER JOIN registered_users AS u
			ON s.user_id = u.id
			WHERE s.id = '$scanId' LIMIT 1";
$result = mysqli_query($config['mysql_con'], $query);
if(!$result || mysqli_num_rows($result) == 0)
{
	$dbError = 'default';
}

list($scanId, $lat, $lng, $locationName, $date, $description, $weapon, $scan, $userId, $categoryName, $ownerFirstName, $ownerLastName) = mysqli_fetch_array($result, MYSQLI_NUM);
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

// open graph meta
$currentUrl = "http://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
$metaImgPath = isset($coverImgSrc) ? "http://$_SERVER[HTTP_HOST]/user_images/scan_pictures/thumbs/$coverImgSrc"
	: "http://$_SERVER[HTTP_HOST]/user_images/scan_pictures/thumbs/default.jpg";

$meta = '<meta property="og:url" content="'.$currentUrl.'" />
		 <meta property="og:title" content="Scan Title" />
		 <meta property="og:description" content="Scan description text" />
		 <meta property="og:image" content="'.$metaImgPath.'" />
		 <meta name="twitter:card" content="summary" />
		 <meta name="twitter:title" content="Scan Title" />
		 <meta name="twitter:description" content="Scan description text" />
		 <meta name="twitter:url" content="'.$currentUrl.'" />
		 <meta name="twitter:image" content="'.$metaImgPath.'" />';

	
//call template
if(!isset($_SESSION['active_user_id']))
	include_once('includes/header-template.php');
else
	include_once('includes/header-sn-template.php');
	
if(isset($dbError))
{
	showErrorAndExit($dbError);
}
?>

<div class="scans-wrapper view-scan-wrapper">
	<div class="scans-welcome-wrapper">
		<div class="wood-background"></div>
		<div class="welcome-container">
			<div class="welcome-text-header">
				<div class="welcome-line"></div>
				<h1>
					Scan Details
				</h1>
			</div>
			<div class="additional-info font-light">
				<?php echo $cmsDescription;?>
			</div>
		</div>
	</div>
	<div class="sn-main-top"></div>
	<div id="notificationContainer"></div>
	<div class="new-scan-content view-scan-content">

		<div class="scan-wrapper content-col">
			<input type="hidden" id="scanPath" value="<?php echo $scanPath;?>">

			<div class="scan-box-bg">
				<div class="scan-box scan-box-loading" id="scan-box"></div>
			</div>
			
			<div class="scan-hidden-buttons">
				<a href="#" id="fullScreenBtn" class="default-btn default-btn-green">FULLSCREEN</a>
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

		<div class="scan-info-wrapper content-col scan-info-col">

			<?php if(isset($_SESSION['active_user_id']) && $_SESSION['active_user_id'] == $userId): ?>
				<div class="scan-btn-holder">
					<a href="edit-scan.php?id=<?php echo $scanId?>" class="default-btn default-btn-green">EDIT SCAN</a>
					<a href="partials/scans/delete_scan.php?scanId=<?php echo $scanId?>" class="default-btn default-btn-red" onclick="return confirm('Are you sure you want to delete this scan?')">DELETE SCAN</a>
				</div>
			<?php endif; ?>

			<div class="field-row">
					<div class="default-angle-div angle-div">OWNER</div>
					<div class="scan-field">
						<a href="profile.php?id=<?php echo $userId;?>" class="green-link" title="<?php echo "$ownerFirstName $ownerLastName's profile";?>">
							<?php echo $ownerFirstName.' '.$ownerLastName?>
						</a>
					</div>
				</div>

			<?php if(!empty($description)): ?>
				<div class="field-row">
					<div class="scan-field description-scan-field">
						<?php echo $description?>
					</div>
				</div>
			<?php endif; ?>

			<?php if(sizeof($imagesArr) > 0): ?>
				<div class="scan-pictures-wrapper field-row">
					<?php
						foreach ($imagesArr as $image)
						{
							$imgPath='user_images/scan_pictures/'.$image['image'];
							$thumbPath='user_images/scan_pictures/bullets/'.$image['image'];
							echo '<a class="fancybox-thumbs" data-fancybox-group="thumb" href="'.$imgPath.'">
							            <img src="'.$thumbPath.'" alt="antlers scan"/>
							        </a>';
						}
					?>
				</div>
			<?php endif; ?>

			<?php if(!empty($lng) && !empty($lat)): ?>
				<input type="hidden" name="lng" id="lng" value="<?php echo $lng;?>"/>
				<input type="hidden" name="lat" id="lat" value="<?php echo $lat;?>"/>
				<div class="field-row">
					<div class="map-label">
						<?php echo $locationName;?>
					</div>
					<div id="map"></div>
				</div>
			<?php endif; ?>
			
			<?php if(!empty($categoryName)): ?>
				<div class="field-row">
					<div class="default-angle-div angle-div">CATEGORY</div>
					<div class="scan-field"><?php echo $categoryName?></div>
				</div>
			<?php endif; ?>

			<?php if(!empty($weapon)): ?>
				<div class="field-row">
					<div class="default-angle-div angle-div">WEAPON USED</div>
					<div class="scan-field"><?php echo $weapon?></div>
				</div>
			<?php endif; ?>

			<?php if(!empty($formatedDate)): ?>
				<div class="field-row">
					<div class="default-angle-div angle-div">DATE</div>
					<div class="scan-field"><?php echo $formatedDate?></div>
				</div>
			<?php endif; ?>
			
			<!-- social share links--> 
			<div class="scan-social-links-wrapper">
				<a href="http://twitter.com/share" class="twitter-share-button" data-url="<?php echo "$currentUrl;"; ?>" data-text="this is the custom text." data-count="horizontal" data-via="nrataTest">Tweet</a>
				<div class="fb-share-button" data-href="<?php echo $currentUrl;?>" data-layout="button_count"></div>
				<div id="fb-root"></div>
			</div>
		</div>
	</div>
	<div class="clear"></div>
</div>
<script type="text/javascript" src="http://platform.twitter.com/widgets.js"></script>
<script>(function(d, s, id) {
	  var js, fjs = d.getElementsByTagName(s)[0];
	  if (d.getElementById(id)) return;
	  js = d.createElement(s); js.id = id;
	  js.src = "//connect.facebook.net/en_US/sdk.js#xfbml=1&version=v2.3";
	  fjs.parentNode.insertBefore(js, fjs);
	}(document, 'script', 'facebook-jssdk'));
</script>

<?php include_once('includes/footer-template.php');?>