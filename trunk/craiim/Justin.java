import java.io.*;
import java.net.*;
import java.util.Enumeration;
import java.util.StringTokenizer;

import martyr.Debug;
import martyr.IRCConnection;
import martyr.services.*;
import martyr.clientstate.*;
import martyr.util.*;
import martyr.commands.*;


/**
 * Main entry point for justin.
 */
public class Justin
{
    
    public static Justin me;

    private IRCConnection connection;
    private AutoReconnect autoReconnect;
    
    private String server;
    // Set quit to true when it is time for justin to shut down.
    private boolean quit = false;
    private int port;
    private String channelName;
    private Channel mainChannel;
    private ClientState clientState;
    private long kickTime = 0; // The last time kick was used.
    private String secret = "justone";
    
    public Justin( String server, int port, String channel )
    {
	this( server, port, channel, null );
    }
    
    public Justin( String server, int port, String channel, String key )
    {
	this.server = server;
	this.port = port;
	this.channelName = channel;
	
	clientState = new JustinClientState( this );
	
	connection = new IRCConnection( clientState );

	// Required services
	new AutoResponder( connection );
	new AutoRegister( connection, "gonkirc", "gonkey", "Gonk U. Lator" );
	
	// We keep a hold on this one so we can disable it when justin
	// quits.
	autoReconnect = new AutoReconnect( connection );

	// Channel to join
	// Note that justin can really only be on one channel at a time.
	// This is a justin issue, not a martyr issue.
	// Jife, http://www.dammfine.com/~bdamm/projects/jife/, is a multi
	// channel bot.
	new AutoJoin( connection, channel, key );

	// Our command interceptor
	// Also intercepts a state change to UNCONNECTED and, if we are
	// ready to quit, shuts down the VM.
	new MessageMonitor( this );
    }
    
    public void connect()
	throws IOException, UnknownHostException
    {
	autoReconnect.go( server, port );
    }
    
    public void joinedChannel( Channel channel )
    {
	mainChannel = channel;
    }
    
    public void printMembers()
    {
	// Print out a list of people in our channel.
	Enumeration members = mainChannel.getMembers();
	while( members.hasMoreElements() )
	{
	    Member member = (Member)members.nextElement();
	    System.out.println(
			       "Member: " + member.getNick() +
			       " Ops: " + member.hasOps() +
			       " Voice: " + member.hasVoice() );
	}
    }
    
    public boolean shouldQuit()
    {
	return quit;
    }
    
    public void say( String msg )
    {
	connection.sendCommand( new MessageCommand( mainChannel.getName(), msg ) );
    }
    
    public void sayAction( String msg )
    {
	connection.sendCommand( new ActionCtcp( mainChannel.getName(), msg ) );
    }
    
    public void incomingMessage( MessageCommand msg )
    {
	cacher.head.cacheMessage(msg.getSource().toString(),msg.getMessage(),false);
    }
    
    private String getParameter( String raw, String command )
    {
	return raw.substring( command.length(), raw.length() );
    }
    
    public IRCConnection getConnection()
    {
	return connection;
    }
    
    
    public static void main( String args[] )
	throws Exception
    {
	String channame = "#tacobeam";
	String server = "irc.freenode.net";
	String key = null;
	
	Justin justin = new Justin( server, 6667, channame, key );
	Debug.setDebugLevel(Debug.FAULT);
	Justin.me = justin;
	botmain foo = new botmain("tacobeambot","hello");
	justin.connect();
    }   
}


