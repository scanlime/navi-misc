<?php
// list server for firestarter

// get our configs
include('config.php');

$serverVers = 0.1;

echo "<html><head></head><body>";
echo "BZFlagFirestarter list server $serverVers : $hostpublicaddr<br>";
echo "!beginlist<br>";
echo "#servername #address #port #game #version #os #maxplayers #currentplayers<br>";

echo "!endlist<br>";
echo "</body></html>";

?>