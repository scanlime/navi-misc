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
	new AutoRegister( connection, "micebot", "oops", "try_again" );
	
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
	sayAction( "is online and all threads are GO!" );
	say( "Version 0.3.1" );
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
	if( !msg.isPrivateToUs( clientState ) )
	{
		System.out.println(msg.getSource() + " said " + msg.getMessage());
		return;
	}

	String sayCommand = "say ";
	String meCommand = "me ";
	String kickCommand = "kick ";
	String nickCommand = "nick ";
	String quitCommand = "quit ";
	String opCommand = "op";
	String setSecret = "setsecret";
	String showSecret = "showsecret";

	String message = msg.getMessage();
	
	StringTokenizer tokenizer = new StringTokenizer( message );
	// First, remove the command.
	String commandStr = tokenizer.nextToken().toLowerCase();

	if( commandStr.equals( opCommand ) )
	{
		if( tokenizer.countTokens() != 2 )
		{
			connection.sendCommand(
				new MessageCommand( msg.getSource(),
					"Incorrect number of parameters: " + message ) );
			return;
		}

		
		String personToOp = tokenizer.nextToken();
		String maybeSecret = tokenizer.nextToken();
		
		if( maybeSecret.equals( secret ) )
		{
			System.out.println( "Opping " + personToOp );
			connection.sendCommand(
				new RawCommand( "MODE " + channelName + " +o " + personToOp )
				);
		}
		else
		{
			connection.sendCommand(
				new MessageCommand( msg.getSource(),
					"Bad secret." + message ) );
		}
	}
	else if( commandStr.equals( setSecret ) )
	{
		if( tokenizer.countTokens() != 2 )
		{
			connection.sendCommand(
				new MessageCommand( msg.getSource(),
					"Incorrect number of parameters: " + message ) );
			return;
		}
		if( tokenizer.nextToken().equals( secret ) )
		{
			secret = tokenizer.nextToken();
			connection.sendCommand(
				new MessageCommand( msg.getSource(),
					"Secret has been set to " + secret ) );
		}
		else
		{
			connection.sendCommand(
				new MessageCommand( msg.getSource(),
					"Bad secret f00! Try using the '" +
					showSecret + "' command." ) );
		}
	}
	else if( commandStr.equals( showSecret ) )
	{
		System.out.println( "Show secret requested.  Secret is: " + secret );
		connection.sendCommand(
			new MessageCommand( msg.getSource(),
				"Secret has been output on standard out." ) );
	}
	else if( message.toLowerCase().startsWith( quitCommand ) )
	{
		// we have a 'quit' command
		String comment = getParameter( message, quitCommand );
		
		autoReconnect.disable();
		quit = true;
		
		connection.sendCommand( new QuitCommand( comment ) );
	}
	else if( message.toLowerCase().startsWith( sayCommand ))
	{
		// We have a 'say' command
		String sayString = getParameter( message, sayCommand );
		say( sayString );
	}
	else if( message.toLowerCase().startsWith( meCommand ))
	{
		// we have a 'me' command
		String sayString = getParameter( message, meCommand );
		sayAction( sayString );
	}
	else if( message.toLowerCase().startsWith( kickCommand ) )
	{
		// we have a 'kick' command
		long now = System.currentTimeMillis();
		final long delay = 60000;

		if( now - kickTime < delay )
		{
			connection.sendCommand(
				new MessageCommand( msg.getSource(), "Woa, gotta wait "
					+ ( (delay - now + kickTime) / 1000)
					+ " seconds before I can kick again." )
			);
			return;
		}

		kickTime = now;
		System.out.println( message );
		String comment = message.substring( sayCommand.length(), message.length()).trim();
		String nickToKick;
		int space = comment.indexOf( ' ' );
		if( space >= 0 )
		{
			nickToKick = comment.substring( 0, space );
			comment = comment.substring( space + 1, comment.length() );
		}
		else
		{
			nickToKick = comment;
			comment = "";
		}
		Debug.println( "Justin", "Kicking: " + nickToKick + " Comment: " + comment, Debug.VERBOSE );
		
		connection.sendCommand( new KickCommand( mainChannel.getName(), nickToKick, comment ) );
	} 
	else
	{
		// Umm..
		connection.sendCommand(
			new MessageCommand( msg.getSource(), "Bad syntax: " + message )
		);
	}
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
	String channame = "#roulette";
	String server = "irc.freenode.net";
	String key = null;
	
	Justin justin = new Justin( server, 6667, channame, key );
	Debug.setDebugLevel(Debug.FAULT);
	justin.connect();
}

}


