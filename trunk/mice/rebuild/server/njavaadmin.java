/**
 * This class handles the administration client.
 * @author Brandon Smith
 * @version 2.0
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class njavaadmin extends nbase
{
	/**
	 * This method constructs the administrator interface thingy
	 * @param gonnection The socket descriptor for the connection type stuff
	 * @param IN The buffered reader that reads data from the socket
	 * @param OUT The output stream writer that writes data to the socket
	 */
	public njavaadmin(Socket ink, BufferedReader IN, OutputStreamWriter OUT)
	{
		super(ink,IN,OUT);
		netdebug = true;
	}
	
	/**
	 * Since this is a subclass of Thread, it has to have a run() method
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void run()
	{
		int moderators, interviews;
		if(!auth()) return;
		moderators = mods();
		interviews = ints();
	}
	
	/**
	 * This method handles the acceptance of the interview peoples
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The number of people being interviewed.
	 */
	public int ints()
	{
		String newkey = key.keygen();
		int interviews,i;
		interviews = Integer.parseInt(read());
		write(newkey);
		aadmin.iuns = new String[interviews];
		aadmin.ipas = new String[interviews];
		for(i=0;i<interviews;i++)
		{
			aadmin.iuns[i] = key.decrypt(newkey,read());
			aadmin.ipas[i] = key.decrypt(newkey,read());
		}
		write("+OK");
		aadmin.confstate = 2;
		return interviews;
	}
	
	/**
	 * This method handles the acceptance of the moderators
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The number of moderators involved in the chat.
	 */
	public int mods()
	{
		String newkey = key.keygen();
		int moderators, i;
		moderators = Integer.parseInt(read());
		write(newkey);
		aadmin.muns = new String[moderators];
		aadmin.mpas = new String[moderators];
		for(i=0;i<moderators;i++)
		{
			aadmin.muns[i] = key.decrypt(newkey,read());
			aadmin.mpas[i] = key.decrypt(newkey,read());
		}
		write("+OK");
		aadmin.confstate = 1;
		return moderators;
	}
	
	/**
	 * This method authenticates the administrator
	 * @author Brandon Smith
	 * @version 2.0
	 * @return true on a successful authentication, false on failure
	 */
	public boolean auth()
	{
		String mykey = key.keygen();
		write(mykey);
		String user = key.decrypt(mykey,read());
		String pass = key.decrypt(mykey,read());
		if(user.compareTo(aadmin.user) == 0 && pass.compareTo(aadmin.pass) == 0) 
			write("+OK");
		else
		{
			write("-ERR");
			closeConnection();
			return false;
		}
		write(""+aadmin.confstate);
		return true;
	}
}