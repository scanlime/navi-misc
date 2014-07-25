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
 * This class is what represents a conversation with someone.
 * @author Brandon Smith
 * @version 2.0
 */
public class person
{
	/** This is the bot that this person gets spoken to through. */
	public botmain mybot;
	/** This is the person's nickname. */
	public String nick;
	/** This is the person's real name. */
	public String name;
	/** This is the person's standing and major. */
	public String standmajor;
	/** This is the next person in the linked list. */
	public person next;
	/** Conversations are state based, this holds the status of this convo. */
	private int status;
	/** Is this person sendable to? */
	private boolean sendable = false;
	
	/**
	 * This constructs the person when a message comes from them.
	 * @param previous The person to be put as my "next" person for linked list.
	 * @param bot The bot that this person gets talked to from.
	 * @param ick The person's nick.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public person(person previous, botmain bot, String ick)
	{
		mybot = bot;
		nick = ick;
		status = 0;
		next = previous;
		mybot.sendMessage(nick,"Hello " + nick + " to make me be quiet, just type 'quit' at any time.");
		mybot.sendMessage(nick,"What is your name (this is what it will appear as if you ask any questions)");
	}
	
	/**
	 * This method handles when this person gets a message.
	 * @param message The message they get.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void handle(String message)
	{
		switch(status)
		{
		case 0:
			name = message;
			mybot.sendMessage(nick,"Thanks " + name + ", now what is your standing (Freshman/Sophomore/Junior/Senior), or if you're a faculty, what is your position?");
			status = 1;
			break;
		case 1:
			standmajor = message;
			mybot.sendMessage(nick,"Alright, and what is your major or department?");
			status = 2;
		  break;
		case 2:
			standmajor = " (" + standmajor + "/" + message + ")";
			mybot.sendMessage(nick,"You are now registered as: " + name + standmajor + ".  To ask a question, just send it to me and I'll submit it for you :)");
			status = 3;
			sendable=true;
			break;
		case 3:
			if(message.startsWith("quit"))
			{
				sendable = false;
				mybot.sendMessage(nick,"Alright, I'll be quiet.  Send me a message, and I'll start talking again.");
				return;
			}
			if(!sendable)
			{
				sendable = true;
				return;
			}
			net temp = new net();
			if(!temp.openConnection("localhost",8080))
			{
				send("I couldn't connect to the server, try again later.");
				status = 3;
				return;
			}
			temp.write("csub");
			temp.write(name + " " + standmajor);
			temp.write(message);
			temp.closeConnection();
			send("Your question has been submitted.");
			status = 3;
			break;
		}
	}
	
	/**
	 * This method sends something to this person
	 * @param tosend The string thats getting sent.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void send(String tosend)
	{
		if(sendable)
			mybot.sendMessage(nick,tosend);
	}
}