/**
 * This class handles the java submission client.
 * @author Brandon Smith
 * @version 2.0
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class nchatup extends nbase
{
	/**
	 * This method initializes this thread with the new connection information
	 * @param host The host that the chat bot is running on
	 * @param port The port number that the server is running on
	 */
	public nchatup(String host, int port)
	{
		try//connect to the AIM update server
		{
			link = new Socket(host,port);
			in = new BufferedReader(new InputStreamReader(link.getInputStream()));
			out = new OutputStreamWriter(link.getOutputStream());
		}
		catch(IOException e){}
		//get the number
	}
	
	/**
	 * Since this is a subclass of Thread, it has to have a run() method
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void run()
	{
	}
}