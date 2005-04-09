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

	public class FString
	{
		string str;

		public
		FString (string new_str)
		{
			str = new_str;
		}

		public string
		Plain
		{
			get { return str; }
		}

		public override string
		ToString () // This should take a formatter.
		{
			// Formatting nonsense goes here.
			return "";
		}

		// Implicit conversion to string
		public static implicit operator
		string (FString s)
		{
			return s.str;
		}
	}

	public abstract class Pad
	{
		int	id;
		FString	name;
		FString	description;
		string	type;	// FIXME: Is a string the best way to store this?
				// No, it isn't.

		public
		Pad (string pad_name, string pad_desc, string pad_type)
		{
			name = new FString (pad_name);
			description = new FString (pad_desc);
			type = pad_type;
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

		public string
		Type
		{
			get { return type; }
		}
	}

	public class InputPad : Pad
	{
		public
		InputPad (string pad_name, string pad_desc, string pad_type) : base (pad_name, pad_desc, pad_type)
		{
		}
	}

	public class OutputPad : Pad
	{
		System.Collections.ArrayList connections;

		public
		OutputPad (string pad_name, string pad_desc, string pad_type) : base (pad_name, pad_desc, pad_type)
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

		public string[,]
		InputDesc ()
		{
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
			// Convert the name to something a little more XML friendly
			// (instead of "Matrix Multiply", we'd have "matrix-multiply")
			string convName = Name ().ToLower ().Replace (" ", "-");

			// Write out a new element with this name
			writer.WriteStartElement (null, convName, null);
			writer.WriteStartAttribute (null, "id", null);
			writer.WriteString( id.ToString() );

			// Serialize the element's CanvasElement to store positioning information
			canvas_element.Serialize (writer);

			// Write out each of the output pads of the element
			foreach (OutputPad pad in outputs) {
				pad.Serialize (writer);
			}

			// Close the element
			writer.WriteEndElement ();
		}

		public virtual void
		DeSerialize (XmlTextReader reader)
		{
			// FIXME - serialize guid
		}
	}

}
