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
 * This is a class that handes person lookups to use the person's "get message"
 * @author Brandon Smith
 * @version 2.0
 */
public class personindex
{
	/** This is the array that holds the bots interanally. */
	public botmain[] bots;
	/** The number of bots that I currently have. */
	public int botcount;
	/** The next bot in the array to be assigned. */
	public int currentbot = 1;
	/** The head of the person linked list. */
	public person masshead;
	/** The head of the interviewee linked list. */
	public interview inthead;
	
	/**
	 * This constructs the personindex.
	 * @param numbots The number of bots involved in the chat.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public personindex(int numbots)
	{
		int botcount = 0;
		bots = new botmain[numbots];
		masshead = null;
		inthead = null;
	}
	
	/**
	 * This method registers a bot with the index.
	 * @param toadd The bot thats to be added.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void registerBot(botmain toadd)
	{
		if(bots[bots.length-1] != null)
			return;
		bots[botcount++] = toadd;
	}
	
	/**
	 * This method handles the messages that comes in from any and all the bots.
	 * @param from The nick of the user who sent the message
	 * @param message The message that came in from them.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void handleMessage(String from, String message)
	{
		System.out.print("Message from " + from);
		interview othertemp = findInterview(from);
		if(othertemp != null)
		{
			System.out.println(" who is an interviewee");
			othertemp.handle(message);
			return;
		}
		person found = findPerson(from);
		if(found != null)
		{
			System.out.println(" who is a normal listener");
			found.handle(message);
			return;
		}
		System.out.println(" who is new to me!");
		bots[0].sendMessage(from,"Hi "+from+" I am just the gate keeper, go ahead and talk to my buddy MICEBot"+currentbot);
		masshead = new person(masshead,bots[currentbot],from);
		currentbot++;
		if(currentbot == botcount) currentbot = 1;
	}
	
	/**
	 * This method registers an interviewee with the index.
	 * @param bot The bot that is going to handle the communication
	 * @param name The nickname of the person being interviewed.
	 * @param pass Their password, but that is transparent to the user.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void addinterview(botmain bot, String name, String pass)
	{
		inthead = new interview(inthead, bot, name, pass);
	}
	
	/**
	 * This method sends a message to everyone in the list.
	 * @param message The message being sent to everyone.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void broadcast(String message)
	{
		person now = masshead;
		while(now != null)
		{
			now.send(message);
			now = now.next;
		}
	}
	
	/**
	 * This method searches for someone in the list with the nick as is passed.
	 * @param nick The nick to search for.
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The interview object associated with that nickname.
	 */
	private interview findInterview(String nick)
	{
		interview temphead = inthead;
		while(temphead != null)
		{
			if(temphead.nick.toUpperCase().compareTo(nick.toUpperCase()) == 0)
				return temphead;
			temphead = temphead.next;
		}
		return null;
	}
	
	/**
	 * This method searches for someone in the list with the nick as is passed.
	 * @param nick The nick to search for.
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The person object associated with that nickname.
	 */
	private person findPerson(String nick)
	{
		person temphead = masshead;
		while(temphead != null)
		{
			if(temphead.nick.toUpperCase().compareTo(nick.toUpperCase()) == 0)
				return temphead;
			temphead = temphead.next;
		}
		return null;
	}
}