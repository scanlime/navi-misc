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
 * TocSetInfoCommand.java
 *
 * Created on July 17, 2002, 9:02 PM
 */

package jaim;

/**
 *
 * @author  paulw
 * @version $version: $
 */
public class TocSetInfoCommand extends TocCommand {

    private String information;
     
    private static String CMD="toc_set_info ";
    /** Creates new TocSetInfoCommand
     *@param information The information about this user can be located.  May contain HTML
     */
    
    public TocSetInfoCommand(String information) {
        this.information=Utils.encodeText(information);
    }
    
    
     public String toString()
    {
        return(CMD+information);
    }

    public byte[] getBytes() {
        return(toString().getBytes());
    }
    
}
