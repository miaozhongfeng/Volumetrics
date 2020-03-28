-- phpMyAdmin SQL Dump
-- version 3.4.8
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Jan 28, 2015 at 02:42 PM
-- Server version: 5.1.66
-- PHP Version: 5.3.27

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `antler`
--
CREATE DATABASE `antler` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE `antler`;

-- --------------------------------------------------------

--
-- Table structure for table `cart`
--

CREATE TABLE IF NOT EXISTS `cart` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `product_id` int(11) NOT NULL,
  `user_id` double NOT NULL,
  `quantity` int(11) NOT NULL,
  `date` datetime NOT NULL,
  `is_user_registered` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=44 ;

--
-- Dumping data for table `cart`
--

INSERT INTO `cart` (`id`, `product_id`, `user_id`, `quantity`, `date`, `is_user_registered`) VALUES
(15, 122, 3, 2, '2015-01-23 15:24:09', 1),
(23, 122, 2147483647, 1, '2015-01-23 16:08:15', 0),
(31, 122, 2147483647, 1, '2015-01-23 16:14:44', 0),
(32, 122, 2222, 1, '2015-01-23 16:16:38', 0),
(33, 122, 285075285, 1, '2015-01-23 16:19:03', 0),
(35, 122, 417598965112, 1, '2015-01-23 16:24:19', 0),
(40, 125, 1, 1, '2015-01-25 17:13:22', 1),
(41, 125, 3, 1, '2015-01-26 08:16:54', 1);

-- --------------------------------------------------------

--
-- Table structure for table `customer_shipping_info`
--

CREATE TABLE IF NOT EXISTS `customer_shipping_info` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `customer_id` int(11) NOT NULL,
  `country_code` varchar(2) NOT NULL,
  `address` varchar(100) NOT NULL,
  `apartment` varchar(100) NOT NULL,
  `city` varchar(30) NOT NULL,
  `state` varchar(60) NOT NULL,
  `zip` varchar(9) NOT NULL,
  `telephone_number` varchar(60) NOT NULL,
  `mobile_number` varchar(16) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=16 ;

--
-- Dumping data for table `customer_shipping_info`
--

INSERT INTO `customer_shipping_info` (`id`, `customer_id`, `country_code`, `address`, `apartment`, `city`, `state`, `zip`, `telephone_number`, `mobile_number`) VALUES
(14, 1, 'MK', 'kosta racin 5', 'apt', 'kocani', 'Macedonia', '2300', '', '123132123'),
(15, 3, 'MK', '4ta Makedonska Brigada broj 52', '', 'Kochani', 'Kochani', '2300', '389-71-681361', '');

-- --------------------------------------------------------

--
-- Table structure for table `friend_requests`
--

CREATE TABLE IF NOT EXISTS `friend_requests` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `inviter_id` bigint(20) NOT NULL,
  `user_id` bigint(20) NOT NULL,
  `date_requested` date NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=26 ;

--
-- Dumping data for table `friend_requests`
--

INSERT INTO `friend_requests` (`id`, `inviter_id`, `user_id`, `date_requested`) VALUES
(12, 1, 34, '2014-11-16'),
(17, 1, 39, '2014-11-17'),
(19, 3, 39, '2014-11-22'),
(21, 3, 40, '2014-12-01'),
(22, 41, 2, '2014-12-04'),
(23, 42, 1, '2014-12-14'),
(24, 44, 3, '2014-12-22'),
(25, 44, 3, '2014-12-22');

-- --------------------------------------------------------

--
-- Table structure for table `friends`
--

CREATE TABLE IF NOT EXISTS `friends` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `user1id` bigint(20) NOT NULL,
  `user2id` bigint(20) NOT NULL,
  `date_requested` date NOT NULL,
  `date_accepted` date DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=8 ;

--
-- Dumping data for table `friends`
--

INSERT INTO `friends` (`id`, `user1id`, `user2id`, `date_requested`, `date_accepted`) VALUES
(5, 34, 3, '2014-11-16', '2014-11-17'),
(6, 38, 3, '2014-11-18', '2014-11-18'),
(7, 1, 3, '2014-11-16', '2014-12-11');

-- --------------------------------------------------------

--
-- Table structure for table `messages`
--

