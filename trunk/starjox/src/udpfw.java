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
 * This class is for opening a single udp port, and forwarding it elsewhere.
 * @author Brandon Smith
 * @version 1.0
 */

import java.net.*;
import java.io.*;
import java.util.*;

public class udpfw extends Thread
{
	/** This holds the string that describes the host getting proxied to. */
	public String host;
	/** The external port (open to the rest of the world). */
	public int eport;
	/** The port on the internal network (what it gets forwarded to). */
	public int iport;
	/** The proxy server's listening socket. */
	public DatagramSocket server;
	
	/**
	 * This sets up the udp forwarder to run with the info given.
	 * @param remotehost The host to forward data to.
	 * @param externalport The port to listen on from the outside world.
	 * @param internalport The port to connect to on the internal network.
	 * @author Brandon Smith
	 * @version 1.0
	 */
	public udpfw(String remotehost, int externalport, int internalport)
	{
		host = remotehost;
		eport = externalport;
		iport = internalport;
		System.out.println("Set to forward udp: localhost:"+eport+" to "+host+":"+iport);
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
		byte[] buf = new byte[4096*16];
		try
		{
			DatagramPacket temp = new DatagramPacket(buf,0);
			InetAddress target = InetAddress.getByName(host);
			
			while(true)
			{
				server.receive(temp);
				temp.setAddress(target);
				server.send(temp);
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
			server = new DatagramSocket(eport);
		}
		catch(Exception e)
		{
			System.out.println("Java says\n" + e + "\nThat probably means something is wrong");
			System.exit(1);
		}
		System.out.println("Set to forward udp: localhost:"+eport+" to "+host+":"+iport);
	}
}
