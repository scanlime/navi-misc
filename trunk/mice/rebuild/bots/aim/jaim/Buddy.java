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
 * Buddy.java
 *
 * Created on 4 May 2002, 12:05
 */

package jaim;

/** This is a buddy object that holds the buddy's name and other
 * information about a buddy.
 * @author Brett Humphreys
 */
 public class Buddy {
	 /** Name of the buddy */
	 private String buddyName;
	 
	 /** Permit value */
	 private boolean permit;
	 
	 /** deny value */
	 private boolean deny;
	 
	 /** Constructor that sets the buddy name 
	  * @param name the name of this buddy.
	  */
	 public Buddy ( String name )
	 {
		 buddyName = name;
	 }
	 
	 /** Gets the buddy name 
	  * @return buddy name
	  */
	  public String getName()
	  {
		  return buddyName;
	  }
	 /** Sets the permit value
	  * @param permitVal what to set permit to
	  */
	 public void setPermit( boolean permitVal )
	 {
		 permit = permitVal;
	 }
	 
	 /** Gets the permit value
	  * @return permit value
	  */
	 public boolean getPermit( )
	 {
		 return permit;
	 }
	 
	 /** Sets the deny value
	  * @param denyVal what to set deny to
	  */
	 public void setDeny( boolean denyVal )
	 {
		 deny = denyVal;
	 }
	 
	 /** Gets the deny value
	  * @return deny value
	  */
	 public boolean getDeny( )
	 {
		 return deny;
	 }
 }
