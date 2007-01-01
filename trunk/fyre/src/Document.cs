/*
 * EditorDocument.cs - represents a document within the pipeline editor.
 *
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

using System.Xml;

namespace Fyre.Editor
{

	class Document
	{
		Fyre.Pipeline			pipeline;
		Layout				layout;
		string				filename;
		bool				saved;

		// Count to know how many documents we've created. We use this for
		// the default filename (Untitled%d), and DnD data descriptors (so
		// we can't drag from one element list to another window).
		static int			count;
		public int			Number;

		public Fyre.Pipeline
		Pipeline
		{
			get {
				return pipeline;
			}
			set {
				pipeline = value;
				if (layout != null)
					layout.pipeline = pipeline;
			}
		}

		public Layout
		Layout
		{
			get {
				return layout;
			}
			set {
				layout = value;
				if (pipeline != null)
					layout.pipeline = pipeline;
			}
		}

		public event System.EventHandler FilenameChanged;
		public string
		Filename
		{
			get {
				if (filename == null)
					return System.String.Format ("Untitiled{0}", Number);
				return filename;
			}
			set {
				filename = value;
				if (FilenameChanged != null)
					FilenameChanged (this, new System.EventArgs ());
			}
		}

		public event System.EventHandler DocumentSaved;
		public bool
		Saved
		{
			get {
				return saved;
			}
			set {
				saved = value;
				if (DocumentSaved != null)
					DocumentSaved (this, new System.EventArgs ());
			}
		}

		public bool
		HasRealFilename
		{
			get {
				return (filename != null);
			}
		}

		public
		Document ()
		{
			Pipeline = new Fyre.Pipeline ();
			Layout = new Layout ();

			// We start out with saved = true, since it doesn't make sense to force (or
			// even allow) our user to save an empty document.
			saved = true;
			filename = null;

			count++;
			Number = count;
		}

		public void
		Save (string filename)
		{
			XmlTextWriter writer = new XmlTextWriter (filename, null);
			writer.Formatting = Formatting.Indented;
			writer.WriteStartDocument ();
			writer.WriteStartElement (null, "fyre-pipeline", null);

			// serialize pipeline graph
			writer.WriteStartElement (null, "pipeline", null);
			Pipeline.Serialize (writer);
			writer.WriteEndElement ();

			// serialize layout
			writer.WriteStartElement (null, "layout", null);
			Layout.Serialize (writer);
			writer.WriteEndElement ();

			Saved = true;
			Filename = filename;

			writer.WriteEndDocument ();
			writer.Close ();
		}

		public void
		Load (string filename)
		{
			XmlTextReader reader = new XmlTextReader (filename);
			reader.WhitespaceHandling = WhitespaceHandling.None;
			try {
				while (reader.Read ()) {
					if (reader.NodeType == XmlNodeType.Element && reader.Depth == 1) {
						if (reader.Name == "pipeline")
							Pipeline.DeSerialize (reader);
						else if (reader.Name == "layout")
							Layout.DeSerialize (reader);
					}
				}

				Filename = filename;
				Saved = true;
			} catch (System.Exception e) {
				ErrorDialog ed = new ErrorDialog (null,
					System.String.Format ("Error loading '{0}'", filename),
					e.ToString ());
				ed.Run ();
				ed.Destroy ();
			}
			reader.Close ();
		}
	}
}
