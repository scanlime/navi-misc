/*
 *   (C) 2002 Paul Wilkinson  wilko@users.sourceforge.net
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

/*
 * JaimTest.java
 *
 * Created on 3 May 2002, 12:26
 */

import java.net.*;
import java.io.*;
import jaim.*;
import java.util.*;

/**
 *
 * @author  paulw
 * @version $Revision: 1.13 $
 */
public class JaimTest implements JaimEventListener {
    
    JaimConnection c;	

    boolean quit=false;
    
    /** Creates new JaimMain */
    public JaimTest() {
        
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        if (args.length < 2) {
            System.out.println("Usage: JaimTest <username> <password>");
        }
        else {
            JaimTest tester=new JaimTest();
            tester.doIt(args[0],args[1]);
        }
        
    }
    
    private void doIt(String username, String password) {
        try {
            c=new JaimConnection("toc.oscar.aol.com",9898);
            c.setDebug(true);               // Send debugging to standard output
            c.connect();
            
            c.addEventListener(this);
            c.watchBuddy("gonkulator");         // Must watch at least one buddy or you will not appear on buddy listings
            
            c.logIn(username,password,50000);
            c.addBlock("");     // Set Deny None
            
            c.setInfo("This buddy is using <a href=\"http://jaimlib.sourceforge.net\">Jaim</a>.");
            
            
            c.setIdle(60);      // Pretend we have been idle for a minute
            c.setAway("I am away right now");
            
            try {
                Thread.sleep(10000);       //Wait for 10 second
            }
            catch (InterruptedException ie) {
            }
            
            c.setIdle(0);      // Pretend we have been idle for a minute
            c.setAway("");

            while (!quit) {         
                try {
                    Thread.sleep(300000);       //Wait for 5 minutes
                }
                catch (InterruptedException ie) {
                }
            }
            
            System.out.println("Disconnecting");
            c.disconnect();
            
        }
        
        catch (IOException e) {
            e.printStackTrace();
        }
        catch (JaimException je) {
            je.printStackTrace();
        }        
    }
    
    /** Receive an event and process it according to its content
     *@param event - The JaimEvent to be processed
     */
    
    
    public void receiveEvent(JaimEvent event) {
        TocResponse tr=event.getTocResponse();
        String responseType=tr.getResponseType();
        if (responseType.equalsIgnoreCase(BuddyUpdateTocResponse.RESPONSE_TYPE)) {
            receiveBuddyUpdate((BuddyUpdateTocResponse)tr);
        }
        else if (responseType.equalsIgnoreCase(IMTocResponse.RESPONSE_TYPE)) {
            receiveIM((IMTocResponse)tr);
        }
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
    
    private void receiveError(ErrorTocResponse et) {
        System.out.println("Error: "+et.getErrorDescription());
    }
    
    private void receiveIM(IMTocResponse im) {
        System.out.println(im.getFrom()+"->"+Utils.stripHTML(im.getMsg()));
        
        try {
            
            c.sendIM(im.getFrom(),"Hello "+im.getFrom() + " I will give live updates to Metro Internet Chat Events",false);
            c.sendIM("gonkulator2","IM From "+im.getFrom(),false);
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
    
    private void receiveEvil(EvilTocResponse er) {
        if (er.isAnonymous()) {
            System.out.println("We have been warned anonymously!");
        }
        else {
            System.out.println("We have been warned by "+er.getEvilBy());
            try {
                c.sendEvil(er.getEvilBy(),false);     // Let's warn them back
                c.addBlock(er.getEvilBy());          // And block them
            }
            catch (Exception e) {
                e.printStackTrace();
            }
        }
        
        System.out.println("New warning level is:"+er.getEvilAmount());
    }
    
    private void receiveGoto(GotoTocResponse gr) {
        System.out.println("Attempting to access "+gr.getURL());
        try {
            InputStream is=c.getURL(gr.getURL());
            if (is!=null) {
                InputStreamReader r=new InputStreamReader(is);
                int chr=0;
                while (chr!=-1) {
                    chr=r.read();
                    System.out.print((char)chr);
                }
                
            }
        }
        catch (IOException e) {
            e.printStackTrace();
        }
    }
    
    private void receiveConfig() {
        System.out.println("Config is now valid.");
        
        try {
            Iterator it=c.getGroups().iterator();
            while (it.hasNext()) {
                Group g=(Group)it.next();
                System.out.println("Group: "+g.getName());
                Enumeration e=g.enumerateBuddies();
                while (e.hasMoreElements()) {
                    Buddy b =(Buddy)e.nextElement();
                    b.setDeny(false);
                    b.setPermit(false);
                    c.watchBuddy(b.getName());
                    if (b.getDeny()) {
                        c.addBlock(b.getName());
                    }
                    if (b.getPermit()) {
                        c.addPermit(b.getName());
                    }
                }
            }
            c.saveConfig();
        }
        catch (Exception je) {
            je.printStackTrace();
        }
    }
}
