/**
 * This interface is what the update network guys implement
 * @author Brandon Smith
 * @version 2.0
 */
public interface aupdatei
{
	/**
	 * This method is called when a question is answered.
	 * @param foo The question that got answered.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void update(question foo);
	
	/**
	 * This method is called when a notice is sent out by the server.
	 * @param foo The notice itself.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void notice(String foo);
}