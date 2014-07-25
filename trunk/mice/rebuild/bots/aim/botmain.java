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
 * This class takes care of the basic operations of a bot.
 * I pulled a lot of this code from the JaimTest program supplied with jaimlib.
 * @author Brandon Smith
 * @version 2.0
 * @see <a href="http://jaimlib.sourceforge.net">jaimlib</a>
 */
public class botmain implements JaimEventListener 
{
	/** The connection for the bot, hence b. */
	public JaimConnection b;
	
	/** A connection to the person index, so it can reply correctly. */
	public personindex index;
	
	/** This holds the value of connected (true for true, false for false). */
	public boolean connected = false;
	
	/**
	 * This method constructs a new bot.
	 * @param username The username the bot has.
	 * @param password The password the bot has.
	 * @param foo The index that this bot gets associated with
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public botmain(String username, String password, personindex foo)
	{
		index = foo;
		try
		{
			System.out.print("Logging " + username + " in......");
			b = new JaimConnection("toc.oscar.aol.com",9898);
	    b.connect();
			b.watchBuddy("gonkulator2");
	    b.logIn(username,password,20000);
	    b.addBlock("");
	    b.setInfo("This is a bot based on <a href=\"http://jaimlib.sourceforge.net\">jaimlib</a>.\nFor more information on MICE <a href=\"http://studentactivities.mscd.edu/\">check it out</a>");
			b.addEventListener(this);
			System.out.println("Complete");
			connected = true;
		}
		catch(Exception e)
		{
	    e.printStackTrace();
		}
	}

	/**
	 * This method is a rip off from JaimTest.java 
	 * @param event The object that holds all the magical information.
	 * @author paulw
	 * @version 2.0
	 */
	public void receiveEvent(JaimEvent event)
	{
		TocResponse tr=event.getTocResponse();
		String responseType=tr.getResponseType();
		if(responseType.equalsIgnoreCase(IMTocResponse.RESPONSE_TYPE))
		{
			receiveIM((IMTocResponse)tr);
		}
	}
  
	/**
	 * This method is called when a message is recieved
	 * @param im The information involved in an instant message
	 * @author Brandon Smith
	 * @version 2.0
	 */
	private void receiveIM(IMTocResponse im)
	{
		//System.out.println(im.getFrom()+"->"+Utils.stripHTML(im.getMsg()));
		String from = im.getFrom();
		String message = Utils.stripHTML(im.getMsg());
		index.handleMessage(from,message);
	}

	/**
	 * This sends a message through this bot.
	 * @param nick The nick of the person to send it to.
	 * @param message The message to send to the person.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void sendMessage(String nick, String message)
	{
		try
		{
	    b.sendIM(nick,message);
		}
		catch(IOException e){}
	}
}