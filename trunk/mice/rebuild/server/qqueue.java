/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class handles all the queue stuff, with locks so nobody messes with it
 * while someone else wants to play with it.
 */

import java.util.*;

public class qqueue
{
	//this holds the state of the queue, so that two people don't try to use it at the same time
	private boolean locked = false;
	
	//this holds the questions for the queue
	private Vector questions;
	
	/**
	 * Creates a new question queue
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public qqueue()
	{
		questions = new Vector();
	}
	
	/**
	 * This method is to get the next question in line
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The next question in line to be answered
	 */
	public question pop()
	{
		return extract(0);
	}
	
	/**
	 * This method adds a question to the end of the question queue.
	 * @param toadd the question to be put at the end of the queue.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void add(question toadd)
	{
		questions.addElement(toadd);
	}	

	/**
	 * This method extracts a specific question from the queue
	 * @param location The location in the queue that the desired question is
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The desired question, or null if the location is out of the range.
	 */
	public question extract(int location)
	{
		question toreturn;
		/* wait for the thing to become "unlocked" */
		while(locked) 
			sleep(4);
		locked = true;
		try
		{
			toreturn = (question) questions.elementAt(location);
			questions.removeElementAt(location);
		}
		catch(ArrayIndexOutOfBoundsException exception)
		{
			locked = false;
			return null;
		}
		locked = false;
		return toreturn;
	}
	
	/**
	 * This method inserts a question into the queue at a specific location
	 * @param toadd The question that needs to be put in
	 * @param location the location that the question is going to go into
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void insert(question toadd, int location)
	{
		while(locked) 
			sleep(4);
		try
		{
			questions.insertElementAt(toadd,location);
		}
		catch(ArrayIndexOutOfBoundsException exception)
		{
			questions.addElement(toadd);
		}
		locked = false;
	}
	
	/**
	 * This method makes this thread sleep for 4 miliseconds in the hopes that it
	 * the other threads let execution complete.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	private void sleep(int miliseconds)
	{
		try
		{
			Thread.sleep(4);
		} 
		catch (InterruptedException e) {}
	}
	
	/**
	 * This method outputs the queue to a string type thing
	 * @author Brandon Smith
	 * @version 2.0
	 * @return A string version of the queue.
	 */
	/*public String toString()
	{
		int i;
		String toreturn = "";
		question temp;
		while(locked) 
		{
			try
			{
				Thread.sleep(4);
			} 
			catch (InterruptedException e) {}
		}
		locked = true;
		try
		{
			for(i=0;i<questions.size();i++)
			{
				temp = (question) questions.elementAt(i);
				toreturn = toreturn + "Question " + i + ":\n" + temp.toString();
			}
		}
		catch(Exception e) {}
		locked = false;
		return toreturn;
	}*/
}