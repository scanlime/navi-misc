/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
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

/**
 * This class handles key generation and String encryption and decryption
 * @author Brandon Smith
 * @version 2.0
 */
 
public class key
{
    /** The keystring for this instance of the object. */
    private String keystring;
	
    /**
     * This is an empty constructor, as this class is static
     * @author Brandon Smith
     * @version 2.0
     */
    public key()
    {
	keystring = key.keygen();
    }

    public key(String kei)
    {
	keystring = kei;
    }

    public String encrypt(String text)
    {
	return key.encrypt(keystring,text);
    }
    
    public String decrypt(String text)
    {
	return key.decrypt(keystring,text);
    }

    public String getKey()
    {
	return keystring;
    }

    /**
     * This method creates a key
     * @author Brandon Smith
     * @version 2.0
     * @return A random cypher key
     */
    public static String keygen()
    {
	String newkey = "";
	keyinfo start, end;
	char i;
	int j;
	end = new keyinfo('0',null);
	start = end;
	for(i=1;i<256;i++)
	    start = new keyinfo(i,start);
	end.next = start;
	while(start != start.next)
	{
	    j = (int) (Math.random() * 256);
	    for(i=0;i<j;i++) start = start.next;
	    newkey = newkey + start.next.code;
	    start.next = start.next.next;
	}
	newkey = newkey + start.code;
	return newkey;
    }
    
    /**
     * This method encrypts a string
     * @param mykey The key that encrypts the string
     * @param cleartext The string that needs to be cyphered
     * @author Brandon Smith
     * @version 2.0
     * @return The encrypted string
     */
    public static String encrypt(String mykey, String cleartext)
    {
	String toreturn = "";
	int i;
	int index;
	if(mykey.length() != 256) return null;
	for(i=0;i<cleartext.length();i++)
	{
	    index = cleartext.charAt(i);
	    toreturn = toreturn + mykey.charAt(index);
	}
	return toreturn;
    }
    
    /**
     * This method decrypts a string
     * @param mykey The key to use to decrypt the string
     * @param encrypted The encrypted string
     * @author Brandon Smith
     * @version 2.0
     * @return The string that was originally put in to be encrypted
     */
    public static String decrypt(String mykey, String encrypted)
    {
	String toreturn = "";
	int i;
	char j;
	char character;
	char alpha = 0;
	if(mykey.length() != 256) return null;
	for(i=0;i<encrypted.length();i++)
	{
	    character = encrypted.charAt(i);
	    j = 0;
	    while(character != mykey.charAt(j)) j++;
	    toreturn = toreturn + (char) j;
	}
	return toreturn;
    }
}
