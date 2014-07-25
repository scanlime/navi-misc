/*
 * Original version: Ben Damm <bdamm@dammfine.com>
 * Changes by: Morgan Christiansson <martyr@mog.se>
 * 	- Spotted bugs
 * 	- Added timer
 * 	- Responds to Invites
 *  - Re-tries a join with a bad key
 *
 * 	Note: Requires Java 1.4
 */
package martyr.services;

import martyr.Debug;
import martyr.GenericAutoService;
import martyr.IRCConnection;
import martyr.InCommand;
import martyr.State;
import martyr.TimerTaskCommand;
import martyr.clientstate.Channel;
import martyr.commands.InviteCommand;
import martyr.commands.JoinCommand;
import martyr.commands.KickCommand;
import martyr.errors.GenericJoinError;

/**
 * <p>AutoJoin joins a group if the IRCConnection is ready.  It will wait until
 * it is ready if it is not (by waiting for the REGISTERED state change).</p>
 *
 * <p>AutoJoin maintains a persistent Join (re-join if kicked).
 * AutoJoin can cease to be persistent by calling the 'disable'
 * method.</p>
 */
public class AutoJoin extends GenericAutoService
{

private String channel = null;
private String key = null;
private TimerTaskCommand joinTimerTask = null;
private long joinTimerTaskDelay = 10*1000;

public AutoJoin( IRCConnection connection, String channel )
{
	this( connection, channel, null );
}

public AutoJoin( IRCConnection connection, String channel, String key )
{
	super( connection );
	
	this.channel = channel;
	this.key = key;

	enable();

	updateState( connection.getState() );
}

protected void updateState( State state )
{

	if( state == State.REGISTERED )
		performJoin();
}

protected void updateCommand( InCommand command_o )
{
	if( command_o instanceof KickCommand )
	{
		KickCommand kickCommand = (KickCommand)command_o;

		if( kickCommand.kickedUs( getConnection().getClientState() ) )
		{
			if( Channel.areEqual(kickCommand.getChannel(), channel))
			{
				performJoin();
			}
			else
			{
				// mog: TODO: Should we really join a channel for which we aren't the AutoJoin:er?
				// BD: You are quite right, this AutoJoin should only worry about itself.
				// getConnection().sendCommand( new JoinCommand( kickCommand.getChannel() ) );
			}
		}
	}
	else if(command_o instanceof GenericJoinError )
	{
		GenericJoinError joinErr = (GenericJoinError)command_o;
		
		if( Channel.areEqual( joinErr.getChannel(), channel ) )
		{
			Debug.println( this, "Failed to join channel: "+joinErr.getComment(), Debug.VERBOSE );
			scheduleJoin();
		}
	}
	else if( command_o instanceof InviteCommand )
	{
		InviteCommand invite = (InviteCommand)command_o;
		if(!getConnection().getClientState().isOnChannel(invite.getChannel()))
		{
			performJoin();
		}
	}
}

private void performJoin()
{
	// TODO
	// race condition, should we make method synchronized or is the chance of this happening to small anyway?
	if(joinTimerTask != null)
	{
		joinTimerTask.cancel();
		joinTimerTask = null;
	}
	getConnection().sendCommand( new JoinCommand( channel, key ) );
}
private void scheduleJoin()
{
	if(joinTimerTask == null || !joinTimerTask.isScheduled())
	{
		joinTimerTask = new TimerTaskCommand(getConnection(), new JoinCommand(channel, key));
		//TODO back off delay on repeated retries?
		getConnection().getCronManager().schedule(joinTimerTask, joinTimerTaskDelay);
	}
}

public String toString()
{
	if( key == null )
		return "AutoJoin [" + channel + "]";
	return "AutoJoin [" + channel + "," + key + "]";
}

// END AutoResponder
}
 


 
