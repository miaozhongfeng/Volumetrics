<?php
# main pages footer template 

//get footer text
$footerText = '';
$query = "SELECT text FROM cms_content WHERE code = 'footerText' LIMIT 1";
$result = mysqli_query($config['mysql_con'], $query);
if($result)
{
	$row = mysqli_fetch_array($result, MYSQLI_NUM);
	$footerText = $row[0];
}

$scripts = isset($scripts)? $scripts : '';
?>

	<div class="footer-affiliates">
		<div class="affiliates-content">
			<div style="padding-left:75px;">AFFILIATES</div>
			<div class="affiliates-list">
				<a href="#" id="affiliate1"></a>
				<a href="#" id="affiliate2"></a>
				<a href="#" id="affiliate3"></a>
				<a href="#" id="affiliate4"></a>
				<a href="#" id="affiliate5"></a>
				<a href="#" id="affiliate6"></a>
			</div>	
		</div>
	</div>
	<div style="text-align: center">
		<div class="footer-links">
			<div class="footer-links-subdiv">
				<a href="index.php" style="border:none;">HOME</a>
				<a href="score.php">SCORING TECHNOLOGY</a>			
				<a href="score.php#track">TRACK YOUR SCORE</a>				
				<a href="about-us.php">ABOUT US</a>			
				<a href="faq.php">FAQ</a>			
				<a href="contact.php">CONTACT NRA-TA</a>			
				<a href="advertising.php">ADVERTISING</a>
			</div>
		</div>
	</div>
	<div class="bottom-footer">
		<div class="footer-green-area">
			<div class="footer-cms-text">
				<?php echo $footerText;?>
			</div>
			
			<!-- To have your trophy analyzed, take your cleaned and dried game-animal trophies to a NRA-TA official recording station in your area. Find A Scoring Station <br />NRA-TA is produced by Krien Method, Inc. -->
		</div>
		<div class="footer-arrow"></div>
	</div>

	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.7.1/jquery.min.js"></script>
	<script src="js/notifications.js"></script>
	<script>
		//header left margin fix
		leftMarginFix();

		$(window).resize(function(){
			leftMarginFix();
		})
		function leftMarginFix(){
			if(($(window).width()) < 900)
			{
				$('.menu-container').addClass('left-margin-fix');
				$('.under-header-content').addClass('left-margin-fix');
			}
			else
			{
				$('.menu-container').removeClass('left-margin-fix');
				$('.under-header-content').removeClass('left-margin-fix');
			}
		}	
	</script>
	<?php echo $scripts;?>


	<?php
		//update online_users
		if(isset($_SESSION['active_user_id']))
		{
			?>
				<script>
					$.ajax({url:'includes/online_users_util.php'});
					setInterval(function(){$.ajax({url:'includes/online_users_util.php'});}, 600*1000);
				</script>
			<?php
		}
	?>
</body>
</html>	