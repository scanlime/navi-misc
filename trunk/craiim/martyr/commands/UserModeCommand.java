package martyr.commands;

import java.util.HashMap;
import java.util.StringTokenizer;

import martyr.Debug;
import martyr.InCommand;
import martyr.clientstate.ClientState;
import martyr.modes.user.InvisibleMode;
import martyr.util.FullNick;

/**
 * Defines a user MODE command.
 */
public class UserModeCommand extends ModeCommand
{

	private FullNick user;
	private FullNick sender;
	//private List modes;

	private static HashMap modeTypes;
	
	public UserModeCommand( String prefix, String userStr,
		StringTokenizer tokens )
	{
//		System.out.println( prefix );
		sender = new FullNick( prefix );
		user = new FullNick( userStr );
	
		if( !sender.equals( user ) ) 
		{
			Debug.println( this, "Odd: mode change for a user that isn't us.", Debug.CRITICAL );
			return;
		}
		
		makeModeTypes();

		//modes = parseModes( modeTypes, tokens );

//		System.out.println( modes );
	}
	
	private void makeModeTypes()
	{
		if( modeTypes == null )
		{
			modeTypes = new HashMap();
			
			// Add new mode types here
			registerMode( modeTypes, new InvisibleMode() );
		}
	}
	
	
	/**
	 * Should not be called, as ModeCommand does the parsing and instantiation
	 * of this class.
	 */
	public InCommand parse( String prefix, String identifier, String params )
	{
		throw new IllegalStateException( "Don't call this method!" );
	}
	
	public String render()
	{
		throw new UnsupportedOperationException("Can't send user modes, yet." );
	}
	
	public FullNick getUser()
	{
		return user;
	}
	
	{
		Debug.println( "TODO: UserModeCommand", "Can't send", Debug.EXCESSIVE );
		Debug.println( "TODO: UserModeCommand", "Does not update client state", Debug.EXCESSIVE );
	}
	public boolean updateClientState( ClientState state )
	{
		// TODO implement
		return false;
	}
	
	public String toString()
	{
		return "UserModeCommand";
	}
	

}


