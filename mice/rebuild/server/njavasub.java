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

public class njavasub extends nbase
{
	/**
	 * This method initializes this thread with the new connection information
	 * @param ink The socket descriptor for the connection type stuff
	 * @param IN The buffered reader that reads data from the socket
	 * @param OUT The output stream writer that writes data to the socket
	 */
	public njavasub(Socket ink, BufferedReader IN, OutputStreamWriter OUT)
	{
		super(ink,IN,OUT);
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