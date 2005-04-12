/*
 * Input.cs - An Element which gives the user an input field in the GUI
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

class Input : Fyre.Element
{
	static Gdk.Pixbuf icon;

	public
	Input ()
	{
		outputs = new Fyre.OutputPad[] {
			new Fyre.OutputPad ("d", "data"),
		};

		SetPadNumbers ();
		NewCanvasElement ();
		NewID ();
	}

	public override string
	Name ()
	{
		return "Input";
	}

	public override string
	Category()
	{
		return "Miscellaneous";
	}

	public override Gdk.Pixbuf
	Icon ()
	{
		if (icon == null)
			icon = new Gdk.Pixbuf (null, "Input.png");
		return icon;
	}

	public override string
	Description ()
	{
		return "Creates an input field\nin the fyre GUI";
	}

	public override void
	Serialize (XmlTextWriter writer)
	{
		base.Serialize (writer);
	}

	public override void
	DeSerialize (XmlTextReader reader)
	{
		base.DeSerialize (reader);
	}

	protected override void
	AddEditWidgets (Gtk.VBox box)
	{
		Gtk.Label category = new Gtk.Label();
		category.Xalign = 0.0f;
		category.Markup = "<b>Type</b>";
		box.PackStart (category, false, true, 0);

		Gtk.ComboBox b = Gtk.ComboBox.NewText ();
		b.AppendText ("Int");
		b.AppendText ("Float");
		b.AppendText ("Boolean");
		b.AppendText ("Color");
		// FIXME - add some typical matrix inputs?
		b.Active = 0;
		Gtk.HBox h = new Gtk.HBox (false, 12);
		Gtk.Label l = new Gtk.Label ();
		h.PackStart (l, false, false, 0);
		h.PackStart (b, false, true, 0);
		box.PackStart (h, false, true, 0);

		base.AddEditWidgets (box);
	}
}
