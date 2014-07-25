/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

/**
 * This class is the main class, handling initialization and the like
 * @author Brandon Smith
 * @version 2.0
 */
public class imain
{
	/**
	 * This is the main static class thingy that gets the server going
	 * @param args The string of arguments from the command line
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void main(String[] args)
	{
		imain listener = new imain(8080);
		listener.handle();
	}

	/** The server socket that listens for stuff */
	private ServerSocket server = null;

	/** The question database */
	public static qbase database = null;
	
	/**
	 * This method constructs the server, and handles the basic initialization
	 * @param port The port that the server listens on.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public imain(int port)
	{
		try 
		{
			server = new ServerSocket(port);
		}
		catch(IOException exception)
		{
			System.out.println(exception);
			System.exit(0);
		}
	}
	
	/**
	 * This method is the method that actually handles connections.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void handle()
	{
		Socket link;
		BufferedReader in;
		OutputStreamWriter out;
		try
		{
			while(true)
			{
				link = server.accept();
				in = new BufferedReader(new InputStreamReader(link.getInputStream()));
				out = new OutputStreamWriter(link.getOutputStream());
			
				switch(clitype(read(link,in)))
				{
				case -10:
					new njavaadmin(link,in,out).start();
					break;
				case -1:
					closeConnection(link);
					break;
				case 1:
					new njavasub(link,in,out).start();
					break;
				case 2:
					new nperlsub(link,in,out).start();
					break;
				case 3:
					new nchatsub(link,in,out).start();
					break;
				case 4:
					new njavamod(link,in,out).start();
					break;
				case 5:
					new njavaint(link,in,out).start();
					break;
				case 6:
					new nchatint(link,in,out).start();
					break;
				}
			}
		}
		catch(Exception e){}
	}
	
	/**
	 * This method converts a string from the socket to an integer representing
	 * the type of client it is.
	 * @param it The string from the socket
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The numeric code for the client
	 */
	public int clitype(String it)
	{
		System.out.println(it);
		if(it == null) return -1;
		if(it.compareTo("jsub") == 0) return 1; //Java Submission
		if(it.compareTo("psub") == 0) return 2; //Perl Submission (CGI)
		if(it.compareTo("csub") == 0) return 3; //Chat Submission
		if(it.compareTo("jmod") == 0) return 4; //Java Moderation
		if(it.compareTo("jint") == 0) return 5; //Java Interview
		if(it.compareTo("cint") == 0) return 6; //Chat Interview
		
		if(it.compareTo("admin") == 0) return -10;
		return -1;
	}
	
	/* Network glue that I can't seem to find a better home for */
	
	/**
	 * This method reads a line from the socket and returns it.
	 * @param link The socket so it can be closed in the event of an error
	 * @param in The buffered reader that describes the socket
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The string from the socket
	 */
	public String read(Socket link, BufferedReader in)
	{
		String toreturn = null;
		try
		{
			toreturn = in.readLine();
		}
		catch(IOException exception)
		{
			closeConnection(link);
		}
		return toreturn;
	}
	
	/**
	 * This method closes the connection in the event of a rogue client
	 * @param link The socket so the method knows what to close
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void closeConnection(Socket link)
	{
		try
		{
			link.close();
		}
		catch(IOException exception){}
	}
}
