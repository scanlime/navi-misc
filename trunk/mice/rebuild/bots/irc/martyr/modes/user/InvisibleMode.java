package martyr.modes.user;

import martyr.Mode;

public class InvisibleMode extends GenericUserMode
{
	public char getChar()
	{
		return 'i';
	}
	
	public boolean requiresParam()
	{
		return false;
	}
	
	public Mode newInstance()
	{
		return new InvisibleMode();
	}
}

