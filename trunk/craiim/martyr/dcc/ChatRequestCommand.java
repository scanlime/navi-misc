package martyr.dcc;

import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;

import martyr.Debug;
import martyr.IRCConnection;
import martyr.OutCommand;
import martyr.commands.CtcpMessage;

/**
 * ChatRequestCommand sets up a socket to listen for a dcc connection
 * and then sends a request to chat to the indicated party.
 * Still experimental.
 */
//TODO make the chat handler have more control over the socket
//handling (eg what to do if the ServerSocket times out)
//
//TODO not working yet
public class ChatRequestCommand implements OutCommand
{

	/**
	 * Defaults to 30 minutes 
	 * */
	public static final int SO_TIMEOUT = 1000*30;

	private String rendered;

	/**
	 * @param conn requred to discover the local address.
	 * */
	public ChatRequestCommand( IRCConnection conn, String toWhom, final DccChatHandler handler )
		throws IOException
	{
		this( conn.getLocalAddress(), toWhom, handler, SO_TIMEOUT );
	}

	public ChatRequestCommand( InetAddress local, String toWhom, final DccChatHandler handler, int timeout )
		throws IOException
	{
		byte[] raw = local.getAddress();

		long addr = 0;

		// find the "32 bit int" representation of the local ip addr.
		for (int octet=0; octet < raw.length; octet++)
		{
			int one = (((int)raw[octet]) & 0xff);
		//	System.out.print("." + one);
			addr = addr << 8;
			addr += one;
		}
		//System.out.println();
		//

		// any port, queue size, localaddr
		ServerSocket ss = new ServerSocket( 0, 1, local );
		ss.setSoTimeout( timeout );

		startHandlerThread(ss, handler);

		int lport = ss.getLocalPort();
		rendered = new CtcpMessage( toWhom, "DCC", "CHAT CHAT " + addr + " " + lport ).render();
		Debug.println( this, rendered, Debug.VERBOSE );
	}

	/**
	 * Called by the constructor, this method starts the thread that
	 * waits for connections and starts the handler.  Subclasses can
	 * override this method if they want to do something before
	 * calling super.startHandlerThread.
	 * */
	protected void startHandlerThread(
			final ServerSocket ss,
			final DccChatHandler handler)
	{
		new HandlerStarter( ss, handler ).start();
	}

	private static class HandlerStarter extends Thread
	{
		private ServerSocket ss;
		private DccChatHandler handler;

		public HandlerStarter( ServerSocket ss, DccChatHandler handler )
		{
			this.ss = ss;
			this.handler = handler;
		}

		public void run()
		{
			try
			{
				Socket s = ss.accept();

				handler.setSocket( s );
				handler.start();
			}
			catch( IOException ioe )
			{
				Debug.println( this, ioe.toString(), Debug.CRITICAL );
				ioe.printStackTrace();
			}
		}
	}

	public String getIrcIdentifier()
	{
		return null;
	}

	public String render()
	{
		return rendered;
	}

// END ChatRequestCommand
}
 


 

