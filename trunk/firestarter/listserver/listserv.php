<?php
// list server for firestarter

// get our configs
include('config.php');
include('cleanup.php');

$serverVers = 0.1;

echo "<html><head></head><body>";

//make sure everyone is current;
cleanupdb(date("U"));

/* Connecting, selecting database */
$link = mysql_connect($dbhost, $dbuser, $dbpasswd)
   or die("Could not connect to $dbhost : " . mysql_error());
echo "BZFlagFirestarter list server $serverVers : $dbhost : $hostpublicaddr<br>";

mysql_select_db($dbname ) or die("Could not select database : $dbname ");

echo "!beginlist<br><table border=\"1\" cellspacing=\"2\" cellpading=\"5\">";
echo "<tr><td>#servername</td><td>#address</td><td>#port</td><td>#game</td><td>#version</td><td>#os</td><td>#maxplayers</td><td>#currentplayers</td><tr>";

// get a list of each field from servers and just dump every field
$query = "SELECT * FROM servers";
$result = mysql_query($query) or die("Query failed : " . mysql_error());

if ($simpleoutput)
{
	echo "list:".mysql_num_rows( $result )." ";
	while ($row = mysql_fetch_array($result, MYSQL_ASSOC))
	{
		echo '&'.$row[servername]; // servername
		echo '&'.$row[address]; // address
		echo '&'.$row[port]; // port
		echo '&'.$row[game]; // game
		echo '&'.$row[version]; // version
		echo '&'.$row[os]; // os
		echo '&'.$row[maxplayers]; // maxplayers
		echo '&'.$row[currentplayers]; // currentplayers
		echo "&& "; 
	}
}
else
{
	while ($row = mysql_fetch_array($result, MYSQL_ASSOC))
	{
		echo "<tr>";
	//	echo '<td>'.$row[id].'</td>'; // id
		echo '<td>'.$row[servername].'</td>'; // servername
		echo '<td>'.$row[address].'</td>'; // address
		echo '<td>'.$row[port].'</td>'; // port
		echo '<td>'.$row[game].'</td>'; // game
		echo '<td>'.$row[version].'</td>'; // version
		echo '<td>'.$row[os].'</td>'; // os
		echo '<td>'.$row[maxplayers].'</td>'; // maxplayers
		echo '<td>'.$row[currentplayers].'</td>'; // currentplayers
		//echo '<td>'.$row[lastupdate].'</td>'; // lastupdate
		echo "</tr>"; 
	}
	echo "</table>";
}

$timestamp = date("U");
echo "the stamp $timestamp<br>";
echo "!endlist<br>";
echo "</body></html>";

/* Closing connection */
mysql_close($link);

?>
