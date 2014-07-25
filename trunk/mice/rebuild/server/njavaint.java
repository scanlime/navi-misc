/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

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
	//The abstract interview class associated with this interview client
	private ainterview view;
	
	/**
	 * This method initializes an interview client.
	 * @param gonnection The Socket that this connects to the client over.
	 * @param IN The BufferedReader that reads stuff in.
	 * @param OUT The OutputStreamWriter for spitting stuff out over the socket.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public njavaint(Socket gonnection, BufferedReader IN, OutputStreamWriter OUT)
	{
		super(gonnection,IN,OUT);
	}
	
	/**
	 * This is the main method for handling the java interview client.
	 * @author Brandon Smith
	 * @version 2.0
	 */
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
	 * This is what actually runs the thing.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void runloop()
	{
		String in;
		String mine;
		mine = null;
		while(true)
		{
			in = read();
			switch(in.charAt(0))
			{
			case 'q': //quit
				return;
			case 'r': //reject
				view.reject();
				write(view.getQuestion());
				break;
			case 's': //submit
				view.answer(read());
				write(view.getQuestion());
				break;
			case 'g': //get a question
				write(view.getQuestion());//This generates two reads for the client too.
				break;
			}
		}
	}
	
	/**
	 * This method pulls the initizization information from the client.
	 * @author Brandon Smith
	 * @version 2.0
	 * @return True when it works, false when it doesn't
	 */
	public boolean authenticate()
	{
		String mykey, rname, uname, pword;
		mykey = key.keygen();
		write(mykey);
		rname = read();
		uname = key.decrypt(mykey,read()).toUpperCase();
		pword = key.decrypt(mykey,read());
		view = new ainterview(rname);
		return view.intinit(imain.database,uname,pword);
	}
}