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
    public static botmain mybot;

    public static void main(String[] args)
    {
	if(args.length < 2)
	    System.out.println("name and password please");
	else
	{
	    mybot = new botmain(args[0],args[1]);
	    try {
		Thread.sleep(300000);       //Wait for 5 minutes
	    }
	    catch (InterruptedException ie) {
	    }
	}
    }
}
