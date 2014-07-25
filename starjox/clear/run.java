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

public class run
{
	public static void main(String[] args) throws InterruptedException
	{
		reader("starjox.conf");
		while(true)
			Thread.sleep(1000000);
	}
	
	/**
	 * This reads the config file
	 * @param filename The location of the template file.
	 * @author Brandon Smith
	 * @version 1.0
	 */
	public static void reader(String filename)
	{
		BufferedReader infile;
		String line;
		try
		{
			infile = new BufferedReader(new FileReader(filename));
			line = infile.readLine();
			while(line != null)
			{
				linehandle(line);
				line = infile.readLine();
			}
			infile.close();
		}
		catch(Exception e)
		{
			System.out.println(e);
		}
	}
	
	/**
	 * This handles the init and run of stuff.
	 */
	private static void linehandle(String line)
	{
		try
		{
			//comment lines :)
			if(line.startsWith("!")) return;
			if(line.startsWith("#")) return;
			if(line.startsWith("//")) return;
			if(line.startsWith(";")) return;
			if(line.startsWith("C")) return;
		
			//bust the line apart
			StringTokenizer info = new StringTokenizer(line," ",false);
			String prot = info.nextToken();
			String src = info.nextToken();
			int srcprt;
			String dest = info.nextToken();
			int destprt;
		
			info = new StringTokenizer(src,":",false);
			src = info.nextToken();
			srcprt = Integer.parseInt(info.nextToken());
		
			info = new StringTokenizer(dest,":",false);
			dest = info.nextToken();
			destprt = Integer.parseInt(info.nextToken());
		
			if(prot.toLowerCase().compareTo("tcp") == 0) 
				new tcpfw(dest,srcprt,destprt).start();
		}
		catch(Exception e)
		{
			return;
		}
	}
}
