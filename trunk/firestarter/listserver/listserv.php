<?php
// list server for firestarter

// get our configs
include('config.php');

$serverVers = 0.1;

echo "<html><head></head><body>";

/* Connecting, selecting database */
$link = mysql_connect($dbhost, $dbuser, $dbpasswd)
   or die("Could not connect to $dbhost : " . mysql_error());
echo "BZFlagFirestarter list server $serverVers : $dbhost : $hostpublicaddr<br>";

mysql_select_db($dbname ) or die("Could not select database : $dbname ");

echo "!beginlist<br><table border=\"1\" cellspacing=\"2\" cellpading=\"5\">";
echo "<tr><td>#id</td><td>#servername</td><td>#address</td><td>#port</td><td>#game</td><td>#version</td><td>#os</td><td>#maxplayers</td><td>#currentplayers<tr>";

$query = "SELECT * FROM servers";
$result = mysql_query($query) or die("Query failed : " . mysql_error());

while ($line = mysql_fetch_array($result, MYSQL_ASSOC)) {
   echo "<tr>";
   foreach ($line as $col_value) {
       echo "<td>$col_value</td> ";
   }
   echo "</tr>";
}
echo "</table>";
echo "!endlist<br>";
echo "</body></html>";

/* Closing connection */
mysql_close($link);

?>