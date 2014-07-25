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

//This is necessary for the jaim library
import jaim.*;

import java.net.*;
import java.io.*;
import java.util.*;

/**
 * This class is the main class for the AIM bot.  This code may be integrated
 * into the server at a later date, but who knows...
 * @author Brandon Smith
 * @version 2.0
 */
public class aimup
{
	/** An array that points to the bots. */
	public static botmain[] bot;
	
	/** The magic button that makes it a server. */
	public static ServerSocket server;

	/** I guess a link is necessary for after the server connects. */
	public static net link;
	
	/** An index of all the people. */
	public static personindex index;

	/**
	 * This is where it all starts.  The Function of Functions!
	 * @param args The command line arguments
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void main(String[] args)
	{
		botmain interviewbot = initbots(30,"fake");
		sleep(4);
		startServer(8082);
		configureServer(interviewbot);
		updateLoop();
		link.closeConnection();
	}
	
	/**
	 * This is for the connection to the main question server, to handle it.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void updateLoop()
	{
		String cmd;
		String qn, qt, an, at;
		while(true)
		{
			cmd = link.read();
			switch(cmd.charAt(0))
			{
			case 'a':
				qn = link.read();
				qt = link.read();
				an = link.read();
				at = link.read();
				index.broadcast(qn + " Asks: " + qt + " To which " + an + " replies: " + at);
				break;
			case 'n':
				index.broadcast(link.read());
				break;
			case 'q':
				return;
			}
		}
	}
	
	/**
	 * This method configures the server for the interviews.
	 * @param bot The bot object that interviewees will get messaged from.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void configureServer(botmain bot)
	{
		int interviews, i;
		String nick, pass;
		String mykey = key.keygen();
		System.out.println(mykey);
		link.write(mykey);
		nick = link.read();
		while(nick.compareTo(".") != 0)
		{
			pass = key.decrypt(mykey,link.read());
			System.out.println(nick + " " + pass);
			index.addinterview(bot,nick,pass);
			nick = link.read();
		}
	}
	
	/**
	 * This method starts a server, and accepts the connection.
	 * @param port The port to run the server on.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void startServer(int port)
	{
		try 
		{
			server = new ServerSocket(port);
			Socket mine = server.accept();
			link = new net(mine);
		}
		catch(IOException exception)
		{
			System.out.println(exception);
			System.exit(0);
		}
	}
	
	/**
	 * This method initializes the bots themselves.
	 * @param count The number of bots to start (1-20)
	 * @param password The password for the bots.
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The bot that will be used for the intervieweees.
	 */
	public static botmain initbots(int count, String password)
	{
		int i,j;
		index = new personindex(count+1);
		bot = new botmain[count+1];
		bot[0] = new botmain("MICEBot",password,index);
		j=5;
		while(!bot[0].connected)
		{
			j += j;
			System.out.println("failed");
			sleep(j);
			bot[0] = new botmain("MICEBot",password,index);
		}

		index.registerBot(bot[0]);
		
		for(i=0;i<count;i++)
		{
			sleep(7);
			bot[i+1] = new botmain("MICEBot"+(i+1),password,index);
			j=5;
			while(!bot[i+1].connected)
			{
				j+=j;
				System.out.println("failed");
				sleep(j);
				bot[i+1] = new botmain("MICEBot"+(i+1),password,index);
			}
			index.registerBot(bot[i+1]);
			sleep(3);
		}
		return bot[0];
	}
	
	/**
	 * This method makes this thread sleep.
	 * @param seconds The duration of the nap in seconds.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void sleep(int seconds)
	{
		try
		{
			Thread.sleep(seconds * 1000);
		}
		catch (Exception ie) {}
	}
}
