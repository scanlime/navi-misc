package martyr;

import java.util.Iterator;

import martyr.clientstate.ClientState;

/**
 * Defines commands that come from the server.  Errors and replies are
 * incoming commands.
 *
 * @see martyr.commands.GenericCommand
 * @see martyr.errors.GenericError
 * @see martyr.replies.GenericReply
 */
public interface InCommand extends Command
{

/**
 * Some commands, when received by the server, can only occur in one
 * state.  Thus, when this command is received, the protocol should
 * assume that it is in that state, and a state change may be
 * triggered.  A command can use the 'unknown' state to indicate it
 * can be received in any state (for example, ping).
 */
State getState();

/**
 * Every incoming command should know how to register itself with the
 * command register.
 */
void selfRegister( CommandRegister commandRegister );

/**
 * Parses a string and produces a formed command object, if it can.
 * Should return null if it cannot form the command object.  The
 * identifier is usually ignored, except in the special case where
 * commands can be identified by multiple identifiers.  In that case,
 * the behaviour of the command may change in sublte ways.
 */
InCommand parse( String prefix, String identifier, String params );

/**
 * Gives the command a copy of the raw string from the server.  Called
 * by IRCConnection after the command is parsed.
 */
void setSourceString( String str );

/**
 * Allows a third party to receive a copy of the raw string.
 */
String getSourceString();

/**
 * Asks the command to ensure that information it knows about the
 * state the server thinks the client is in matches what we have.
 * Returns true if state changes were made.
 */
boolean updateClientState( ClientState state );


/**
 * Returns an iterator of String objects over the attribute names
 * for this command.  Warning: Still new, support for this is not
 * yet widespread.  Should return all possible attribute keys, not just
 * those that have a value in the current context.
 */
Iterator getAttributeKeys();

/**
 * Returns the attribute, or null if the attribute does not exist,
 * or is not defined.
 */
String getAttribute( String key );
}




