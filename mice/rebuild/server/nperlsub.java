/**
 * This class handles the perl submission client, CGI written in perl
 * @author Brandon Smith
 * @version 2.0
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class nperlsub extends nbase
{
	/** This is the head of the linked list of njavasub threads */
	public static nperlsub head;
	/**
	 * This method handles the submission and thread management stuff
	 */
	public static void go(Socket LINK, BufferedReader IN, OutputStreamWriter OUT)
	{
			head = new nperlsub();
			head.reinit(LINK,IN,OUT);
			head.start();
	}
	
	/** A reference to the next perl submission thread. */
	public nperlsub next;
	
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
	public void reinit(Socket ink, BufferedReader IN, OutputStreamWriter OUT)
	{
		link = ink;
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
	}
	
	/**
	 * This method washes the hex out of the stuff, cause I don't know how to do
	 * it correctly in perl.
	 * @param toclean The string that has the HTML string encoding stuff
	 * @author Brandon Smith
	 * @version 2.0
	 * @return A String that is all clean of that nasty HEX encoded ASCII
	 */
	private String hexwash(String toclean)
	{
		String toreturn, temp;
		int value = 32;
		StringTokenizer tokens = new StringTokenizer(toclean,"%",false);
		toreturn = tokens.nextToken();
		while(tokens.hasMoreTokens())
		{
			//squeeze the juice out of it!
			temp = tokens.nextToken();
			System.out.print(temp);
			System.out.println("  :" + temp.substring(2));
			value = asciiizer(temp.charAt(0)) * 16 + asciiizer(temp.charAt(1));
			toreturn = toreturn + (char) value + temp.substring(2);
		}
		return toreturn;
	}
	
	/**
	 * This method turns ASCII into a value that can be mistaken for HEX
	 * @param switcher The character that is an ASCII representation of HEX
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The numeric version, such that '0' becomes 0 and 'B' becomes 11
	 */
	private char asciiizer(char switcher)
	{
		char toreturn = 32;
		System.out.print("passed: " + switcher);
		if(switcher >= '0' && switcher <= '9')
			toreturn  = (char) (switcher - '0');
		if(switcher >= 'A' && switcher <= 'F')
			toreturn  = (char) ((switcher - 'A') + 10);
		System.out.println(" returning " + (int) toreturn);
		return toreturn;
	}
}