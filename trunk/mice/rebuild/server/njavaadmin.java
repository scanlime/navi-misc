/**
 * This class handles the administration client.
 * @author Brandon Smith
 * @version 2.0
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class njavaadmin extends nbase
{
	/**
	 * This method constructs the administrator interface thingy
	 * @param gonnection The socket descriptor for the connection type stuff
	 * @param IN The buffered reader that reads data from the socket
	 * @param OUT The output stream writer that writes data to the socket
	 */
	public njavaadmin(Socket ink, BufferedReader IN, OutputStreamWriter OUT)
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
		closeConnection();
	}
}