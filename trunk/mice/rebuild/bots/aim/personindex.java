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
 */
 
public class personindex
{
	public botmain[] bots;
	public int botcount;
	public int currentbot;
	public person masshead;
	public interview inthead;
	
	public personindex(int numbots)
	{
		int botcount = 0;
		bots = new botmain[numbots];
	}
	
	public void registerBot(botmain toadd)
	{
		if(bots[bots.length-1] != null)
			return;
		bots[botcount++] = toadd;
	}
	
	public void handleMessage(String from, String message)
	{
		person found = findPerson(from);
		if(found != null)
		{
			found.handle(message);
			return;
		}
		interview othertemp = findInterview(from);
		if(othertemp != null)
		{
			othertemp.handle(message);
			return;
		}
		masshead = new person(masshead,bots[currentbot],from);
		currentbot++;
		if(currentbot == botcount) currentbot = 0;
	}
	
	public void broadcast(String message)
	{
		person now = masshead;
		while(now != null)
		{
			now.send(message);
			now = now.next;
		}
	}
	
	private interview findInterview(String nick)
	{
		interview temphead = inthead;
		while(inthead != null)
		{
			if(temphead.nick.compareTo(nick) == 0)
				return temphead;
			temphead = temphead.next;
		}
		return null;
	}
	
	private person findPerson(String nick)
	{
		person temphead = masshead;
		while(temphead != null)
		{
			if(temphead.nick.compareTo(nick) == 0)
				return temphead;
			temphead = temphead.next;
		}
		return null;
	}
}