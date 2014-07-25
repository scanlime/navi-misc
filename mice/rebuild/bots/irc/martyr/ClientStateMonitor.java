package martyr;

import java.util.Observable;
import java.util.Observer;

/**
 * ClientStateMonitor asks commands to update the client state.
 */
public class ClientStateMonitor implements Observer
{

private IRCConnection connection;

private boolean enabled = false;

/**
 * This should only be called by the IRCConnection itself.
 */
ClientStateMonitor( IRCConnection connection )
{
	this.connection = connection;

	enable();
}

public void enable()
{
	if( enabled )
		return;
	enabled = true;

	connection.addCommandObserver( this );
}

public void disable()
{
	if( !enabled )
		return;
	connection.removeCommandObserver( this );
	enabled = false;
}

public void update( Observable observable, Object command_o )
{
	InCommand command = (InCommand)command_o;
	
	try
	{
		if( command.updateClientState( connection.getClientState() ) )
			Debug.println( "ClientStateMonitor", "Client state updated", Debug.NORMAL );
	}
	catch( Throwable e )
	{
		Debug.println( "ClientStateMonitor", "Client state update failed.",
			Debug.CRITICAL );
		Debug.println( "ClientStateMonitor", e.toString(), Debug.CRITICAL);
		e.printStackTrace();
	}
	
}

// ===== END ClientStateMonitor
}

