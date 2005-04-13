/*
 * Element.cs - abstract data type defining an Element
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
using System;

namespace Fyre
{
	public struct PadConnection
	{
		public System.Guid	element;
		public int		pad;
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
		System.Collections.ArrayList connections;

		public
		OutputPad (string name, string desc) : base (name, desc)
		{
			connections = new System.Collections.ArrayList ();
		}

		public void
		Connect (PadConnection connection)
		{
			connections.Add (connection);
		}

		public void
		Disconnect (PadConnection connection)
		{
			connections.Remove (connection);
		}

		public void
		DisconnectAll ()
		{
			connections.Clear ();
		}

		public void
		Serialize (XmlTextWriter writer)
		{

			// Write the source and destination tags for each connection in this pad
			foreach (PadConnection conn in connections) {
				// Create a new element for this edge
				writer.WriteStartElement (null, "edge", null);

				// Write the source pad
				writer.WriteStartAttribute (null, "source", null);
				writer.WriteString (Id.ToString());
				writer.WriteEndAttribute ();

				// Write the destination pad
				writer.WriteStartAttribute (null, "dest", null);
				writer.WriteString (String.Format ("{0}:{1}", conn.element.ToString (), conn.pad));
				writer.WriteEndAttribute ();

				// End the element
				writer.WriteEndElement ();
			}
		}

		public void
		DeSerialize (XmlTextReader reader)
		{
			// TODO: Implement
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
		public CanvasElement		canvas_element;

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
		NewCanvasElement ()
		{
			canvas_element = new CanvasElement ();
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

		public string[,]
		InputDesc ()
		{
			if (inputs == null)
				return null;
			string[,] desc = new string[inputs.Length,2];

			for (int i = 0; i < inputs.Length; i++) {
				desc[i,0] = inputs[i].Name;
				desc[i,1] = inputs[i].Description;
			}

			return desc;
		}

		public string[,]
		OutputDesc ()
		{
			if (outputs == null)
				return null;
			string [,] desc = new string[outputs.Length,2];

			for (int i = 0; i < outputs.Length; i++) {
				desc[i,0] = outputs[i].Name;
				desc[i,1] = outputs[i].Description;
			}

			return desc;
		}

		public virtual void
		Serialize (XmlTextWriter writer)
		{
		}

		public void
		Write (XmlTextWriter writer)
		{
			// Convert the name to something a little more XML friendly
			// (instead of "Matrix Multiply", we'd have "matrix-multiply")
			string convName = Name ().ToLower ().Replace (" ", "-");

			// Write out a new element with this name
			writer.WriteStartElement (null, convName, null);
			writer.WriteStartAttribute (null, "id", null);
			writer.WriteString (id.ToString ());
			writer.WriteEndAttribute ();

			// FIXME - serialize comment

			// Serialize the element's CanvasElement to store positioning information
			canvas_element.Serialize (writer);

			// Write out each of the output pads of the element
			foreach (OutputPad pad in outputs) {
				pad.Serialize (writer);
			}

			// Hand the writer off to the subclass to serialize any extra data we've got
			Serialize (writer);

			// Close the element
			writer.WriteEndElement ();
		}

		public virtual void
		DeSerialize (XmlTextReader reader)
		{
			// FIXME - deserialize everything
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
			s.Add (v);
			h.ShowAll ();
			// FIXME - set up callbacks on the buffer
		}

		public virtual bool
		Check (Type[] t, out Type[] to)
		{
			to = new Type[] {};
			return true;
		}
	}

}
