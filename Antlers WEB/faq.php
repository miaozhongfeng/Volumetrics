<?php

include_once('includes/database-access.php');
include_once('includes/enums.php');


$title = 'FAQ';
$styles = '<link rel="stylesheet" type="text/css" href="css/about-us.css">';
session_start();

if(!isset($_SESSION['active_user_id']))
	include_once('includes/header-template.php');
else
	include_once('includes/header-sn-template.php');

//get faq image
$query = "SELECT path FROM cms_content WHERE code = 'faqImage' LIMIT 1";
$result = mysqli_query($config['mysql_con'], $query);
if($result)
{
	$row = mysqli_fetch_array($result, MYSQLI_NUM);
	$imgPath = 'cms/media/'.$row[0];
}

//get questions
$query = "SELECT sc.title, sc.text 
					FROM cms_subcontent AS sc
					INNER JOIN cms_content AS c
					ON sc.content_id = c.id AND c.code = 'faq'";
$result = mysqli_query($config['mysql_con'], $query);

?>
<div class="wood-header"></div>
<div class="styling-up"></div>

<div class="faq-container">
	<?php 
	if(isset($imgPath))
	{ 
		echo '<img src="'.$imgPath.'" alt="Antlers" class="faq-top-image"/>'; 
	} 

	if($result)
	{
		while(list($question, $answer) = mysqli_fetch_array($result, MYSQLI_NUM))
		{
			?>
				<div class="about-us-header angle-div">
					<?php echo $question; ?>
				</div>
				<div class="faq-answer cmsContentWrapper">
					<?php echo $answer; ?>
				</div>
			<?php
		}
	}

	?>
	
	<!-- <div class="about-us-header angle-div">
		Q: How can I get scan data for my trophy?
	</div>
	<div class="faq-answer cmsContentWrapper">
		A: You can order shipping box for your trophy (go to "Home" -> "Click Here To Get Your Trophy Scanned"). After receiving box 
		you can send your trophy to BigGameDesign. After making scan data for your trophy we will send it back and you will get 
		information about track number for it. By using track number you will be able to load scan data in our web site.
	</div>
	<div class="about-us-header angle-div">
		Q: How can I load my trophy by track number?
	</div>
	<div class="faq-answer ">
		A: You can enter track number on "Home" page and press "Go".
	</div>
	<div class="about-us-header angle-div">
		Q: How can I manage scan data in 3D viewer?
	</div>
	<div class="faq-answer">
		A: It is possible to manipulate 3D object by mouse. Dragging with left mouse button pressed - rotating, with right mouse button 
		- moving. Use mouse scroll for zoom in/zoom out and button at the right top to refit object.
	</div> -->

		
</div>
<br><br>
<?php
include_once('includes/footer-template.php');
?>
<script>
	$('.about-us-header').width(parseInt($('.about-us-header').width() + 10) + 'px');
</script>