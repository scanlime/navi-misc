/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

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
	public nchatup(aupdate register)
	{
		register.add(this);
	}
	
	/**
	 * This method initializes a connection with the AIM Bot
	 * @param host The hostname of the server with the bot.
	 * @param port The port the bot server is running on.
	 * @author Brandon Smith
	 * @version 2.0
	 * @return false on failure, true on success.
	 */
	public boolean connect(String host, int port)
	{
		try//connect to the AIM update server
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
	 * This method initializes the bot server, and must be called after connect.
	 * @param name An array of usernames.
	 * @param pass A parallel array of passwords.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void init(String[] name, String[] pass)
	{
		int i;
		String mykey = read();
		for(i=0;i<name.length;i++)
		{
			if(name[i].startsWith("AIM0"))
			{
				write(name[i].substring(4));
				write(key.encrypt(mykey,pass[i]));
			}
		}
		write(".");
	}
	
	/**
	 * This method sends the update command to the AIM bot server.
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
	 * This method sends a notice to everyone on the AIM bot server.
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
	 * This method makes the AIM Bot server quit, like at the end of the chat.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void quit()
	{
		write("quit");
		closeConnection();
	}
}