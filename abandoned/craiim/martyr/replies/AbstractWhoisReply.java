package martyr.replies;

import martyr.Debug;
import martyr.util.ParameterIterator;

public abstract class AbstractWhoisReply extends GenericReply
{

	private String target = null;

	/**
	 * Factory constructor.
	 * */
	public AbstractWhoisReply()
	{
	}

	public AbstractWhoisReply( String params )
	{
		ParameterIterator pi = getParams( params );
		parseParams( pi );
	}

	public abstract String getIrcIdentifier();

	/**
	 * Parse the parameters, but the target param has already been
	 * stripped off.
	 * */
	protected abstract void parseParams( ParameterIterator pi );

	/**
	 * @return the target of the whois
	 * */
	public String getTarget()
	{
		return target;
	}

	/**
	 * @param params the params string passed to "parse".
	 * @return a parameter iterator, with the whois target already
	 * stripped off.
	 * */
	protected ParameterIterator getParams( String params )
	{
		ParameterIterator pi = new ParameterIterator(params);
		pi.next(); // throw away our own nick
		String target = (String)pi.next();
		Debug.println( "AbstractWhoisReply", "Whois target: " + target, Debug.EXCESSIVE );

		return pi;
	}

}

