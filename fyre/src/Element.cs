/*
 * Element.cs - abstract data type defining an Element
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

using System.Xml;
using System;

namespace Fyre
{
	public class PadConnection
	{
		public System.Guid	source_element;
		public int		source_pad;

		public System.Guid	sink_element;
		public int		sink_pad;

		public void
		Write (XmlTextWriter writer)
		{
			writer.WriteStartElement (null, "connection", null);

			// source
			writer.WriteStartAttribute (null, "from", null);
			writer.WriteString (System.String.Format ("{0}:{1}", source_element.ToString ("d"), source_pad));
			writer.WriteEndAttribute ();

			// sink
			writer.WriteStartAttribute (null, "to", null);
			writer.WriteString (System.String.Format ("{0}:{1}", sink_element.ToString ("d"), sink_pad));
			writer.WriteEndAttribute ();

			writer.WriteEndElement ();
		}

		public static PadConnection
		CreateFromXml (XmlTextReader reader)
		{
			PadConnection connection = new PadConnection ();
			string[] s;

			reader.MoveToAttribute ("from");
			s = reader.Value.Split(':');
			connection.source_element = new System.Guid (s[0]);
			connection.source_pad = System.Int32.Parse (s[1]);

			reader.MoveToAttribute ("to");
			s = reader.Value.Split(':');
			connection.sink_element = new System.Guid (s[0]);
			connection.sink_pad = System.Int32.Parse (s[1]);

			return connection;
		}
	}

	public abstract class Pad
	{
		public int	id;
		string		name;
		string		description;

		public
		Pad (string name, string desc)
		{
			this.name        = name;
			this.description = desc;
		}

		// Serialization / Deserialization
		public void
		Serialize (XmlWriter writer)
		{
		}

		public void
		Deserialize (XmlReader reader)
		{
		}

		// Properties.
		public int
		Id
		{
			get { return id; }
		}

		public string
		Name
		{
			get { return name; }
		}

		public string
		Description
		{
			get { return description; }
		}
	}

	public class InputPad : Pad
	{
		public
		InputPad (string name, string desc) : base (name, desc)
		{
		}
	}

	public class OutputPad : Pad
	{
		public
		OutputPad (string name, string desc) : base (name, desc)
		{
		}
	}

	public enum ElementFlags {
		Input  = 1 << 0,
		Output = 1 << 1,
	};

	public abstract class Element
	{
		abstract public string		Name ();
		abstract public string		Category ();
		abstract public string		Description ();
		abstract public Gdk.Pixbuf	Icon ();

		// private element-specific data
		public System.Guid		id;

		public InputPad [] 		inputs;
		public OutputPad [] 		outputs;

		// User-defined comment. This is really only here for the user's sake - they
		// can use it for whatever they want, and it will show up in the tooltip.
		public string			comment;

		// Flags associated with this element
		public ElementFlags		flags;

		protected
		Element ()
		{
			flags = 0;
		}

		protected void
		NewID ()
		{
			id = System.Guid.NewGuid ();
		}

		protected void
		SetPadNumbers ()
		{
			if (inputs != null)
				for (int i = 0; i < inputs.Length; i++)
					inputs[i].id = i;
			if (outputs != null)
				for (int i = 0; i < outputs.Length; i++)
					outputs[i].id = i;
		}

		// Convenience functions for gettin the longest name for input and output pads.
		public string
		LongestInputPadName ()
		{
			string name = "";

			foreach (InputPad pad in inputs) {
				if (pad.Name.Length > name.Length)
					name = pad.Name;
			}

			return name;
		}

		public string
		LongestOutputPadName ()
		{
			string name = "";

			foreach (OutputPad pad in outputs) {
				if (pad.Name.Length > name.Length)
					name = pad.Name;
			}

			return name;
		}

		public virtual void
		Serialize (XmlTextWriter writer)
		{
		}

		public string
		XmlName ()
		{
			// Convert the name to something a little more XML friendly
			// (instead of "Matrix Multiply", we'd have "matrix-multiply")
			string name = Name ().ToLower ().Replace (" ", "-");

			// XML doesn't like things starting with numbers. since the XML name
			// doesn't really matter much, just replace all numbers with text
			name = name.Replace ("0", "zero");
			name = name.Replace ("1", "one");
			name = name.Replace ("2", "two");
			name = name.Replace ("3", "three");
			name = name.Replace ("4", "four");
			name = name.Replace ("5", "five");
			name = name.Replace ("6", "six");
			name = name.Replace ("7", "seven");
			name = name.Replace ("8", "eight");
			name = name.Replace ("9", "nine");
			return name;
		}

		public void
		Write (XmlTextWriter writer)
		{
			// Write out a new element with this name
			writer.WriteStartElement (null, XmlName (), null);

			// Write out ID
			writer.WriteStartAttribute (null, "id", null);
			writer.WriteString (id.ToString ());
			writer.WriteEndAttribute ();

			// Write out comment
			writer.WriteStartAttribute (null, "comment", null);
			writer.WriteString (comment);
			writer.WriteEndAttribute ();

			// Hand the writer off to the subclass to serialize any extra data we've got
			Serialize (writer);

			// Close the element
			writer.WriteEndElement ();
		}

		public virtual void
		DeSerialize (XmlTextReader reader)
		{
		}

		public void
		Read (XmlTextReader reader)
		{
			// Hand the reader off to the subclass to deserialize any
			// extra data we've got
			DeSerialize (reader);

			if (!reader.MoveToFirstAttribute ())
				return;

			do {
				if (reader.Name == "id")
					id = new System.Guid (reader.Value);
				else if (reader.Name == "comment")
					comment = reader.Value;
			} while (reader.MoveToNextAttribute ());
		}

		public void
		Edit (Gtk.Window transient)
		{
			// Every element has at least a "comments" field in this dialog. Subclasses
			// can override AddEditWidgets in order to add their own settings.
			Gtk.Dialog d = new Gtk.Dialog ();

			d.Title = System.String.Format ("Edit {0} Properties", Name ());
			d.TransientFor = transient;
			d.Modal = true;
			d.HasSeparator = false;

			// Set the default size of the dialog to be 640x480
			d.DefaultWidth = 640;
			d.DefaultHeight = 480;

			// Stick everything inside of a scrolled window.
			Gtk.ScrolledWindow s = new Gtk.ScrolledWindow ();
			Gtk.VBox v = new Gtk.VBox (false, 6);
			v.BorderWidth = 6;
			s.AddWithViewport (v);
			d.VBox.PackStart (s, true, true, 0);

			// FIXME - we should include some information about the Element here,
			// before any of the settings.

			// Add everything to the dialog
			AddEditWidgets (v);
			d.AddButton (Gtk.Stock.Ok, Gtk.ResponseType.Ok);

			// Show everything and run
			s.ShadowType = Gtk.ShadowType.None;
			s.Show ();
			v.Show ();
			d.Run ();
			d.Destroy ();
		}

		protected virtual void
		AddEditWidgets (Gtk.VBox box)
		{
			// Subclasses will override this in order to add their own widgets to
			// the edit dialog.  We expect these subclasses to call
			// base.AddEditWidgets() at whatever point they want the Element stuff
			// to appear. Generally, this should be after all of the useful stuff.
			Gtk.Label name = new Gtk.Label ();
			name.Markup = "<b>Comments</b>";
			name.Xalign = 0.0f;
			name.Show ();
			box.PackStart (name, false, true, 0);

			Gtk.HBox h = new Gtk.HBox (false, 12);
			box.PackStart (h, true, true, 0);

			Gtk.Label spacer = new Gtk.Label ();
			h.PackStart (spacer, false, false, 0);

			Gtk.ScrolledWindow s = new Gtk.ScrolledWindow ();
			s.ShadowType = Gtk.ShadowType.In;
			s.HeightRequest = 300;
			h.PackStart (s, true, true, 0);

			Gtk.TextView v = new Gtk.TextView ();
			if (comment != null)
				v.Buffer.Text = comment;
			s.Add (v);
			h.ShowAll ();

			v.Buffer.Changed += new System.EventHandler (CommentsBufferChanged);
		}

		void
		CommentsBufferChanged (object o, System.EventArgs args)
		{
			Gtk.TextBuffer buffer = (Gtk.TextBuffer) o;
			comment = buffer.Text;
		}

		public virtual Type[]
		Check (Type[] t)
		{
			return null;
		}
	}

}
