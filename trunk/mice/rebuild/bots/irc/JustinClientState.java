import martyr.clientstate.ClientState;
import martyr.clientstate.Channel;

public class JustinClientState extends ClientState
{

private Justin justin;

public JustinClientState( Justin justin )
{
	this.justin = justin;
}

/**
 * We want to use our own channel object, so we create it here.
 */
public void addChannel( String channel )
{
	addChannel( new JustinChannel( channel , justin ) );
	justin.joinedChannel( getChannel( channel ) );
}

}



