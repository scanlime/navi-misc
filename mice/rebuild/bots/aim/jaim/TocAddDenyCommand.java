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
 * TocAddDenyCommand.java
 *
 * Created on 4 May 2002, 13:57
 */

package jaim;

import java.util.Vector;

/**
 *
 * @author  paulw
 * @version $Revision: 1.3 $
 */
public class TocAddDenyCommand extends TocCommand {

    private static String CMD="toc_add_deny";
    
    Vector buddyList;
    
    /** Creates new TocAddBuddyCommand */
    public TocAddDenyCommand() {
        buddyList=new Vector();
    }

    public void addDeny(String buddy)
    {
        buddyList.add(Utils.normalise(buddy));
    }
    
    public String toString()
    {
        StringBuffer output=new StringBuffer(CMD);
        for (int i=0;i<buddyList.size();i++)
        {
            output.append(' ');
            output.append((String)buddyList.elementAt(i));
        }
        return(output.toString());
    }
    
    
    public byte[] getBytes() {
        return(toString().getBytes());
    }
    
}
