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
 * This class is for setting up the proxy server and creating new connections.
 * @author Brandon Smith
 * @version 1.0
 */

import java.net.*;
import java.io.*;
import java.util.*;

public class smain
{
	/** This holds the string that describes the host getting proxied to. */
	public static String host;
	/** The external port (open to the rest of the world). */
	public static int eport;
	/** The port on the internal network (what it gets forwarded to). */
	public static int iport;
	/** The proxy server's listening socket. */
	public static ServerSocket server;
	
	/**
	 * This is the main function, starts everything going.
	 * @param args The command line arguments to this program.
	 * @author Brandon Smith
	 * @version 1.0
	 */
	public static void main(String[] args)
	{
		argparse(args);
		System.out.println("Forwarding localhost:"+eport+" to "+host+":"+iport);
		startserver();
		listen();
	}
	
	/**
	 * This method is what handles the listening and initializing connections
	 * @author Brandon Smith
	 * @version 1.0
	 */
	public static void listen()
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
				new pipe(in,out).start();
				//slot b into tab b
				out = sin.getOutputStream();
				in = sout.getInputStream();
				new pipe(in,out).start();
				
				//tell the world I am happy!
				System.out.println("Connection from "+sout.getInetAddress().getHostAddress());
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
	public static void startserver()
	{
		try
		{
			server = new ServerSocket(eport);
		}
		catch(Exception e)
		{
			System.out.println("Java says\n" + e + "\nThat probably means something is wrong");
			System.exit(1);
		}
	}
	
	/**
	 * This method is responsible for argument parsing and variable init stuff.
	 * @param args The raw command line arguments.
	 * @author Brandon Smith
	 * @version 1.0
	 */
	public static void argparse(String[] args)
	{
		try
		{
			host = args[0];
			eport = Integer.parseInt(args[1]);
			if(args.length > 2)
				iport = Integer.parseInt(args[2]);
			else
				iport = eport;
		}
		catch(Exception e)
		{
			System.out.println("Usage: java <host to proxy to> <port to proxy on>");
			System.exit(1);
		}
	}
}