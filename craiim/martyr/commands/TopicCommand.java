package martyr.commands;

import java.util.Date;

import martyr.CommandRegister;
import martyr.Debug;
import martyr.InCommand;
import martyr.clientstate.Channel;
import martyr.clientstate.ClientState;

public class TopicCommand extends GenericCommand
{

private String channel;
private String topic;

public static final String IDENTIFIER_PRIMARY = "TOPIC";
public static final String IDENTIFIER_SECONDARY = "332";

public TopicCommand()
{
	this( null, null );
}

public TopicCommand( String channel, String topic )
{
	this.channel = channel;
	this.topic = topic;
}

public String getIrcIdentifier()
{
	//
	// This command uses "TOPIC" on outgoing, so that is why we use
	// "TOPIC" here instead of "332".
	//
	return IDENTIFIER_PRIMARY;
}

public void selfRegister( CommandRegister commandRegister )
{
	commandRegister.addCommand( IDENTIFIER_PRIMARY, this );
	commandRegister.addCommand( IDENTIFIER_SECONDARY, this );
}

public InCommand parse( String prefix, String identifier, String params )
{
	// when the command is used as a reply, the nick is parameter 0.
	if( identifier.equals( IDENTIFIER_SECONDARY ) )
		return new TopicCommand( getParameter(params, 1), getParameter(params, 2) );
	else
		return new TopicCommand( getParameter(params, 0), getParameter(params, 1) );
}

public String renderParams()
{
	return getChannel() + " :" + getTopic();
}

public String getTopic()
{
	return topic;
}

public String getChannel()
{
	return channel;
}

public boolean updateClientState( ClientState state )
{
	Debug.println( "Topic", "Channel: " + channel , Debug.EXCESSIVE );
	Channel chan = state.getChannel( channel );
	chan.setTopic( topic );
	chan.setTopicDate( new Date() );
	return true;
}

}

