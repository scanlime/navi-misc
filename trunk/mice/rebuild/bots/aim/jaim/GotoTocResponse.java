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

/** This response is delivered to a {@link JaimEventListener } when a GOTO response is received from TOC
 * @author paulw
 * @version $Revision: 1.3 $
 */
public class GotoTocResponse extends TocResponse implements TocResponseHandler {

    String windowName;
    boolean autoResponse;
    String URL;
    
    public static final String RESPONSE_TYPE="GOTO_URL";
    
    /** Creates new GotoTocResponse */
    public GotoTocResponse() {
        windowName="";
        URL="";
    }
    
    /** Obtain the suggested window name for this URL
     * @return The window name
     */    
    public String getWindowName()
    {
        return(windowName);
    }
    
    /** Obtain the URL
     * @return The URL
     */    
    public String getURL()
    {
        return(URL);
    }
    
  

    /** Parse an incoming  response string
     * @param str The string to be parsed
     */    
    public TocResponse parseString(java.lang.String str) {
        GotoTocResponse tr=new GotoTocResponse();
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
                windowName=str.substring(0,colonPos);
                URL=str.substring(colonPos+1);
                
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
