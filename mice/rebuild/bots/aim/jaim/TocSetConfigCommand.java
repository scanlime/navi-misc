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
 * TocSetConfigCommand.java
 *
 * Created on October 11, 2002, 9:08 AM
 */

package jaim;

import java.util.Enumeration;

/**
 *
 * @author  paulw
 */
public class TocSetConfigCommand extends TocCommand {
    
    private StringBuffer config;
    
    private static String CMD="toc_set_config ";

    /** Creates a new instance of TocSetConfigCommand */
    public TocSetConfigCommand() {
        config=new StringBuffer();
    }
    
    public void addGroup(Group g)
    {
        config.append("g "+g.getName()+"\n");
        Enumeration buddies=g.enumerateBuddies();
        while (buddies.hasMoreElements())
        {
            Buddy b = (Buddy)buddies.nextElement();
            config.append("b "+b.getName()+"\n");
            if (b.getPermit())
            {
                config.append("p "+b.getName()+"\n");
            }
            if (b.getDeny())
            {
                config.append("d "+b.getName()+"\n");
            }
        }
    }
        
    
    public String toString()
    {
        return(CMD+'"'+config.toString()+'"');
    }

    public byte[] getBytes() {
        return(toString().getBytes());
    }
    
}
