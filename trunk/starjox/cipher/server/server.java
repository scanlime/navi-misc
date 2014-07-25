/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/**
* This class is for opening a single server port, and forwarding it elsewhere.
 * @author Brandon Smith
 * @version 1.0
 */

import java.net.*;
import java.io.*;
import java.util.*;

public class server extends Thread
{
	/** The external port (open to the rest of the world). */
	public int eport;
	
	/** The proxy server's listening socket. */
	public ServerSocket srver;
	
	/**
		* This sets up the tcp forwarder to run with the info given.
	 * @param externalport The port to listen on from the outside world.
	 * @author Brandon Smith
	 * @version 1.0
	 */
	public server(int externalport)
	{
		eport = externalport;
	}
	
	/**
		* This method starts the server, and listens for connections
	 * @author Brandon Smith
	 * @version 1.0
	 */
	public void run()
	{
		startserver();
		listen();
	}
	
	/**
		* This method is what handles the listening and initializing connections
	 * @author Brandon Smith
	 * @version 1.0
	 */
	public void listen()
	{
		Socket sout = null;
		Socket sin = null;
		OutputStream out = null;
		InputStream in = null;
		key nukey;
		while(true) 
		{
	    try
		{
			// get the sockets
			sout = srver.accept();
			nukey = sendKey(sout);
			sin = initConnection(sout);
			
			//slot a into tab a
			out = sout.getOutputStream();
			in = sin.getInputStream();
			new epipe(in,out,nukey).start();
			
			//slot b into tab b
			out = sin.getOutputStream();
			in = sout.getInputStream();
			new dpipe(in,out,nukey).start();
		}
			
	    catch(Exception e)
		{
				System.out.println(e);
		}
		}
	}
	
	public key sendKey(Socket client) throws Exception
	{
		int i;
		key foo = new key();
		int[] kei = foo.getKey();
		OutputStream out = client.getOutputStream();
		
		for(i=0;i<256;i++)
		{
	    out.write(kei[i]);
	    out.flush();
		}
		return foo;
	}
	
	public Socket initConnection(Socket client) throws Exception
	{
		String host = "";
		String tport = "";
		int iport;
		int temp;
		
		InputStream in = client.getInputStream();
		
		/* get the hostname (probably localhost) */
		temp = in.read();
		
		while(temp != 0)
		{
	    host = host + (char) temp;
	    temp = in.read();
		}
		
		/* get the desired port, 110 for POP3, etc..*/
		temp = in.read();
		while(temp != 0)
		{
	    tport = tport + (char) temp;
	    temp = in.read();
		}
		iport = Integer.parseInt(tport);
		return new Socket(host,iport);
	}
	
	/**
		* This method starts the server.
	 * @author Brandon Smith
	 * @version 1.0
	 */
	public void startserver()
	{
		try
	{
		srver = new ServerSocket(eport);
	}
		catch(Exception e)
	{
	    System.out.println(e);
	    System.exit(1);
	}
	}
	
	public static void main(String[] args)
	{
		new server(8080).start();
	}
}
