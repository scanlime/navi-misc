/**
 * This class is a moderator abstract interface
 */

public class amoderate
{
	//the current question that this person is dealing with
	private question current;
	
	//the question queues that the moderator has access to
	private qqueue input;
	private qqueue rjct;
	private qqueue[] targets;
	private String[] names;

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
		if(current != null) return null;
		current = input.pop();
		return current.toNet();
	}
	
	/**
	 * This method accepts the question and puts it in the correct queue
	 * @param queue The queue associated with the person that this question is for
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void accept(int queue)
	{
		if(current == null) return;
		if(queue >= targets.length) return;
		targets[queue].add(current);
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
}