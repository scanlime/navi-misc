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
 * This class represents the conversation with a person being interviewed.
 * @author Brandon Smith
 * @version 2.0
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

	/** State based conversations need to know what state they're in. */
	public int state;
	
	/**
	 * This method constructs a new interview person
	 * @param prev The previous person in the linked list.
	 * @param bot The bot that this person gets spoken to from.
	 * @param NICK The nickname of the person being interviewed.
	 * @param PASS The password of the person being interviewed.
	 */
	public interview(interview prev, botmain bot, String NICK, String PASS)
	{
		next = prev;
		nick = NICK;
		com = new net();
		state = 0;
	}
	
	/**
	 * This method is called when a message is sent to the program to this nick.
	 * @param message The message that was sent to this person
	 * @author Brandon Smith
	 * @version 2.0
	 */
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
			String mykey = com.read();
			com.write(nick);
			com.write(key.encrypt(mykey,pass));
			com.write(name);
			send("Go ahead and send me a message when you're ready for your first question.  If you want to not answer a question, type 'skip' and you will get the next question.");
			state = 3;
			break;

		case 2: //This is all about sending the answer off, and getting the next question on its way.
			if(message.toUpperCase().compareTo("SKIP") == 0) com.write("reject");
			else
			{
				com.write("answ");
				StringTokenizer botfood = new StringTokenizer(message,"\n",false);
				message = "";
				while(botfood.hasMoreTokens())
					message = message + "<br>" + botfood.nextToken();
				com.write(message);
			}
		case 3: //this is the "raw" get state, in that it gets a question without sending an answer
			String tname, tques;
			com.write("get");
			tname = com.read();
			if(tname.charAt(0) == '-')
			{
				send("No questions right now, I'll message you when I have one for you.");
				state = 4;
			}
			tname = com.read();
			tques = com.read();
			send(tname + " Asks: " + tques);
			state = 2;
			break;
		case 4:
			send("Hold your horses! I said I would message you when I got a message.");
			break;
		}
	}
	
	/**
	 * This method handles sending something to this particular interviewee.
	 * @param message The message to send to the person
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void send(String message)
	{
		mybot.sendMessage(nick,message);
	}
}