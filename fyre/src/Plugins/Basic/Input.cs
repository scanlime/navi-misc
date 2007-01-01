/*
 * Input.cs - An Element which gives the user an input field in the GUI
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

class Input : Fyre.Element
{
	static Gdk.Pixbuf	icon;
	int			type;

	public
	Input () : base ()
	{
		outputs = new Fyre.OutputPad[] {
			new Fyre.OutputPad ("d", "data"),
		};

		type = 0;

		flags = Fyre.ElementFlags.Input;

		SetPadNumbers ();
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
		writer.WriteStartAttribute (null, "type", null);
		writer.WriteString (System.String.Format ("{0}", type));
		writer.WriteEndAttribute ();
	}

	public override void
	DeSerialize (XmlTextReader reader)
	{
		reader.MoveToAttribute ("type");
		type = System.Int32.Parse (reader.Value);
	}

	protected override void
	AddEditWidgets (Gtk.VBox box)
	{
		Gtk.Label category = new Gtk.Label();
		category.Xalign = 0.0f;
		category.Markup = "<b>Type</b>";
		category.Show ();
		box.PackStart (category, false, true, 0);

		Gtk.HBox h = new Gtk.HBox (false, 12);
		Gtk.Label l = new Gtk.Label ();
		Gtk.ComboBox b = Gtk.ComboBox.NewText ();
		b.AppendText ("Int");
		b.AppendText ("Float");
		b.AppendText ("Boolean");
		b.AppendText ("Color");
		// FIXME - add some typical matrix inputs?
		h.PackStart (l, false, false, 0);
		h.PackStart (b, false, true, 0);
		h.ShowAll ();
		box.PackStart (h, false, true, 0);

		category = new Gtk.Label();
		category.Xalign = 0.0f;
		category.Markup = "<b>Bounds</b>";
		category.Show ();
		box.PackStart (category, false, true, 0);

		// integer bounds
		h = new Gtk.HBox (false, 12);
		l = new Gtk.Label ();
		Gtk.Table t = new Gtk.Table (2, 2, false);
		t.Attach (new Gtk.Label ("Lower:"), 0, 1, 0, 1);
		t.Attach (new Gtk.Label ("Upper:"), 0, 1, 1, 2);
		t.Attach (new Gtk.SpinButton (-1000f, 1000f, 1f), 1, 2, 0, 1);
		t.Attach (new Gtk.SpinButton (-1000f, 1000f, 1f), 1, 2, 1, 2);
		t.RowSpacing = 6;
		t.ColumnSpacing = 6;
		h.PackStart (l, false, false, 0);
		h.PackStart (t, false, true, 0);
		h.ShowAll ();
		box.PackStart (h, false, true, 0);


		// FIXME - add type-specific information.

		b.Changed += new System.EventHandler (TypeComboChanged);
		b.Active = type;

		base.AddEditWidgets (box);
	}

	void
	TypeComboChanged (object o, System.EventArgs e)
	{
		Gtk.ComboBox combo = (Gtk.ComboBox) o;
		type = combo.Active;
		// FIXME - change visibility on type-specific widgets
	}
}
