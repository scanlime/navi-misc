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
	/**
	 * This method initializes this thread with the new connection information
	 * @param ink The socket descriptor for the connection type stuff
	 * @param IN The buffered reader that reads data from the socket
	 * @param OUT The output stream writer that writes data to the socket
	 */
	public nperlsub(Socket ink, BufferedReader IN, OutputStreamWriter OUT)
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
		String name, postn, question;
		name = read();
		postn = read();
		question = read();
		name = name + " (" + postn + ")";
		name = hexwash(name);
		question = hexwash(question);
		asubmit.submit(name,question);
		closeConnection();
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
		String toreturn = "", temp;
		int value = 32;
		StringTokenizer tokens = new StringTokenizer(toclean,"%",false);
		if(tokens.hasMoreTokens())
			toreturn = tokens.nextToken();
		while(tokens.hasMoreTokens())
		{
			//squeeze the juice out of it!
			temp = tokens.nextToken();
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
		if(switcher >= '0' && switcher <= '9')
			toreturn  = (char) (switcher - '0');
		if(switcher >= 'A' && switcher <= 'F')
			toreturn  = (char) ((switcher - 'A') + 10);
		return toreturn;
	}
}