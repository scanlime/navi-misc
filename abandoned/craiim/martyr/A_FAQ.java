package martyr;
/**
 * Some Frequenty Asked Questions.
 *
 * <h2>How do I use Martyr?</h2>
 * Take a good look at the IRCConnection class.  Also check out the
 * programs in the test package.  There are several small testing
 * programs that show how to use the fundamentals of Martyr.  For a
 * better example, check out the 
 * {@link <a href="http://www.dammfine.com/~bdamm/projects/martyr/justin.tar.gz">justin</a>} bot.
 *
 * <h2>How do I get rid of all the output?</h2>
 * <p>Martyr is very verbose (for now) with its debugging output.  You
 * can disable this by calling <code>Debug.setDebugLevel( Debug.FAULT );</code>
 * </p>
 *
 * <h2>What happened to Command?</h2>
 * <p>Command got divided into InCommand and OutCommand.  Sorry about
 * breaking the API, but Command's representation of two seperate
 * ideas was getting to be a real pain.</p>
 *
 * <p>Existing code using an expression such as (object instanceof
 * Command) will still return true on command objects, however, the
 * Command interface no longer defines much.</p>
 *
 * <h2>How do I add socks support?</h2>
 * <p>
 * Martyr does not directly support SOCKS, but it can work with an external
 * SOCKS library.  <code>IRCConnection</code> allows you to pass a custom
 * <code>Socket</code> instance into the <code>connect(...)</code> method.
 * Thus, a library such as {@link <a href="http://sourceforge.net/projects/jsocks/">jsocks</a>} is ideal for use with Martyr.
 * </p>
 *
 * <p>
 * Note that while <code>IRCConnection.connect</code> allows you to pass in
 * your own <code>Socket</code>, <code>AutoReconnect</code> does not
 * have a way to pass your own <code>Socket</code> to
 * <code>IRCConnection</code> on a reconnect.  You can either subclass
 * <code>AutoReconnect</code> and override the getSocket method, or
 * subclass IRCConnection and override connect.</p>
 *
 * @see martyr.IRCConnection
 */
public class A_FAQ
{
	private A_FAQ()
	{
	}
}
