package martyr;

/**
 * This class has been deprecated, use
 * martyr.services.AutoReconnect instead.
 */
public class AutoReconnect extends martyr.services.AutoReconnect
{
	/**
	 * @deprecated Use martyr.services.AutoReconnect instead.
	 */
	public AutoReconnect( IRCConnection connection, int maxAttempts,
			int sleepBetween, boolean disableOnQuit )
	{
		super( connection, maxAttempts, sleepBetween, disableOnQuit );
	}

	/**
	 * @deprecated Use martyr.services.AutoReconnect instead.
	 * */
	public AutoReconnect( IRCConnection connection, int maxAttempts,
			int sleepBetween )
	{
		super( connection, maxAttempts, sleepBetween );
	}

	/**
	 * @deprecated Use martyr.services.AutoReconnect instead.
	 */
	public AutoReconnect( IRCConnection connection )
	{
		super( connection );
	}
}
