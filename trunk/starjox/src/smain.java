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
 */

import java.net.*;
import java.io.*;
import java.util.*;

public class smain
{
	public static String host;
	public static int eport; //external port (for the server)
	public static int iport; //internal port (for what it forwards to)
	public static ServerSocket server;
	
	public static void main(String[] args)
	{
		argparse(args);
		System.out.println("Forwarding localhost:"+eport+" to "+host+":"+iport);
		startserver();
		listen();
	}
	
	public static void listen()
	{
		Socket sout;
		Socket sin;
		OutputStream out;
		InputStream in;
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
			}
		}
		catch(Exception e)
		{
			System.out.println(e);
		}
	}
	
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