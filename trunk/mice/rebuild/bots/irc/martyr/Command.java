package martyr;

/**
 * Defines an object which is a command, either incoming or outgoing.
 */
public interface Command 
{
	/**
	 * Returns the string IRC uses to identify this command.  Examples:
	 * NICK, PING, KILL, 332.  Not strictly required for OutCommands
	 * as the irc identifier is expected to be part of the reder()
	 * result.
	 */
	String getIrcIdentifier();
}

