/*
 * Pipeline.cs - manages the pipeline
 *
 * Fyre - a generic framework for computational art
 * Copyright (C) 2004-2007 Fyre Team (see AUTHORS)
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

	public class Pipeline
	{
		public Hashtable	element_store;
		public ArrayList	connections;

		public
		Pipeline ()
		{
			element_store = new Hashtable ();
			connections = new ArrayList ();
		}

		public bool
		Empty
		{
			get { return (element_store.Count == 0); }
		}

		public void
		Serialize (XmlTextWriter writer)
		{
			// run through all the elements, adding them to the XML
			foreach (DictionaryEntry entry in element_store) {
				Element e = (Element) entry.Value;
				e.Write (writer);
			}

			for (int i = 0; i < connections.Count; i++) {
				PadConnection e = (PadConnection) connections[i];
				e.Write (writer);
			}
		}

		public void
		DeSerialize (XmlTextReader reader)
		{
			while (reader.Read () && reader.NodeType == XmlNodeType.Element && reader.Depth == 2) {
				if (reader.Name == "connection") {
					PadConnection connection = PadConnection.CreateFromXml (reader);
					connections.Add (connection);
				} else {
					Element e = ElementFactory.Instance.CreateFromXml (reader.Name);
					e.Read (reader);

					// Just add directly to the store
					element_store.Add (e.id.ToString ("d"), e);
				}
			}

			OnChanged (new System.EventArgs ());
		}

		public void
		Clear ()
		{
			element_store.Clear ();
		}

		public void
		Check ()
		{
			/* Here's how the checking algorithm will work:
			 *
			 * We keep two lists of elements: those classified as "inputs" and "outputs"
			 * We start with all the elements listed as inputs. For each of these, we
			 * assign PadConnections the types that the inputs produce. We create a list
			 * with all of those elements which now have things at their inputs.
			 *
			 * For each of these elements which now have all their input pads fulfilled,
			 * we run Check(). These pad types are stored and we refill the list with all
			 * of the elements which are connected to those we just checked. If we check
			 * an "output" pad, we remove it from its list.
			 *
			 * We're done when either the outputs list or our intermediate list is empty.
			 * If it's the latter, we know that our pipeline isn't fully connected.
			 * Any pad connections left over in this case are listed as having no type.
			 */
		}

		public event System.EventHandler Changed;
		public void
		OnChanged (System.EventArgs e)
		{
			if (Changed != null)
				Changed (this, e);
		}
	}
}
