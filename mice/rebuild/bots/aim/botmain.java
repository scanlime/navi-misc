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

public class botmain implements JaimEventListener 
{
    /** The connection for the bot, hence b. */
    public JaimConnection b;

    public botmain(String username, String password)
    {
	try
	{
	    b = new JaimConnection("toc.oscar.aol.com",9898);
	    b.connect();

	    b.addEventListener(this);
	    b.watchBuddy("gonkulator2");

	    b.logIn(username,password,5000);
	    b.addBlock("");

	    b.setInfo("This is a bot based on <a href=\"http://jaimlib.sourceforge.net\">jaimlib</a>.");
	    
	}
	catch(Exception e)
	{
	    e.printStackTrace();
	}
    }

    /** This method is a rip off from JaimTest.java */
    public void receiveEvent(JaimEvent event)
    {
	TocResponse tr=event.getTocResponse();
        String responseType=tr.getResponseType();
	

	if (responseType.equalsIgnoreCase(BuddyUpdateTocResponse.RESPONSE_TYPE)) {
            receiveBuddyUpdate((BuddyUpdateTocResponse)tr);
        }
        else if (responseType.equalsIgnoreCase(IMTocResponse.RESPONSE_TYPE)) {
            receiveIM((IMTocResponse)tr);
        }
	/*
        else if (responseType.equalsIgnoreCase(EvilTocResponse.RESPONSE_TYPE)) {
            receiveEvil((EvilTocResponse)tr);
        }
	else if (responseType.equalsIgnoreCase(GotoTocResponse.RESPONSE_TYPE)) {
	    receiveGoto((GotoTocResponse)tr);
	}
	else if (responseType.equalsIgnoreCase(ConfigTocResponse.RESPONSE_TYPE)) {
	    receiveConfig();
	}
	else if (responseType.equalsIgnoreCase(ErrorTocResponse.RESPONSE_TYPE)) {
	    receiveError((ErrorTocResponse)tr);
	}
	*/
	else if (responseType.equalsIgnoreCase(LoginCompleteTocResponse.RESPONSE_TYPE)) {
	    System.out.println("Login is complete");
	}
	else if (responseType.equalsIgnoreCase(ConnectionLostTocResponse.RESPONSE_TYPE)) {
	    System.out.println("Connection lost!");
	}
	else {
	    System.out.println("Unknown TOC Response:"+tr.toString());
	}
    }
  
    private void receiveIM(IMTocResponse im) {
        System.out.println(im.getFrom()+"->"+Utils.stripHTML(im.getMsg()));
        
        try {
            
            b.sendIM(im.getFrom(),"Hello "+im.getFrom(),false);
            b.sendIM("gonkulator2","IM From "+im.getFrom(),false);
        }
        catch (IOException e) {
        }
    }
    
    private void receiveBuddyUpdate(BuddyUpdateTocResponse bu) {
        System.out.println("Buddy update: "+bu.getBuddy());
        if (bu.isOnline()) {
            System.out.println("Online");
        }
        else {
            System.out.println("Offline");
        }
        
        if (bu.isAway()) {
            System.out.println("Away");
        }
        
        System.out.println("evil: "+bu.getEvil());
        
        System.out.println("Idle: "+bu.getIdleTime());
        
        System.out.println("On since "+bu.getSignonTime().toString());
    }
}
