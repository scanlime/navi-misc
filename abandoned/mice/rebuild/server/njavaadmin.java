/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

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
	 * @param ink The socket descriptor for the connection type stuff
	 * @param IN The buffered reader that reads data from the socket
	 * @param OUT The output stream writer that writes data to the socket
	 */
	public njavaadmin(Socket ink, BufferedReader IN, OutputStreamWriter OUT)
	{
		super(ink,IN,OUT);
		//netdebug = true;
	}
	
	/**
	 * Since this is a subclass of Thread, it has to have a run() method
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void run()
	{
		int moderators, interviews;
		if(!auth()) return;
		switch(aadmin.confstate)
		{
		case 0:
			moderators = mods();
		case 1:
			interviews = ints();
			dbaseinit();
		case 2:
			webinit();
		case 3:
			aiminit();
		case 4:
			noticeloop();
			closeConnection();
		}
	}
	
	/**
	 * This method handles the notice loop type stuff.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void noticeloop()
	{
		String cmd;
		while(true)
		{
			cmd = read();
			if(cmd.compareTo("notice") == 0)
				imain.database.updater.sendNotice(read());
			if(cmd.compareTo("quit") == 0)
				return;
		}
	}
	
	/**
	 * This handles the bits of communication that set up the web update services.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void webinit()
	{
		nhtmlup web = new nhtmlup(imain.database.updater);
		String host, filename;
		int port;
		host = read();
		port = Integer.parseInt(read());
		filename = read();
		/* Start the web service client thingy */
		if(!web.connect(host,port))
		{
			write("-ERR No server on that host/port.");
			return;
		}
		if(!web.init(filename))
		{
			write("-ERR Specified filename for the HTML file was not writeable.");
			return;
		}
		write("+OK");
		aadmin.confstate = 3;
	}
	
	/**
	 * This handles the bits of communication that set up the AIM services.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void aiminit()
	{
		nchatup aim = new nchatup(imain.database.updater);
		String host;
		int port;
		host = read();
		port = Integer.parseInt(read());
		/* Start the aim service client thingy */
		if(!aim.connect(host,port))
		{
			write("-ERR");
			return;
		}
		aim.init(aadmin.iuns,aadmin.ipas);
		write("+OK");
		aadmin.confstate = 4;
	}
	
	/**
	 * This just initializes the other users and their passwords
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void dbaseinit()
	{
		imain.database = new qbase(aadmin.iuns.length, aadmin.muns.length);
		imain.database.moduser = aadmin.muns;
		imain.database.modpass = aadmin.mpas;
		imain.database.interviewuser = aadmin.iuns;
		imain.database.interviewpass = aadmin.ipas;
		asubmit.init(imain.database);
	}
	
	/**
	 * This method handles the acceptance of the interview peoples
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The number of people being interviewed.
	 */
	public int ints()
	{
		String newkey = key.keygen();
		int interviews,i;
		interviews = Integer.parseInt(read());
		write(newkey);
		aadmin.iuns = new String[interviews];
		aadmin.ipas = new String[interviews];
		for(i=0;i<interviews;i++)
		{
			aadmin.iuns[i] = key.decrypt(newkey,read()).toUpperCase();
			aadmin.ipas[i] = key.decrypt(newkey,read());
		}
		write("+OK");
		aadmin.confstate = 2;
		return interviews;
	}
	
	/**
	 * This method handles the acceptance of the moderators
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The number of moderators involved in the chat.
	 */
	public int mods()
	{
		String newkey = key.keygen();
		int moderators, i;
		moderators = Integer.parseInt(read());
		write(newkey);
		aadmin.muns = new String[moderators];
		aadmin.mpas = new String[moderators];
		for(i=0;i<moderators;i++)
		{
			aadmin.muns[i] = key.decrypt(newkey,read());
			aadmin.mpas[i] = key.decrypt(newkey,read());
		}
		write("+OK");
		aadmin.confstate = 1;
		return moderators;
	}
	
	/**
	 * This method authenticates the administrator
	 * @author Brandon Smith
	 * @version 2.0
	 * @return true on a successful authentication, false on failure
	 */
	public boolean auth()
	{
		String mykey = key.keygen();
		write(mykey);
		String user = key.decrypt(mykey,read());
		String pass = key.decrypt(mykey,read());
		if(user.compareTo(aadmin.user) == 0 && pass.compareTo(aadmin.pass) == 0) 
			write("+OK");
		else
		{
			write("-ERR");
			closeConnection();
			return false;
		}
		write(""+aadmin.confstate);
		return true;
	}
}
