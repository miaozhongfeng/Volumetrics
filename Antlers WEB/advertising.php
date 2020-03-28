<?php
$title = '';
$styles = '<link rel="stylesheet" type="text/css" href="css/about-us.css">';
session_start();

include_once('includes/database-access.php');
include_once('includes/enums.php');

if(!isset($_SESSION['active_user_id']))
	include_once('includes/header-template.php');
else
	include_once('includes/header-sn-template.php');

$contactMail = 'admin@nrata.com';//default contact email
$cmsContents = array();
$query = sprintf("SELECT code, text FROM cms_content WHERE section_id = %d", CmsSections::Advertisement);
$result = mysqli_query($config['mysql_con'], $query);
if($result)
{
	while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
	$cmsContents[$row['code']] = $row;
}

if(array_key_exists('advContactMail', $cmsContents))
{
	$contactMail = $cmsContents['advContactMail']['text'];
}
?>

<div class="wood-header advertising-header">
	<div class="wood-header-text">
		<div class="wood-header-line wood-header-line-left"></div>
		<h1>
			Advertisement
		</h1>
		<div class="wood-header-line wood-header-line-right"></div>
	</div>
</div>
<div class="styling-up"></div>

<div class="advertising-container">
	<!-- If you are interested in advertisement on this website please contact us at: -->
	<?php if(array_key_exists('advMainText', $cmsContents)) echo $cmsContents['advMainText']['text'];?>
	<div class="green-area">
		<a href="mailto:<?php echo $contactMail;?>"><?php echo $contactMail;?></a>
	</div>	
</div>


<?php
include_once('includes/footer-template.php');
?>
