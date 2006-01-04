/*
 * AMC.cs - classes handy for working with Acclaim motion capture (AMC) files.
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

namespace AMC
{
	class Frame
	{
		public System.Collections.Hashtable	data;

		public
		Frame ()
		{
			data = new System.Collections.Hashtable ();
		}

		public void
		AddBone (string[] tokens)
		{
			string name = tokens[0];

			// hack to take care of trailing space
			// FIXME - this isn't really very robust. The AMC file doesn't make any
			// requirements about whitespace, so if there's a lot of it in weird
			// places, this breaks horribly. One possibility for solving this is to
			// use ArrayLists, but move over to the generics compiler, so we don't
			// run into huge boxing overhead
			if (tokens[tokens.Length - 1].Length == 0) {
				float[] values = new float[tokens.Length - 2];
				for (int i = 0; i < tokens.Length - 2; i++)
					values[i] = (float) (System.Double.Parse (tokens[i + 1]));
				data.Add (name, values);
			} else {
				float[] values = new float[tokens.Length - 1];
				for (int i = 0; i < tokens.Length - 1; i++)
					values[i] = (float) (System.Double.Parse (tokens[i + 1]));
				data.Add (name, values);
			}
		}

		public System.Collections.IDictionaryEnumerator
		GetEnumerator ()
		{
			return data.GetEnumerator ();
		}

		public void
		Write (System.IO.StreamWriter stream)
		{
			foreach (System.Collections.DictionaryEntry entry in data) {
				stream.Write (System.String.Format ("{0} ", entry.Key));
				float[] values = (float[]) entry.Value;
				for (int i = 0; i + 1 < values.Length; i++)
					stream.Write (System.String.Format ("{0} ", values[i]));
				stream.Write (System.String.Format ("{0}\n", values[values.Length - 1]));
			}
		}
	}

	class File
	{
		System.Collections.ArrayList		comments;
		public System.Collections.ArrayList	frames;
		public float[][]			min, max;

		protected
		File ()
		{
			comments = new System.Collections.ArrayList ();
			frames   = new System.Collections.ArrayList ();
		}

		public static File
		Load (string filename)
		{
			File file = new File ();
			Frame current_frame = null;

			System.IO.StreamReader stream = System.IO.File.OpenText (filename);
			if (file == null)
				return null;

			string line;
			while ((line = stream.ReadLine ()) != null) {
				// Comments
				if (line[0] == '#' || line[0] == ':') {
					file.comments.Add (line);
					continue;
				}

				try	{
					System.Int32.Parse (line);
					// If this parsed correctly, we're starting a new frame.
					if (current_frame != null)
						file.frames.Add (current_frame);
					current_frame = new Frame ();
				} catch (System.Exception e) {
					// If not, add the data to the frame
					string[] tokens = line.Split (' ');
					current_frame.AddBone (tokens);
				}
			}

			stream.Close ();

			return file;
		}

		public void
		Save (string filename)
		{
			System.IO.StreamWriter stream = System.IO.File.CreateText (filename);
			if (stream == null)
				return;

			foreach (string line in comments)
				stream.Write (System.String.Format ("{0}\n", line));

			for (int i = 0; i < frames.Count; i++) {
				stream.Write (System.String.Format ("{0}\n", i));
				Frame frame = (Frame) frames[i];
				frame.Write (stream);
			}

			stream.Close ();
		}

		public void
		FindExtents ()
		{
			Frame f = (Frame) frames[0];

			min = new float[f.data.Count] [];
			max = new float[f.data.Count] [];

			for (int i = 0; i < f.data.Count; i++) {
				float[] bone = (float[]) f.data[i];
				min[i] = new float[bone.Length];
				max[i] = new float[bone.Length];
				for (int j = 0; j < bone.Length; j++) {
					min[i][j] = bone[j];
					max[i][j] = bone[j];
				}
			}

			// run through all the frames, finding bounds
			// FIXME - this should probably happen during import, instead of
			// a second pass
			foreach (Frame frame in frames) {
				for (int i = 0; i < frame.data.Count; i++) {
					float[] bone = (float[]) frame.data[i];
					for (int j = 0; j < bone.Length; j++) {
						if (bone[j] < min[i][j]) min[i][j] = bone[j];
						if (bone[j] > max[i][j]) max[i][j] = bone[j];
					}
				}
			}
		}
	}
}
