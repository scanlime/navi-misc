/*
 * Equation.cs - An Element which executes an equation entered by the user
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

class Equation : Fyre.Element
{
	static Gdk.Pixbuf icon;

	public
	Equation () : base ()
	{
		NewID ();
	}

	public override string
	Name ()
	{
		return "Equation";
	}

	public override string
	Category()
	{
		return "Arithmetic";
	}

	public override Gdk.Pixbuf
	Icon ()
	{
		if (icon == null)
			icon = new Gdk.Pixbuf (null, "Equation.png");
		return icon;
	}

	public override string
	Description ()
	{
		return "Computes an equation";
	}

	public override Fyre.Type[]
	Check (Fyre.Type[] t)
	{
		return null;
	}

	protected override void
	AddEditWidgets (Gtk.VBox box)
	{
		Gtk.Label category = new Gtk.Label ();
		category.Xalign = 0.0f;
		category.Markup = "<b>Equation</b>";
		category.Show ();
		box.PackStart (category, false, true, 0);

		Gtk.HBox hbox = new Gtk.HBox (false, 12);
		Gtk.VBox vbox = new Gtk.VBox (false, 6);

		Gtk.Label help = new Gtk.Label ();
		help.Xalign = 0.0f;
		help.Text = "Enter an equation. If you are entering a system, put one equation per line.";
		vbox.PackStart (help, false, true, 0);

		Gtk.ScrolledWindow swin = new Gtk.ScrolledWindow ();
		Gtk.TextView text = new Gtk.TextView ();
		swin.Add (text);
		swin.ShadowType = Gtk.ShadowType.In;
		vbox.PackStart (swin, true, true, 0);

		Gtk.Label l = new Gtk.Label ();
		hbox.PackStart (l, false, false, 0);
		hbox.PackStart (vbox, true, true, 0);
		hbox.ShowAll ();

		box.PackStart (hbox, true, true, 0);

		base.AddEditWidgets (box);
	}
}
