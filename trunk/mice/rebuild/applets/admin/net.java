/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class should be pretty easy to understand.  This is a static class, so
 * the applet can only have one connection at a time, which should be fine.
 * It could be pretty easily modified to be a non-static class if the need came.
 * @author Brandon Smith
 * @version 2.0
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class net
{
	private static Socket link;
	private static BufferedReader in;
	private static OutputStreamWriter out;
	
	/**
	 * This method opens a connection and takes care of all the pre/post 
	 * conditions for read and write.
	 * @param host The host to connect to.
	 * @param port The port on the host to connect to.
	 * @author Brandon Smith
	 * @version 2.0
	 * @return True on a successful connection, False
	 */
	public static boolean openConnection(String host, int port)
	{
		try 
		{
	    link = new Socket(host,port);
	    in = new BufferedReader(new InputStreamReader(link.getInputStream()));
	    out = new OutputStreamWriter(link.getOutputStream());
		}
		catch(IOException exception) {
	    return false;
		}
		return true;
	}
    
	/**
	 * This method closes the connection
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void closeConnection()
	{
		try 
		{
	    link.close();
		}
		catch(IOException exception) 
		{
		}
	}

	/**
	 * This method reads a string from the socket and returns it
	 * @author Brandon Smith
	 * @version 2.0
	 * @return A String with the network newline
	 */
	public static String read()
	{
		String toreturn = null;
		try
		{
	    toreturn = in.readLine();
		}
		catch(IOException exception)
		{
	    closeConnection();
		}
		return toreturn;
	}
	
	/**
	 * This method writes a string to the predescribed socket.
	 * @param tosend The string to send
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void write(String tosend)
	{
		try
		{
	    out.write(tosend + "\r\n");
	    out.flush();
		}
		catch(IOException exception)
		{
	    closeConnection();
		}
	}
}