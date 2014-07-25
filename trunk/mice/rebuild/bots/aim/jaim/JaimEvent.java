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

/** The JaimEvent object is delivered to all registered {@link JaimEventListener}
 * @see JaimConnection#addEventListener 
 * @author  paulw
 * @version $revision: $
 */
public class JaimEvent extends java.util.EventObject {

    private TocResponse tocResponse;
    
    /** Creates new JaimEvent */
    public JaimEvent(Object source, TocResponse tocResponse) {
        super(source);
        this.tocResponse=tocResponse;
    }
    
    public TocResponse getTocResponse()
    {
        return(tocResponse);
    }

}
