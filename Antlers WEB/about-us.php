<?php
$title = 'About Us';
$styles = '<link rel="stylesheet" type="text/css" href="css/about-us.css">';
session_start();

if(!isset($_SESSION['active_user_id']))
	include_once('includes/header-template.php');
else
	include_once('includes/header-sn-template.php');

//get about us image
$query = "SELECT path FROM cms_content WHERE code = 'aboutUsImage' LIMIT 1";
$result = mysqli_query($config['mysql_con'], $query);
if($result)
{
	$row = mysqli_fetch_array($result, MYSQLI_NUM);
	$imgPath = 'cms/media/'.$row[0];
}

//get about us sections
$query = "SELECT sc.title, sc.text 
					FROM cms_subcontent AS sc
					INNER JOIN cms_content AS c
					ON sc.content_id = c.id AND c.code = 'aboutUs'";
$result = mysqli_query($config['mysql_con'], $query);
?>

<div class="wood-header">
	<div class="wood-header-text">
		<div class="wood-header-line wood-header-line-left"></div>
		<h1>
			About Us
		</h1>
		<div class="wood-header-line wood-header-line-right"></div>
	</div>
</div>
<div class="styling-up"></div>

<div class="about-us-container">
	<?php 

	if(isset($imgPath))
	{
		echo '<img src="'.$imgPath.'" alt="Antlers" class="aboutus-top-image"/>';
	}

	if($result)
	{
		while(list($title, $text) = mysqli_fetch_array($result, MYSQLI_NUM))
		{
			?>
				<div class="about-us-header angle-div">
					<?php echo $title; ?>
				</div>
				<div class="about-us-text">
					<?php echo $text; ?>
				</div>
			<?php
		}
	}
	?>
</div>
<br/><br/>

<?php
include_once('includes/footer-template.php');
?>
<script>
	$('.about-us-header').width(parseInt($('.about-us-header').width() + 10) + 'px');
</script>