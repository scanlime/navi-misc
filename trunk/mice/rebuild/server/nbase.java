/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * A superclass for the services implemented (submission, administration, moderation and answering).
 * It has all the network stuff for the client handlers so as to make them cleaner :)
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class nbase extends Thread
{
	/** The socket descriptor */
	protected Socket link;
	/** The buffered reader that is associated with the socket */
	protected BufferedReader in;
	/** The stream writer that is associcated with the socket */
	protected OutputStreamWriter out;
	/** Sets the network debug.  True for on, False for off */
	protected boolean netdebug = false;
	
	/**
	 * This is an empty constructor for those who do it themselves.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public nbase(){}

	/**
	 * This is the default constructor for setting the network stuff up.
	 * @param gonnection the socket descriptor passed in.
	 * @param IN the buffered reader thingy for input.
	 * @param OUT the writer thingy for output.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public nbase(Socket gonnection, BufferedReader IN, OutputStreamWriter OUT)
	{
		super();
		link = gonnection;
		in = IN;
		out = OUT;
	}
	
	/**
	 * This method should be overwritten by the network clients that handle it
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void run()
	{
		closeConnection();
	}
	
	/**
	 * This method reads a single line from a socket.
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The string read from the socket.
	 */
	protected String read()
	{
		String toreturn = null;
		try
		{
			toreturn = in.readLine();
		}
		catch(IOException exception)
		{
			System.out.println("Error in recieving: " + exception);
			closeConnection();
		}
		if(netdebug) System.out.println("C: " + toreturn);
		return toreturn;
	}

	/**
	 * Writes a string to the socket.
	 * @param tosend The string to be sent
	 * @author Brandon Smith
	 * @version 2.0
	 */
	protected void write(String tosend)
	{
		try
		{
			out.write(tosend + "\r\n");
			if(netdebug) System.out.println("S: " + tosend);
	    out.flush();
		}
		catch(IOException exception)
		{
	    System.out.println("Error in sending:" + exception);
	    closeConnection();
		}
	}

	/**
	 * Closes the connection with the connected host
	 * @author Brandon Smith
	 * @version 2.0
	 */
	protected void closeConnection()
	{
		try 
		{
			link.close();
		}
		catch(IOException exception) 
		{
			System.out.println(exception);
		}
		if(netdebug) System.out.println("Connection Closed");
	}
}
