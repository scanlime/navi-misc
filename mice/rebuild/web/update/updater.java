/**
 * This handles the updating of the web page.
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class updater
{
	public static BufferedReader in;
	public static OutputStreamWriter out;
	public static Socket connection;
	public static element head = null;
	
	/**
	 * The Main function from which all function calls come forth.
	 * @param args The arguments from the command line, ignored.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void main(String[] args)
	{
		startServer(8081);
		element.strgs = new reader("template.html");
		mainLoop(read());
		closeConnection();
	}

	/**
	 * This method starts the server, and accepts the first connection.
	 * @param port The port that the server runs on.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void startServer(int port)
	{
		try
		{
			ServerSocket server = new ServerSocket(port);
			connection = server.accept();
			in = new BufferedReader(new InputStreamReader(connection.getInputStream()));
			out = new OutputStreamWriter(connection.getOutputStream());
			server.close();
		}
		catch(IOException exception)
		{
			System.out.println(exception);
			System.exit(0);
		}
	}

	/**
	 * This method is what handles the transactions and stuff.
	 * @param filename The filename that gets written to for the HTML page
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void mainLoop(String filename)
	{
		String cmd;
		while(true)
		{
			cmd = read();
			switch(cmd.charAt(0))
			{
			case 'a':
				break;
			case 'n':
				break;
			case 'q':
				return;
			}
		}
	}

	/**
	 * This method writes what is in the element array to the page.
	 * @param filename The file that gets written into
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void updatePage(String filename)
	{
		try 
		{ 
			PrintWriter outfile = new PrintWriter(new BufferedWriter(new FileWriter(filename)));
			outfile.close();
		}
		catch(IOException exception)
		{
			write("-ERR couldn't open page for writing. " + exception.toString());
			System.exit(1);
		}
		write("+OK");
	}

/**********************************************************************************/
	//just the typical network single line read/write stuff, to make it all happy and good
	
	/**
	 * This method reads a string from the socket
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The string read from the socket.
	 */
	public static String read()
	{
		String toreturn = null;
		try {
			toreturn = in.readLine();
		}
		catch(IOException exception) {
			System.out.println("Error in recieving: " + exception);
			closeConnection();
		}
		return toreturn;
	}

	/**
	 * This method writes a string to the socket.
	 * @param tosend The string being sent to the socket.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void write(String tosend)
	{
		try	{
			out.write(tosend + "\r\n");
			out.flush();
		}
		catch(IOException exception){
			System.out.println("Error in sending:" + exception);
			closeConnection();
		}	
	}
	
	/**
	 * This method closes the connection
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void closeConnection()
	{
		try{
			connection.close();
		}
		catch(IOException exception){
			System.out.println(exception);
		}
	}
}	
