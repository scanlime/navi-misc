/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class is a moderator abstract interface
 */

public class amoderate
{
	/** the current question that this person is dealing with. */
	public question current;
	
	//the question queues that the moderator has access to
	private qqueue input;
	private qqueue rjct;
	private qqueue[] targets;
	private String[] names;
	
	//These give ID to this moderator for logging purposes.
	private int ID;
	private static int nextID = 0;
	
	/**
	 * Initializes the moderator interface
	 * @param root The question database that the moderator pulls from
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public amoderate(qbase root)
	{
		input = root.input;
		rjct = root.rejected;
		targets = root.waiting;
		names = root.interviewuser;
		ID = amoderate.nextID++;
	}

	/**
	 * This method authenticates the moderator within the server
	 * @param root The question database that the moderator pulls from
	 * @param username The username of this moderator
	 * @param password The password associated with the moderator
	 * @author Brandon Smith
	 * @version 2.0
	 * @return True on success, false on access denied!
	 */
	public boolean authenticate(qbase root, String username, String password)
	{
		return root.authenticateMod(username,password);
	}
	
	/**
	 * This returns the next question from the input queue
	 * @author Brandon Smith
	 * @version 2.0
	 * @return A network ready string for sending purposes
	 */
	public String getQuestion()
	{
		if(current != null)
			return "Please accept or reject: " + current.toNet();
		current = input.pop();
		if(current == null)
			return "No Questions\r\nAt This Time";
		return current.toNet();
	}
	
	/**
	 * This method accepts the question and puts it in the correct queue
	 * @param queue The queue associated with the person that this question is for
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void accept(int[] queue)
	{
		//if there is no question being moderated, forget anyone said anything
		if(current == null) return;
		if(queue.length == 0) return;
		
		//cycle through and push these into the accepted queues.
		int i;
		for(i=0;i<queue.length;i++)
		{
			//lets make sure I'm in the range with this
			if(queue[i] < targets.length && queue[i] >= 0)
			{
				if(i ==0)
					targets[queue[i]].add(current);
				else
					targets[queue[i]].add(current.duplicate());
			}
		}
		//all done, lets clean up a tad.
		current = null;
	}

	/**
	 * This method rejects the question, putting it in the reject bin
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void reject()
	{
		if(current == null) return;
		rjct.add(current);
		current = null;
	}
	
	/**
	 * This method handles logging
	 */
}