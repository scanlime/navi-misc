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
 * TocSignonCommand.java
 *
 * Created on 4 May 2002, 11:20
 */

package jaim;

/**
 *
 * @author  paulw
 * @version $Revision: 1.3 $
 */
public class TocSignonCommand extends TocCommand {
    
    private String server;
    private String username;
    private String password;
    private int port;
    
    private static final String AGENTNAME="jaim01";
    
    /** Creates new TocSignonCommand */
    public TocSignonCommand(String server, int port, String username, String password) {
        this.server=server;
        this.port=port;
        this.username=Utils.normalise(username);
        this.password=Utils.roast(password);
    }

    public byte[] getBytes() {
        return toString().getBytes();
    }
    
    public String toString()
    {
       String temp="toc_signon login.oscar.aol.com 5159 "+username+" "+password+" english "+AGENTNAME; 
       return(temp);
    }
    
    public void parseString(java.lang.String str) {
    }
    
}
