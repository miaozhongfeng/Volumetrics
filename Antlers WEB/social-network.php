<?php

$page = 'social-network';
$styles = '<link rel="stylesheet" type="text/css" href="css/slick.css">
			<link rel="stylesheet" type="text/css" href="css/sn-style.css">
			<link rel="stylesheet" type="text/css" href="css/jquery.fancybox.css">';
$scripts = '<script type="text/javascript" src="js/sn-load-content.js"></script>
			<script type="text/javascript" src="js/slick.min.js"></script>
			<script type="text/javascript" src="js/autosuggest.js"></script>
			<script type="text/javascript" src="js/jquery.fancybox.js"></script>
			<script src="http://ajax.aspnetcdn.com/ajax/jquery.validate/1.13.0/jquery.validate.js"></script>';			
			
$headerText	= 'NRA-TA SOCIAL NETWORK';		
session_start();

if(!isset($_SESSION['active_user_id']))
	include_once('includes/header-template.php');
else
	include_once('includes/header-sn-template.php');
?>

<div class="sn-menu-container">
	<?php include_once('partials/sn-menu.php');?>
</div>

<div id="sn-ajax-content">
	<!-- here goes the content -->
</div>

<?php include_once('includes/footer-template.php');?>

<script>
	/*$( document ).ready(function() {
		$('.slick-slider').slick({
	        infinite: true,
	        placeholders: false,
	        speed: 300,
	        slidesToShow: 4,
	        touchMove: false,
	        slidesToScroll: 1,
	        autoplay: true,
	        prevArrow:'<div class="prevArrow"></div>',
	        nextArrow:'<div class="nextArrow"></div>'
	    });
	});*/

	function getPaginationContent(link ,ajaxUrl, contentContainer, msgContainer){
		var page = link.attr('href').split('=').pop();
		$.ajax({
			type: "POST",
	        url: ajaxUrl,
	        data: { page: page}
		})
		.success(function(data){
			//success
			if(data)
				$(contentContainer).html(data);
			//error
			else
				showNotification('Sorry, some error occurred. Please refresh the page and try again.','error', msgContainer);
		});
	}
</script>