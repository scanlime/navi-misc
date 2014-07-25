import martyr.*;
import martyr.util.*;
import martyr.commands.*;
import martyr.replies.*;

public class MessageMonitor extends GenericAutoService
{
	private Justin justin;

	public MessageMonitor( Justin justin )
	{
		super( justin.getConnection() );
		this.justin = justin;

		enable();
	}

	public void updateCommand( InCommand command )
	{
		if( command instanceof MessageCommand )
		{
			MessageCommand msg = (MessageCommand)command;
			justin.incomingMessage( msg );
		}
		else if( command instanceof NamesEndReply )
		{
			justin.printMembers();
		}
	}

	protected void updateState( State state )
	{
		if( state == State.UNCONNECTED && justin.shouldQuit() )
			System.exit(0);
	}
}



