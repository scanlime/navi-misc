package martyr.commands;

import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.StringTokenizer;

import martyr.InCommand;
import martyr.Mode;
import martyr.clientstate.Channel;
import martyr.clientstate.ClientState;
import martyr.modes.channel.AnonChannelMode;
import martyr.modes.channel.BanMode;
import martyr.modes.channel.ExceptionMode;
import martyr.modes.channel.InviteMaskMode;
import martyr.modes.channel.InviteOnlyMode;
import martyr.modes.channel.KeyMode;
import martyr.modes.channel.LimitMode;
import martyr.modes.channel.ModeratedMode;
import martyr.modes.channel.NoExtMsgMode;
import martyr.modes.channel.OperMode;
import martyr.modes.channel.PrivateMode;
import martyr.modes.channel.SecretMode;
import martyr.modes.channel.TopicLockMode;
import martyr.modes.channel.VoiceMode;
import martyr.util.FullNick;

/**
 * Defines the ChannelMode command.  Can be used to send a Channel
 * mode.  For receiving, this defines which channel modes Martyr knows
 * about and passes them on to the Channel object.  Note that the
 * actual logic of what happens when a mode arrives lies in the
 * clientstate.Channel object.
 */
public class ChannelModeCommand extends ModeCommand
{

	private String channelName;
	private FullNick sender;
	
	private List modes;

	private static HashMap modeTypes;

	/**
	 * For receiving a mode command.
	 */
	public ChannelModeCommand( String prefix, String channelName,
		StringTokenizer params )
	{
		makeModes();

		this.channelName = channelName;

		modes = parseModes( modeTypes, params );

		// System.out.println( modes );
	}

	/**
	 * For sending a mode discovery.
	 */
	public ChannelModeCommand( String channelName )
	{	
		sender = null;
		this.channelName = channelName;

		// Empty list, no modes.
		modes = new LinkedList();
	}

	public void makeModes()
	{
		if( modeTypes == null )
		{
			modeTypes = new HashMap();
			
			registerMode( modeTypes, new BanMode() );
			registerMode( modeTypes, new KeyMode() );
			registerMode( modeTypes, new OperMode() );
			registerMode( modeTypes, new VoiceMode() );
			registerMode( modeTypes, new LimitMode() );
			// registerMode( modeTypes, new QuietMode() );
			registerMode( modeTypes, new SecretMode() );
			registerMode( modeTypes, new PrivateMode() );
			registerMode( modeTypes, new NoExtMsgMode() );
			registerMode( modeTypes, new ExceptionMode() );
			registerMode( modeTypes, new TopicLockMode() );
			registerMode( modeTypes, new ModeratedMode() );
			registerMode( modeTypes, new InviteMaskMode() );
			registerMode( modeTypes, new InviteOnlyMode() );
			registerMode( modeTypes, new AnonChannelMode() );
		}
	}
	
	/**
	 * Shouldn't be called, as ModeCommand should be responsible for parsing
	 * and creating this class.
	 */
	public InCommand parse( String prefix, String identifier, String params )
	{
		throw new IllegalStateException( "Don't call this method!" );
	}
	
	public String render()
	{
		return "MODE " + channelName + renderParams();
	}

	public String renderParams()
	{
		Iterator modesI = modes.iterator();

		String modes = "";
		String params = "";

		while( modesI.hasNext() )
		{
			Mode mode = (Mode)modesI.next();
			
			if( mode.getSign() != Mode.Sign.NOSIGN )
			{
				modes += (mode.getSign() == Mode.Sign.POSITIVE ? "+" : "-" );
			}
			modes += mode.getChar();
			
			if( mode.getParam() != null )
			{
				// Does the parameter list already have params?
				// If so, stick in a space.
				if( params.length() > 0 )
				{
					params += " ";
				}
				params += mode.getParam();
			}
		}
		
		return modes + " " + params;
	}
	
	public String getChannel()
	{
		return channelName;
	}
	
	public FullNick getSender()
	{
		return sender;
	}
	
	/**
	 * Passes the modes on to the clientstate.Channel object.
	 */
	public boolean updateClientState( ClientState state )
	{
		boolean changed = false;
		
		Iterator modesI = modes.iterator();
		Channel channel = state.getChannel( channelName );
		
		while( modesI.hasNext() )
		{
			Mode mode = (Mode)modesI.next();
			
			channel.setMode( mode );
			
			changed = true;
		}
		
		return changed;
	}
	
	
}


