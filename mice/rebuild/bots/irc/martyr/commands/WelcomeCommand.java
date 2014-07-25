package martyr.commands;

import martyr.CommandRegister;
import martyr.Debug;
import martyr.InCommand;
import martyr.clientstate.ClientState;
import martyr.util.FullNick;
import martyr.util.ParameterIterator;



/**
 * Defines the commands that a server issues to welcome us.  These are
 * identified with 001, 002... etc.  These commands are only received
 * after we register, unlike the NOTICE command.
 */
public class WelcomeCommand extends GenericInCommand
{

private String notice;
private String nick;

/** Factory */
public WelcomeCommand()
{
	this( null, null );
}

/**
 * Used by parse to create an instance of WelcomeCommand.
 * */
public WelcomeCommand( String nick, String notice )
{
	this.notice = notice;
	this.nick = nick;
	Debug.println( this, "Nick is: `" + nick + "'", Debug.VERBOSE );
	Debug.println( this, "Notice is: `"+notice+"'", Debug.VERBOSE );
}

/**
 * Parses a string and produces a formed command object, if it can.
 * Should return null if it cannot form the command object.
 */
public InCommand parse( String prefix, String identifier, String params )
{
	ParameterIterator pi = new ParameterIterator( params );
	String nick = pi.next().toString();
	String notice = null;
	if( pi.hasNext() )
	{
		// We are looking at a "nick :msg" pair
		notice = pi.next().toString();
	}
	else
	{
		// There is only one parameter, a notice.
		notice = nick;
		nick = null;
	}
	if( pi.hasNext() )
	{
		Debug.println( this, "More than two parameters, confused.", Debug.BAD );
	}


	//String str = getParameter( params, 0 );
	// 
	return new WelcomeCommand( nick, notice );
}

/**
 * Sets the nick of the client state, if there is one included with
 * this command.
 */
public boolean updateClientState( ClientState state )
{
	Debug.println( this, "updated client state with: " + new FullNick( nick ), Debug.EXCESSIVE );
	state.setNick( new FullNick( nick ) );

	return true;
}

/**
 * Returns the string IRC uses to identify this command.  Examples:
 * NICK, PING, KILL, 332.  In our case, there is no one thing.
 */
public String getIrcIdentifier()
{
	return "001";
}

public void selfRegister( CommandRegister commandRegister )
{
	commandRegister.addCommand( "001", this );
	commandRegister.addCommand( "002", this );
	commandRegister.addCommand( "003", this );
	commandRegister.addCommand( "004", this );
	commandRegister.addCommand( "005", this );
}

public String getNotice()
{
	return notice;
}

/**
 * @return the nick received with this command, or null if there isn't
 * one.
 * */
public String getNick()
{
	return nick;
}

public String toString()
{
	return "WelcomeCommand";
}

}


