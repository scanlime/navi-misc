/**
 * This class handles the java interview client.
 * @author Brandon Smith
 * @version 2.0
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class njavaint extends nbase
{
	public ainterview view;
	
	public njavaint(Socket gonnection, BufferedReader IN, OutputStreamWriter OUT)
	{
		super(gonnection,IN,OUT);
	}
	
	public void run()
	{
		if(!authenticate())
		{
			write("-ERR");
			closeConnection();
			return;
		}
		write("+OK");
		runloop();
		closeConnection();
	}
	
	/**
	 */
	public void runloop()
	{
		
	}
	/**
	 * @return True when it works, false when it doesn't
	 */
	public boolean authenticate()
	{
		String mykey, rname, uname, pword;
		mykey = key.keygen();
		write(mykey);
		rname = read();
		uname = key.decrypt(mykey,read());
		pword = key.decrypt(mykey,read());
		view = new ainterview(rname);
		return view.intinit(imain.database,uname,pword);
	}
}