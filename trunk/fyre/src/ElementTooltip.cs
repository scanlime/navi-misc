/*
 * ElementTooltip.cs - a widget which displays a pretty tooltip
 *	with information about an Element.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2005 David Trowbridge and Micah Dowty
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
		[Glade.Widget] Gtk.Box			inputs;
		[Glade.Widget] Gtk.Box			outputs;

		public ElementTooltip (Element e)
		{
			Glade.XML gxml = new Glade.XML (null, "element-tooltip.glade", "tooltip", null);
			gxml.Autoconnect (this);

			element_icon.Pixbuf = e.Icon ();
			element_name.Markup =
				"<span weight=\"bold\" size=\"large\">" +
				e.Name () +
				"</span>";
			element_description.Markup = BuildString (e.Description ());

			this.inputs.PackStart  (CreateDescTable (e.InputDesc  ()), false, true, 0);
			this.outputs.PackStart (CreateDescTable (e.OutputDesc ()), false, true, 0);
		}

		public void Show ()
		{
			tooltip.ShowAll ();
		}

		public void Hide ()
		{
			tooltip.HideAll ();
		}

		public void Move (double x, double y)
		{
			tooltip.Move ((int) x, (int) y);
		}

		private Gtk.Table CreateDescTable (string[,] s)
		{
			// Create a 2xn table from a list of strings
			uint len = (uint) s.Length / 2;
			Gtk.Table t = new Gtk.Table (len, 2, false);
			for (uint i = 0; i < len; i++) {
				Gtk.Label variable = new Gtk.Label ();
				Gtk.Label desc     = new Gtk.Label ();

				variable.Xalign = 0.0f;
				desc.Xalign     = 0.0f;

				variable.Markup = BuildString (s[i,0]);
				desc.Markup     = BuildString (s[i,1]);

				t.Attach (variable, 0, 1, i, i+1);
				t.Attach (desc,     1, 2, i, i+1);
				t.ColumnSpacing = 12;
				t.RowSpacing = 3;
			}
			return t;
		}

		string BuildString (string n)
		{
			return "<span size=\"small\">" + n + "</span>";
		}
	}

}
