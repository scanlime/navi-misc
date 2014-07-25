package martyr.replies;

import java.util.StringTokenizer;

import martyr.Debug;
import martyr.InCommand;
import martyr.clientstate.Channel;
import martyr.clientstate.ClientState;

public class NamesReply extends GenericReply
{

private String names;
private String channel;

/** For use as a factory. */
public NamesReply()
{
}

public NamesReply( String channel, String names )
{
	this.names = names;
	this.channel = channel;
}

public String getIrcIdentifier()
{
	return "353";
}

public InCommand parse( String prefix, String identifier, String params )
{
	return new NamesReply( getParameter( params, 2 ), getParameter( params, 3 ) );
}

/**
 * Adds the list of names to the client state.
 */
public boolean updateClientState( ClientState state )
{
	boolean stateChanged = false;
	
	// 1) Get the Channel
	Channel channelObj = state.getChannel( channel );

	if( channel == null )
	{
		Debug.println( "NamesReply", "Channel is null", Debug.CRITICAL);
		return false;
	}
	
	if( channelObj == null )
	{
		Debug.println( "NamesReply",
			"No channel object for channel: " + channel, Debug.FAULT);
		return false;
	}
	
	
	// 2) Parse out names
	StringTokenizer tokens = new StringTokenizer( names, " " );
	
	while( tokens.hasMoreTokens() )
	{
		String nick = tokens.nextToken();
		
		// 3) Check that the user is not already in the list
		if( !channelObj.isMemberInChannel( nick ) )
		{
			channelObj.addMember( nick, this );
			stateChanged = true;
		}
	}

	return stateChanged;
}

}




