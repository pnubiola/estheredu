<?php

  if ( strcasecmp($_SERVER['REQUEST_METHOD'] , 'POST') != 0) {
    $savedir = $_SERVER['QUERY_STRING'];
    if (substr($savedir , -1) == '/') $savedir = substr($savedir , 0 ,-1);

?>
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8" />
<title>HTML5 Full-Screen Demonstration</title>
<script>
//request new image
var lastloadfile = "" ;
var lastreviewfile = "" ;  
resizeImage = function(imgobj , screenmode){
  var nh = imgobj.naturalHeight;
  var nw = imgobj.naturalWidth;
  var wh;
  var ww;
  if (screenmode) {
    wh = screen.height;
    ww = screen.width;
  }else{
    wh = window.innerHeight;
    ww = window.innerWidth;
  }
  imgobj.parentNode.width = ww;
  imgobj.parentNode.height = wh;
  
  if ((nh / wh) > (nw / ww)){
    imgobj.heitght = wh;
    imgobj.width = wh * nw / nh ;
    imgobj.style.top = 0;
    imgobj.style.left = (ww - imgobj.width) / 2;
  }else {
    imgobj.width = ww;
    imgobj.height = ww * nh / nw;
    imgobj.style.position = "fixed";
    imgobj.style.top = (wh - imgobj.height) / 2;
    imgobj.style.left = 0;
  }
}
function getXMLHttpRequest() 
{
    if (window.XMLHttpRequest) {
        return new window.XMLHttpRequest;
    }
    else {
        try {
            return new ActiveXObject("MSXML2.XMLHTTP.3.0");
        }
        catch(ex) {
            return null;
        }
    }
}

function handler() {
  if(this.readyState == this.DONE) {
    if(this.status == 200 && this.responseText != null ) {
      // success!
      if (this.responseText.length() === 0) return;
      if (this.responseText == "nodata") return;
      var res = this.responseText.split(';')
      if (res[0] == "n") lastloadfile = res[1];
      else lastreviewfile = res[1];
      var ob = document.getElementById("resizeid");
<?php
      echo "ob.src = \"{$savedir}/\" + res[1];\n"; 
?>
      resizeImage(ob , true);
      return;
    }
  }
}
var intervaling = function(){
    var req = getXMLHttpRequest();
    req.onreadystatechange = handler ;
<?php
    echo "req.open(\"POST\", \"" . basename( __FILE__ ) . "\");\n";
?>
    req.setRequestHeader("Content-Type", "text/plain;charset=UTF-8");
<?php
    echo "req.send(\"lastloadfile=\\\"\" + lastloadfile + \"\\\";lastreviewfile=\\\"\" + lastreviewfile + \"\\\";readdir=\\\"{$savedir}\\\"\");\n";
?>
}
</script>
<style>
* {
	padding: 0;
	margin: 0;
}

html, body
{
	height: 100%;
	font-family: "Segoe UI", arial, helvetica, freesans, sans-serif;
	color: #333;
	background-color: #fff;
	overflow-y: auto;
	overflow-x: hidden;
}

body {
	margin: 0px;
}

p {
	margin: 0 0 1em 0;
}

/*figure {
	position: relative	;
	display: block;
	width: 100%;
	float: right;
	padding: 0;
/*	margin: 1em;*
	cursor: pointer;
}

figure img {
	display: block;
	left: 0;
	right: 0;
	max-width: 100%;
	top: 0;
	bottom: 0;
	margin: auto;
	cursor: pointer;	
}

figure:-moz-full-screen img {
	position: fixed;
}

figure:-ms-fullscreen {
	width: auto;
}

figure:-ms-fullscreen img {
	position: fixed;
}

figure:fullscreen img {
	position: fixed; 
}*/
#myimage {
    position: relative	;
    display: block;
    width: 100%;
    float: right;
}

#myimage:-ms-fullscreen {
    width: auto;
}
#textid{
  top: 20em;
  left: 20em;
  position: relative;
  color: #FF0000;
  background-color: #FFFFFF;
  font-size: x-large
}
#resizeid{
	display: none;
	left: 0;
	right: 0;
	max-width: 100%;
	top: 0;
	bottom: 0;
	margin: auto;
	cursor: pointer;	
}
#resizeid:-moz-full-screen {
	position: fixed;
}
#resizeid:-ms-fullscreen {
	position: fixed;
}
#resizeid:fullscreen {
	position: fixed;
}
</style>
</head>
<body>

