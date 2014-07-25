/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class handles the chat based submission client(s).
 * @author Brandon Smith
 * @version 2.0
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class nchatsub extends nbase
{
	/**
	 * This is an empty constructor for the initial initialization
	 * @param gonnection The socket descriptor for the connection type stuff
	 * @param IN The buffered reader that reads data from the socket
	 * @param OUT The output stream writer that writes data to the socket
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public nchatsub(Socket gonnection, BufferedReader IN, OutputStreamWriter OUT)
	{
		link = gonnection;
		in = IN;
		out = OUT;
	}
	
	/**
	 * Since this is a subclass of Thread, it has to have a run() method
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void run()
	{
		String name, question;
		name = read();
		question = read();
		asubmit.submit(name,question);
		closeConnection();
	}
}