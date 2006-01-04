/*
 * WriteData.cs - extracts a single dof from an amc file and prints the
 * results, for plotting histograms
 *
 * Copyright (C) 2005-2006 David Trowbridge
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

class WriteData
{
	public static void
	Main (string[] args)
	{
		if (args.Length != 3) {
			System.Console.WriteLine ("Usage: WriteData.exe [file] [bone] [dof]", args[0]);
			return;
		}

		string filename = args[0];
		string bone = args[1];
		int dof = System.Int32.Parse (args[2]);

		AMC.File f = AMC.File.Load (filename);
		foreach (AMC.Frame frame in f.frames) {
			float[] data = (float[]) frame.data[bone];
			if (data[dof] < -180f)
				data[dof] += 360;
			if (data[dof] > 180f)
				data[dof] -= 360;
			System.Console.WriteLine ("{0}", data[dof]);
		}
	}
}
