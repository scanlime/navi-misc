<?php
// list server for firestarter
// get our configs
include('config.php');

echo "<html><head></head><body>";

/* Connecting, selecting database */
$link = mysql_connect($dbhost, $dbuser, $dbpasswd)
   or die("remove failed::Could not connect to $dbhost : " . mysql_error());
echo "BZFlagFirestarter list server $serverVers : $dbhost : $hostpublicaddr<br>";
mysql_select_db($dbname )
	or die("remove failed::Could not select database : $dbname ");
	
$deletequery = "DELETE FROM servers WHERE id = ".$id;
mysql_query($deletequery)
	or die("cleanupdb::delete failed : " . mysql_error());

echo 'remove processed<br>servername='.$servername.":".$port;
echo '<br>id='.$id;

echo "</body></html>";

/* Closing connection */
mysql_close($link);

?>


			
