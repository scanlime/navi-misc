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
 * TocEvilCommand.java
 *
 * Created on 6 May 2002, 09:05
 */

package jaim;

/**
 *
 * @author  paulw
 * @version $Revision: 1.3 $
 */
public class TocEvilCommand extends TocCommand {

    private String buddy;
    private boolean anonymous;
   
    /** Creates new TocEvilCommand */
    public TocEvilCommand(String buddy, boolean anonymous) {
        this.buddy=Utils.normalise(buddy);
        this.anonymous=anonymous;
    }
    
    public String toString()
    {
        String ret="toc_evil "+buddy;
        if (anonymous)
        {
            ret=ret+" anon";
        }
        else
            ret=ret+" norm";
        
        return(ret);
    }
    
    public byte[] getBytes() {
        return toString().getBytes();
    }
    
}
