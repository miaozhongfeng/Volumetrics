<?php
 ob_start();	
	include_once('includes/functions.php');
	
	// Here we get all the information from the fields sent over by the form.
	$nameForm = $_POST['name'];
	$emailForm = $_POST['email'];
	$messageForm = $_POST['message'];
	$enquiryForm = $_POST['enquiry']; 
	 
	$clientEmail1 = 'ivan.s@scopicsoftware.com';
	$clientEmail2 = 'valentyn.g@scopicsoftware.com';
	$clientEmail3 = 'administrator@nrata.com';
	$clientName  = 'Client Name';
	$subjectMsg = 'NATURE OF ENQUIRY: '.$enquiryForm;
	$contentMsg = 'A message was sent from Antlers contact form.<br/><br/>FROM: '.$nameForm.'<br/>Email: '.$emailForm.'<br/><br/>Message: '.$messageForm.'<br/>';
	$fromNameMsg = 'Antlers Contact Form';
	 
	$SendToAdmin1 = SendMail( $clientEmail1, $contentMsg, $subjectMsg, $fromNameMsg);
	$SendToAdmin2 = SendMail( $clientEmail2, $contentMsg, $subjectMsg, $fromNameMsg);
	$SendToAdmin3 = SendMail( $clientEmail3, $contentMsg, $subjectMsg, $fromNameMsg);
	
	if ( $SendToAdmin1 == false || $SendToAdmin2 == false || $SendToAdmin3 == false ) {
 $output = ob_get_contents();
 ob_end_clean();	
		$notificationText = "Your message was not send! Please try again later!";
		$notificationType = "error";
		$notificationThirdClass = "failure-class";
		echo customNotifications($notificationText, $notificationType, $notificationThirdClass, "style='left: 0px;'");
	}
	else {	
		$contentBackMsg = "Dear $nameForm, <br/><br/> Thank you for contacting the NRA-TA team. We will get back to you shortly if required. <br/><br/>Regards<br/>NRA-TA team.";
		$subjectBackMsg = "Thank you for getting in touch with us!";
		$fromNameBackMsg = "NRA-TA";		
		$SendToClient = SendMail( $emailForm, $contentBackMsg, $subjectBackMsg, $fromNameBackMsg );	
 $output = ob_get_contents();
 ob_end_clean();
		$notificationText = "Your message was successfully sent! We will get you back shortly if required!";
		$notificationType = "success";
		$notificationThirdClass = "success-class";
		echo customNotifications($notificationText, $notificationType, $notificationThirdClass, "style='left: 0px;'");
	}	
?>