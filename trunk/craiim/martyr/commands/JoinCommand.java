package martyr.commands;

import martyr.Debug;
import martyr.InCommand;
import martyr.clientstate.Channel;
import martyr.clientstate.ClientState;
import martyr.util.FullNick;


/**
 * Defines JOIN command.
 */
public class JoinCommand extends GenericCommand
{

private String channel;
private String secret;
private FullNick user;

/** For use as a factory */
public JoinCommand()
{
	this.user = null;
	this.channel = null;
	this.secret = null;
}

/**
 * This constructor is used with an incoming JOIN command from the server.
 */
private JoinCommand( FullNick user, String channel )
{
	this.user = user;
	this.channel = channel;
	this.secret = null;
}

/**
 * This constructor is used to make a request to join a channel that
 * requires a secret key to join.
 *
 * @param user The user
 * @param secret The secret key required to enter the channel, or null of
 * none.
 */
public JoinCommand( String channel, String secret )
{
	this.secret = secret;
	this.user = null;
	this.channel = channel;
}

/**
 * This constructor is used to make a request to join a channel.
 */
public JoinCommand( String channel )
{
	this.secret = null;
	this.user = null;
	this.channel = channel;
}

public InCommand parse( String prefix, String identifier, String params )
{
	return new JoinCommand( new FullNick( prefix ), getParameter( params, 0 ) );
}

public String getIrcIdentifier()
{
	return "JOIN";
}

public String renderParams()
{
	if( secret == null )
		return channel;
	else
		return channel + " " + secret;
}

public String getChannel()
{
	return channel;
}

public String getSecret()
{
	return secret;
}

public FullNick getUser()
{
	return user;
}

public boolean weJoined( ClientState state )
{
	return user.equals( state.getNick() );
}

public boolean updateClientState( ClientState state )
{
	if( weJoined( state ) )
	{
		// We've joined a group.
		Debug.println( "JOIN", "We've joined " + channel, Debug.VERBOSE );
		state.addChannel( channel );
		return true;
	}
	else
	{
		// Someone else joined the group.
		Debug.println( "JOIN", "" + user + " joined " + channel, Debug.VERBOSE );
		// 1) Grab group
		Channel channelObj = state.getChannel( channel );
		// 2) Add user
		channelObj.addMember( user, this );
		return true;
	}
}
	

}


