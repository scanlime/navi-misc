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
 * BuddyUpdateTocResponse.java
 *
 * Created on 5 May 2002, 21:19
 */

package jaim;

import java.util.Date;
import java.util.StringTokenizer;

/** A BuddyUpdateTocResponse is delivered to a {@link JaimEventListener } when a buddy update is received from the TOC server
 * @author paulw
 * @version $Revision: 1.7 $
 */
public class BuddyUpdateTocResponse extends TocResponse implements TocResponseHandler {

    private String buddyName;
    private boolean online;
    private int evil;
    private int idleTime;
    private boolean onAOL;
    private boolean unconfirmed;
    private boolean admin;
    private boolean confirmed;
    private Date signonTime;
    private boolean away;
    
    public static String RESPONSE_TYPE="UPDATE_BUDDY";
    
    /** Creates new BuddyUpdateTocResponse */
    public BuddyUpdateTocResponse() {
        buddyName="";
        online=false;
        evil=0;
        idleTime=0;
        onAOL=false;
        unconfirmed=false;
        admin=false;
        confirmed=false;
        away=false;
    }

    /** The parseString method is used to populate the fields of this class from a Buddy Update string from the TOC server
     * @param str The String containing the buddy update
     */    
    public TocResponse parseString(java.lang.String str) {
        BuddyUpdateTocResponse tr=new BuddyUpdateTocResponse();
        tr.doParse(str);
        return(tr);
    }
    
    private void doParse(String str)
    {
        cmd=str;
        StringTokenizer st=new StringTokenizer(str,":");
        
        st.nextToken();
        buddyName=st.nextToken();
        String onlineStr=st.nextToken();
        if (onlineStr.equals("T"))
        {
            online=true;
        }
        else
        {
            online=false;
        }
        
        evil=Integer.parseInt(st.nextToken());
        long signon=Long.parseLong(st.nextToken());
        signonTime=new Date(signon*1000);
        idleTime=Integer.parseInt(st.nextToken());
        String userclass=st.nextToken();
        if (userclass.charAt(0) == 'A')
            onAOL=true;
        if (userclass.charAt(1) == 'A')
        {
            admin=true;
        }
        else
        {
            if (userclass.charAt(1)=='U')
            {
                unconfirmed=true;
            }
            else
            {
                if(userclass.charAt(1)=='O')
                {
                    confirmed=true;
                }
            }
        }
        if (userclass.length()>2)
        {
        if (userclass.charAt(2)=='U')
        {
            away=true;
        }
        }
    }
    
    /** Get the away status of the buddy specified by this update
     * @return true if the buddy is "away"
     */    
    public boolean isAway()
    {
        return(away);
    }
    
    /** Get the response type of  this response.  This method is used by the response dispatcher within JaimConnection
     * @return The response type
     */    
    public String getResponseType() {
        return RESPONSE_TYPE;
    }
    
    /** Obtain the buddy name from this update
     * @return The buddy name
     */    
    public String getBuddy()
    {
        return(buddyName);
    }
    
    /** Obtain the online status of this buddy update
     * @return true if the buddy is on line
     */    
    public boolean isOnline()
    {
        return(online);
    }
    
    /** Obtain the idle time of this buddy
     * @return The idle time in seconds
     */    
    public int getIdleTime()
    {
        return(idleTime);
    }
    
    /** Obtain the "Evil" (Warning) level of this buddy
     * @return The warning level as a percentage
     */    
    public int getEvil()
    {
        return(evil);
    }
    
    /** Is this buddy an "Administrator"
     * @return true if an administrator
     */    
    public boolean isAdmin()
    {
        return(admin);
    }
    
    /** IS this buddy a "confirmed" user
     * @return True if this buddy is confirmed
     */    
    public boolean isConfirmed()
    {
        return(confirmed);
    }
    
    /** Is this user an "Unconfirmed user"
     * @return True if this user is unconfirmed
     */    
    public boolean isUnconfirmed()
    {
        return(unconfirmed);
    }
    
    /** Get the signon time of this buddy
     * @return The date/time of signon
     */    
    public Date getSignonTime()
    {
        return(signonTime);
    }
    
    /** Returns true if this response handler can handle the specified response.
     * @param Response - the response string from TOC.  This is the part of the response before the first ':'
     * @return true if the response can be handled
     */
    public boolean canHandle(String Response) {
        return(Response.equalsIgnoreCase(RESPONSE_TYPE));
    }    
    
}
