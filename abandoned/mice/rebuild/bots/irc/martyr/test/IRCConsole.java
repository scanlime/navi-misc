package martyr.test;

import java.awt.BorderLayout;
import java.awt.Frame;
import java.awt.TextArea;
import java.awt.TextField;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import martyr.services.AutoResponder;
import martyr.GenericCommandAutoService;
import martyr.IRCConnection;
import martyr.InCommand;
import martyr.commands.RawCommand;

/**
 * IRC Console is a simple program to let you issue raw IRC commands.  It
 * uses Martyr to manage to socket and process auto commands.
 */
 
public class IRCConsole extends Frame
{
	public static final long serialVersionUID = 1;
	private transient IRCConnection connection;
	private TextArea outputArea;
	private TextField inputArea;
	
	public static void main( String args[] )
		throws Exception
	{
		if( args.length != 2 )
		{
			System.err.println("Args: server port" );
			System.exit(1);
		}
		String server = args[0];
		int port = Integer.parseInt( args[1] );
		
		IRCConsole frame = new IRCConsole();
		
		
		frame.setVisible( true );
		frame.setSize( 600, 400 );

		frame.connect( server, port );
	}

	public IRCConsole()
	{
		connection = new IRCConnection();
		new AutoResponder( connection );

		outputArea = new TailTextArea( "", 60, 5 );
		inputArea = new TextField();
		
		new CommandListener( connection, outputArea );
		
		inputArea.addActionListener( new ActionListener()
		{
			public void actionPerformed( ActionEvent ae )
			{
				outputArea.append( "SEND: " + inputArea.getText() + "\n" );
				connection.sendCommand( new RawCommand( inputArea.getText() ));
				inputArea.setText("");
			}
		} );
		
		setLayout( new BorderLayout() );
		
		add( outputArea, BorderLayout.CENTER );
		add( inputArea, BorderLayout.SOUTH );
	}
	
	public void connect( String server, int port )
		throws java.io.IOException, java.net.UnknownHostException
	{
		connection.connect( server, port );
	}
	
	public static class CommandListener extends GenericCommandAutoService
	{
		private TextArea ta;
		
		public CommandListener( IRCConnection conn, TextArea texta )
		{
			super( conn );
			this.ta = texta;

			this.enable();
		}

		public void updateCommand( InCommand c )
		{
			ta.append( "RECV: " + c.getSourceString() + "\n" );
		}
	}
	
}

/** TailTextArea 'tails' any text that is added with the 'append' method.
    That is, it will add the text then make sure it is visible. */
class TailTextArea extends TextArea {
    public TailTextArea( String str, int a, int b ) {
        super( str, a, b, SCROLLBARS_BOTH );
    }
    public void append( String str ) {
        super.append( str );
        setCaretPosition( getText().length() - 1 );
    }
}

