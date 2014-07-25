package martyr.commands;

import martyr.InCommand;
import martyr.clientstate.ClientState;
import martyr.util.FullNick;


/**
 * Defines the PRIVMSG command.  Messages can be sent to groups or to users.
 */
public class MessageCommand extends GenericCommand
{

private FullNick from;
private String dest;
private String message;


/** Factory */
public MessageCommand()
{
	from = null;
	dest = null;
	message = null;
}

/**
 * Used to send a message.
 */
public MessageCommand( String dest, String message )
{
	this( null, dest, message );
}

/**
 * Used to send a message.
 */
public MessageCommand( FullNick dest, String message )
{
	this( dest.getNick(), message );
}

public MessageCommand( FullNick source, String dest, String message )
{
	this.from = source;
	this.dest = dest;
	this.message = message;
}

/**
 * Parses a string and produces a formed command object, if it can.
 * Should return null if it cannot form the command object.
 */
public InCommand parse( String prefix, String identifier, String params )
{
	FullNick from = new FullNick( prefix );
	String dest = getParameter( params, 0 );
	String msg = getParameter( params, 1 );

	if( CtcpMessage.isCtcpString( msg ) )
	{
		return new CtcpMessage( from, dest, msg );
	}
	
	return new MessageCommand( from, dest, msg );
}

/**
 * Returns the string IRC uses to identify this command.  Examples:
 * NICK, PING, KILL, 332
 */
public String getIrcIdentifier()
{
	return "PRIVMSG";
}

/**
 * Renders the parameters of this command.
 */
public String renderParams()
{
	return dest + " :" + message;
}

public FullNick getSource()
{
	return from;
}

public String getDest()
{
	return dest;
}

public String getMessage()
{
	return message;
}

/**
 * Returns true if the message is both private and for us.
 */
public boolean isPrivateToUs( ClientState state )
{
	return state.getNick().equals( dest );
}

}


