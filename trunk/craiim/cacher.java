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

public class cacher
{
    public static cacher head;
    
    public cacher next;

    public String nick;

    public String cached = "";

    public boolean quiet = false;

    public botmain bot;
    
    public cacher(String name, botmain sender)
    {
	nick = name;
	bot = sender;
	new sleeper(this).start();
    }

    public void cacheMessage(String from, String message, boolean action)
    {
	if(action) cached = cached + "<br>* " + from + message;
	else cached = cached + "<br>[" + from + "] " + message;
	if(next != null) next.cacheMessage(from,message,action);
    }

    public void speak(String message)
    {
	quiet = false;
	if(message.startsWith("stfu"))
	{
	    quiet = true;
	    return;
	}
	if(message.startsWith("/me"))
	    message = "*AIM:"+nick+ message.substring(3);
	else
	    message = "<AIM:"+nick+"> " + message;
	cacher.head.cacheMessage("AIM:"+nick,message,false);
	Justin.me.say(message);
    }

    public void flush()
    {
	if(cached.equals(""))
	    return;
	if(!quiet) bot.sendMessage(nick,cached);
	cached = "";
    }
}
