package martyr;

/**
 * This class has been deprecated, use
 * martyr.services.AutoJoin insead.
 * */
public class AutoJoin extends martyr.services.AutoJoin 
{
	/**
	 * @deprecated Use martyr.services.AutoJoin instead.
	 * */
	public AutoJoin( IRCConnection connection, String channel )
	{
		super( connection, channel );
	}

	/**
	 * @deprecated Use martyr.services.AutoJoin instead.
	 * */
	public AutoJoin( IRCConnection connection, String channel, String key )
	{
		super( connection, channel, key );
	}
}
