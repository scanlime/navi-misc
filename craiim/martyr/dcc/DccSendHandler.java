package martyr.dcc;

import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

import martyr.Debug;

/**
 * A DccSendHandler manages a DCC "send" (to us) file transfer.  If
 * you wish to use this class to handle a DCC session, provide an
 * implementation for getStreamForReceive in a subclass.
 * 
 * @see martyr.dcc.AutoDccSetup#getDccSendHandler
 * @see martyr.test.DccTestBot
 * @see #getStreamForReceive
 */
public class DccSendHandler extends Thread
{
	private Socket socket_;
	private String filename_;
	private int filesize_;

	private static int DEFAULT_BUF_SIZE = 5000;

	public DccSendHandler( Socket socket, String filename, int filesize )
	{
		this.socket_ = socket;
		this.filename_ = filename;
		this.filesize_ = filesize;
	}
	
	/**
	 * A call to this method means that a connection has been established
	 * and processing the file should commence.  Note that normally
	 * one would not override this method, but rather, override
	 * getStreamForReceive.
	 *
	 * @see #getStreamForReceive
	 */
	protected void handleDccSend( Socket socket, String filename, int filesize )
		throws IOException
	{
		OutputStream dest = getStreamForReceive( filename, filesize );

		// read/write as per protocol
		
		InputStream incoming = socket.getInputStream();
		DataOutputStream outgoing 
			= new DataOutputStream( socket.getOutputStream() );
		
		// The generally accepted packet size is 1024 bytes.  However,
		// this is arbitrary, therefore we will make a somewhat larger
		// buffer.
		byte[] buffer = new byte[DEFAULT_BUF_SIZE];

		int totalin = 0;
		try
		{

			while( true )
			{
				int recvd = incoming.read( buffer );
				
				if( recvd < 0 )
				{
					// We are done
					break;
				}
				dest.write( buffer, 0, recvd );
				totalin += recvd;
				outgoing.writeInt( totalin );
			}
		}
		finally
		{
			dest.close();
		}
		
		Debug.println( this, "Done writing file: " + filename, Debug.VERBOSE );
		Debug.println( this, "Total bytes: " + totalin, Debug.VERBOSE );
		socket.close();
	}
	
	/**
	 * This method should return an output stream that a file can be
	 * placed into.  Default implementation throws a
	 * FileNotFoundException.  This is the only method you must
	 * override (in this class) to provide fully functional DCC file transfers.
	 */
	protected OutputStream getStreamForReceive( String filename, int filesize )
		throws IOException
	{
		throw new FileNotFoundException("Refusing to write '" + filename + "'" );
	}
	
	public void run()
	{
		try
		{
			handleDccSend( socket_, filename_, filesize_ );
		}
		catch( IOException ioe )
		{
			ioe.printStackTrace();
		}
	}
}

