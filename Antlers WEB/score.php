<?php
$page = 'score';
$styles = ' <link rel="stylesheet" href="scrolbar/jquery.mCustomScrollbar.css" />
			<link rel="stylesheet" type="text/css" href="css/score-style.css">	
			<link rel="stylesheet" type="text/css" href="css/video-player.css">';
$scripts = '<script type="text/javascript" src="js/score-load-content.js"></script>';

session_start();

if(!isset($_SESSION['active_user_id']))
	include_once('includes/header-template.php');
else
	include_once('includes/header-sn-template.php');

?>
	<div class="background-wood-cover">
		<div class="background-wood-cover-content">
			
			<div id="ajax-content"></div>

			<div class="middle-gray-area-up">
				<div class="middle-gray-area-up-content">
					<a href="#" class="green-score-btn" data-url="partials/score-home.php" data-id="home"><div class="btn-symbol-normal"></div>Scoring Technology</a>
					<a href="#" class="green-score-btn" data-url="partials/score-run-software.php" data-id="run"><div class="btn-symbol-normal"></div>Run Software</a>
					<a href="scoreTrack.php" class="green-score-btn" data-url="partials/score-track.php" data-id="track"><div class="btn-symbol-normal"></div>Track your score</a>
					<a href="scoreEmail.php" class="green-score-btn" data-url="partials/score-email.php" data-id="email"><div class="btn-symbol-normal"></div>E-mail the score</a>
				</div>
			</div>
		</div>
	</div>

<?php include_once('includes/footer-template.php')?>