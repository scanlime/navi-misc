/**
 * This class handles the java submission client.
 * @author Brandon Smith
 * @version 2.0
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class nchatup extends nbase implements aupdatei
{
	/**
	 * This method initializes this thread with the new connection information
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public nchatup(String host, int port)
	{
	}
	
	public boolean connect(String host, int port)
	{
		try//connect to the AIM update server
		{
			link = new Socket(host,port);
			in = new BufferedReader(new InputStreamReader(link.getInputStream()));
			out = new OutputStreamWriter(link.getOutputStream());
		}
		catch(IOException e){}
	}
	
	public void update(question tosend){}
	public void notice(String tosend){}
	public void quit(){}
}