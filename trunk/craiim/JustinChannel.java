import martyr.Debug;
import martyr.replies.NamesReply;
import martyr.clientstate.ClientState;
import martyr.clientstate.Channel;
import martyr.InCommand;
import martyr.Mode;

public class JustinChannel extends Channel
{

private Justin justin;

public JustinChannel( String chanName, Justin justin )
{
	super( chanName );

	this.justin = justin;
}

/**
 * We override the cononical version of addMember.
 */
public void addMember( String name, InCommand why )
{
	super.addMember( name, why );
	if( ! (why instanceof NamesReply) )
		System.out.println("IN: " + name);
}

/**
 * We override the cononical version of removeMember.
 */
public void removeMember( String name, InCommand why )
{
	super.removeMember( name, why );
	System.out.println("OUT: " + name);
}

public void setMode( Mode mode )
{
	super.setMode( mode );

	Debug.println( this, "Mode: " + mode, Debug.VERBOSE );
}

public String toString()
{
	return "JustinChannel";
}

}



