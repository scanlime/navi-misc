package martyr.dcc;

import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.util.StringTokenizer;

import martyr.Debug;
import martyr.GenericCommandAutoService;
import martyr.IRCConnection;
import martyr.InCommand;
import martyr.commands.CtcpMessage;

/**
 * <p>AutoDccSetup enables an application to "listen" for when another
 * client is offering a DCC connection to us.
 * AutoDccSetup checks incoming CTCPs to see if they are DCC
 * connection requests, and if so, sets up the connection.  You can
 * use the AutoDccSetup by subclassing it with your own AutoDccSetup
 * and overriding the appropriate methods.</p>
 *
 * <p>In order to use this class you must provide an implementation for
 * authorizeConnect.  Additionally, you will want to override
 * getDccChatHandler and/or getDccSendHandler, so that a customized
 * version of the stock DccChatHandler and DccSendHandler is provided.
 * This gives you easy access to the chat stream and file stream,
 * respectively.</p>
 *
 * <p>Note that all the methods in this class are executed in the event
 * handler thread.  The dcc session should occur in a seperate thread,
 * which is why getDccChatHandler and getDccSendHandler return
 * seperate classes.  The handler classes are threads which are
 * spawned by AutoDccSetup and should self-terminate.</p>
 *
 * <p>DccTestBot provides a simple example of how DCC works.</p>
 *
 * @see martyr.test.DccTestBot
 * @see #getDccChatHandler
 * @see #getDccSendHandler
 *
 */
public abstract class AutoDccSetup extends GenericCommandAutoService
{

public AutoDccSetup( IRCConnection connection )
{
	super( connection );
	enable();
}

/**
 * Checks to see if the message is a CTCP message, and if so, calls
 * handleCtcp.  Exceptions thrown by handleCtcp are ignored.  If you
 * wish to handle exceptions, you should override this method.  Simply
 * check if updated is an instance of CtcpMessage, and then call
 * handleCtcp.
 */
public void updateCommand( InCommand updated )
{
	if( updated instanceof CtcpMessage )
	{
		try
		{
			handleCtcp( (CtcpMessage)updated );
		}
		catch( IOException ioe )
		{
			ioe.printStackTrace();
		}
		catch( DccException dcce )
		{
			dcce.printStackTrace();
		}
	}
}

/**
 * @throws InvalidDccException if the DCC of not of a known type
 * @throws UnauthorizedDccException if the DCC is not authorized
 * @throws IOException if the connection failed.
 */
protected void handleCtcp( CtcpMessage ctcpMsg )
	throws DccException, IOException
{
	if( ! ctcpMsg.getAction().toLowerCase().equals( "dcc" ) )
	{
		// Nothing to see
		return;
	}
	
	Debug.println( this, ctcpMsg.getMessage(), Debug.EXCESSIVE );
	StringTokenizer tokens = new StringTokenizer( ctcpMsg.getMessage() );
	
	String typeStr = tokens.nextToken();
	String argument = tokens.nextToken();
	String addressStr = tokens.nextToken();
	String portStr = tokens.nextToken();
	String fileSizeStr = ( tokens.hasMoreTokens() ? tokens.nextToken() : null );

	// Bug: addressStr is often incorrect.
	// Therefore, use source of message.
	addressStr = ctcpMsg.getSource().getHost();
	
	// At this point we should decide if we want to proceed or
	// not.
	InetAddress netaddr = InetAddress.getByName( addressStr );
	int port = Integer.parseInt( portStr );
	int fileSize = ( fileSizeStr == null ? -1 : Integer.parseInt( fileSizeStr ) );
	
	DccType type = DccType.getInstance( typeStr );
	
	if( ! authorizeConnect( type, argument, netaddr, port, fileSize ) )
	{
		throw new UnauthorizedDccException( "Denied." );
	}
	
	// At this point we should proceed with making the connection and
	// setting up a stream.
	Socket socket = makeSocket( netaddr, port );
	
	handleDccConnection( socket, type, argument, fileSize );
}

/**
 * Returns a DCC chat handler.  A chat session consists of each side
 * sending a line of text over the socket, whenever desired.
 */
protected DccChatHandler getDccChatHandler( Socket socket )
{
	return new DccChatHandler( socket );
}

/**
 * Returns a DCC file "send" (receive to us) handler.  A DCC file
 * transfer session is fairly slow, but effective, peer to peer
 * scheme.  A DccSendHandler subclass does not need to worry about the
 * details of the protocol, a subclass merely needs to provide a
 * stream that the file can be written out to.
 */
protected DccSendHandler getDccSendHandler( Socket socket, String filename, int filesize )
{
	return new DccSendHandler( socket, filename, filesize );
}

/**
 * Handles a chat or send connection.  It calls either
 * getDccSendHandler or getDccChatHandler and calls start() on the
 * result.
 */
protected void handleDccConnection( Socket socket, DccType type, String filename, int filesize )
{
	if( type == DccType.SEND )
	{
		getDccSendHandler( socket, filename, filesize ).start();
	}
	else
	{
		getDccChatHandler( socket ).start();
	}
}

/**
 * Generates a connected socket, connected to the machine that made
 * the request for a DCC.
 */
protected Socket makeSocket( InetAddress dest, int port )
	throws IOException
{
	return new Socket( dest, port );
}

/**
 * This is called after parsing the request to find out if the
 * connection should be performed.  Note that execution of this
 * occurrs in the event thread, so if an option is presented to the
 * user, a subclass should override update(...) so that processing
 * occurs in its own thread and does not block the event handler.
 *
 * @param type Either DccType.SEND or DccType.CHAT
 * @param argument "CHAT" if a chat, a filename if a file.
 * @param netaddr The address, although only the raw IP will be
 * useful.
 * @param port The port to connect on
 * @param filesize The filesize, if one was provided.
 */
protected abstract boolean authorizeConnect( DccType type, String argument, InetAddress netaddr, int port, int filesize );



public String toString()
{
	return "AutoDccSetup";
}


// END AutoResponder
}
 


 

