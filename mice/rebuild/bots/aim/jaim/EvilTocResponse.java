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
 * EvilTocResponse.java
 *
 * Created on 6 May 2002, 16:49
 */

package jaim;

import java.util.StringTokenizer;

/** An EvilTocResponse is delivered to a {@link JaimEventListener } when the signed on buddy is "eviled" or warned
 * @author paulw
 * @version $Revision: 1.6 $
 */
public class EvilTocResponse extends TocResponse implements TocResponseHandler {

    private boolean anonymousEvil;
    private int evilAmount;
    private String evilBy;
    
    public static final String RESPONSE_TYPE="EVILED";
    
    /** Creates new EvilTocResponse */
    public EvilTocResponse() {
        anonymousEvil=true;
        evilBy="";
        evilAmount=0;
    }

    /** Parse the evil message from the TOC server
     * @param str The evil message
     */    
    public TocResponse parseString(java.lang.String str) {
        EvilTocResponse tr=new EvilTocResponse();
        tr.doParse(str);
        return(tr);
    }
    
    private void doParse(String str)
    {
            
        StringTokenizer st=new StringTokenizer(str,":");
        
        st.nextToken();     // skip over "EVILED"
        evilAmount=Integer.parseInt(st.nextToken());
        if (st.hasMoreTokens())
        {
            evilBy=st.nextToken();
            anonymousEvil=false;
        }
        else
        {
            anonymousEvil=true;
        }
    }
    
    /** Get the evil amount from this response.  This is the current evil or warning level for the authenticated buddy, not the increment specified by the last warning
     * @return The cumulative evil or warning level
     */    
    public int getEvilAmount()
    {
        return(evilAmount);
    }
    
    /** Obtain the name of the buddy that issued the warning.
     * @return The buddy name that issued the warning
     * @see #isAnonymous
     */    
    public String getEvilBy()
    {
        return(evilBy);
    }
    
    /** Obtain the anonymous status of this warning
     * @return true if this warning was issued anonymously
     */    
    public boolean isAnonymous()
    {
        return(anonymousEvil);
    }
    
    /** Used by the response dispatcher
     * @return The response type
     */    
     public String getResponseType() {
        return RESPONSE_TYPE;
    }
    
  
   /** Returns true if this response handler can handle the specified response.
     * @param Response - the response string from TOC.  This is the part of the response before the first ':'
     * @return true if the response can be handled
     */
    public boolean canHandle(String Response) {
        return(Response.equalsIgnoreCase(RESPONSE_TYPE));
    }
    
}
