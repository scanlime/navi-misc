package martyr;

import java.util.Hashtable;

import martyr.commands.InviteCommand;
import martyr.commands.JoinCommand;
import martyr.commands.KickCommand;
import martyr.commands.MessageCommand;
import martyr.commands.ModeCommand;
import martyr.commands.NickCommand;
import martyr.commands.NoticeCommand;
import martyr.commands.PartCommand;
import martyr.commands.PingCommand;
import martyr.commands.QuitCommand;
import martyr.commands.TopicCommand;
import martyr.commands.WelcomeCommand;
import martyr.errors.ChannelBannedError;
import martyr.errors.ChannelInviteOnlyError;
import martyr.errors.ChannelLimitError;
import martyr.errors.ChannelWrongKeyError;
import martyr.errors.NickInUseError;
import martyr.replies.ChannelCreationReply;
import martyr.replies.LUserClientReply;
import martyr.replies.LUserMeReply;
import martyr.replies.LUserOpReply;
import martyr.replies.ModeReply;
import martyr.replies.NamesEndReply;
import martyr.replies.NamesReply;
import martyr.replies.TopicInfoReply;
import martyr.replies.WhoisChannelsReply;
import martyr.replies.WhoisEndReply;
import martyr.replies.WhoisIdleReply;
import martyr.replies.WhoisServerReply;
import martyr.replies.WhoisUserReply;

/**
 * CommandRegister is basically a big hashtable that maps IRC
 * identifiers to command objects that can be used as factories to
 * do self-parsing.  CommandRegister is also the central list of
 * commands.
 */
public class CommandRegister
{

private Hashtable commands;
public CommandRegister()
{
	commands = new Hashtable();

	// Note that currently, we only have to register commands that
	// can be received from the server.
	new InviteCommand().selfRegister( this );
	new JoinCommand().selfRegister( this );
	new KickCommand().selfRegister( this );
	new MessageCommand().selfRegister( this );
	new ModeCommand().selfRegister( this );
	new NickCommand().selfRegister( this );
	new NoticeCommand().selfRegister( this );
	new PartCommand().selfRegister( this );
	new PingCommand().selfRegister( this );
	new QuitCommand().selfRegister( this );
	new TopicCommand().selfRegister( this );
	new WelcomeCommand().selfRegister( this );

	// Register errors
	new ChannelBannedError().selfRegister( this );
	new ChannelInviteOnlyError().selfRegister( this );
	new ChannelLimitError().selfRegister( this );
	new ChannelWrongKeyError().selfRegister( this );
	new NickInUseError().selfRegister( this );

	// Register replies
	new ChannelCreationReply().selfRegister( this );
	new LUserClientReply().selfRegister( this );
	new LUserMeReply().selfRegister( this );
	new LUserOpReply().selfRegister( this );
	new ModeReply().selfRegister( this );
	new NamesEndReply().selfRegister( this );
	new NamesReply().selfRegister( this );
	new TopicInfoReply().selfRegister( this );
	new WhoisChannelsReply().selfRegister( this );
	new WhoisEndReply().selfRegister( this );
	new WhoisIdleReply().selfRegister( this );
	new WhoisServerReply().selfRegister( this );
	new WhoisUserReply().selfRegister( this );
}

public void addCommand( String ident, InCommand command )
{
	commands.put( ident, command );
}

public InCommand getCommand( String ident )
{
	return (InCommand)commands.get( ident );
}

}

