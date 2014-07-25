package martyr.commands;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;

import martyr.CommandRegister;
import martyr.Debug;
import martyr.InCommand;
import martyr.Mode;
import martyr.OutCommand;
import martyr.State;
import martyr.clientstate.Channel;
import martyr.clientstate.ClientState;

/**
 * Defines MODE command.  Since the MODE command is of two distinct
 * types, this class is really more of a command mini-factory.  It
 * determines which type of command it is, either a UserModeCommand or
 * a ChannelModeCommand.
 *
 */
public class ModeCommand implements InCommand, OutCommand
{
	public static final String IDENTIFIER = "MODE";
	private String source;
	
	/** For use as a factory */
	public ModeCommand()
	{
	}
	
	public Iterator getAttributeKeys()
	{
		return new LinkedList().iterator();
	}

	public String getAttribute( String key )
	{
		return null;
	}

	public static void registerMode( Map modes, Mode mode )
	{
		Character modeChar = new Character( mode.getChar() );

		if( modes.get( modeChar ) != null )
		{
			Debug.println( "ModeCommand", "Warning: Two modes with same letter: " + 
				modes.get( modeChar ) + " and " + mode, Debug.CRITICAL );
		}
		
		modes.put( modeChar, mode );
	}
	
	public State getState()
	{
		return State.REGISTERED;
	}
	
	public void selfRegister( CommandRegister reg )
	{
		reg.addCommand( IDENTIFIER, this );
	}

	public String getIrcIdentifier()
	{
		return IDENTIFIER;
	}
	
	// Example
	// <pre>:repp_!bdamm@dammfine.com MODE #bytesex +oo z * repp_telnet</pre>
	public InCommand parse( String prefix, String identifier, String params )
	{
		// there are two kinds of modes.  Either a channel mode, or a user
		// mode.  We need to figure out which we are dealing with, and
		// return that.
		
		// TODO: Research: Should we specify delimiters other than whitespace?
		StringTokenizer tokens = new StringTokenizer( params );
	
		String str = tokens.nextToken();
	
		//Debug.println( this, "Prefix: " + prefix + " str: " + str
		//	+ " total: " + params, Debug.EXCESSIVE );
		
		// Malformed command.
		if( str == null )
			return null;
		
		// Should we check to see if the string is really a channel
		// that we know about?
		if( Channel.isChannel( str ) )
		{
			return new ChannelModeCommand( prefix, str, tokens );
		}
		else 
		{
			return new UserModeCommand( prefix, str, tokens );
		}
	}
	
	/**
	 * Should not be called, as ModeCommand doesn't actually represent a
	 * command.  Use UserModeCommand or ChannelModeCommand instead.
	 */
	public String render()
	{
		throw new IllegalStateException("Don't try to send ModeCommand!");
	}
	
	public void setSourceString( String source )
	{
		this.source = source;
	}
	
	public String getSourceString()
	{
		return source;
	}
	
	/**
	 * Does nothing, as this is a factory command.
	 */
	public boolean updateClientState( ClientState cs )
	{
		// Nothing here, move on.
		return false;
	}
	
	public String toString()
	{
		return "ModeCommand";
	}
	
	/** Takes a mode string, such as: '+ooo A B C' or '+o A +o B' or even
	 * '+o-o A B' and returns a List containing Mode objects that
	 * correspond to the modes specified.
	 *
	 * @param modes is a Map of Character to Mode objects.
	 * @param tokens is the sequence of tokens making up the parameters of
	 * the command.
	 */
	public List parseModes( Map modes, StringTokenizer tokens )
	{
		LinkedList results = new LinkedList();
	
		while( true )                                         
		{
			if( tokens.hasMoreTokens() )
			{
				parseOneModeSet( modes, tokens, results );
			}
			else
			{
				return results;
			}
		}
	}
	
	/**
	 * Parses one group of modes.  '+ooo A B C' and not '+o A +o B'.  It
	 * will parse the first group it finds and will ignore the rest.
	 */
	private void parseOneModeSet( Map modes, StringTokenizer tokens, List results )
	{
  		// A list of modes that we have.
		LinkedList localModes = new LinkedList();
		
		Mode.Sign sign = Mode.Sign.NOSIGN;
		String chars = tokens.nextToken();
		
		int stop = chars.length();
		for( int i = 0; i < stop; ++i )
		{
			char lookingAt = chars.charAt( i );
			if( lookingAt == '+' )
				sign = Mode.Sign.POSITIVE;
			else if( lookingAt == '-' )
				sign = Mode.Sign.NEGATIVE;
			else if( lookingAt == ':' )
				// This is to get around a bug in some ircds
				continue;
			else
			{
				// A real mode character!
				Mode mode = (Mode)modes.get( new Character( lookingAt ) );
				if( mode == null )
				{
					//TODO: Is there some way we can figure out if the mode
					// we don't know anything about needs a parameter?
					// Things get messy if it does need a parameter, and we
					// don't eat the string.
					Debug.println( this, "Unknown mode: " + lookingAt, Debug.BAD );
				}
				else
				{
					mode = mode.newInstance();
					mode.setSign( sign );
					localModes.add( mode );
				}
			}
		}
		
		// Now we know what modes are specified, and whether they are
		// positive or negative.  Now we need to fill in the parameters for
		// any that require parameters, and place the results in the result
		// list.
		Iterator modesI = localModes.iterator();

		while( modesI.hasNext() )
		{
			Mode mode = (Mode)modesI.next();
			
			/*
			 * What we do if the server doesn't pass us a parameter
			 * for a mode is rather undefined - except that we don't
			 * want to run off the end of the tokens.  So we just
			 * ignore it.  The problem is that we don't always know
			 * when a server is going to send us a parameter or not.
			 * We can only hope that servers don't send ambiguous
			 * masks followed by more modes instead of a parameter.
			 */
			if( mode != null && mode.requiresParam() && tokens.hasMoreTokens() )
			{
				mode.setParam( tokens.nextToken() );
			}

			results.add( mode );
		}
	}
}


