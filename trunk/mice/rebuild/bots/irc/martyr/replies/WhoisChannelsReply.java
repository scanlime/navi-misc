package martyr.replies;

import martyr.InCommand;
import martyr.util.ParameterIterator;
import martyr.Debug;
import java.util.HashSet;
import java.util.Set;
import java.util.StringTokenizer;

public class WhoisChannelsReply extends AbstractWhoisReply
{

	private String channels;

	/**
	 * Factory constructor.
	 * */
	public WhoisChannelsReply()
	{
	}

	public WhoisChannelsReply( String params )
	{
		super( params );
	}

	public String getIrcIdentifier()
	{
		return "319";
	}

	/**
	 * @return a space-delimited list of channels
	 * */
	public String getChannels()
	{
		return channels;
	}

	/**
	 * @return a set of Strings of channels
	 * */
	public Set getChannelSet()
	{
		StringTokenizer tokens = new StringTokenizer( channels );
		Set set = new HashSet();
		while( tokens.hasMoreTokens() )
		{
			set.add( tokens.nextToken() );
		}

		return set;
	}

	protected void parseParams( ParameterIterator pi )
	{
		channels = pi.last(); // Channels

		Debug.println( "WhoisChannelsReply", "channels: " + channels, Debug.EXCESSIVE );
	}

	public InCommand parse( String prefix, String identifier, String params )
	{
		return new WhoisChannelsReply( params );
	}

}

