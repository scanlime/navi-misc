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
 * NickTocResponse.java
 *
 * Created on 6 May 2002, 17:21
 */

package jaim;

/**
* The NicTocResponse is used internally to manage the TOC signon process.  It is not delivered to clients of {@link JaimConnection}
 * @author  paulw
 * @version $Revision: 1.6 $
 */
public class NickTocResponse extends TocResponse implements TocResponseHandler {

    private String nickName;
    
    public static final String RESPONSE_TYPE="NICK";
    
    
    /** Creates new NickTocResponse */
    public NickTocResponse() {
        nickName="";
    }

    
    public TocResponse parseString(java.lang.String str) {
        NickTocResponse tr=new NickTocResponse();
        tr.doParse(str);
        return(tr);
    }
    
    private void doParse(String str)
    {
        int colonPos=str.indexOf(':');
        
            if (colonPos != -1)
            {
                nickName=str.substring(colonPos+1);
            }
        
        
    }
    
    public String getNickName()
    {
        return(nickName);
    }
    
    
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
