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

public class tcpfw extends Thread
{
	/** This holds the string that describes the host getting proxied to. */
	public String host;
	/** The external port (open to the rest of the world). */
	public int eport;
	/** The port on the internal network (what it gets forwarded to). */
	public int iport;
	/** The proxy server's listening socket. */
	public ServerSocket server;
	
	/**
	 * This sets up the tcp forwarder to run with the info given.
	 * @param remotehost The host to forward data to.
	 * @param externalport The port to listen on from the outside world.
	 * @param internalport The port to connect to on the internal network.
	 * @author Brandon Smith
	 * @version 1.0
	 */
	public tcpfw(String remotehost, int externalport, int internalport)
	{
		host = remotehost;
		eport = externalport;
		iport = internalport;
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
		try
		{
			while(true)
			{
				// get the sockets
				sout = server.accept();
				sin = new Socket(host,iport);

				//slot a into tab a
				out = sout.getOutputStream();
				in = sin.getInputStream();
				new tcppipe(in,out,"In -> Out").start();
				//slot b into tab b
				out = sin.getOutputStream();
				in = sout.getInputStream();
				new tcppipe(in,out,"Out -> In").start();
				
				//tell the world I am happy!
			}
		}
		catch(Exception e)
		{
			System.out.println(e);
		}
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
			server = new ServerSocket(eport);
		}
		catch(Exception e)
		{
			System.out.println(e);
			System.exit(1);
		}
	}
}
