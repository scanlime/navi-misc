/*
 * Original version: Ben Damm <bdamm@dammfine.com>
 * Changes by: Mog
 * 	- Retains the nick that is in use
 * 	*/
package martyr.errors;

import martyr.InCommand;
import martyr.State;
import martyr.util.FullNick;

public class NickInUseError extends GenericError
{
private FullNick _nick;

public NickInUseError()
{
	_nick = null;
}
public NickInUseError(FullNick nick)
{
	_nick = nick;
}

public State getState()
{
	return State.UNKNOWN;
}

public String getIrcIdentifier()
{
	return "433";
}

public InCommand parse( String prefix, String identifier, String params )
{
	return new NickInUseError(new FullNick(getParameter(params, 1)));
}

/**
 * @return The nick in use.
 */
public FullNick getNick()
{
	return _nick;
}

}

