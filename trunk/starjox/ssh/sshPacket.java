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
 * This class represents an encrypted ssh packet
 */

public class sshPacket
{
	/** This is the data portion of the packet. */
	int[] edata;
	
	public sshPacket(int[] bytes)
	{
		int length = bytes.length + 5; //original data and type and checksum
		int padding = 8 - (length % 8); //calculate the size of the padding
		edata = new int[length + padding + 4]; //allocate the array w/ length
		
		edata[0] = (length >> 24) & 0xff;
		edata[1] = (length >> 16) & 0xff;
		edata[2] = (length >>  8) & 0xff;
		edata[3] =  length        & 0xff;
		
	}
}