<!--header>
<h1>HTML5 Full-Screen Demonstration</h1>
</header-->
<igure id="myimage">
<img id="resizeid" src="squirrel.jpg" alt="a squirrel" />
</figure>

<div id="textid">
  Click over de image to toogle full-screen and initiate de View loop
</div>
<!--
<p>This demonstration illustrates how to use the HTML5 full-screen API which has been implemented in IE11, Firefox, Chrome, Opera and Safari. Click the image to toggle full-screen view.</p>

<p>For further information, please refer to the article <a href="http://www.sitepoint.com/html5-full-screen-api-revisited/"><strong>How to Use the HTML5 Full-Screen API (Again)</strong></a>&hellip;</p>

<p>This example code was developed by <a href="http://twitter.com/craigbuckler">Craig Buckler</a> of <a href="http://optimalworks.net/">OptimalWorks.net</a> for <a href="http://sitepoint.com/">SitePoint.com</a>.</p>

<p>Please view the source and use this code as you wish. A link back to the article is appreciated.</p>
-->

<script>

  var  dheight = document.documentElement.height;
  var  dwidth = document.documentElement.width;
// full-screen available?
if (
	document.fullscreenEnabled || 
	document.webkitFullscreenEnabled || 
	document.mozFullScreenEnabled ||
	document.msFullscreenEnabled
) {

	// image container
	var i = document.getElementById("myimage");
	var j = document.getElementById("resizeid");
	// click event handler
	i.onclick = function() {
		var mode;
	    
		// in full-screen?
		if (
			document.fullscreenElement ||
			document.webkitFullscreenElement ||
			document.mozFullScreenElement ||
			document.msFullscreenElement
		) {

			// exit full-screen
			if (document.exitFullscreen) {
				document.exitFullscreen();
			} else if (document.webkitExitFullscreen) {
				document.webkitExitFullscreen();
			} else if (document.mozCancelFullScreen) {
				document.mozCancelFullScreen();
			} else if (document.msExitFullscreen) {
				document.msExitFullscreen();
			}
			
			mode = false;
			document.documentElement.height = dheight;
			document.documentElement.width;
		}
		else {
		
			// go full-screen
			if (i.requestFullscreen) {
				this.requestFullscreen();
			} else if (i.webkitRequestFullscreen) {
				i.webkitRequestFullscreen();
			} else if (i.mozRequestFullScreen) {
				i.mozRequestFullScreen();
			} else if (i.msRequestFullscreen) {
				i.msRequestFullscreen();
			}
			mode = true;
		}
		document.getElementById("resizeid").style.display = mode ? "block" : "none" ;
		document.getElementById("resizeid").style.visibility = mode ? "visible" : "hidden" ;
		resizeImage(document.getElementById("resizeid"),mode);
	}

}

</script>

</body>
</html>

<?php
  return;
  }
  $by = file_get_contents("php://input");
  $tmp = explode(';' , $by);
  $fi = array();
  
  foreach( $tmp as $w) {
    if ( ($p = strpos($w,"=")) > 0) {
      $fi[trim(substr($w,0,$p))] = trim(substr($w,$p + 1), " \t\n\r\0\x0B\"" );
    }
      
  }
  if (!array_key_exists('lastloadfile',$fi) || !array_key_exists('lastreviewfile',$fi) || !array_key_exists('readdir',$fi) ) {
    http_response_code(400);
    return;
  }
  $fi['readdir'] = realpath($fi['readdir']);
  if (substr($fi['readdir'] , -1 ) != DIRECTORY_SEPARATOR){
    $fi['readdir'] .= DIRECTORY_SEPARATOR;
  }
  
  $list = file_get_contents("{$fi['readdir']}filelist.data");
  //send header before send data
  header("Content-Type: text/html; charset=utf-8");
  if (empty($list)) {
    echo "nodata";
    return;
  }
  $list = explode("\r\n" , $list);
  if (empty($fi['lastloadfile'])){
    echo "n;{$list[0]}";
    return;
  }
  if ($list[count($list) -1] == $fi['lastloadfile']){
    if ($list[count($list) -1] == $fi['lastreviewfile']){
      echo "r;{$list[0]}";
      return; 
    }
    $k = array_search($fi['lastreviewfile'] , $list);
    echo "r;{$list[$k === false? 0 : $k]}";
    return;
  }
   $k = array_search($fi['lastloadfile'] , $list);
   echo "n;{$list[$k === false? 0 : $k]}";
?>