<?php

/**
 * Documentation, License etc.
 *
 * @package EstherEdu.php
 * Created on: 24/08/2014
 * Author: Pere Nubiola Radigales
 */

/* This file is part of EstherEdu.php

    EstherEdu.php is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EstherEdu.php is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EstherEdu.  If not, see <http://www.gnu.org/licenses/>.
*/


/*function exception_error_handler($errno, $errstr, $errfile, $errline ) {
    throw new ErrorException($errstr, 0, $errno, $errfile, $errline);
}*/
function prename($preyear){
  $currentdate = new DateTime(NULL , new DateTimeZone('UTC'));
  $yeardate = new DateTime();
  $yeardate->setTimezone(new DateTimeZone('UTC'));
  $yeardate->setDate($currentdate->format('Y') - ($preyear == false ? 0 : 1) , 1 , 1);
  $yeardate->setTime(0,0);
  $diff = $currentdate->getTimestamp() - $yeardate->getTimestamp();
  $prename = '0000000' . dechex($diff);
  $prename = substr($prename , - 7);
  return $prename;
}

function firtsfile($dir){
  if (!($handle = opendir($dir))) return false;
  while ( ($entry = readdir($handle)) !== false) {
    if (preg_match("/(^[[:xdigit:]]{7})([[:upper:]]+)([[:print:]]*)(\.jpeg|\.jpg|.png|,svn)/", $entry, $matches)) {
      closedir($handle);
      if ( hecdec($matches[1]) <= hexdec(prename(false))) return false;
      return true;
    }
  }
  closedir($handle);  
}

  ini_set('display_errors', 'On');
  error_reporting(E_ALL | E_STRICT);

  

//  set_error_handler("exception_error_handler");
  error_log("Esther edu: init");
  $method = $_SERVER['REQUEST_METHOD'];

  if (strcasecmp($method , 'POST') != 0 ){
    echo "<!DOCTYPE html><html><head></head><body><H1>error 405 {$method}</body></html>";
    http_response_code(405);
    return ;
  }


  $head = $_SERVER[ 'CONTENT_TYPE' ];
  $p = strpos($head , ';');
  error_log("Esther edu: processing data");

  if (strcmp(trim(substr($head , 0 , $p)," \t\n\r\0\x0B;"),'multipart/form-data') != 0) {
    echo "<!DOCTYPE html><html><head></head><body><H1>error 406 {$head}</body></html>";
    http_response_code(406);
    return ;
  }
  $p = strpos(strtolower($head) , 'boundary=' , $p);
  
  if ($p === false){
    echo "<!DOCTYPE html><html><head></head><body><H1>error 406 no boundary</body></html>";
    http_response_code(406);
    return;
  }
  $boundary = trim(substr($head , $p + strlen('boundary=')));
  $by =   "\r\n" . file_get_contents("php://input");
  error_log("Esther edu: data readed");
  $savedir = realpath(".". DIRECTORY_SEPARATOR);
  $prename = prename(firtsfile($savedir));
  ?>
<!DOCTYPE html>
<html>
<head></head>
<body>
<?php
  echo "local path: {$savedir}<br/>"; 
  $p = strpos($by , "\r\n--{$boundary}");
  if ($p === false) {
	echo "<H1>error 400 {$boundary} boundary not found</body></html>";
	return;
  }
  $b = 0 ;
  while ($p !== false){
    $b++;
    if ($b > 3) breaK;
    $p += 6 + strlen($boundary);
    $ha = array();
    $mz = 0;
    echo "boundary: {$boundary}<br>" ;
    while(($p2 = strpos($by , "\r\n" , $p)) !== false){
      $mz ++;
      if ($mz > 5) break;
      if (trim(substr($by,$p, $p2 -$p)) == false) {
		$p = $p2 + 2;
		break;
      }
      $p1 = strpos($by , ":" , $p);
      if ($p1 === false || trim(substr($by , $p , $p1 - $p)) == false) {
		$p2 = substr($by ,$p , $p2- $p1);
		echo "<H1>error 400 {$p2}</body></html>";
		http_response_code(400);
		return;
      }
      $ha[strtolower(trim(substr($by , $p , $p1 - $p)))] = trim(substr($by , $p1 + 1 , $p2 - $p1 - 1));
      $p = $p2 + 2;
    }
    if (!array_key_exists('content-disposition', $ha)){
      http_response_code(400);
      return;
    }
    print "<pre>";
    print_r($ha);
    print "</pre>";
    echo "<br><br>";
    $tmpa = explode(';' , $ha['content-disposition']);
    print "<pre>";
    print_r($tmpa);
    print "</pre>";
    
    $fi = array();
    foreach($tmpa as $w){
      If (strpos($w , "=") !== false) {
		$b1 = explode('=',$w);
		$fi[trim($b1[0])] = trim($b1[1] , "\"");
      }
    }
    if (!array_key_exists('content-type',$ha) && !array_key_exists('name', $fi) && $fi['name'] !== 'queryString' ){
		echo "<H1>error 400 {$ha['content-disposition']} not allowed</body></html>";
		http_response_code(400);
		return;      
    }
    error_log("Esther edu: array maked");
    print "<pre>" ;
     print_r($fi);
    print "</pre>";
    echo "<br><Br>";
    $p = $p2 + 2;
    $p1 =  strpos($by , "\r\n--{$boundary}",$p);
    $parcialbody = substr($by , $p , $p1 -$p);
    if (array_key_exists('name', $fi) && $fi['name'] == 'queryString'){
      if (is_dir($parcialbody) === false){
		mkdir($parcialbody , 0777,true);
      }
      $savedir = realpath($parcialbody);
      $prename = prename($savedir);
      echo "path: {$savedir} <br/>";
    }else switch($ha['content-type']) {
      case 'image/gif':
      case 'image/jpeg':
      case 'image/png':
      case 'image/svg+xml':
      	if (array_key_exists('content-transfer-encoding' , $ha ) && 
      		strtolower($ha['content-transfer-encoding']) == 'base64'){
	  		$parcialbody = base64_decode($parcialbody);
		}
		if (substr($savedir , -1) != DIRECTORY_SEPARATOR) $savedir .= DIRECTORY_SEPARATOR;
		$fil = $savedir . $prename . $fi['filename'];
		file_puts_content($fil ,$parcialbody , LOCK_EX); 
		$fil = $savedir . 'filelist.data';
		file_puts_content($fil , "{$prename}{$fi['filename']}\n\r" ,LOCK_EX | FILE_APPEND );
    }
    if (substr($by , $p1 + 4 + strlen($boundary) ,2) == '--') break;
    $p = $p1;
    unset($ha); 
  }
   
?>
end html
</body>
</html>