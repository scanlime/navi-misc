/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This is a static class that allows for a quick and easy connections and 
 * socket IO.
 * @author Brandon Smith
 * @version 2.0
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class net
{
	/** The Socket descriptor for the link. */
	public static Socket link;
  /** The reader for the read() method. */  
	public static BufferedReader in;
	/** The writer for the write() method. */
	public static OutputStreamWriter out;
	
	/**
	 * This method initializes a connection with a server
	 * @param host A string that resolves to a host out there.
	 * @param port The port that the server is running on, on the host.
	 * @author Brandon Smith
	 * @version 2.0
	 * @return True on a successful connection, false on a failed connection.
	 */
	public static boolean openConnection(String host, int port)
	{
		try 
		{
	    link = new Socket(host,port);
	    in = new BufferedReader(new InputStreamReader(net.link.getInputStream()));
	    out = new OutputStreamWriter(net.link.getOutputStream());
		}
		catch(IOException exception) 
		{
	    return false;
		}
		return true;
	}
	
	/**
	 * This method closes the connection to the connected to host
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
	 * This method reads a string from the socket
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The string, fresh from the socket.
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
	 * This method writes data to the socket it was initialized with.
	 * @param tosend The string of text that needs to be sent.
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