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

import jaim.*;
import java.net.*;
import java.io.*;
import java.util.*;

public class interview
{
	/** The next interview person in the linked list. */
	public interview next;

	/** The nickname of the person. */
	public String nick;
	
	/** The password for this person. */
	public String pass;
	
	/** The real name of the person. */
	public String name;
	
	/** The Bot I do communications through. */
	public botmain mybot;
	
	/** The network connection to the server. */
	public net com;

	public int state;
	
	public interview(interview prev, botmain bot, String NICK, String PASS)
	{
		next = prev;
		nick = NICK;
		com = new net();
		state = 0;
	}
	
	public void handle(String message)
	{
		switch(state)
		{
		case 0:
			send("What is your name?  This way I get your real name with your answers.");
			state = 1;
			break;
		case 1:
			name = message;
			if(!com.openConnection("localhost",8080))
			{
				send("Connection to server failed, contact the administrator");
				return;
			}
			com.write("cint");
			send("Go ahead and send me a message when you're ready for your first question");
			state = 2;
			break;
		case 2:
			
			break;
		case 3:
			break;
		case 4:
		}
	}
	
	public void send(String message)
	{
		mybot.sendMessage(nick,message);
	}
}