/**
 * This is the class that handles the abstraction for submitting a question, thus simplifying the network protocol for it.
 * @author Brandon Smith
 * @version 2.0
 */

public class asubmit
{
	/** Holds the question queue that is for unwashed questions. */
	public static qqueue putin;
	
	/** 
	 * This method initializes the queue
	 * @param root The question database this pulls from
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void init(qbase root)
	{
		putin = root.input;
	}
	
	/**
	 * This method submits a question into the database, pure and simple.
	 * @param Name The name of the person submitting the quesiton.
	 * @param Question The question itself (no newlines permitted)
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void submit(String Name, String Question)
	{
		//this is where we have checks for auto-rejection
	
		//create a new question
		question in = new question(Name,Question);
		//add it to the input queue
		putin.add(in);
	}
}
