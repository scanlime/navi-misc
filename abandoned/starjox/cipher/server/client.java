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

public class client extends Thread
{
    /** The external port (open to the rest of the world). */
    public int myport;

    /** The hostname of the tunneling server */
    public String rtarget;

    /** The ultimate destination of the information. */
    public String rhost;
    public String rport;

    /** The proxy server's listening socket. */
    public ServerSocket server;

    /**
     * This sets up the tcp forwarder to run with the info given.
     * @param port The port to listen on.
     * @param rHost The host that is listening for tunneling attempts.
     * @param rTarget The hostname of the remote server's target.
     * @param rPort The port that the remote host connects to
     * @author Brandon Smith
     * @version 1.0
     */
    public client(int lport, String rTarget, String rHost, String rPort)
    {
	myport = lport;
	rtarget = rTarget;
	rhost = rHost;
	rport = rPort;
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
	Socket clear = null;
	Socket crypted = null;
	OutputStream out = null;
	InputStream in = null;
	key nukey;
	while(true)
	{
	    try
	    {
		// get the sockets
		clear = server.accept();
		crypted = new Socket(rtarget,8080);

		nukey = getKey(crypted);
		initConnection(crypted);
		
		//slot a into tab a
		in = clear.getInputStream();
		out = crypted.getOutputStream();
		new epipe(in,out,nukey).start();

		//slot b into tab b
		in = crypted.getInputStream();
		out = clear.getOutputStream();
		new dpipe(in,out,nukey).start();
	    }
	
	    catch(Exception e)
	    {
		System.out.println(e);
	    }
	}
    }

    public key getKey(Socket client) throws Exception
    {
	int i;
	key foo;
	int[] kei = new int[256];
	InputStream in = client.getInputStream();
	for(i=0;i<256;i++)
	    kei[i] = in.read();
	foo = new key(kei);
	return foo;
    }

    public void initConnection(Socket client) throws Exception
    {
	OutputStream out = client.getOutputStream();
	int i;
	for(i=0;i<rhost.length();i++)
	{
	    out.write(rhost.charAt(i));
	    out.flush();
	}
	out.write(0);
	out.flush();
	
	for(i=0;i<rport.length();i++)
	{
	    out.write(rport.charAt(i));
	    out.flush();
	}
	out.write(0);
	out.flush();
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
	    server = new ServerSocket(myport);
	}
	catch(Exception e)
	{
	    System.out.println(e);
	    System.exit(1);
	}
    }

    public static void main(String[] args)
    {
	int lport = 0;
	String server="";
	String thost="";
	String tport="";
	try
	{
	    lport = Integer.parseInt(args[0]);
	    server = args[1];
	    thost = args[2];
	    tport = args[3];
	}
	catch(Exception e)
	{
	    System.out.println("Usage: java client <local port> <tunnel host> "+
			       "<target host> <target port>");
	    System.exit(1);
	}
	System.out.println("Starting tunnel from localhost:"+lport+" through "+
			   server+":8080 to " + thost+":"+tport);
	new client(lport,server,thost,tport).start();
    }
}
