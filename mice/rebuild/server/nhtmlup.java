/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class handles communication to the HTML page server thing.
 * @author Brandon Smith
 * @version 2.0
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class nhtmlup extends nbase implements aupdatei
{
	/**
	 * This method initializes this thread with the new connection information
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public nhtmlup(aupdate register)
	{
		register.add(this);
	}
	
	/**
	 * This method initializes a connection with the html updater
	 * @param host The hostname of the server with the bot.
	 * @param port The port the bot server is running on.
	 * @author Brandon Smith
	 * @version 2.0
	 * @return false on failure, true on success.
	 */
	public boolean connect(String host, int port)
	{
		try
		{
			link = new Socket(host,port);
			in = new BufferedReader(new InputStreamReader(link.getInputStream()));
			out = new OutputStreamWriter(link.getOutputStream());
		}
		catch(IOException e)
		{
			return false;
		}
		return true;
	}
	
	/**
	 * This method initializes the html server, and must be called after connect.
	 * @param filename The filename that 
	 * @author Brandon Smith
	 * @version 2.0
	 * @return true on a successful writing of the server, false on failure
	 */
	public boolean init(String filename)
	{
		String get;
		write(filename);
		get = read();
		if(get.charAt(0) == '-') return false;
		return true;
	}
	
	/**
	 * This method sends the update command to the html update server.
	 * @param tosend The question that just got answered.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void update(question tosend)
	{
		write("answer");
		write(tosend.toNet());
	}
	
	/**
	 * This method sends a notice to the html update server.
	 * @param tosend The String thats being sent to everyone
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void notice(String tosend)
	{
		write("notice");
		write(tosend);
	}
	
	/**
	 * This method makes the html update server quit, like at the end of the chat.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void quit()
	{
		write("quit");
		closeConnection();
	}
}