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
 
public class person
{
	public botmain mybot;
	
	public String nick;
	
	public String name;
	
	public String standmajor;
	
	public person next;
	
	private int status;
	
	public person(person previous, botmain bot, String ick)
	{
		mybot = bot;
		nick = ick;
		status = 0;
		mybot.sendMessage(nick,"What is your name (this is what it will appear as if you ask any questions)");
	}
	
	public void handle(String message)
	{
		switch(status)
		{
		case 0:
			name = message;
			mybot.sendMessage(nick,"Thanks " + name + ", now what is your standing (Freshman/Sophomore/Junior/Senior), of if you're a faculty, what is your position?");
			status = 1;
			break;
		case 1:
			standmajor = message;
			mybot.sendMessage(nick,"Alright, and what is your major or department?");
			status = 2;
		  break;
		case 2:
			standmajor = standmajor + "/" + message;
			mybot.sendMessage(nick,"You are now registered as: " + name + "(" + standmajor + ").  To ask a question, simply greet me, and I will ask you for your question :)");
			status = 3;
			break;
		case 3:
			mybot.sendMessage(nick,message + " to you too!  Go ahead and ask your question.");
			status = 4;
			break;
		case 4:
			mybot.sendMessage(nick,"Your question has been submitted.");
			status = 3;
			break;
		}
	}
}