package martyr.replies;

import martyr.InCommand;
import martyr.util.ParameterIterator;
import martyr.Debug;

public class WhoisServerReply extends AbstractWhoisReply
{

	private String serverName;
	private String serverDesc;

	/**
	 * Factory constructor.
	 * */
	public WhoisServerReply()
	{
	}

	public WhoisServerReply( String params )
	{
		super( params );
	}

	public String getIrcIdentifier()
	{
		return "312";
	}

	/**
	 * @return the DNS name of the server
	 * */
	public String getServerName()
	{
		return serverName;
	}

	/** 
	 * @return the free-form description of the server
	 * */
	public String getServerDescription()
	{
		return serverDesc;
	}

	protected void parseParams( ParameterIterator pi )
	{
		serverName = (String)pi.next(); // Server name
		serverDesc = (String)pi.next(); // Server description
		Debug.println( "WhoisServerReply", "server name: " + serverName, Debug.EXCESSIVE );
		Debug.println( "WhoisServerReply", "server desc: " + serverDesc, Debug.EXCESSIVE );
	}

	public InCommand parse( String prefix, String identifier, String params )
	{
		return new WhoisServerReply( params );
	}

}

