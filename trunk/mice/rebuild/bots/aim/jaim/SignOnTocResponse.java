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
 * SignOnTocResponse.java
 *
 * Created on 4 May 2002, 13:29
 */

package jaim;

/**
 * The SignOnTocResponse is used internally to manage the TOC signon process.  It is not delivered to clients of {@link JaimConnection}
 * @author  paulw
 * @version $Revision: 1.5 $
 */
public class SignOnTocResponse extends TocResponse implements TocResponseHandler {

    String version;
    
    public static final String RESPONSE_TYPE="SIGN_ON";
    
    
    /** Creates new SignOnTocResponse */
    public SignOnTocResponse() {
        version="";
    }
    
    public String getResponseType() {
        return(RESPONSE_TYPE);
    }
    
    protected String getVersion()
    {
        return(version);
    }
    
   
    public TocResponse parseString(String str)
    {
        SignOnTocResponse tr=new SignOnTocResponse();
        tr.doParse(str);
        return(tr);
    }
    
    private void doParse(String str)
    {
        cmd=str;
        int colonpos=str.indexOf(':');
        if (colonpos != -1)
        {
            version=str.substring(colonpos+1);
        }
    }
    
    /** Returns true if this response handler can handle the specified response.
     * @param Response - the response string from TOC.  This is the part of the response before the first ':'
     * @return true if the response can be handled
     */
    public boolean canHandle(String Response) {
        return(Response.equalsIgnoreCase(RESPONSE_TYPE));
    }
    
}
