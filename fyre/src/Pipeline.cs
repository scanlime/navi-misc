/*
 * Pipeline.cs - manages the pipeline
 *
 * Fyre - a generic framework for computational art
 * Copyright (C) 2004-2005 Fyre Team (see AUTHORS)
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

using System.Collections;
using System.Xml;

namespace Fyre
{

	class UidComparer : IComparer
	{
		public int
		Compare (object x, object y)
		{
			byte[] xb = (byte[]) x;
			byte[] yb = (byte[]) y;

			for (int i = 0; i < 16; i++) {
				if (xb[i] < yb[i]) return -1;
				if (xb[i] > yb[i]) return 1;
			}
			return 0;
		}
	}

	class Pipeline
	{
		Hashtable		element_store;
		UidComparer		comparer;
		public bool		saved;
		public string		filename;

		public bool
		Empty
		{
			get { return (element_store.Count == 0); }
		}

		public
		Pipeline ()
		{
			comparer = new UidComparer ();
			element_store = new Hashtable (null, comparer);

			// We start out with saved = true, since it doesn't make sense to
			// force the user to save something they haven't made any changes
			// to. As soon as they start messing with things, this toggles.
			saved = true;
			filename = null;
		}

		public void
		AddElement (Element e)
		{
			element_store.Add (e.id, e);
			System.Console.WriteLine ("Adding {0} {1} to the pipeline", e.Name (), e.id.ToString ("d"));
			e.Edit (null);
			saved = false;
		}

		public void
		Save (string filename)
		{
			if (saved)
				return;

			XmlTextWriter writer = new XmlTextWriter (filename, null);
			writer.Formatting = Formatting.Indented;
			writer.WriteStartDocument ();
			writer.WriteStartElement (null, "fyre-pipeline", null);

			foreach (DictionaryEntry entry in element_store) {
				Element e = (Element) entry.Value;
				e.Write (writer);
			}

			writer.WriteEndDocument ();
			writer.Close ();

			this.filename = filename;
			saved = true;
		}

		public void
		Load (string filename)
		{
			XmlTextReader reader = new XmlTextReader (filename);
			while (reader.Read ()) {
				if (reader.NodeType == XmlNodeType.Element && reader.Depth == 1)
					System.Console.WriteLine ("read: {0}", reader.Name);
			}

			this.filename = filename;
			saved = true;
		}

		public void
		Clear ()
		{
			saved = true;
			filename = null;
			element_store.Clear ();
		}
	}

}
