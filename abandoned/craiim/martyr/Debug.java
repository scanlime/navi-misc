package martyr;

import java.io.PrintStream;
import java.util.Date;

/**
 * Aids in debugging and logging for martyr.
 */
public class Debug
{

/**
 * Signals in internal fault, similar to failling an assertion but
 * the program doesn't halt.  Exception thrown.
 */
public static final int FAULT = 0;
/**
 * Something went wrong, maybe not our fault.  Exception thrown.
 */
public static final int CRITICAL = 5;
/**
 * Some kind of badness is going on here, but it is within acceptable limits
 * and handled nicely.
 */
public static final int BAD = 6;
/**
 * Information about normal procedures.
 */
public static final int NORMAL = 10;
/**
 * Information about some details of normal procedures.
 */
public static final int VERBOSE = 15;
/**
 * Excessive information about internals that no human should need to
 * know or care about.
 */
public static final int EXCESSIVE = 20;

// Defaults
private static PrintStream out = System.out;
private static int level = EXCESSIVE;
// Messages with this level or lower get stack dumps after the
// message.  (Default: CRITICAL and FAULT get stack dumps).
private static int stackDumpLevel = CRITICAL;

/**
 * Sets the output stream to where the debug messages will go.
 */
public synchronized static void setOutputStream( PrintStream out )
{
	Debug.out = out;
}

public synchronized static PrintStream getOutputStream()
{
	return Debug.out;
}

/**
 * Sets the debug level so that all messages equal to or less than
 * 'level' will be output.
 */
public synchronized static void setDebugLevel( int level )
{
	Debug.level = level;
}

/**
 * Writes out the message if msgLevel is less than or equal to the
 * internal level.
 */
public synchronized static void println( String sender, String message, int msgLevel )
{
	if( msgLevel <= level )
	{
		String d = (new Date()).toString();
		
		if( sender != null )
			out.println( d + " " + sender + ": " + message );
		else
			out.println( d + " " + message );
	}
	if( msgLevel <= stackDumpLevel )
	{
		Throwable toss = new Throwable();
		toss.fillInStackTrace();
		toss.printStackTrace( out );
	}
}

public static void println( Object sender, String message, int msgLevel )
{
	println( sender.toString(), message, msgLevel );
}

}

