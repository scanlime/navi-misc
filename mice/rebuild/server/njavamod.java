/**
 * This class handles the java moderation client.
 * @author Brandon Smith
 * @version 2.0
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class njavamod extends nbase
{
	public amoderate moder;

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
		super(gonnection,IN,OUT);
		netdebug = true;
	}
	
	/**
	 * Since this is a subclass of Thread, it has to have a run() method
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void run()
	{
		if(!authenticate())
		{
			write("-ERR");
			closeConnection();
		}
		write("+OK");
		cliinit();
		modloop();
		closeConnection();
	}
	
	public void cliinit()
	{
		int i;
		int users = imain.database.interviewuser.length;
		write("" + users);
		for(i=0;i<users;i++)
			write(imain.database.interviewuser[i]);
	}
	
	
	public void modloop()
	{
		
	}
	
	/**
	 * This method handles the authentication of the client.
	 * @author Brandon Smith
	 * @version 2.0
	 * @return True on a successful authentication, false on failure
	 */
	public boolean authenticate()
	{
		String mykey, uname, pword;
		mykey = key.keygen();
		write(mykey);
		uname = key.decrypt(mykey,read());
		pword = key.decrypt(mykey,read());
		moder = new amoderate(imain.database);
		return moder.authenticate(imain.database,uname,pword);
	}
}