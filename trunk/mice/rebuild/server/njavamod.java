/**
 * This class handles the java submission client.
 * @author Brandon Smith
 * @version 2.0
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class njavamod extends nbase
{
	/**
	 * This is an empty constructor for the initial initialization
	 * @param gonnection The socket descriptor for the connection type stuff
	 * @param IN The buffered reader that reads data from the socket
	 * @param OUT The output stream writer that writes data to the socket
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public njavamod(Socket gonnection, BufferedReader IN, OutputStreamWriter OUT)
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
	}
}