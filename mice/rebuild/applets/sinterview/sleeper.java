/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
 
/**
 * This class waits for a question.
 * @author Brandon Smith
 * @version 2.0
 */

import jaim.*;
import java.net.*;
import java.io.*;
import java.util.*;

public class sleeper extends Thread
{
	/** The parent that handles this. */
	public interview daddy;

	/** The network connection to the server. */
	public net com;
	
	/**
	 * This method constructs a new interview person
	 * @param parent The parent interviewee that is waiting for a question.
	 */
	public sleeper(interview parent)
	{
		daddy = parent;
		com = daddy.com;
	}
	
	/**
	 * This method is the thread that gets spawned off if there aren't any questions available
	 * @author Brandon Smith
	 * @version 2.1
	 */
	public void run()
	{
		boolean happy = false;
		String in,tname,tques;
		daddy.listening = false;
		while(!happy)
		{
			sleep(5000);
			com.write("get");
			in = com.read();
			happy = !(in.charAt(0) == '-');
		}
		tname = com.read();
		tques = com.read();
		daddy.send(tname + " Asks: " + tques);
		daddy.listening = true;
	}
		
	/**
	 * This method makes this thread sleep.
	 * @param miliseconds The duration of the nap in seconds.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void sleep(int miliseconds)
	{
		try
		{
			Thread.sleep((long) miliseconds);
		}
		catch (Exception ie)
		{
			System.out.println("Exception in Interview sleep\n" + ie);
		}
	}
}
