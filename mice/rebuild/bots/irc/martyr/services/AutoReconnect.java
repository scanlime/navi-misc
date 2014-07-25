package martyr.services;

import java.io.IOException;
import java.net.UnknownHostException;

import martyr.Debug;
import martyr.IRCConnection;
import martyr.InCommand;
import martyr.GenericAutoService;
import martyr.State;
import martyr.clientstate.ClientState;
import martyr.commands.QuitCommand;

/**
 * <p>AutoReconnect performs the job of reconnecting to the server, if
 * the connection is terminated unexpectedly.  AutoReconnect 
 * will try to reconnect 5 times, and then give up.  If AutoReconnect
 * intercepts a QUIT command before the state change that is issued by
 * us (bounced back from the server) then it will not try to
 * reconnect.</p>
 *
 * <p>Note that AutoReconnect has no play in nick negotiation, and as
 * such, a failed nick negotiation does not count as a connection
 * retry.</p>
 *
 * <p>Testing note: Does a server send a QUIT before a forceful
 * removal from the network?  Should AutoReconnect not intercept
 * QUITs? Certainly not all servers send QUITs even when you QUIT on
 * your own; this class should be put into a command-out chain as well.</p>
 */
public class AutoReconnect extends GenericAutoService
{

private static final int DBG_LVL = Debug.VERBOSE;

private int attempt;
private int maxAttempts;
private int sleepTime;
private boolean disableOnQuit;

// How many times to try reconnecting?  
public static final int DEFAULT_MAX_ATTEMPTS = 5;
// TODO This is a rather simplistic method, personally I would like to
// see a version of this class that implements a backoff algorithm.

// If we tried to connect, but failed, how long do we wait until we
// try again (ms)?
public static final int DEFAULT_CONNECT_SLEEPTIME = 1000;

// If we get a QUIT command from the server notifying us that we have
// QUIT, then self-disable so that we don't reconnect.
public static final boolean DEFAULT_DISABLE_ON_QUIT = true;

/**
 * @param connection The IRCConnection
 * @param maxAttempts The number of tries to do before giving up
 * @param sleepBetween Milliseconds to sleep between retries
 */
public AutoReconnect( IRCConnection connection, int maxAttempts,
		int sleepBetween, boolean disableOnQuit )
{
	super( connection );

	this.disableOnQuit = disableOnQuit;
	this.maxAttempts = maxAttempts;
	this.sleepTime = sleepBetween;
	this.attempt = 0;

	enable();
}

public AutoReconnect( IRCConnection connection, int maxAttempts,
		int sleepBetween )
{
	this( connection, maxAttempts, sleepBetween, DEFAULT_DISABLE_ON_QUIT );
}

/**
 * Initializes with reasonable defaults.
 */
public AutoReconnect( IRCConnection connection )
{
	this( connection, DEFAULT_MAX_ATTEMPTS, DEFAULT_CONNECT_SLEEPTIME );
}

/**
 * Attempts to connect, returning only when a connection has been made
 * or repeated connections have failed.
 * */
public void go( String server, int port )
{
	doConnectionLoop( server, port );
}

/**
 * Attempts a single connection to the server.  The connection is done
 * by asking the client state what server and port we are supposed to
 * be connected to, and then calling IRCConnection.connect(...).
 * */
protected void connect()
	throws IOException
{
	ClientState cstate = getConnection().getClientState();
	connect( cstate.getServer(), cstate.getPort() );
}

/**
 * Attempts a single connection to the server. 
 * */
protected void connect( String server, int port )
	throws UnknownHostException, IOException
{
	getConnection().connect( server, port );
}

protected void updateState( State state )
{
	Debug.println( this, "Update with state " + state, Debug.EXCESSIVE );
	if( state == State.UNCONNECTED )
	{
		// This should only happen if we were connected and then
		// disconnected.  Try connecting.
		doConnectionLoop();	
	}

	Debug.println( this, "Returned from " + state, Debug.EXCESSIVE );
}

/**
 * Calls connect() followed by failedToConnect(...) until a connection
 * is made.  Gets the server and port from the client state, implying
 * that a connection was already attempted before.
 * */
protected void doConnectionLoop()
{
	// Tell called proc to use client state info
	doConnectionLoop( null, -1 );
}

/**
 * Calls connect() followed by failedToConnect(...) until a connection
 * is made, or the service is disabled.
 * */
protected void doConnectionLoop( String server, int port )
{
	boolean keeptrying = true;

	while( keeptrying && enabled )
	{
		Exception error = null;
		try
		{
			if( server == null )
			{
				// Try getting the info from the client state
				connect();
			}
			else
			{
				connect( server, port );
			}
			keeptrying = false;
		}
		catch( Exception e )
		{
			error = e;
			keeptrying = true;
		}

		if( keeptrying )
		{
			keeptrying = failedToConnect( error );
		}
	}
}

/**
 * Called when the final failure has occurred.  Default implementation
 * does nothing.
 * */
protected void finalFailure()
{
	Debug.println( this, "Final failure.", DBG_LVL );
}

/**
 * Called when a failure to connect occurs.  This method should do
 * whatever needs to be done between connection attempts; usually this
 * will be sleeping the current thread and checking if we should stop
 * trying to reconnect.
 *
 * @return true if another attempt at connecting should occur
 * */
protected boolean failedToConnect( Exception error )
{
	Debug.println( this, "Error connecting: " + error, DBG_LVL );

	this.attempt++;

	// abort if we've tried too many times
	if( attempt >= maxAttempts )
	{
		Debug.println( this, "Tried " + attempt + " times, giving up.", DBG_LVL );
		finalFailure();
		return false;
	}

	// Well, try again damnit!
	// Sleep for a bit first.
	try
	{
		Thread.sleep( sleepTime );
	}
	catch( InterruptedException ie )
	{
		// And we're going to do what?
		return false;
	}

	return true;
}

/**
 * AutoReconnect will disable itself if it sees a quit command
 * generated by returned from the server.  This usually happens just
 * before the server terminates the connection.  Note that this would
 * really be better if we could intercept messages on their way out,
 * but Martyr doesn't do that.
 */
protected void updateCommand( InCommand command )
{
	if( disableOnQuit 
			&& command instanceof QuitCommand 
			&& ((QuitCommand)command).isOurQuit(getConnection().getClientState()) )
	{
		Debug.println( this, "Disabling due to receiving own QUIT.", DBG_LVL );
		disable();
	}
}

public String toString()
{
	return "AutoReconnect [" + attempt + "]";
}

}
 


 
