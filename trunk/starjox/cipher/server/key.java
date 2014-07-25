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
    public int[] keystring;
	
    /**
     * This is an empty constructor, as this class is static
     * @author Brandon Smith
     * @version 2.0
     */
    public key()
    {
	keystring = key.keygen();
    }

    public key(int[] kei)
    {
	keystring = kei;
    }

    public int[] encrypt(int[] text)
    {
	return key.encrypt(keystring,text);
    }
    
    public int[] decrypt(int[] text)
    {
	return key.decrypt(keystring,text);
    }

    public int[] getKey()
    {
	return keystring;
    }

    /**
     * This method creates a key
     * @author Brandon Smith
     * @version 2.0
     * @return A random cypher key
     */
    public static int[] keygen()
    {
	int[] newkey = new int[256];
	keyinfo start, end;
	int i;
	int j;
	int k;
	end = new keyinfo(0,null);
	start = end;
	for(i=1;i<256;i++)
	    start = new keyinfo(i,start);
	end.next = start;
	k = 0;
	while(start != start.next)
	{
	    j = (int) (Math.random() * 256);
	    for(i=0;i<j;i++) start = start.next;
	    newkey[k++] = start.next.code;
	    start.next = start.next.next;
	}
	newkey[255] = start.code;
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
    public static int[] encrypt(int[] mykey, int[] cleartext)
    {
	int[] toreturn = new int[cleartext.length];
	int i;
	int index;
	if(mykey.length != 256) return null;
	for(i=0;i<cleartext.length;i++)
	{
	    index = cleartext[i];
	    toreturn[i] = mykey[index];
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
    public static int[] decrypt(int[] mykey, int[] encrypted)
    {
	int[] toreturn = new int[encrypted.length];
	int i,j,character,alpha=0;
	if(mykey.length != 256) return null;
	for(i=0;i<encrypted.length;i++)
	{
	    character = encrypted[i];
	    j = 0;
	    while(character != mykey[j]) j++;
	    toreturn[i] = j;
	}
	return toreturn;
    }

    private static class keyinfo
    {
	/** The character this bit of the list. */
	public int code;
	/** A reference to the next node in the list */
	public keyinfo next;
	
	/**
	 * This method creates a new node
	 * @param in The character this node holds
	 * @param prev A reference to the previous node so it can be set.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public keyinfo(int in, keyinfo prev)
	{
	    code = in;
	    next = prev;
	}
    }
}
