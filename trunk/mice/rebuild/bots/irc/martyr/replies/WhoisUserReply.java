package martyr.replies;

import martyr.InCommand;
import martyr.util.ParameterIterator;
import martyr.Debug;

public class WhoisUserReply extends AbstractWhoisReply
{

	private String host;
	private String name;

	/**
	 * Factory constructor.
	 * */
	public WhoisUserReply()
	{
	}

	public WhoisUserReply( String params )
	{
		super( params );
	}

	public String getIrcIdentifier()
	{
		return "311";
	}

	protected void parseParams( ParameterIterator pi )
	{
		pi.next(); // throw away the nick
		host = (String)pi.next(); // hostmask
		Debug.println( "WhoisUserReply", "host: " + host, Debug.EXCESSIVE );
		name = pi.last(); // the "Name"
		Debug.println( "WhoisUserReply", "name: " + name, Debug.EXCESSIVE );
	}

	public InCommand parse( String prefix, String identifier, String params )
	{
		return new WhoisUserReply( params );
	}

}