CREATE TABLE IF NOT EXISTS `messages` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `sender_id` bigint(20) NOT NULL,
  `recipient_id` bigint(20) NOT NULL,
  `subject` varchar(60) NOT NULL,
  `message` longtext NOT NULL,
  `is_read` tinyint(1) NOT NULL DEFAULT '0',
  `date` datetime NOT NULL,
  `deleted_by_sender` tinyint(1) NOT NULL DEFAULT '0',
  `deleted_by_recipient` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=31 ;

--
-- Dumping data for table `messages`
--

INSERT INTO `messages` (`id`, `sender_id`, `recipient_id`, `subject`, `message`, `is_read`, `date`, `deleted_by_sender`, `deleted_by_recipient`) VALUES
(25, 1, 3, 'bujrum', 'poraka', 1, '2014-12-14 16:11:32', 0, 0),
(26, 3, 1, 'yeey', 'test', 0, '2014-12-16 08:26:37', 0, 0),
(28, 3, 1, 'hello', 'hi', 1, '2014-12-16 08:27:10', 0, 0),
(29, 3, 34, 'Test Message', 'Hi Stojan', 1, '2014-12-20 06:52:33', 0, 0),
(30, 1, 3, 'proba2', 'J', 0, '2014-12-20 17:27:47', 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `messages_archive`
--

CREATE TABLE IF NOT EXISTS `messages_archive` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `sender_id` bigint(20) NOT NULL,
  `recipient_id` bigint(20) NOT NULL,
  `subject` varchar(60) NOT NULL,
  `message` longtext NOT NULL,
  `date` datetime NOT NULL,
  `deleted_by_sender` tinyint(1) NOT NULL DEFAULT '0',
  `deleted_by_recipient` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=27 ;

--
-- Dumping data for table `messages_archive`
--

INSERT INTO `messages_archive` (`id`, `sender_id`, `recipient_id`, `subject`, `message`, `date`, `deleted_by_sender`, `deleted_by_recipient`) VALUES
(20, 30, 27, 'asd', 'asdasdasd', '2011-12-11 21:29:46', 1, 0),
(21, 30, 27, 'asd', 'asdasdasd', '2011-12-11 21:29:46', 1, 0),
(22, 27, 30, 'asd', 'asdasdasd', '2011-12-11 21:29:46', 0, 0),
(25, 27, 30, 'asd', 'asdasdasd', '2011-12-11 21:29:46', 0, 0),
(26, 27, 30, 'asd', 'asdasdasd', '2011-12-11 21:29:46', 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `news`
--

CREATE TABLE IF NOT EXISTS `news` (
  `id` int(9) NOT NULL AUTO_INCREMENT,
  `date` varchar(10) NOT NULL,
  `headline` varchar(100) NOT NULL,
  `body` longtext NOT NULL,
  `image` varchar(150) NOT NULL DEFAULT 'default.jpg',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=43 ;

--
-- Dumping data for table `news`
--

INSERT INTO `news` (`id`, `date`, `headline`, `body`, `image`) VALUES
(37, '17/11/14', 'Trophy News!', '<p><strong>Hello!</strong></p>\r\n\r\n<p><strong>It is a test article.&nbsp;</strong></p>\r\n\r\n<hr />\r\n<p>Kay&acute;s personal experience as a long time hunter, technology freak and small business owner led him to utilize two breakthrough technologies which make the NRA-TA process so accurate and unique:</p>\r\n\r\n<p>&nbsp;</p>\r\n\r\n<ul>\r\n	<li>3D laser scanning went mainstream and handheld devices became available at a price point that allowed individuals (not just universities or large corporations) to begin exploring the device&acute;s possibilities.</li>\r\n	<li>Rapid prototyping services used to build low volume prototype plastic components and customized jewelry were becoming competitively priced commodities.</li>\r\n</ul>\r\n\r\n<p>&nbsp;</p>\r\n\r\n<p>After speaking with everyone from engineers at NASA to black powder squirrel hunters, Kay began to refine the 3D scanning process, software and web interface in order to provide a simple scanning and reproduction service to his customers. NRA-TA was incorporated in 2007 in order to formally establish this new business venture.After speaking with everyone from engineers at NASA to black powder squirrel hunters, Kay began to refine the 3D scanning process, software and web interface in order to provide a simple scanning and reproduction service to his customers. NRA-TA was incorporated in 2007 in order to formally establish this new business venture.</p>\r\n', '643384.png'),
(38, '21/11/14', 'Test news 2', '<p><em>Test news 2</em></p>\r\n\r\n<hr />\r\n<h2>This is H2&nbsp;</h2>\r\n\r\n<p>Kay&acute;s personal experience as a long time hunter, technology freak and small business owner led him to utilize two breakthrough technologies which make the NRA-TA process so accurate and unique:</p>\r\n\r\n<p>&nbsp;</p>\r\n\r\n<ul>\r\n	<li>3D laser scanning went mainstream and handheld devices became available at a price point that allowed individuals (not just universities or large corporations) to begin exploring the device&acute;s possibilities.</li>\r\n	<li>Rapid prototyping services used to build low volume prototype plastic components and customized jewelry were becoming competitively priced commodities.</li>\r\n</ul>\r\n\r\n<p>&nbsp;</p>\r\n\r\n<p>After speaking with everyone from engineers at NASA to black powder squirrel hunters, Kay began to refine the 3D scanning process, software and web interface in order to provide a simple scanning and reproduction service to his customers. NRA-TA was incorporated in 2007 in order to formally establish this new business venture.</p>\r\n', '997754.png'),
(39, '21/11/14', 'Long intro', '<p>After speaking with everyone from engineers at NASA to black powder squirrel hunters, Kay began to refine the 3D scanning process, software and web interface in order to provide a simple scanning and reproduction service to his customers. NRA-TA was incorporated in 2007 in order to formally establish this new business venture.After speaking with everyone from engineers at NASA to black powder squirrel hunters, Kay began to refine the 3D scanning process, software and web interface in order to provide a simple scanning and reproduction service to his customers. NRA-TA was incorporated in 2007 in order to formally establish this new business venture.</p>\r\n\r\n<hr />\r\n<p>Kay&acute;s personal experience as a long time hunter, technology freak and small business owner led him to utilize two breakthrough technologies which make the NRA-TA process so accurate and unique:</p>\r\n\r\n<p>&nbsp;</p>\r\n\r\n<ul>\r\n	<li>3D laser scanning went mainstream and handheld devices became available at a price point that allowed individuals (not just universities or large corporations) to begin exploring the device&acute;s possibilities.</li>\r\n	<li>Rapid prototyping services used to build low volume prototype plastic components and customized jewelry were becoming competitively priced commodities.</li>\r\n</ul>\r\n\r\n<p>&nbsp;</p>\r\n\r\n<p>After speaking with everyone from engineers at NASA to black powder squirrel hunters, Kay began to refine the 3D scanning process, software and web interface in order to provide a simple scanning and reproduction service to his customers. NRA-TA was incorporated in 2007 in order to formally establish this new business venture.</p>\r\n', '82737.jpg'),
(40, '1/12/14', 'Test Production News', '<p>Warning: Instert horizontal line after introduction text. This will split the text.</p>\r\n\r\n<hr />\r\n<p>Warning: Text that starts from here will be placed in body of the artcle. Text that is above line (in Admin section) is actually introducion text that will be shown in fromt page for News. This text will be shown only when &quot;Read More&quot; article is clicked.</p>\r\n', '265174.jpg');

-- --------------------------------------------------------

--
-- Table structure for table `online_users`
--

CREATE TABLE IF NOT EXISTS `online_users` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `user_id` bigint(20) NOT NULL,
  `timestamp` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=51 ;

--
-- Dumping data for table `online_users`
--

INSERT INTO `online_users` (`id`, `user_id`, `timestamp`) VALUES
(17, 32, '2014-12-01 12:39:44'),
(21, 40, '2014-12-01 14:32:48'),
(26, 42, '2014-12-14 02:55:43'),
(27, 34, '2014-12-20 08:40:54'),
(32, 44, '2015-01-19 17:51:24'),
(39, 41, '2015-01-12 09:32:44'),
(50, 3, '2015-01-27 13:56:21');

-- --------------------------------------------------------

--
-- Table structure for table `product_archive`
--

CREATE TABLE IF NOT EXISTS `product_archive` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `old_id` int(11) NOT NULL,
  `name` varchar(60) NOT NULL,
  `description` longtext NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=9 ;

--
-- Dumping data for table `product_archive`
--

INSERT INTO `product_archive` (`id`, `old_id`, `name`, `description`) VALUES
(8, 124, 'test 2', '123123123');

-- --------------------------------------------------------

--
-- Table structure for table `product_categories`
--

CREATE TABLE IF NOT EXISTS `product_categories` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(60) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=19 ;

--
-- Dumping data for table `product_categories`
--

INSERT INTO `product_categories` (`id`, `name`) VALUES
(1, 'Clearance'),
(2, 'Archery'),
(3, 'Optics'),
(4, 'Hunting Electronics'),
(5, 'Treestands & Blinds'),
(6, 'Hunting Decoys'),
(7, 'Wildlife Management'),
(8, 'Hunting Knives & Tools'),
(9, 'Hunting Accessories'),
(10, 'Outfitter''s Camp'),
(11, 'Predator Hunting'),
(12, 'Waterfowling Gear'),
(13, 'DVDs, CDs, and Decals'),
(18, 'asd');

-- --------------------------------------------------------

--
-- Table structure for table `product_images`
--

CREATE TABLE IF NOT EXISTS `product_images` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `product_id` int(11) NOT NULL,
  `name` varchar(150) NOT NULL,
  `is_cover_image` tinyint(1) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=133 ;

--
-- Dumping data for table `product_images`
--

INSERT INTO `product_images` (`id`, `product_id`, `name`, `is_cover_image`) VALUES
(70, 61, '61_113159180.jpg', 1),
(71, 62, '62_851165771.jpg', 1),
(72, 62, '62_231933594.png', 0),
(73, 62, '62_907501220.jpg', 0),
(74, 62, '62_278747559.jpg', 0),
(75, 62, '62_791473388.png', 0),
(81, 65, '65_852752685.jpg', 1),
(83, 65, '65_636627197.jpg', 0),
(84, 65, '65_940979003.jpg', 0),
(85, 65, '65_412200928.png', 0),
(91, 69, '69_414150484.jpg', 1),
(97, 72, '72_169993641.jpg', 1),
(103, 74, '74_194953907.png', 1),
(104, 74, '74_286646269.jpg', 0),
(105, 74, '74_505891079.jpg', 0),
(112, 76, '76_872527438.png', 1),
(113, 76, '76_200429264.jpg', 0),
(114, 76, '76_74132492.jpg', 0),
(118, 73, '73_84367634.jpg', 0),
(120, 73, '73_154497486.png', 1),
(121, 73, '73_150421799.png', 0),
(122, 73, '73_476002290.png', 0),
(127, 122, '122_315043109.jpg', 1),
(129, 122, '122_823454029.png', 0),
(130, 122, '122_802282881.png', 0),
(131, 122, '122_724974273.png', 0),
(132, 122, '122_486144309.jpg', 0);

-- --------------------------------------------------------

--
-- Table structure for table `products`
--

CREATE TABLE IF NOT EXISTS `products` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(60) NOT NULL,
  `description` longtext NOT NULL,
  `category_id` int(11) NOT NULL,
  `price` decimal(10,0) NOT NULL,
  `shipping_price` decimal(10,0) DEFAULT NULL,
  `shipping_price_type` enum('0','1','2') NOT NULL DEFAULT '0',
  `quantity` int(11) NOT NULL,
  `cover_image` varchar(150) NOT NULL DEFAULT 'default.jpg',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=126 ;

--
-- Dumping data for table `products`
--

INSERT INTO `products` (`id`, `name`, `description`, `category_id`, `price`, `shipping_price`, `shipping_price_type`, `quantity`, `cover_image`) VALUES
(122, 'Test 1', 'Description description  description  description description description description description description description description description description description description description description description description description description description description description description description description', 1, 12, 0, '2', 1212, '122_315043109.jpg'),
(125, 'test 23', 'Descriptasdh ashd askjdhas as', 2, 123, 0, '0', 123, 'default.jpg');

-- --------------------------------------------------------

--
-- Table structure for table `registered_users`
--

CREATE TABLE IF NOT EXISTS `registered_users` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `username` varchar(30) NOT NULL,
  `password` varchar(64) NOT NULL,
  `firstname` varchar(30) NOT NULL,
  `lastname` varchar(30) NOT NULL,
  `email` varchar(30) NOT NULL,
  `personalInfo` varchar(300) DEFAULT NULL,
  `isDeleted` tinyint(4) NOT NULL DEFAULT '0',
  `isVerified` tinyint(4) NOT NULL DEFAULT '0',
  `dateCreated` date NOT NULL,
  `dateLastLogin` datetime DEFAULT NULL,
  `salt` varchar(64) NOT NULL,
  `profile_picture` varchar(150) NOT NULL DEFAULT 'default.jpg',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=48 ;

--
-- Dumping data for table `registered_users`
--

INSERT INTO `registered_users` (`id`, `username`, `password`, `firstname`, `lastname`, `email`, `personalInfo`, `isDeleted`, `isVerified`, `dateCreated`, `dateLastLogin`, `salt`, `profile_picture`) VALUES
(2, 'David Krien', '7c1c1db9d2a6676235c40a25e42a15287cda056d5270a19988d5b4aa9ce5e833', 'David', 'Krien', 'david@nrata.com', NULL, 0, 1, '2014-10-20', '2014-10-20 18:26:05', '90c51ffe3aabc71d01c555ac6a9ba3de22a385f18fad6a2ca14891f507772062', 'default.jpg'),
(3, 'ivanstojanov1990', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Ivan', 'Stojanov', 'ivan.stojanov1990@gmail.com', 'LOL', 0, 1, '2014-10-18', '2014-10-23 18:38:15', '3a8271fd0714c74d73b249eb3be32c52d1ecfa22bd8a3ed5c09d303b1681bd5a', '825060.jpg'),
(1, 'andrej', 'ed02457b5c41d964dbd2f2a609d63fe1bb7528dbe55e1abf5b52c249cd735797', 'Andrej', 'Manev', 'amanev1@yahoo.com', 'hackerishte :D', 0, 1, '2014-11-14', '2014-11-03 21:28:24', '946a9df89569f202481891b8e370d621a9c77267f05f442f8aef8986f4157c69', '961644.jpg'),
(32, 'zoki2', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Zoran', 'Stojanov', 'zokis3@hotmail.com', '', 0, 1, '2014-11-14', '2014-11-14 21:28:18', '7fec6bcd304bbd6686dfc5542c37db73e764afe08c83660be284bb7307b014ed', '577570.jpg'),
(33, 'kirepavlov', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Kire', 'Pavlov', 'odobruvanje1@yahoo.com', NULL, 0, 0, '2014-11-14', '2014-11-14 21:29:00', 'eb288821db667488a10439cc822c712154198c3988ee98346e71c23196640644', 'default.jpg'),
(34, 'TestActive', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Stojan', 'Biskov', 'atvinetnkorisnik2@gmail.com', '', 0, 1, '2014-11-14', '2014-11-14 21:30:38', 'b60caaa44bcc941111c356cadf1fc1d6903cba97afd937b7a0d1def5863c8e1e', '393276.jpg'),
(35, 'danielstojkov', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Borce', 'Mancevski', 'gose.delcev@ugd.edu.mk', '', 0, 1, '2014-11-14', '2014-11-14 21:31:03', '06609ae1bee80b88f97fa1e4165ee8ded60733e72bcaef88599f076d244dae82', '169167.jpg'),
(36, 'antonijasapkova', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Antonija', 'Zdraveva', 'zani@hotmail.com', NULL, 1, 0, '2014-11-15', '2014-11-15 10:33:51', '4c56988313dfe9c2c8b08d235cdca4a51888677c77cbfc567857759155440cec', 'default.jpg'),
(37, 'test', 'ed02457b5c41d964dbd2f2a609d63fe1bb7528dbe55e1abf5b52c249cd735797', 'Test', 'Test', 'ivans@facebook.com', NULL, 0, 0, '2014-11-15', '2014-11-15 13:15:30', '3a530051b0d157cb018a013fae9d2c5c96d7f009a9f94501121be0505901fa2f', 'default.jpg'),
(38, 'ScopicSoftware', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Scopic', 'Software', 'antlersteam@scopicsoftware.com', '', 0, 1, '2014-11-15', '2014-11-15 21:15:18', 'bcd147b984b8100207d0021a9dc27a47e046bb21c04adef6efb9c7e72c205250', '137123.jpg'),
(39, 'testuser1', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'val', 'g', 'vdgnatyuk@gmail.com', NULL, 0, 1, '2014-11-17', '2014-11-17 12:42:33', 'd14ef637616bfb746dc9ce609a16e1704c94c60ebafbaee04cba6dd13eb70f3b', 'default.jpg'),
(40, 'anabaceva', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Ana', 'Andreeva', 'anuska454', '', 0, 1, '2014-11-16', '2014-12-01 00:00:00', 'd14ef637616bfb746dc9ce609a16e1704c94c60ebafbaee04cba6dd13eb70f3a', '461505.jpg'),
(41, 'antlerscore', '733bd8b82d1d2a317cb30b1600c8464a3659a7230606096f357f60b45e6dcc3e', 'Jim', 'Piper', 'jpiper3@gmail.com', '', 0, 1, '2014-12-04', '2014-12-04 22:48:27', 'ef0417a2af4aff172463fd8acf4c0205ee0b58751e1ff64d7e761cc634a4e36d', '825555.jpg'),
(42, 'mexico', 'ef797c8118f02dfb649607dd5d3f8c7623048c9c063d532cc95c5ed7a898a64f', 'andrej', 'manev', 'ace.ristik@gmail.com', NULL, 0, 1, '2014-12-14', '2014-12-14 07:52:43', '2e6f21e66cced2d63e77a772671b1cb9f7e9db9c54f403ebbc43b9ba9aeb977c', 'default.jpg'),
(43, 'testUser11', '3cc849279ba298b587a34cabaeffc5ecb3a044bbf97c516fab7ede9d1af77cfa', 'myName', 'myLastName', 'mymail@gmail.com', NULL, 0, 0, '2014-12-15', '2014-12-15 18:41:51', 'd987942b14e66556e83709a9b244b10403dbae0327a1b4c9d567349a7a136686', 'default.jpg'),
(44, 'joanna.k@scopicsoftware.com', 'abddc93c1cc188d9b8d8ae191b004f218a6717211008042d69c2f2ad0c79c04d', 'Joanna', 'Kusiak', 'joanna.k@scopicsoftware.com', '', 0, 1, '2014-12-22', '2014-12-22 22:01:16', '1dbf956b9884d779ea01e492fce78edfe04c246e36b97902d89214a011f9b2c8', '627588.jpg'),
(45, 'test2', '96cae35ce8a9b0244178bf28e4966c2ce1b8385723a96a6b838858cdd6ca0a1e', 'test2', 'test2', 'test2@test.com', NULL, 0, 0, '2014-12-24', '2014-12-24 21:35:51', 'd34e91a6620459abed06dd73d771457fbdeb46d33b2c600f598aadde4818373a', 'default.jpg'),
(46, 'test3', '96cae35ce8a9b0244178bf28e4966c2ce1b8385723a96a6b838858cdd6ca0a1e', 'test3', 'test3', 'test3@werf.asd', NULL, 0, 0, '2014-12-24', '2014-12-24 21:38:18', 'ca051f5fe25f52591ec672f6eb3c3c9bf39e7a8bc087774a9be826023b106661', 'default.jpg'),
(47, 'Pig Pen', 'd0818cb88d8293fc65285aba653c31060fb6bcc06bdf9b6182262288a859713b', 'jeff', 'JOHNSTON', 'jh_johnston@yahoo.com', NULL, 0, 0, '2015-01-28', '2015-01-28 18:09:13', 'a1088294b1a0dd9de101e94c76679b746749aff19ea8e0cf363e8668dc0a2d9d', 'default.jpg');

-- --------------------------------------------------------

--
-- Table structure for table `shipping_prices`
--

CREATE TABLE IF NOT EXISTS `shipping_prices` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `product_id` int(11) NOT NULL,
  `range_from` int(11) NOT NULL,
  `range_to` int(11) NOT NULL,
  `price` decimal(10,0) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=51 ;

--
-- Dumping data for table `shipping_prices`
--

INSERT INTO `shipping_prices` (`id`, `product_id`, `range_from`, `range_to`, `price`) VALUES
(48, 122, 1, 12, 12),
(49, 122, 13, 15, 123),
(50, 122, 16, 123, 123123);

-- --------------------------------------------------------

--
-- Table structure for table `trophy_categories`
--

CREATE TABLE IF NOT EXISTS `trophy_categories` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(60) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=18 ;

--
-- Dumping data for table `trophy_categories`
--

INSERT INTO `trophy_categories` (`id`, `name`) VALUES
(1, 'Bear'),
(2, 'Cougar & Jaguar'),
(3, 'Walrus'),
(4, 'Non-Typical American Elk'),
(5, 'Roosvelt''s And Elk'),
(6, 'Typical American Elk'),
(7, 'Non-Typical Mule Deer & Blacktail'),
(8, 'Non-Typical Whitetail Deer'),
(9, 'Typical Mule Deer & Blacktail'),
(10, 'Typical Whitetail Deer'),
(11, 'Moose'),
(12, 'Caribou'),
(13, 'Pronghorn'),
(14, 'Bison'),
(15, 'Muskox'),
(16, 'Rocky Mountain Goat'),
(17, 'Sheep');

-- --------------------------------------------------------

--
-- Table structure for table `trophyroom`
--

CREATE TABLE IF NOT EXISTS `trophyroom` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `category_id` int(11) NOT NULL,
  `score` varchar(30) DEFAULT NULL,
  `location` varchar(100) DEFAULT NULL,
  `hunter` varchar(60) DEFAULT NULL,
  `owner` varchar(60) NOT NULL,
  `date` date DEFAULT NULL,
  `key_measurements` longtext,
  `image` varchar(150) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=26 ;

--
-- Dumping data for table `trophyroom`
--

INSERT INTO `trophyroom` (`id`, `category_id`, `score`, `location`, `hunter`, `owner`, `date`, `key_measurements`, `image`) VALUES
(1, 1, NULL, 'Hays KS', 'David Krien', 'David Krien', '2000-00-00', '', '111111.jpg'),
(2, 1, NULL, '', 'David Krien', 'David Krien', '2001-00-00', '', '111112.jpg'),
(3, 1, NULL, 'Bird City KS', 'David Krien', 'David Krien', '2002-00-00', '', '111113.jpg'),
(4, 1, '', 'Bird City KS', 'David Krien', 'David Krien', '2003-00-00', '', '111114.jpg'),
(11, 1, NULL, 'St. Francis KS', 'David Krien', 'David Krien', '2005-00-00', '', '111116.jpg'),
(12, 1, NULL, 'Deer Camp', 'David Krien', 'David Krien', '2006-00-00', '', '111117.jpg'),
(13, 1, NULL, 'Deer Camp', 'David Krien', 'David Krien', '2006-00-00', '', '111118.jpg'),
(14, 1, NULL, 'Deer Camp', 'David Krien', 'David Krien', '2006-00-00', '', '111119.jpg'),
(15, 1, NULL, 'Deer Camp', 'David Krien', 'David Krien', '2007-00-00', '', '111110.jpg'),
(17, 1, NULL, '', 'David Krien', 'David Krien', NULL, '', '111121.jpg'),
(18, 1, NULL, '', 'David Krien', 'David Krien', NULL, '', '111122.jpg'),
(19, 3, NULL, '', 'David Krien', 'David Krien', NULL, '', '111123.jpg'),
(20, 2, NULL, '', 'David Krien', 'David Krien', NULL, '', '111124.jpg'),
(21, 2, NULL, '', 'David Krien', 'David Krien', NULL, '', '111125.jpg'),
(22, 1, NULL, '', 'David Krien', 'David Krien', NULL, '', '111126.jpg'),
(23, 1, NULL, 'Deer Camp', 'David Krien', 'David Krien', '2009-00-00', '', '111127.jpg'),
(24, 2, NULL, 'Bird City', 'David Krien', 'David Krien', NULL, '', '111128.jpg'),
(25, 17, '', '', 'David Krien', 'David Krien', '0000-00-00', '', '111129.jpg');

-- --------------------------------------------------------

--
-- Table structure for table `trophyroom_slider`
--

CREATE TABLE IF NOT EXISTS `trophyroom_slider` (
  `id` int(5) NOT NULL AUTO_INCREMENT,
  `path` varchar(150) NOT NULL,
  `orderNum` int(2) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=30 ;

--
-- Dumping data for table `trophyroom_slider`
--

INSERT INTO `trophyroom_slider` (`id`, `path`, `orderNum`) VALUES
(27, '870388.jpg', 1),
(28, '696992.jpg', 2),
(29, '516698.jpg', 3);

-- --------------------------------------------------------

--
-- Table structure for table `wishlist`
--

CREATE TABLE IF NOT EXISTS `wishlist` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `product_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `date` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=10 ;

--
-- Dumping data for table `wishlist`
--

INSERT INTO `wishlist` (`id`, `product_id`, `user_id`, `date`) VALUES
(1, 75, 30, '2015-01-11 20:47:10'),
(2, 74, 30, '2015-01-11 20:52:23'),
(3, 75, 31, '0000-00-00 00:00:00'),
(4, 75, 32, '0000-00-00 00:00:00'),
(5, 76, 1, '2015-01-11 15:25:17'),
(6, 77, 1, '2015-01-11 17:31:08'),
(9, 122, 1, '2015-01-25 15:34:43');

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
