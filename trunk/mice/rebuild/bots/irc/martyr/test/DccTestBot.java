package martyr.test;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import martyr.services.AutoJoin;
import martyr.services.AutoRegister;
import martyr.services.AutoResponder;
import martyr.Debug;
import martyr.GenericCommandAutoService;
import martyr.IRCConnection;
import martyr.InCommand;
import martyr.OutCommand;
import martyr.commands.MessageCommand;
import martyr.dcc.AutoDccSetup;
import martyr.dcc.ChatRequestCommand;
import martyr.dcc.DccChatHandler;
import martyr.dcc.DccSendHandler;
import martyr.dcc.DccType;


/**
 *
 * <p>The DccTestBot waits for DCC connections and establishes
 * connections on request, see the source for how it is done.</p>
 *
 * <p>If it receives a DCC chat offer, it will accecpt the connection and
 * send back anything "heard" on the dcc channel.</p>
 *
 * <p>If it receives a file offer, it will accept the connection and
 * write the file to the directory the bot was started in.</p>
 *
 * <p>If it receives a PRIVMSG saying "chatme" it will attempt to
 * establish a chat connection with the sender, by offering a dcc chat
 * session.  If the session is established, it will echo back
 * everything said to it.  Not yet implemented.</p>
 *
 * <p>If it received a PRIVMSG saying "fileme xxxx" it will attempt to send a
 * file of size xxxx to the sender, by offering a DCC SEND session.
 * If the session is established, it will send some random text 
 * under the name "test.txt".  Not yet implemented.</p>
 */
public class DccTestBot 
{

/**
 * This DCC send handler simply writes all files to the current
 * directory, taking care to not overwrite an existing one.
 */
static class TestSendHandler extends DccSendHandler
{
	public TestSendHandler( Socket socket, String filename, int size ) 
	{
		super( socket, filename, size );
	}
	
	protected OutputStream getStreamForReceive( String filename, int size )
		throws IOException
	{
		File file = new File( filename );
		
		boolean b = file.createNewFile();
		if( !b ) return null;
		
		FileOutputStream fos = new FileOutputStream( file );
		
		return fos;
	}
}

/**
 * This DCC chat handler simply echoes back everything said to it.
 */
static class TestChatHandler extends DccChatHandler
{
	public TestChatHandler( Socket socket )
	{
		super( socket ); // super socket! wow!
	}

	public TestChatHandler()
	{
	}

	public void handleDccChat( Socket socket )
		throws IOException
	{
		Debug.println( this, "Handling DCC chat", Debug.VERBOSE );
		try
		{
			BufferedReader br = 
				new BufferedReader( 
					new InputStreamReader( socket.getInputStream() ) );
			
			PrintWriter pw = 
				new PrintWriter( 
					new OutputStreamWriter( socket.getOutputStream() ) );

			doChat( br, pw );
			
			socket.close();
		}
		finally
		{
			Debug.println( this, "Terminated DCC chat", Debug.VERBOSE );
		}
	}

	/**
	 * Default implementation just echoes back every line received.
	 * Uses \n as a line terminator.
	 * */
	protected void doChat( BufferedReader in, PrintWriter out )
		throws IOException
	{
		String rline;
			
		while( (rline = in.readLine()) != null )
		{
			out.print( rline + "\n" );
			out.flush();
		}
	}

	public String toString()
	{
		return "TestChatHandler";
	}
}

private IRCConnection connection;


private static class MessageMonitor extends GenericCommandAutoService 
{
	public MessageMonitor( IRCConnection connection )
	{
		super( connection );
	}

	public void updateCommand( InCommand command )
	{
		if( command instanceof MessageCommand )
		{
			MessageCommand mc = (MessageCommand)command;
			if( mc.isPrivateToUs( this.connection.getClientState() ) )
			{
				handleCommand( mc );

				
			}
		}
	}

	protected void handleCommand( MessageCommand mc )
	{
		if( mc.getMessage().equals( "talk" ) )
		{
			// Offer a chat request to the sender

			try
			{
				OutCommand out =
					new ChatRequestCommand( this.connection,
							mc.getSource().getNick(), new TestChatHandler(  ));

				this.connection.sendCommand( out );
			}
			catch( IOException ioe )
			{
				Debug.println( this, "Can't form connection", Debug.CRITICAL );
				ioe.printStackTrace();
			}
		}
		else
		{
			Debug.println( this, "Unknown command: " + mc.getMessage(), Debug.VERBOSE );
		}
	}
}

public DccTestBot( String nick )
{
	connection = new IRCConnection();
	AutoRegister autoReg = new AutoRegister( connection, nick, "foo_user", "foo_name" );
	AutoResponder autoRes = new AutoResponder( connection );
	AutoJoin autoJoin = new AutoJoin( connection, "#martyr" );
	AutoDccSetup dccS = new AutoDccTestSetup( connection );

	// Our command interceptor
	// Also intercepts a state change to UNCONNECTED and, if we are
	// ready to quit, shuts down the VM.
	new MessageMonitor( connection);
}

public void connect( String server, int port )
	throws UnknownHostException, IOException
{
	connection.connect( server, port );
}


public static void main( String args[] )
	throws Exception
{
	if( args.length != 3 )
	{
		System.out.println( "Requires nick, server, port" );
		System.exit(1);
	}
	Debug.println( "main", "Starting test", Debug.NORMAL );

	DccTestBot bot = new DccTestBot( args[0] );

	Debug.println( "main", "Connecting", Debug.NORMAL );
	bot.connect( args[1], Integer.parseInt( args[2] ) );

	// Allow 15 seconds for the authentication and auto-join.
	// Of course no real program would do something idiotic like this.
	// See Justin for a better example.
	//Thread.sleep(15000);
}

/**
 * This AutoDccSetup returns the custom chat and send handlers when
 * requested to do so.  This provided the critical hook into allowing
 * the DccTestBot to define the behaviour of these operations.
 */
static class AutoDccTestSetup extends AutoDccSetup
{
	public AutoDccTestSetup( IRCConnection conn )
	{
		super( conn );
	}

	protected boolean authorizeConnect( DccType type, String argument,
			InetAddress netaddr, int port, int filesize )
	{
		return true;
	}

	protected DccSendHandler getDccSendHandler( Socket socket, String filename, int size )
	{
		return new TestSendHandler( socket, filename, size );
	}

	protected DccChatHandler getDccChatHandler( Socket socket )
	{
		return new TestChatHandler( socket );
	}
}

}

