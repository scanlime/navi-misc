package martyr.modes.user;

import martyr.modes.GenericMode;

/**
 * 
 */
public abstract class GenericUserMode extends GenericMode
{
	public boolean recordInChannel()
	{
		return false;
	}

	/**
	 * Well, this is kind of irrelevent isn't it?
	 */
	public boolean onePerChannel()
	{
		return false;
	}
}


