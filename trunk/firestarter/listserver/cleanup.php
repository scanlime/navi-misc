<?php

// get our configs


function cleanupdb($thetime)
{
	include('config.php');
	/* Connecting, selecting database */
	$link = mysql_connect($dbhost, $dbuser, $dbpasswd)
		or die("cleanupdb::Could not connect to $dbhost : " . mysql_error());
	
	mysql_select_db($dbname ) or die("Could not select database : $dbname ");

	
	//get a list of each field from servers and kill any that have old update times
	$query = "SELECT * FROM servers";
	$result = mysql_query($query)
		or die("Query failed : " . mysql_error());
	
	while ($line = mysql_fetch_array($result, MYSQL_ASSOC))
	{
		if ($thetime - $line[lastupdate] > 5*60 )	// if it's over 5 min
		{			
			$deletequery = "DELETE FROM servers WHERE id = ".$line[id];
			mysql_query($deletequery)
				or die("cleanupdb::delete failed : " . mysql_error());
		}
	}
	/* Closing connection */
	mysql_close($link);
}
?>