<?php

// list server for firestarter

$dbhost = 'localhost';
$dbname = 'fslistdb';
$dbuser = 'xxxxx';
$dbpasswd = 'xxxxx';

$serverVers = 0.1;

echo "<html><head></head><body>";
echo "BZFlagFirestarter list server $serverVers<br>";
echo "!beginlist<br>";
echo "#servername #address #port #game #version #os #maxplayers #currentplayers<br>";

echo "!endlist<br>";
echo "</body></html>";

?>