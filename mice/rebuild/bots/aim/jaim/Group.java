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
 * Group.java
 *
 * Created on 4 May 2002, 12:05
 */

package jaim;

import java.util.Vector;
import java.util.List;
import java.util.Enumeration;

/** This is a logical user group.  It holds a set of users.
 * @author Brett Humphreys
 */
public class Group {
    
    /** Vector of buddies for this group */
    private Vector buddies = new Vector();
    
    /** Name of this group */
    private String groupName;
    
    /** This constructor sets the name of the group
     * @param name the group name
     */
    public Group( String name ) {
        groupName = name;
    }
    
    /** This method adds a buddy to the end of the group
     * @param buddy The buddy object to associate with this group
     */
    public void addBuddy(Buddy buddy) {
        buddies.add(buddy);
    }
    
    /** This method adds a buddy to the specified location in the group
     *  If the specified location is beyond the end of the group, then the buddy is added to the end of the group
     * @param buddy The buddy object to associate with this group
     * @param pos the position to add the buddy
     */
    public void addBuddy(Buddy buddy,int pos) {
        if (pos > buddies.size()) {
            buddies.add(buddy);
        }
        else {
            buddies.add(pos,buddy);
        }
    }
    
    /** This method gets the group name
     * @return the group name
     */
    public String getName() {
        return groupName;
    }
    
    /** This method returns the buddies in this group
     * @return an Enumeration of {@link Buddy} objects
     */
    public Enumeration enumerateBuddies() {
        return buddies.elements();
    }
    
    /** This method returns the number of buddies in this group
     * @return buddy count
     */
    public int getBuddyCount() {
        return(buddies.size());
    }

    /** This method returns the buddies in this group
     * @return a Collection of {@link Buddy} objects     
     */
    public java.util.Collection getBuddies() {
        java.util.Collection cReturn = new java.util.Vector(buddies);
        return cReturn;
    }

}
