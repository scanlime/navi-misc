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

	public abstract class Pad
	{
		System.Guid	id;		// Does this need to be globally unique?
		string		name;
		string		description;
		string		type;	// FIXME: Is a string the best way to store this?

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
	}

	public class OutputPad : Pad
	{
		PadConnection [] connections;
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
