<?php
header ("Content-Type: text/xml");
echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n";
$content .= "<!DOCTYPE rss [<!ENTITY % HTMLlat1 PUBLIC \"-//W3V//ENTITIES Latin 1 for XHTML//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml-lat1.ent\">]>\n";
$content .= "<rss version=\"0.92\" xml:base=\"http://firestarter.bakadigital.com/\">\n";
$content .= "<channel>\n";
$content .= "  <title>Firestarter list server</title>\n";
$content .= "  <description>Firestarter List Server Feed</description>\n";
$content .= "  <link>http://firestarter.bakadigital.com/list/rss-gen.php</link>\n";
$content .= "  <language>en</language>\n";

// rss generator for listserver.
// get config
include('config.php');

$link = mysql_connect ($dbhost, $dbuser, $dbpasswd) or die ("rss-gen failed: Cannot connect to database server $dbhost: error: " . mysql_error());
mysql_select_db ($dbname) or die ("rss-gen failed: Cannot connect to database $dbname: error: " . mysql_error());

$query = "select * from servers";
$result = mysql_query ($query) or die ("Rss-gen query failed: error " . mysql_error());

    // Generate the lists while data exists
    // Append to $content
    while ($row = mysql_fetch_array ($result)) {
    	$content .= "  <item>\n";
		$content .= "    <title>$row[servername]</title>\n";
		$content .= "    <id>$row[id]</id>\n";
		$content .= "    <servername>$row[servername]</servername>\n";
		$content .= "    <address>$row[address]</address>\n";
		$content .= "    <port>$row[port]</port>\n";
		$content .= "    <game>$row[game]</game>\n";
		$content .= "    <version>$row[version]</version>\n";
		$content .= "    <os>$row[os]</os>\n";
		$content .= "    <maxplayers>$row[maxplayers]</maxplayers>\n";
		$content .= "    <currentplayers>$row[currentplayers]</currentplayers>\n";
		$content .= "    <lastupdate>$row[lastupdate]</lastupdate>\n";
		$content .= "  </item>\n";
    }
	// Finish off the channel and the RSS feed.
    $content .= "  </channel>\n";
    $content .= "</rss>\n";
	
// Display the content
	echo ($content);

// Free the result and close the db link
mysql_free_result ($result);
mysql_close($link);
?>
