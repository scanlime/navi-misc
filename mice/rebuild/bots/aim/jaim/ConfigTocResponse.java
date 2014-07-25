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
 * ConfigTocResponse.java
 * Created on 1, October 2002
 */
 package jaim;
 
 import java.util.*;
 
 /** A ConfigTocResponse contains the config message received from
  * the toc server.
  *This response is handled by the JaimConnection class, but may also be used by client programs.
  *Once this event has been received, information returned from {@link JaimConnection#getGroups} is valid
  * @author Brett Humphreys, Paul Wilkinson
  */
 public class ConfigTocResponse extends TocResponse implements TocResponseHandler {
	
       public static String RESPONSE_TYPE="CONFIG";
     
	/** The Vector of Group objects */
	private Vector buddyList = new Vector();
        
        /** The HashMap of known buddies */
        private HashMap buddies;
	
	/** The mode for this configuration */
	private int mode;
        
        /** Value for mode that indicates PERMIT ALL mode */
        public static final int PERMIT_ALL=1;
        
        /** Value for mode that indicates DENY ALL mode */
        public static final int DENY_ALL=2;
        
        /** Value for mode that indicates PERMIT SOME mode */
        public static final int PERMIT_SOME=3;
        
        /** Value for mode that indicates DENY SOME mode */
        public static final int DENY_SOME=4;
	
	/** Returns an Enumeration of groups.  Each Entry is a {@link Group} 
         * Each group then has an Enumeration of buddies within that group See {@link Group#enumerateBuddies}.  
	 * @return list of Group elements or an empty list if none are found.
	 */
	public Enumeration enumerateGroups()
	{
		return buddyList.elements();
	}

	/** Returns a Collection of groups. Each element is a {@link Group)
	 * @return the groups
         */
    public Collection getGroups() {
        java.util.Collection result = new Vector(buddyList);
        return result;
    }


	/** Get the response type of  this response.  This method is used by the response dispatcher within JaimConnection
	* @return The response type
	*/    
	public String getResponseType() {
		return RESPONSE_TYPE;
	}
	/** Parses the config string.
	 */
        public TocResponse parseString(String message)
        {
            ConfigTocResponse tr = new ConfigTocResponse();
            tr.doParse(message);
            return(tr);
        }
        
	private void doParse(String message)
	{
		cmd=message;
		int colonIndex = message.indexOf(':'); 
		//throw away the first word.
		message = message.substring(colonIndex+1, message.length());
                buddies = new HashMap();
		StringTokenizer tok = new StringTokenizer(message,"\n");
		String itemType;
		String itemValue;
		Group currentGroup=null;
		Buddy tmpBuddy;
		while( tok.hasMoreTokens() )
		{
			// Can't tokenize on both \n and space since there could be spaces
			// in the name, so parsing by hand.
			itemType = tok.nextToken();
			int firstSpace = itemType.indexOf(' ');
			itemValue = itemType.substring(firstSpace+1, itemType.length());
			itemType = itemType.substring(0, firstSpace);

			char type = itemType.charAt(0);
			switch (type)
                        {
                            case 'g':
				currentGroup = new Group(itemValue);
				buddyList.add(currentGroup);
                                break;
                        
                            case 'b':
			
				tmpBuddy = getBuddy(itemValue);
				//this shouldn't happen, but:
				if(currentGroup==null)
				{
					currentGroup = new Group("<unknown>");
					buddyList.add(currentGroup);
				}
				currentGroup.addBuddy(tmpBuddy);
				
				
                                break;
                                
                            case 'p':
				tmpBuddy = getBuddy(itemValue);
				tmpBuddy.setPermit(true);
                                break;
                                
                            case 'm':
				setMode(Integer.valueOf(itemValue).intValue());
                                break;
                                
                            case 'd':
			
				tmpBuddy = getBuddy(itemValue);
				tmpBuddy.setDeny(true);
                                break;
                        }
                }
	}
	
        /** Return an existing Buddy with the specified name or return a new buddy if the name is not known
         *  The buddy is added to the buddies hash if it is a new buddy
         *  @param The name of the buddy we are looking for
         *  @return The buddy object
         */
        
        private Buddy getBuddy(String buddyName)
        {
            Buddy retBuddy = (Buddy)buddies.get(buddyName);
            if (retBuddy== null)
            {
                retBuddy=new Buddy(buddyName);
                buddies.put(buddyName,retBuddy);
            }
            return(retBuddy);
        }
        
	/** Sets the mode for this configuration
	 * @param modeVal the string value of the mode (1-4)
	 */
	 public void setMode( int modeVal )
	 {
		 mode = modeVal;
	 }
	 
	 /** Gets the mode for this configuration
	  * @return mode for the configuration
	  */
	  public int getMode( )
	  {
		  return mode;
	  }
          
          /** Returns true if this response handler can handle the specified response.
           * @param Response - the response string from TOC.  This is the part of the response before the first ':'
           * @return true if the response can be handled
           */
          public boolean canHandle(String Response) {
           return(Response.equalsIgnoreCase(RESPONSE_TYPE));
          }
          
 }	 
