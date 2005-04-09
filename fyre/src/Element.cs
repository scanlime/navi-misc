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

namespace Fyre
{
	public struct PadConnection
	{
		System.Guid	element;
		int		pad;
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
			return "";
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

		// Properties.
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
		Connect (InputPad pad)
		{
			connections.Add (pad);
		}

		public void
		Disconnect (InputPad pad)
		{
			connections.Remove (pad);
		}

		public void
		DisconnectAll ()
		{
			connections.Clear ();
		}
	}

	public abstract class Element
	{
		abstract public string		Name ();
		abstract public string		Category ();
		abstract public string		Description ();
		abstract public Gdk.Pixbuf	Icon ();

		// These are expected to be in pango markup, for any
		// necessary subscripts, UTF-8 trickery, etc
		abstract public string[,]	InputDesc ();
		abstract public string[,]	OutputDesc ();

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

		public virtual void
		Serialize (XmlWriter writer)
		{
			// FIXME - serialize guid
		}

		public virtual void
		DeSerialize (XmlReader reader)
		{
			// FIXME - serialize guid
		}
	}

}
