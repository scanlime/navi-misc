/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This interface is what the update network guys implement
 * @author Brandon Smith
 * @version 2.0
 */
public interface aupdatei
{
	/**
	 * This method is called to initialize the updater
	 * @param host The host that the update server is running on
	 * @param port The port on the host that the server is on
	 * @author Brandon Smith
	 * @version 2.0
	 * @return True on a successful connection, false on a failed one
	 */
	public boolean connect(String host, int port);

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
	
	/**
	 * This method is called to turn off the update clients
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void quit();
}