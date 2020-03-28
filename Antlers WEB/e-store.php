<?php
$styles = '<link rel="stylesheet" type="text/css" href="css/sn-style.css">
			<link rel="stylesheet" type="text/css" href="css/estore.css">';
$scripts = '<script type="text/javascript" src="js/pagination.js"></script>';
$subPage = 'e-store';

include_once("includes/database-access.php");
include_once("includes/header-estore-template.php");

//get cms text
$cmsMainText = '';
$query = "SELECT text FROM cms_content WHERE code = 'estoreMainText' LIMIT 1";
$result = mysqli_query($config['mysql_con'], $query);
if($result)
{
	$row = mysqli_fetch_array($result, MYSQLI_NUM);
	$cmsMainText = $row[0];
}


//get categories
$query = "SELECT * FROM product_categories";
$result = mysqli_query($config['mysql_con'], $query);
$categoriesArr = array();
if(!$result)
{
	showErrorAndExit('default');
}
else
{
	while($row = mysqli_fetch_array($result, MYSQLI_ASSOC))
	array_push($categoriesArr, $row);
}

//if opened via details link, check clicked category
if(isset($_GET['category']) && !empty($_GET['category']) && ctype_digit($_GET['category']))
{
	$selectedCategory = $_GET['category'];
}
else
{
	$selectedCategory = "";
}
?>
<div class="sn-main-container">
	<div class="sn-main-top">
		<div class="arrow-holder">
			<div class="sn-main-arrow" style="left:675px"></div>
		</div>
	</div>
	<div class="sn-scrolable-margin"></div>
	<div class="sn-scrolable">
		<div class="estore-content shop-container">
			<div class="estore-notification-container"><?php showSessionNotification();?></div>
			<div class="sidebar-menu">
				<div class="angle-div default-angle-div">CHOOSE CATEGORY</div>
				<?php $activeClass = $selectedCategory == '' ? ' category-active' : '';?>
				<a href="#" class="product-category-link<?php echo $activeClass;?>" data-cat-id="">All</a>
				<?php
					foreach ($categoriesArr as $category)
					{
						$activeClass = $category['id'] == $selectedCategory ? ' category-active' : '';
						echo '<a href="#" data-cat-id="'.$category['id'].'" class="product-category-link'.$activeClass.'">'.$category['name'].'</a>';
					}
				?>
			</div>
			<!-- product items -->

			<div class="shop-items-wrapper">
				<div class="cmsContentWrapper">
					<?php echo $cmsMainText; ?>
				</div>
				<br/>
				<div id="ajaxContent" style="display:inline-block;"></div>
				<div class="clear"></div>
				<input type="hidden" id="categoryIdHdn" value="<?php echo $selectedCategory?>">
				<div class="pagination"></div>
			</div>
			<div class="clear"></div>
		</div>
		<br/><br/>
	</div>
</div>

<?php include_once("includes/footer-template.php");?>
<script>
loadContent(1);

$('.pagination').on('click', 'a', function(e){
	e.preventDefault();
	loadContent($(this).data('page'), $('#categoryIdHdn').val());
});

//load content
function loadContent(page){
	showLoader();
	$.ajax({
        url: 'partials/estore/load_products.php',
        type: 'POST',
        dataType: 'json',
        data: {page: page, category: $('#categoryIdHdn').val()},
        success: function (obj) {            
        	if(obj)
			{					
				if(obj.data.error)
				{
					showNotification('default','error', '.estore-content');
					exit();
				}

				var html = '';

				if(obj.results.length > 0)
				{
					for(key in obj.results)
					{
						var id = obj.results[key].id;
						var name = obj.results[key].name ? obj.results[key].name : '';
						if(name.length > 17)
							name = name.substr(0, 14) + '...'
						var description = obj.results[key].description ? obj.results[key].description : '';
						if(description.length > 50)
							description = description.substr(0, 48) + '...'
						var price = obj.results[key].price ? obj.results[key].price : '';
						var image = obj.results[key].cover_image ? 'user_images/products/thumbs/' + obj.results[key].cover_image : '';
						html+='<div class="shop-item">'+
							'<h3 style="margin-bottom:10px;">'+name+'</h3>'+
							'<a href="product-details.php?id='+id+'" class="shop-item-img-holder product-details-link">'+
								'<img src="'+image+'"/>'+
							'</a>'+
							'<div class="shop-item-text">' +description+ '</div>'+
							'<div>'+
								'<h4 class="price-left">Price</h4>'+
								'<h4 class="price-right">'+price+'$</h4>'+
							'</div>'+
						'</div>';
					}

					var isLast = obj.data.totalResults > obj.data.page * obj.data.resultsPerPage ? false : true;
					//pagination.js
					$('.pagination').show();
					$('.pagination').html(pagination(obj.data.page, obj.data.totalPages, isLast));

				}
				else
				{
					html = '<br /><br /><h4 class="text-canter">No products</h4>';
					$('.pagination').hide();
				}
				$('#ajaxContent').html(html);
			}			
			else
				showNotification('default','error', '.estore-content');
        },
        error: function (obj) {showNotification('default','error', '.estore-content');}
    });
}

$('.product-category-link').click(function(e){
	e.preventDefault();
	$('.category-active').removeClass('category-active');
	$(this).addClass('category-active');
	$('#categoryIdHdn').val($(this).data('cat-id'));
	loadContent(1);
})

function showLoader(){
	var ajaxContainerHeight =  $('#ajaxContent').height();
	if(ajaxContainerHeight < 300)
		ajaxContainerHeight = 300;
	var elem = '<div id="ajaxLoaderDiv" style="height:'+ajaxContainerHeight+'px"></div>';
	$('#ajaxContent').html(elem);
}
</script>
