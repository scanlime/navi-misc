/**
 * This serves as an abstract interface for interview protocol classes
 */

public class ainterview
{
	//the current question that is being dealt with
	private question current;
	
	//the question queues that this particular person cares about
	private qqueue pullfrom;		//the queue it pulls from
	private qqueue answ;		//the queue that gets answers
	private qqueue rjct;		//the queue that gets rejects
	
	private String identifier; //the name associated with this interviewee

	/**
	 * The interview contstructor, a new instance is created with each client
	 * @param outname The name that goes out with every question answered
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public ainterview(String outname)
	{
		identifier = outname;
	}
	
	/**
	 * The interview initialization method without authentication
	 * @param root The question database this pulls from
	 * @param name The identifier for this interviewee
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void intinit(qbase root, String name)
	{
		pullfrom = root.getIntQqueue(name);
		answ = root.answered;
		rjct = root.rejected;
		current = null;
	}

	/**
	 * The interview initialization method with authentication
	 * @param root The question database this pulls from
	 * @param name The identifier for this interviewee
	 * @param pass The password for the interviewee
	 * @author Brandon Smith
	 * @version 2.0
	 * @return True on sucess, false on failure
	 */
	public boolean intinit(qbase root, String name, String pass)
	{
		pullfrom = root.getIntQqueue(name);
		if(!root.authenticateInt(name,pass)) return false;
		answ = root.answered;
		rjct = root.rejected;
		current = null;
		return true;
	}
	
	/**
	 * This method returns a string ready to be sent across the network
	 * @author Brandon Smith
	 * @version 2.0
	 * @return A String thats network ready.
	 */
	public String getQuestion()
	{
		if(current != null) return null;
		current = pullfrom.pop();
		if(current == null) return null;
		return current.toNet();
	}
	
	/**
	 * This method handles the answering of the current question
	 * @param answer The string that has the answer in it
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void answer(String answer)
	{
		if(current == null) return;
		current.addAnswer(identifier,answer);
		answ.add(current);
		//
		//hit the abstract interface for outgoing stuff
		//
		current = null;
	}
	
	/**
	 * This method rejects the current question
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