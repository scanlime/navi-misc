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
	/** This is the head of the linked list of njavasub threads */
	public static njavasub head;
	/**
	 * This method handles the submission and thread management stuff
	 */
	public static void go(Socket LINK, BufferedReader IN, OutputStreamWriter OUT)
	{
			head = new njavasub();
			head.reinit(LINK,IN,OUT);
			head.start();
	}
	
	/** A reference to the next java submission thread. */
	public njavasub next;
	
	/**
	 * This is an empty constructor for the initial initialization
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public njavasub(){}
	
	/**
	 * This method initializes this thread with the new connection information
	 * @param gonnection The socket descriptor for the connection type stuff
	 * @param IN The buffered reader that reads data from the socket
	 * @param OUT The output stream writer that writes data to the socket
	 */
	public void reinit(Socket gonnection, BufferedReader IN, OutputStreamWriter OUT)
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