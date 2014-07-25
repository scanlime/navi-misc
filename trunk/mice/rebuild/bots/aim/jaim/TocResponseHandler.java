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

package jaim;

/**
 *
 * @author  paulw
 * @version $revision: $
 */
public interface TocResponseHandler {

    /** Returns true if this response handler can handle the specified response.
     *@param Response - the response string from TOC.  This is the part of the response before the first ':'
     *@return true if the response can be handled
     */
    
    public boolean canHandle(String Response);
    
    /** Parse the provided response
     *@param Response - the response from the TOC server.  This is the full TOC response string
     *@return - A TocResponse object that represents this response
     */
    
    public TocResponse parseString(String Response);
    
}

