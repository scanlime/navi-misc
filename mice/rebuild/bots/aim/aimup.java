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

public class aimup
{
	public static botmain[] bot;
	
	public static ServerSocket server;

	public static net link;
	
	public static personindex index;

	public static void main(String[] args)
	{
		botmain interviewbot = initbots(1,"wrongpassword");
		startServer(8082);
		sleep(2);
		configureServer(interviewbot);
		updateLoop();
		link.closeConnection();
	}
	
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
				index.broadcast(qn + " Askes: " + qt + " To which " + an + " replies: " + at);
				break;
			case 'n':
				index.broadcast(link.read());
				break;
			case 'q':
				return;
			}
		}
	}
	
	public static void configureServer(botmain bot)
	{
		int interviews, i;
		String nick, pass;
		String mykey = key.keygen();
		System.out.println(mykey);
		link.write(mykey);
		interviews = Integer.parseInt(link.read());
		for(i=0;i<interviews;i++)
		{
			nick = link.read();
			pass = key.decrypt(mykey,link.read());
			index.addinterview(bot,nick,pass);
		}
	}
	
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
	
	public static botmain initbots(int count, String password)
	{
		int i,j;
		index = new personindex(1);
		bot = new botmain[1];
		bot[0] = new botmain("MICEBot",password,index);
		index.registerBot(bot[0]);
		
		for(i=1;i<count;i++)
		{
			sleep(3);
			bot[i] = new botmain("MICEBot"+i,password,index);
			index.registerBot(bot[i]);
		}
		return bot[0];
	}
	
	public static void sleep(int seconds)
	{
		try
		{
			Thread.sleep(seconds * 1000);
		}
		catch (Exception ie) {}
	}
}
