<?php
// list server for firestarter
// get our configs
include('config.php');

echo "<html><head></head><body>";

/* Connecting, selecting database */
$link = mysql_connect($dbhost, $dbuser, $dbpasswd)
   or die("update failed::Could not connect to $dbhost : " . mysql_error());
echo "BZFlagFirestarter list server $serverVers : $dbhost : $hostpublicaddr<br>";
mysql_select_db($dbname )
	or die("update failed::Could not select database : $dbname ");

$now = date("U");
$query = "UPDATE servers SET servername='".$servername."',address='".$address."',port=".$port.",game='".$game."',version=".$version.",os='".$os."',maxplayers=".$maxplayers.",currentplayers=".$currentplayers.",lastupdate=".$now." WHERE id=".$id;
$result = mysql_query( $query )
	or die("update failed ::insert : " . mysql_error());

echo 'update processed<br>servername='.$servername.":".$port;
echo '<br>id='.$id;

echo "</body></html>";

/* Closing connection */
mysql_close($link);

?>