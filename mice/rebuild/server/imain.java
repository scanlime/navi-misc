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
	private ServerSocet server = null;
	
	/**
	 * This is the main static class thingy that gets the server going
	 * @param args The string of arguments from the command line
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void main(String[] args)
	{
		System.out.println("Hello World");
		imain listener = new imain(8080);
	}
	
	/**
	 * This method constructs the server, and handles the basic initialization
	 * @param port The port that the server listens on.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public imain(int port)
	{
		BufferedReader in;
		OutputStreamWriter out;
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
		while(true)
		{
			Socket connection = server.accept();
			in = new BufferedReader(new InputStreamReader(connection.getInputStream()));
			out = new OutputStreamWriter(connection.getOutputStream());
		}
	}
}