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
	public static int port;
	
	public static void main(String[] args)
	{
		argparse(args);
		System.out.println("proxy for " + host + " on port " + port);
	}
	
	public static void listen()
	{
		
	}
	
	public static void startserver()
	{
	
	}
	
	public static void argparse(String[] args)
	{
		try
		{
			host = args[0];
			port = Integer.parseInt(args[1]);
		}
		catch(Exception e)
		{
			System.out.println("Usage: java <host to proxy to> <port to proxy on>");
			System.exit(1);
		}
	}
}