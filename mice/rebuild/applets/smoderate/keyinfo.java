/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This is a node in a circular linked list that makes it so that letters are 
 * returned, but not repeated.
 * @author Brandon Smith
 * @version 2.0
 */

public class keyinfo
{
	/** The character this bit of the list. */
	public char code;
	/** A reference to the next node in the list */
	public keyinfo next;
	
	/**
	 * This method creates a new node
	 * @param in The character this node holds
	 * @param prev A reference to the previous node so it can be set.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public keyinfo(char in, keyinfo prev)
	{
		code = in;
		next = prev;
	}
	
	/**
	 * This method returns a string representation of the list
	 * @author Brandon Smith
	 * @version 2.0
	 * @return A string that is the full length of the list
	 */
	public String toString()
	{
		if(next != null)
			return code + "" + next.toString();
		else
			return code + "";
	}
}