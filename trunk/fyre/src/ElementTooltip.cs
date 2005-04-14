/*
 * ElementTooltip.cs - a widget which displays a pretty tooltip
 *	with information about an Element.
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

namespace Fyre
{

	public class ElementTooltip
	{
		[Glade.Widget] Gtk.Window		tooltip;
		[Glade.Widget] Gtk.Image		element_icon;
		[Glade.Widget] Gtk.Label		element_name;
		[Glade.Widget] Gtk.Label		element_description;
		[Glade.Widget] Gtk.Box			pads;

		public
		ElementTooltip (Element e)
		{
			Glade.XML gxml = new Glade.XML (null, "element-tooltip.glade", "tooltip", null);
			gxml.Autoconnect (this);

			element_icon.Pixbuf = e.Icon ();
			element_name.Markup =
				"<span weight=\"bold\" size=\"large\">" +
				e.Name () +
				"</span>";
			element_description.Markup = BuildString (e.Description ());

			this.pads.PackStart  (CreateDescTable (e), false, true, 0);
		}

		public void
		Show ()
		{
			tooltip.ShowAll ();
		}

		public void
		Hide ()
		{
			tooltip.HideAll ();
		}

		public void
		Move (double x, double y)
		{
			tooltip.Move ((int) x, (int) y);
		}

		private Gtk.Table
		CreateDescTable (Element e)
		{
			Gtk.Table t = new Gtk.Table (0, 3, false);
			if (e == null)
				return t;

			uint row = 0;

			t.ColumnSpacing = 12;
			t.RowSpacing = 3;

			if (e.inputs != null) {
				// Resize the table to fit the inputs
				t.Resize ((uint) (e.inputs.Length + 1), 3);

				// Add the category label
				Gtk.Label category = new Gtk.Label ();
				category.Xalign = 0.0f;
				category.Markup = "<b>Inputs</b>";
				t.Attach (category, 0, 3, row, row+1, Gtk.AttachOptions.Fill, Gtk.AttachOptions.Expand, 0, 0);
				row++;

				// Add each of the inputs
				for (int i = 0; i < e.inputs.Length; i++) {
					Gtk.Label name = new Gtk.Label ();
					Gtk.Label desc = new Gtk.Label ();

					name.Markup = BuildString (e.inputs[i].Name);
					desc.Markup = BuildString (e.inputs[i].Description);

					name.Xalign = 0.0f;
					desc.Xalign = 0.0f;

					t.Attach (name, 1, 2, row, row+1);
					t.Attach (desc, 2, 3, row, row+1);

					row++;
				}
			}

			if (e.outputs != null) {
				// Resize the table to fit the outputs
				t.Resize ((uint) (t.NRows + e.outputs.Length + 1), 3);

				// Add the category label
				Gtk.Label category = new Gtk.Label ();
				category.Xalign = 0.0f;
				category.Markup = "<b>Outputs</b>";
				t.Attach (category, 0, 3, row, row+1, Gtk.AttachOptions.Fill, Gtk.AttachOptions.Expand, 0, 0);
				row++;

				// Add each of the outputs
				for (int i = 0; i < e.outputs.Length; i++) {
					Gtk.Label name = new Gtk.Label ();
					Gtk.Label desc = new Gtk.Label ();

					name.Markup = BuildString (e.outputs[i].Name);
					desc.Markup = BuildString (e.outputs[i].Description);

					name.Xalign = 0.0f;
					desc.Xalign = 0.0f;

					t.Attach (name, 1, 2, row, row+1);
					t.Attach (desc, 2, 3, row, row+1);

					row++;
				}
			}

			return t;
		}

		string
		BuildString (string n)
		{
			return "<span size=\"small\">" + n + "</span>";
		}
	}

}
