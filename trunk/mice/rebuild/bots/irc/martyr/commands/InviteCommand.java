package martyr.commands;

import martyr.InCommand;
import martyr.util.FullNick;
import martyr.util.ParameterIterator;

/**
 * @author <a href="mailto:martyr@mog.se">Morgan Christiansson</a>
 */
public class InviteCommand extends GenericCommand {

	private String _channel;
	private String _nick;
	private FullNick _user;
	
	/** For use as a factory */
	public InviteCommand()
	{
		_channel = null;
		_nick = null;
		_user = null;
	}

	private InviteCommand(FullNick user, String nick, String channel) 
	{
		_user = user;
		_nick = nick;
		_channel = channel;
	}
	
	public InviteCommand(String nick, String channel) 
	{
		_nick = nick;
		_channel = channel;
	}
	
	public InviteCommand(FullNick nick, String channel) 
	{
		this(nick.getNick(), channel);
	}
	
	/* (non-Javadoc)
	 * @see martyr.Command#parse(java.lang.String, java.lang.String, java.lang.String)
	 */
	public InCommand parse(String prefix, String identifier, String params) 
	{
			ParameterIterator iter = new ParameterIterator(params);
			return new InviteCommand( new FullNick( prefix ), (String)iter.next(), (String)iter.next() );
	}

	/* (non-Javadoc)
	 * @see martyr.commands.GenericCommand#getIrcIdentifier()
	 */
	public String getIrcIdentifier() 
	{
		return "INVITE";
	}

	/* (non-Javadoc)
	 * @see martyr.commands.GenericCommand#renderParams()
	 */
	public String renderParams() 
	{
		return _nick+" "+_channel;
	}

	/**
	 * @return The channel invited to.  */
	public String getChannel() 
	{
		return _channel;
	}

	/**
	 * @return The nick sending the invite.
	 */
	public String getNick() 
	{
		return _nick;
	}

	/**
	 * @return The user the invite is sent to. 
	 */
	public FullNick getUser() 
	{
		return _user;
	}
}

