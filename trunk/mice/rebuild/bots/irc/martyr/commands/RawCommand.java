package martyr.commands;

import java.util.StringTokenizer;

import martyr.OutCommand;

public class RawCommand implements OutCommand
{

private String sourceString;
private String ident;

/**
 * Tries to use the first "word" in the command as the identifier.
 * Using this constructor is not recommended.
 */
public RawCommand( String raw )
{
	sourceString = raw;
	StringTokenizer tokens = new StringTokenizer( raw );
	ident = tokens.nextToken();
}

/**
 * The rendered command will be <code>identifier + " " +
 * parameters</code>.  This constructure simply allows a correct
 * response to the <code>getIrcIdentifier</code> method.
 */
public RawCommand( String identifier, String parameters )
{
	ident = identifier;
	sourceString = ident + " " + parameters;
}

/**
 * Returns the identifier, if supplied, or null.
 */
public String getIrcIdentifier()
{
	return ident;
}

/**
 * Simply returns the string given in the constructor.
 */
public String render()
{
	return sourceString;
}

}


