/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class handles the updating of the various update client situations
 */

import java.util.*;

public class aupdate
{
	private Vector list;
	
	/**
	 * Creates a new instance of the abstract updater
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public aupdate()
	{
		list = new Vector();
	}
	
	/**
	 * Registers a specific update class with the updater
	 * @param mynew The newly added update interface object
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void add(aupdatei mynew)
	{
		list.add(mynew);
	}
	
	/**
	 * Removes a specific updater class from the vector
	 * @param tokill The update interface to remove
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void remove(aupdatei tokill)
	{
		list.remove(tokill);
	}
	
	/**
	 * This method tells all the update clients associated with it to update
	 * @param completed the question that just got answered
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void sendAnswer(question completed)
	{
		int i;
		aupdatei foo;
		Object[] temp = list.toArray();
		for(i=0;i<temp.length;i++)
		{
			foo = (aupdatei) temp[i];
			foo.update(completed);
		}
	}
	
	/**
	 * This method sends a notice out to all the clients
	 * @param note The notice that everyone gets. (no newlines)
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void sendNotice(String note)
	{
		int i;
		aupdatei foo;
		Object[] temp = list.toArray();
		for(i=0;i<temp.length;i++)
		{
			foo = (aupdatei) temp[i];
			foo.notice(note);
		}
	}
}