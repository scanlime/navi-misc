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
 * GenericTocCommand.java
 *
 * Created on 4 May 2002, 12:07
 */

package jaim;

/** A GenericTocResponse is used internally in the Response parsing and processing logic of {@link JaimConnection}
 * @author paulw
 * @version $Revision: 1.5 $
 */
public class GenericTocResponse extends TocResponse implements TocResponseHandler {

    /** Creates new GenericTocCommand */
    public GenericTocResponse() {
        this.cmd="";
    }
    
    /** Parse an incoming string
     * @param str The response string to be parsed
     */    
    public TocResponse parseString(String str)
    {
        GenericTocResponse tr=new GenericTocResponse();
        tr.doParse(str);
        return tr;
    }
    
    private void doParse(String str)
    {
        cmd=str;
    }

    /** Get a byte array that contains the response
     * @return The response as an array of bytes
     */    
    public byte[] getBytes() {
        return(cmd.getBytes());
    }
    
    /** Convert this response to a string
     * @return The response as a string
     */    
    public String toString()
    {
        return(cmd);
    }
    
    /** Used in the response dispatching process
     * @return The respnse type
     */    
    public String getResponseType()
    {
        return("UNKNOWN");
    }
    
    /** Returns true if this response handler can handle the specified response.
     * @param Response - the response string from TOC.  This is the part of the response before the first ':'
     * @return true if the response can be handled
     */
    public boolean canHandle(String Response) {
        return(true);
    }
    
}
