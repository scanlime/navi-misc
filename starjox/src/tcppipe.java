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
 * This class is for handling each connection made, and tunneling it.
 * @author Brandon Smith
 * @version 1.0
 */

import java.net.*;
import java.io.*;
import java.util.*;

public class tcppipe extends Thread
{
    /** for ID output stuff. */
    public static int nextID = 0;
    
    /** The input stream this tcppipe reads from. */
    public InputStream in;
    /** The output stream this tcppipe writes to. */
    public OutputStream out;
    /** The String for outputting what I transmit */
    public String label;
    /** My ID */
    public int ID;
   
    
    /**
     * A constructor for the tcppipe object
     * @param from The input stream to read 'from'
     * @param to The output stream to write 'to'
     * @author Brandon Smith
     * @version 1.0
     */
    public tcppipe(InputStream from, OutputStream to, String nope)
    {
	in = from;
	out = to;
	label = nope;
	ID = tcppipe.nextID++;
    }
    
    /**
     * This method implements the run method for Threaded stuff.
     * @author Brandon Smith
     * @version 1.0
     */
    public void run()
    {
	int temp;
	try
	{
	    while(true)
	    {
		temp = in.read();
		out.write(temp);
		out.flush();
		logger.log(ID,label,temp);
		if(temp == -1) return;
	    }
	}
	catch(Exception e)
	{
	    System.out.println(e);
	}
    }
}
