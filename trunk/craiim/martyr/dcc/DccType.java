package martyr.dcc;

public class DccType
{
	public static final DccType CHAT = new DccType( "CHAT" );
	public static final DccType SEND = new DccType( "SEND" );

	private final String dccType;

	private DccType( String type )
	{
		this.dccType = type;
	}
	
	/**
	 * Returns the DCC TYPE tag, either CHAT or SEND.
	 */
	public String toString()
	{
		return dccType;
	}

	public static DccType getInstance( String typeStr )
		throws InvalidDccException
	{
		typeStr = typeStr.toUpperCase();
		
		if( typeStr.equals( CHAT.toString() ) )
		{
			return CHAT;
		}
		else if( typeStr.equals( SEND.toString() ) )
		{
			return SEND;
		}
		else
			throw new InvalidDccException("Unknown DCC type " + typeStr);
	}
}
