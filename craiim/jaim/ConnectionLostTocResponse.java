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
 * ConnectionLostTocResponse.java
 *
 * Created on November 2, 2002, 2:52 PM
 */

package jaim;

/**
 * This is a "pseudo" TOC response - it is delivered to JaimLib clients to indicate that the connection to the server has been lost.
 * @author  wilko
 * @version: $revision: $
 */
public class ConnectionLostTocResponse extends TocResponse {
    
     public static final String RESPONSE_TYPE="CONNECTIONLOST";
    
    /** Creates a new instance of LoginCompleteTocResponse */
     public ConnectionLostTocResponse() {
    }
    
    public String getResponseType() {
        return (RESPONSE_TYPE);
    }
    
    public String toString()
    {
        return (RESPONSE_TYPE);
    }
    
}
