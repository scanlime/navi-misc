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
 * ErrorTocResponse.java
 *
 * Created on 4 May 2002, 14:52
 */

package jaim;

import java.util.MissingResourceException;

/** This TOC response is sent to a {@link JaimEventListener } when an error message is received from the TOC server
 *
 * @author paulw
 * @version $Revision: 1.7 $
 */
public class ErrorTocResponse extends TocResponse implements TocResponseHandler {

    int errorCode;
    String errorText;
    
    public static final String RESPONSE_TYPE="ERROR";
    
    /** Creates new ErrorTocResponse */
    public ErrorTocResponse() {
        errorCode=0;
        errorText="";
    }

    
    /** Parse the error response string sent by the TOC server
     * @param str The error response string
     */    
     public TocResponse parseString(String str)
    {
        ErrorTocResponse tr=new ErrorTocResponse();
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
            if (colonPos!=-1)
            {
                errorCode=Integer.parseInt(str.substring(0,colonPos));
                errorText=str.substring(colonPos+1);
            }
            else
            {
                errorCode=Integer.parseInt(str);
            }
        }
    
    }
    
    /** Obtain the error code for this response
     * @return The error code
     */    
    public int getErrorCode()
    {
        return(errorCode);
    }
    
    /** Get the error text (if any) associated with this error response
     * @return The error text
     */    
    public String getErrorText()
    {
        return(errorText);
    }
    
    
    /** Obtain the error message that corresponds to this error.
     * @return The error text with any applicable error argument text inserted
     */
    public String getErrorDescription() {
        try {
            StringBuffer desc=new StringBuffer(java.util.ResourceBundle.getBundle("com/wilko/jaim/TocErrorDescriptions").getString(Integer.toString(errorCode)));
            String sDesc=desc.toString();
            int argpos=sDesc.indexOf("%s");
            if (argpos != -1) {
                desc.replace(argpos,argpos+1,errorText);
            }
            return(desc.toString());
            
        }
        catch (MissingResourceException e) {
            return("Unable to locate error description:"+e.toString());
        }
    }
    
    /** Obtain the error message that corresponds to the specified error code
     * @param code The error code
     * @return The error text
     */    
     static public String getErrorDescription(int code)
    {
        try
        {
            return(java.util.ResourceBundle.getBundle("com/wilko/jaim/TocErrorDescriptions").getString(Integer.toString(code)));
        }
        catch (MissingResourceException e)
        {
            return("Unable to locate error description:"+e.toString());
        }
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
