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

	class PipelineCommand
	{
		// We store this for use in the menu - we want to show "undo <blah>", etc.
		public string	Name;

		public virtual void
		Do (Hashtable element_store)
		{
		}

		public virtual void
		Undo (Hashtable element_store)
		{
		}
	};

	class Pipeline
	{
		Hashtable		element_store;
		public bool		saved;
		public string		filename;

		public ArrayList	undo_stack;
		public ArrayList	redo_stack;

		public
		Pipeline ()
		{
			element_store = new Hashtable ();

			// We start out with saved = true, since it doesn't make sense to
			// force the user to save something they haven't made any changes
			// to. As soon as they start messing with things, this toggles.
			saved = true;
			filename = null;

			// Create our undo and redo stacks
			undo_stack = new ArrayList ();
			redo_stack = new ArrayList ();
		}

		public bool
		Empty
		{
			get { return (element_store.Count == 0); }
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

			OnChanged (new System.EventArgs ());
		}

		public void
		Load (string filename)
		{
			XmlTextReader reader = new XmlTextReader (filename);
			while (reader.Read ()) {
				if (reader.NodeType == XmlNodeType.Element && reader.Depth == 1) {
					Element e = ElementFactory.Instance.CreateFromXml (reader.Name);
					e.Read (reader);

					// Just add directly to the store
					element_store.Add (e.id.ToString ("d"), e);
				}
			}

			this.filename = filename;
			saved = true;

			OnChanged (new System.EventArgs ());
		}

		public void
		Clear ()
		{
			saved = true;
			filename = null;
			element_store.Clear ();
		}

		public void
		Do (PipelineCommand command)
		{
			command.Do (element_store);
			undo_stack.Add (command);

			saved = false;

			OnChanged (new System.EventArgs ());
		}

		public void
		Undo ()
		{
			PipelineCommand command = (PipelineCommand) undo_stack[undo_stack.Count - 1];
			undo_stack.RemoveAt (undo_stack.Count - 1);

			command.Undo (element_store);

			redo_stack.Add (command);

			if (undo_stack.Count == 0)
				saved = true;

			OnChanged (new System.EventArgs ());
		}

		public void
		Redo ()
		{
			PipelineCommand command = (PipelineCommand) redo_stack[redo_stack.Count - 1];
			redo_stack.RemoveAt (redo_stack.Count - 1);

			command.Do (element_store);

			undo_stack.Add (command);

			saved = false;

			OnChanged (new System.EventArgs ());
		}

		public event System.EventHandler Changed;
		protected void
		OnChanged (System.EventArgs e)
		{
			if (Changed != null)
				Changed (this, e);
		}
	}

	namespace Commands
	{
		class Add : PipelineCommand
		{
			System.Guid		id;
			Element			e;

			public
			Add (Element e)
			{
				Name = "Add " + e.Name ();
				id = e.id;
				this.e = e;
			}

			public override void
			Do (Hashtable element_store)
			{
				element_store.Add (id.ToString ("d"), e);
			}

			public override void
			Undo (Hashtable element_store)
			{
				element_store.Remove (id.ToString ("d"));
			}
		}

		class Cut : PipelineCommand
		{
			public Cut (Element e)
			{
				Name = "Cut " + e.Name ();
			}

			public override void
			Do (Hashtable element_store)
			{
				// FIXME - implement
			}

			public override void
			Undo (Hashtable element_store)
			{
				// FIXME - implement
			}
		}

		class Paste : PipelineCommand
		{
			public Paste (Element e)
			{
				Name = "Paste " + e.Name ();
			}

			public override void
			Do (Hashtable element_store)
			{
				// FIXME - implement
			}

			public override void
			Undo (Hashtable element_store)
			{
				// FIXME - implement
			}
		}

		class Delete : PipelineCommand
		{
			public Delete (Element e)
			{
				Name = "Delete " + e.Name ();
			}

			public override void
			Do (Hashtable element_store)
			{
				// FIXME - implement
			}

			public override void
			Undo (Hashtable element_store)
			{
				// FIXME - implement
			}
		}
	}

}
