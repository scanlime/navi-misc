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
 * TocSetIdleCommand.java
 *
 * Created on July 17, 2002, 9:21 PM
 */

package jaim;

/**
 *
 * @author  paulw
 * @version $version: $
 */
public class TocSetIdleCommand extends TocCommand {

    private int idle;
    private static final String CMD="toc_set_idle ";
    
    /** Creates new TocSetIdleCommand
     *@param idleSecs - the period for which the user has been idle
     */
    public TocSetIdleCommand(int idleSecs) {
        idle=idleSecs;
    }
    
    public String toString()
    {
        return(CMD+idle);
    }

    public byte[] getBytes() {
        return(toString().getBytes());
    }
    
}
