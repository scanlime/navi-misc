<?php
// list server for firestarter
// get our configs
include('config.php');

echo "<html><head></head><body>";

/* Connecting, selecting database */
$link = mysql_connect($dbhost, $dbuser, $dbpasswd)
   or die("add failed::Could not connect to $dbhost : " . mysql_error());
echo "BZFlagFirestarter list server $serverVers : $dbhost : $hostpublicaddr<br>";
mysql_select_db($dbname )
	or die("add failed::Could not select database : $dbname ");

$now = date("U");
$query = "INSERT INTO servers (servername,address,port,game,version,os,maxplayers,currentplayers,lastupdate) values ( '$servername','$address','$port','$game','$version','$os','$maxplayers','$currentplayers','$now');";
$result = mysql_query( $query )
	or die("add failed ::insert : " . mysql_error());

$lastID = mysql_insert_id();

echo 'add processed<br>servername=' . $servername . ':' . $port;
echo '<br>id=' . $lastID.'&nbsp';

echo '</body></html>';

/* Closing connection */
mysql_close($link);

?>