/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class handles the chat based interview clients.
 * @author Brandon Smith
 * @version 2.0
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class nchatint extends nbase
{
	public ainterview view;

	/**
	 * This is an empty constructor for the initial initialization
	 * @param gonnection The socket descriptor for the connection type stuff
	 * @param IN The buffered reader that reads data from the socket
	 * @param OUT The output stream writer that writes data to the socket
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public nchatint(Socket gonnection, BufferedReader IN, OutputStreamWriter OUT)
	{
		super(gonnection,IN,OUT);
		//netdebug = true;
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
			closeConnection();
			return;
		}
		runloop();
		closeConnection();
	}
	
	/**
	 * This method handles the bread and butter of the connection.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	private void runloop()
	{
		String cmd;
		while(true)
		{
			cmd = read();
			switch(cmd.charAt(0))
			{
			case 'a'://answer a question
				view.answer(read());
				break;
			case 'r'://reject a question
				view.reject();
				break;
			case 'g'://
				cmd = view.getQuestion();
				if(cmd.compareTo("No Questions\r\nAt This Time") == 0)
				{
					write("-ERR");
					
					/*
					while(cmd.compareTo("No Questions\r\nAt This Time") == 0)
					{
						try{
							Thread.sleep(20000);
						}
						catch(Exception e){}
						cmd = view.getQuestion();
					}
					*/
				}
				else
				{
					write("+OK");
					write(cmd);
				}
				break;
			}
		}
	}
	
	/**
	 * This method handles the authentication of the client
	 * @author Brandon Smith
	 * @version 2.0
	 */
	private boolean authenticate()
	{
		String uname, pword, rname;
		String mykey = key.keygen();
		write(mykey);
		uname = "AIM0" + read();
		pword = key.decrypt(mykey,read());
		rname = read();
		view = new ainterview(rname);
		return view.intinit(imain.database,uname,pword);
	}
}
