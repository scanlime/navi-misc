package martyr.commands;

import java.util.Enumeration;

import martyr.Debug;
import martyr.InCommand;
import martyr.clientstate.Channel;
import martyr.clientstate.ClientState;
import martyr.util.FullNick;
/**
 * <p>Defines QUIT command.  The QUIT command asks the irc server to
 * disconnect us, and we can optionally give a reason.  The QUIT
 * command is also received by us if someone on a channel we are on
 * quits.</p>
 *
 * <p>What should be done to signal to the framework that the
 * disconnection that should come from the server is legit, and we
 * shouldn't try to re-connecet?  For now it will be assumed that the
 * user of the framework will signal all the appropriate classes that
 * a legit disconnection will happen (ie AutoRegister which will try
 * to re-connect otherwise).</p>
 */
public class QuitCommand extends GenericCommand
{

private String reason;
private FullNick user;

/** For use as a factory */
public QuitCommand()
{
	this( null, null );
}

/** For use as an incoming command. */
public QuitCommand( FullNick user, String reason )
{
	this.user = user;
	this.reason = reason;
}

/** For use as an outgoing command. */
public QuitCommand( String reason )
{
	this( null, reason );
}

public InCommand parse( String prefix, String identifier, String params )
{
	return new QuitCommand( new FullNick( prefix ), getParameter( params, 0 ) );
}

public String getIrcIdentifier()
{
	return "QUIT";
}

public String renderParams()
{
	return ":" + reason;
}

public String getReason()
{
	return reason;
}

public FullNick getUser()
{
	return user;
}

/**
 * Returns true if we are the ones quitting.
 */
public boolean isOurQuit( ClientState state )
{
	return user.equals( state.getNick() );
}

/** If we are quitting, we won't be worrying about our client state.
 * If someone else is leaving, then remove them from all the groups
 * they are in.
 */
public boolean updateClientState( ClientState state )
{
	System.out.println( "Nick: " + state.getNick().toString() );
	if( isOurQuit(state) )
	{
		// We've quit
		Debug.println( "QUIT", "We've quit: " + reason, Debug.VERBOSE );
		
		// What should we do with the client state here?
		return true;
	}
	else
	{
		// Someone else quit.  We need to remove them from each group
		// they are in.
		Debug.println( "QUIT", "" + user + " quit: " + reason, Debug.VERBOSE );
		
		// 1) Grab channels
		Enumeration channelNames = state.getChannelNames();
		while( channelNames.hasMoreElements() )
		{
			String chanName = channelNames.nextElement().toString();
		
			// 2) Remove from group.
			Channel channelObj = (Channel)state.getChannel( chanName);
			channelObj.removeMember( user, this );
		}
		
		return true;
	}
}
	

}


