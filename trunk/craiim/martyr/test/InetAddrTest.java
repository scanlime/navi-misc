package martyr.test;

import java.net.InetAddress;

/**
 * This is simply a unit test to determine the behaviour of the
 * InetAddress class.
 */
public class InetAddrTest
{

public static void main( String args[] )
	throws Exception
{
	if( args.length != 1 )
	{
		System.out.println( "Requires a parameter to pass to InetAddress.getByName(...)" );
		System.exit(1);
	}
	
	InetAddress netaddr = InetAddress.getByName( args[0] );

	System.out.println( "getAddress: " + netaddr.getAddress() );
	System.out.println( "getHostAddress: " + netaddr.getHostAddress() );
	System.out.println( "getHostName: " + netaddr.getHostName() );
}

}

