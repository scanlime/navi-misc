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
 * TocIMResponse.java
 *
 * Created on 4 May 2002, 14:38
 */

package jaim;

/** This response is delivered to a {@link JaimEventListener } when an instant message is received
 * @author paulw
 * @version $Revision: 1.6 $
 */
public class IMTocResponse extends TocResponse implements TocResponseHandler {

    String from;
    boolean autoResponse;
    String msg;
    
    public static final String RESPONSE_TYPE="IM_IN";
    
    /** Creates new TocIMResponse */
    public IMTocResponse() {
        from="";
        msg="";
        autoResponse=false;
    }
    
    /** Obtain the name of the buddy who sent this instant message
     * @return The senders name
     */    
    public String getFrom()
    {
        return(from);
    }
    
    /** Obtain the message
     * @return The message
     * @see Utils#stripHTML
     */    
    public String getMsg()
    {
        return(msg);
    }
    
    /** Is this response an automatically generated response?
     * @return true if this is an automatically generated response
     */    
    public boolean getAutoResponse()
    {
        return(autoResponse);
    }

    /** Parse an incoming IM response string
     * @param str The string to be parsed
     */    
    public TocResponse parseString(java.lang.String str) {
        IMTocResponse tr=new IMTocResponse();
        tr.doParse(str);
        return(tr);
    }
    
    private void doParse(String str)
    {
        cmd=str;
        int colonPos=str.indexOf(':');
        if (colonPos!=-1)
        {
            str=str.substring(colonPos+1);
            colonPos=str.indexOf(':');
            if (colonPos != -1)
            {
                from=str.substring(0,colonPos);
                str=str.substring(colonPos+1);
                colonPos=str.indexOf(':');
                if (str.charAt(0) == 'T')
                {
                    autoResponse=true;
                }
                if (colonPos != -1)
                {
                    msg=str.substring(colonPos+1);
                }
            }
        }
            
    }
    
    /** Obtain the response type for response dispatching purposes
     * @return The response type
     */    
    public String getResponseType() {
        return(RESPONSE_TYPE);
    }
    
    /** Returns true if this response handler can handle the specified response.
     * @param Response - the response string from TOC.  This is the part of the response before the first ':'
     * @return true if the response can be handled
     */
    public boolean canHandle(String Response) {
        return (Response.equalsIgnoreCase(RESPONSE_TYPE));
    }
    
}
