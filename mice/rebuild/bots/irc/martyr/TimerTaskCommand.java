package martyr;

import java.util.TimerTask;

// TODO: BD: Unit test
// TODO: BD: synchronization semantics?

/**
 * This class delays sending a command to the IRC connection.
 *
 * @author <a href="mailto:martyr@mog.se">Morgan Christiansson</a>
 */
public class TimerTaskCommand extends TimerTask
{

	private IRCConnection _conn;
	private OutCommand _cmd;
	public TimerTaskCommand(IRCConnection conn, OutCommand cmd)
	{
		_conn = conn;
		_cmd = cmd;
	}
	/* (non-Javadoc)
	 * @see java.util.TimerTask#run()
	 */
	public void run()
	{
		_conn.sendCommand(_cmd);
		isScheduled = false;
	}

	private boolean isScheduled = true;
	
	/* (non-Javadoc)
	 * @see java.util.TimerTask#cancel()
	 */
	public boolean cancel()
	{
		boolean ret = super.cancel();
		isScheduled = false;
		return ret;
	}

	/**
	 * @return true if the command has yet to run or is running, false
	 * otherwise.
	 */
	public boolean isScheduled()
	{
		return isScheduled;
	}

}

