<?php

include_once('includes/database-access.php');
include_once('includes/enums.php');

$page = 'index';
$styles = '<link rel="stylesheet" type="text/css" href="css/index.css">';

session_start();

if(!isset($_SESSION['active_user_id']))
	include_once('includes/header-template.php');
else
	include_once('includes/header-sn-template.php');

//get cms content - all home page content items + footer
$cmsContents = array();
$query = sprintf("SELECT code, title, text, path FROM cms_content WHERE section_id = '%d' || code = 'footerText'", CmsSections::HomePage);
$result = mysqli_query($config['mysql_con'], $query);
if($result)
{
	while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
	{
		$cmsContents[$row['code']] = $row;
	}
}
//print_r($cmsContents['homeVideo']);

?>

	<div class="home-background-top-container">
		<div class="video-container">
			<?php if(array_key_exists('homeVideo', $cmsContents)){
				$videoPath = 'cms/media/'.$cmsContents['homeVideo']['path'];

				$videScreenshot = '';
				if(array_key_exists('homeVideoScreenshot', $cmsContents)){
					$videScreenshot = 'cms/media/'.$cmsContents['homeVideoScreenshot']['path'];
				}
			?>
				<video id="bgVideo" autoplay loop <?php if(!empty($videScreenshot)) echo'poster="'.$videScreenshot.'"';?>>
		      		<source src="<?php echo $videoPath;?>" type="video/mp4">
		      		<?php if(!empty($videScreenshot)) echo'<img src="'.$videScreenshot.'" />';?>
		    	</video>
		    <?php } ?>
	    	<?php if(isset($_SESSION['active_user_id'])){?>
	    	<a href="my-scans.php" class="swf-button-container" style="left:150px;">
	    	<?php } else { ?>
	    	<a href="login.php" class="swf-button-container" style="left:150px;">
	    	<?php }?>

				<div class="button-off"></div>
				<div class="flashContent">
					<object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" width="400" height="400" id="Untitled-1" align="middle">
						<param name="movie" value="Untitled-1.swf" />
						<param name="quality" value="best" />
						<param name="bgcolor" value="#ffffff" />
						<param name="play" value="true" />
						<param name="loop" value="true" />
						<param name="wmode" value="transparent" />
						<param name="scale" value="showall" />
						<param name="menu" value="true" />
						<param name="devicefont" value="false" />
						<param name="salign" value="" />
						<param name="allowScriptAccess" value="sameDomain" />
						<!--[if !IE]>-->
						<object type="application/x-shockwave-flash" data="images/rottating_btn.swf" width="285" height="285">
							<param name="movie" value="Untitled-1.swf" />
							<param name="quality" value="best" />
							<param name="bgcolor" value="#ffffff" />
							<param name="play" value="true" />
							<param name="loop" value="true" />
							<param name="wmode" value="transparent" />
							<param name="scale" value="showall" />
							<param name="menu" value="true" />
							<param name="devicefont" value="false" />
							<param name="salign" value="" />
							<param name="allowScriptAccess" value="sameDomain" />
						<!--<![endif]-->
							<div class="no-flash"></div>
						<!--[if !IE]>-->
						</object>
						<!--<![endif]-->
					</object>
				</div>

				<?php if(isset($_SESSION['active_user_id'])){?>
		    	<div class="swf-btn-text" id="btn-my-scans"></div>
		    	<?php } else { ?>
		    	<div class="swf-btn-text" id="btn-login"></div>
		    	<?php }?>
				
			</a>

			<a href="score.php" class="swf-button-container" style="left:465px;">
				<div class="button-off"></div>
				<div class="flashContent">
					<object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" width="400" height="400" id="Untitled-1" align="middle">
						<param name="movie" value="Untitled-1.swf" />
						<param name="quality" value="best" />
						<param name="bgcolor" value="#ffffff" />
						<param name="play" value="true" />
						<param name="loop" value="true" />
						<param name="wmode" value="transparent" />
						<param name="scale" value="showall" />
						<param name="menu" value="true" />
						<param name="devicefont" value="false" />
						<param name="salign" value="" />
						<param name="allowScriptAccess" value="sameDomain" />
						<!--[if !IE]>-->
						<object type="application/x-shockwave-flash" data="images/rottating_btn.swf" width="285" height="285">
							<param name="movie" value="Untitled-1.swf" />
							<param name="quality" value="best" />
							<param name="bgcolor" value="#ffffff" />
							<param name="play" value="true" />
							<param name="loop" value="true" />
							<param name="wmode" value="transparent" />
							<param name="scale" value="showall" />
							<param name="menu" value="true" />
							<param name="devicefont" value="false" />
							<param name="salign" value="" />
							<param name="allowScriptAccess" value="sameDomain" />
						<!--<![endif]-->
							<div class="no-flash"></div>
						<!--[if !IE]>-->
						</object>
						<!--<![endif]-->
					</object>
				</div>

				<div class="swf-btn-text" id="btn-score"></div>
			</a>

			<a href="social-network.php" class="swf-button-container" style="left:780px;">
				<div class="button-off"></div>
				<div class="flashContent">
					<object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" width="400" height="400" id="Untitled-1" align="middle">
						<param name="movie" value="Untitled-1.swf" />
						<param name="quality" value="best" />
						<param name="bgcolor" value="#ffffff" />
						<param name="play" value="true" />
						<param name="loop" value="true" />
						<param name="wmode" value="transparent" />
						<param name="scale" value="showall" />
						<param name="menu" value="true" />
						<param name="devicefont" value="false" />
						<param name="salign" value="" />
						<param name="allowScriptAccess" value="sameDomain" />
						<!--[if !IE]>-->
						<object type="application/x-shockwave-flash" data="images/rottating_btn.swf" width="285" height="285">
							<param name="movie" value="Untitled-1.swf" />
							<param name="quality" value="best" />
							<param name="bgcolor" value="#ffffff" />
							<param name="play" value="true" />
							<param name="loop" value="true" />
							<param name="wmode" value="transparent" />
							<param name="scale" value="showall" />
							<param name="menu" value="true" />
							<param name="devicefont" value="false" />
							<param name="salign" value="" />
							<param name="allowScriptAccess" value="sameDomain" />
						<!--<![endif]-->
							<div class="no-flash"></div>
						<!--[if !IE]>-->
						</object>
						<!--<![endif]-->
					</object>
				</div>

				<div class="swf-btn-text" id="btn-share"></div>
			</a>

	    	<div class="video-bottom">
	    		<div class="video-text-arrow"></div>
	    		<div class="video-text-container">
		    		<div>
		    		<?php if(array_key_exists('homeVideoText', $cmsContents)) echo $cmsContents['homeVideoText']['text'];?>
		    			<!-- <p class="font-bold" style="margin-top:0;padding-top:18px;">
		    			"The NRA-TA is the most unique, innovative, quickest and accurate method of scoring trophy animals 
		ever devised. It takes the ‘I think it should be scored this way’ out of scoring trophies." -Mr. Whitetail
						</p>
			    		<div class="font-light" style="padding-bottom:10px;">
			    			NRA-Trophy Analysis (NRA-TA) uses digital scanning technology to acquire exact three-dimensional models of any big 
			game trophy. Once acquired, our proprietary software will accurately score your trophy in a fraction of the time required 
			by manual scoring methods.
			    		</div> -->
		    		</div>
		    	</div>
	    	</div>
		</div>
	</div>
	<div class="homepage-bottom">
		
		<div class="footer-affiliates" style="background-image:none;">
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
		<div class="empty-green"></div>
		<div class="scan-container">
			<div class="homepage-down-arrow"></div>
			<div class="scan-content">
				<div class="scan-left">
					<?php 
					if(array_key_exists('homeImage', $cmsContents))
					{
						$imagePath = 'cms/media/'.$cmsContents['homeImage']['path'];
						echo '<img src="'.$imagePath.'" />';
					}
					?>
				</div>
				<div class="scan-right">
					<div class="cmsContentWrapper">
						<?php if(array_key_exists('homeBottomText', $cmsContents)) echo $cmsContents['homeBottomText']['text']; ?>
					</div>
					<div style="float:left;width:330px;">
						<div class="cmsContentWrapper">
							<?php if(array_key_exists('homeImageText', $cmsContents)) echo $cmsContents['homeImageText']['text'];?>
						</div>
						<div class="homepage-download-container">
							<a class="download-btn" href="resources/Antlers-Installer.msi">DOWNLOAD</a> 
							<p>
								Click Here to Download the New Antler Software<br />
								Right Click and Save As Target (6mb)
							</p>
						</div>
					</div>

					<!-- STL -->
					<?php if(array_key_exists('homeAntlers', $cmsContents)) {

					 	$stlPath = 'cms/media/'.$cmsContents['homeAntlers']['path']; 
						?>

						<input type="hidden" id="stlPath" value="<?php echo $stlPath;?>" />
						<div id="rotatingAntler" style="float:right;width:250px; height:250px;border: 3px solid #B8B8A2;">
							<img id="rotate-antler-btn" class="" src="./images/rotate-antler-btn.png" /><br />
						</div>	

					 <?php } ?>
				</div>
			</div>
		</div>
		<br>
		<div style="text-align:center;">
			<footer class="homepage-footer">
				<div class="homepage-footer-subdiv">
					<a href="index.php" style="border:none;">HOME</a>
					<a href="score.php">SCORING TECHNOLOGY</a>			
					<a href="score.php#track">TRACK YOUR SCORE</a>				
					<a href="about-us.php">ABOUT US</a>			
					<a href="faq.php">FAQ</a>			
					<a href="contact.php">CONTACT NRA-TA</a>			
					<a href="advertising.php">ADVERTISING</a>			
				</div>
				<div class="footer-text-bottom">
					<?php if(array_key_exists('footerText', $cmsContents)) echo $cmsContents['footerText']['text']; ?>
					<!-- To have your trophy analyzed, take your cleaned and dried game-animal trophies to a NRA-TA official recording station in your area. Find A Scoring <br/>Station NRA-TA is produced by Krien Method, Inc. -->
				<div>
			</footer>
		</div>
	</div>


	<!-- How It Works Popup -->
	<?php if(array_key_exists('howItWorksText', $cmsContents)){ ?>
	<div class="howitworks-popup default-popup" id="howItWorksPopup">
		<div class="close-btn" id="closeHowItWorks"></div>
		<div class="howitworks-text">
			<h2><?php echo $cmsContents['howItWorksText']['title'];?></h2><br/>
			<?php
				if(array_key_exists('howItWorksImage', $cmsContents)){ 
					$howItWorksImgPath = 'cms/media/'. $cmsContents['howItWorksImage']['path'];
					echo' <img src="'.$howItWorksImgPath.'" alt="Antlers">';
				}
			?>

			<div class="cmsContentWrapper">
				<?php echo $cmsContents['howItWorksText']['text']?>
			</div>
		</div>
	</div>
	<?php } ?>



	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
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

	<script src="./threeJS/three.min.js"></script>
	<script src="./threeJS/STLLoader.js"></script>
	<script src="./threeJS/TrackballControls.js"></script>
	<script src="./threeJS/Detector.js"></script>
	<script type="text/javascript" src="./threeJS/main.js"></script>
	<script type="text/javascript" src="js/popup.js"></script>
	<script>
		$( document ).ready(function() {
			$("#rotate-antler-btn").on( "click", function() {
				mesh.rotation.x += 0.05;
			});
		});
	</script>
	<script type="text/javascript">
		$('#howItWorksBtn').click(function(e){
			e.preventDefault();
			$('.howitworks-popup').Popup({
				overlayClose: true,
				clearContentOnClose: false
			});
		});
		$('#closeHowItWorks').click(function(){
			closePopup('fullscreen-overlay', false);
		});
	</script>
</body>
</html>

