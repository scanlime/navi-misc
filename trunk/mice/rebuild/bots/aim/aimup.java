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
		personindex index = new personindex(1);
		bot = new botmain[1];
		bot[0] = new botmain("MICEBot","password",index);
		index.registerBot(bot[0]);
		/*
		for(i=1;i<6;i++)
		{
			sleep(3);
			bot[i] = new botmain("MICEBot"+i,"password",index);
			index.registerBot(bot[i]);
		}
		*/
		while(true)
		{
			sleep(100);
		}
	}
	
	public static void sleep(int seconds)
	{
		try
		{
			Thread.sleep(seconds * 1000);
		}
		catch (Exception ie) {}
	}
}
