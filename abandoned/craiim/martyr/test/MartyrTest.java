package martyr.test;

import martyr.services.AutoJoin;
import martyr.services.AutoReconnect;
import martyr.services.AutoRegister;
import martyr.services.AutoResponder;
import martyr.Debug;
import martyr.IRCConnection;
import martyr.commands.QuitCommand;

public class MartyrTest
{

/** A really simple test, all this does is login to a network and
 * join a channel, say hello, then die after 15 seconds.
 * */
public static void main( String args[] )
	throws Exception
{
	// Please do NOT copy-and-paste this code, you must have an
	// AutoService of your own in order to do anything useful (ie
	// interactive).
	Debug.println( "main", "Starting test", Debug.NORMAL );
	IRCConnection connection = new IRCConnection();
	// Alive connections do not keep the program running
	connection.setDaemon(true);

	AutoReconnect autoReconnect = new AutoReconnect( connection );
	AutoRegister autoReg = new AutoRegister( connection, "foo_nick", "foo_user", "foo_name" );
	AutoResponder autoRes = new AutoResponder( connection );
	//AutoJoin autoJoin = new AutoJoin( connection, "#martyr-", "test" );
	AutoJoin autoJoin = new AutoJoin( connection, "#martyr" );

	Debug.println( "main", "Connecting", Debug.NORMAL );

	// Note: Doesn't return until a connection is made
	autoReconnect.go( "irc.f00f.net", 6667 );

	// Allow 15 seconds for the authentication and auto-join.
	// Of course no real program would do something idiotic like this.
	// See Justin for a better example.
	Thread.sleep(15000);

	//autoReconnect.disable();
	connection.sendCommand( new QuitCommand("Time to die."));

	// Another few seconds to watch the aftermath of disconnecting
	// This might be enough time to see the AutoReconnect re-connect.
	Thread.sleep(10000);

	// Now it will really die.
}

}

