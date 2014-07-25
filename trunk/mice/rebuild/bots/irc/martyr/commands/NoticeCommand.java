package martyr.commands;

import martyr.InCommand;
import martyr.State;


/**
 * Defines the NOTICE command.
 */
public class NoticeCommand extends GenericCommand
{

private String notice;

/** Factory */
public NoticeCommand()
{
	notice = null;
}

public NoticeCommand( String notice )
{
	this.notice = notice;
}

public State getState()
{
	return State.UNKNOWN;
}

/**
 * Parses a string and produces a formed command object, if it can.
 * Should return null if it cannot form the command object.
 */
public InCommand parse( String prefix, String identifier, String params )
{
	String str = getParameter( params, 0 );
	return new NoticeCommand( str );
}

/**
 * Returns the string IRC uses to identify this command.  Examples:
 * NICK, PING, KILL, 332
 */
public String getIrcIdentifier()
{
	return "NOTICE";
}

/**
 * Renders the parameters of this command.
 */
public String renderParams()
{
	return ":" + notice;
}

public String getNotice()
{
	return notice;
}

}


