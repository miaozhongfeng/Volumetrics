<?php
$page = 'trophy-room';
$styles = '<link rel="stylesheet" type="text/css" href="css/slick.css">
		   <link rel="stylesheet" type="text/css" href="css/scans.css">
		   ';
$scripts = '<script type="text/javascript" src="js/slick.min.js"></script>';
include_once('includes/database-access.php');
include_once('includes/functions.php');

session_start();


if(!isset($_SESSION['active_user_id']))
	include_once('includes/header-template.php');
else
	include_once('includes/header-sn-template.php');
?>

<?php
//get slide images

$query = "SELECT path FROM trophyroom_slider ORDER BY orderNum";
$result = mysqli_query($config['mysql_con'], $query);

?>
<div class="scans-wrapper my-trophyroom-wrapper">
	<div class="scans-welcome-wrapper">
		<div class="slick-slider" style="margin-bottom:0;">
			<?php 
				//show default image
				if(!$result || mysqli_affected_rows($config['mysql_con']) == 0)
					echo'<div class="trophy-room-cover-img" style="background-image:url(user_images/trophyroom_slider/default-slide.jpg)"></div>';
				//show images from database
				else
				{
					while (list($path) = mysqli_fetch_array($result, MYSQLI_NUM))
					{
						echo'<div class="trophy-room-cover-img" style="background-image:url(user_images/trophyroom_slider/'.$path.')" style="width:100% !important;"></div>';
					}
				}
			?>
		</div>
<?php

//get cms page description
$cmsDescription = '';
$query = "SELECT text FROM cms_content WHERE code = 'trophyroomDesc' LIMIT 1";
$result = mysqli_query($config['mysql_con'], $query);
if($result)
{
	$row = mysqli_fetch_array($result, MYSQLI_NUM);
	$cmsDescription = $row[0];
}


//get categories
$categories = array();
$query = "SELECT id, name FROM trophy_categories ORDER BY orderNum";
$result = mysqli_query($config['mysql_con'], $query);
while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
{
	array_push($categories, $row);
}
?>

		<div class="welcome-container">
			<div class="welcome-text-header">
				<div class="welcome-line"></div>
				<h1>
					Welcome to NRA-TA Trophy Room
				</h1>
			</div>
			<div class="additional-info font-light">
				<?php echo $cmsDescription;?>
			</div>
		</div>
	</div>
	<div class="sn-main-top"></div>
	<div id="notificationContainer"></div>

	<div class="trophy-room-content">
		<?php
		foreach($categories as $cat)
		{
			$query = sprintf("SELECT s.id, sp.image 
				FROM scans AS s
				LEFT JOIN scan_pictures AS sp
				ON s.id = sp.scan_id AND sp.is_cover = '1'
				WHERE s.category_id = %d AND included_in_trophyroom = '1'
				ORDER BY s.id DESC
				LIMIT 5", $cat['id']);

			$result = mysqli_query($config['mysql_con'], $query) or die(mysqli_error($config['mysql_con']));
			if($result && mysqli_num_rows($result) > 0)
			{
				?>

				<div class="trophyroom-category-header">
					<div class="trophyroom-category-text-wrapper">
						<div class="header-line"></div>
						<h1>Top 5 <?php echo $cat['name']?></h1>
					</div>
				</div>
				<div class="trophyroom-category-arrow"></div>
				<div class="trophyroom-items-wrapper">
				<?php
					while(list($scanId, $image) = mysqli_fetch_array($result, MYSQLI_NUM))
					{
						if(isset($image))
							$imgPath = 'user_images/scan_pictures/thumbs/'.$image;
						else
							$imgPath = 'user_images/scan_pictures/thumbs/default.jpg';

						echo '<div class="image-holder"><a href="scan.php?id='.$scanId.'" style="background-image:url('.$imgPath.')" class="scan-link"></a></div>';
					}//end foreach scan in category
				?>
				</div>
				<?php
			}//end if results
		}//end foreach category

		?>




	</div><!-- trophy-room-content -->

</div>
<?php include_once('includes/footer-template.php');?>
<script>

	//slider 
	$('.slick-slider').slick({
		infinite: true,
        placeholders: false,
        speed: 500,
        autoplay: true,
        prevArrow:'<div class="prevArrow"></div>',
	    nextArrow:'<div class="nextArrow"></div>'
	});

	
</script>