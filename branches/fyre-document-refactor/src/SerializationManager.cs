/*
 * SerializationManager.cs - Handles document conversion to/from XML
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

using System.Xml;

namespace Fyre
{
	class SerializationManager
	{
		Pipeline		pipeline;
		Layout			layout;

		public
		SerializationManager (Pipeline pipeline, Layout layout)
		{
			this.pipeline = pipeline;
			this.layout   = layout;
		}

		public void
		Save (string filename)
		{
			XmlTextWriter writer = new XmlTextWriter (filename, null);
			writer.Formatting = Formatting.Indented;
			writer.WriteStartDocument ();
			writer.WriteStartElement (null, "fyre-pipeline", null);

			pipeline.Serialize (writer);
			// serialize layout

			writer.WriteEndDocument ();
			writer.Close ();
		}

		public void
		Load (string filename)
		{
			XmlTextReader reader = new XmlTextReader (filename);
			try {
				while (reader.Read ()) {
					if (reader.NodeType == XmlNodeType.Element) {
					}
				}
				reader.Close ();
			} catch (System.Exception e) {
				// FIXME - show error
				return;
			}
		}
	}
}
