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

//This is necessary for the jaim library
import jaim.*;

import java.net.*;
import java.io.*;
import java.util.*;

public class aimup
{
	public static botmain[] bot;

	public static void main(String[] args)
	{
		int i,j;
		bot = new botmain[6];
		bot[0] = new botmain("MICEBot","password");
		for(i=1;i<6;i++)
			bot[i] = new botmain("MICEBot"+i,"password");
		try
		{
			Thread.sleep(8000);       //Wait for 5 minutes
		}
		catch (InterruptedException ie) {}
		
		for(j=0;j<10;j++)
		for(i=0;i<6;i++)
		{
			bot[i].sendMessage("Joshonthinkpad","Hi Josh, this is from bot number:"+i);
			//bot[i].sendMessage("SilmeTheBard","ding " + i);
			//bot[i].sendMessage("gonkulator3","ding " + i);
			//bot[i].sendMessage("kittikins5","Hi Katie!");
			try
			{
				Thread.sleep(4000);       //Wait for 5 minutes
			}
			catch (InterruptedException ie) {}
		}
		
		try
		{
			Thread.sleep(300000);       //Wait for 5 minutes
		}
		catch (InterruptedException ie) {}
	}
}
