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
			if (e == null)
				return new Gtk.Table (0, 0, false);

			string [,] inputs = e.InputDesc ();
			string [,] outputs = e.OutputDesc ();

			// Create a 2xn table from a list of strings. We expect
			// that any special syntax we need here is encoded in
			// pango markup or unicode.
			uint input_len;
			if (inputs == null)
				input_len = 0;
			else
				input_len = (uint) inputs.Length / 2;

			uint output_len;
			if (outputs == null)
				output_len = 0;
			else
				output_len = (uint) outputs.Length / 2;

			uint len = (uint) (input_len + output_len + 2);
			Gtk.Table t = new Gtk.Table (len, 2, false);

			// Input category.
			Gtk.Label category = new Gtk.Label ();
			category.Markup = "<b>Inputs</b>";
			t.Attach (category, 0, 1, 0, 1);

			for (uint i = 0; i < (uint) input_len; i++) {
				Gtk.Label variable = new Gtk.Label ();
				Gtk.Label desc     = new Gtk.Label ();

				variable.Xalign = 0.0f;
				desc.Xalign     = 0.0f;

				variable.Markup = BuildString (inputs[i,0]);
				desc.Markup     = BuildString (inputs[i,1]);

				t.Attach (variable, 0, 1, i+1, i+2);
				t.Attach (desc,     1, 2, i+1, i+2);
				t.ColumnSpacing = 12;
				t.RowSpacing = 3;
			}

			// Output category.
			category = new Gtk.Label ();
			category.Markup = "<b>Outputs</b>";
			t.Attach (category, 0, 1, input_len+1, input_len+2);

			for (uint i = 0; i < output_len; i++) {
				Gtk.Label variable = new Gtk.Label ();
				Gtk.Label desc     = new Gtk.Label ();

				variable.Xalign = 0.0f;
				desc.Xalign     = 0.0f;

				variable.Markup = BuildString (outputs[i,0]);
				desc.Markup     = BuildString (outputs[i,1]);

				uint pos = i + input_len + 2;
				t.Attach (variable, 0, 1, pos, pos+1);
				t.Attach (desc,     1, 2, pos, pos+1);
				t.ColumnSpacing = 12;
				t.RowSpacing = 3;
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
