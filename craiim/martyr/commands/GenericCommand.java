package martyr.commands;

import martyr.OutCommand;

/**
 * Defines a generic command.  Most commands will simply have to
 * override the getIrcIdentifier method and implement the parse and
 * render methods using convenience methods.
 */
public abstract class GenericCommand extends GenericInCommand implements OutCommand
{

/**
 * Forms a string appropriate to send to the server.  All commands can
 * be sent by the client.
 */
public String render()
{
	// no prefix, since we are sending as a client.
	return getIrcIdentifier() + " " + renderParams();
}

/**
 * Renders the parameters of this command.
 */
public abstract String renderParams();

}


