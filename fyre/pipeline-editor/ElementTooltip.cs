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

using System;
using Gtk;
using Glade;

public class ElementTooltip
{
	[Widget] Gtk.Window	tooltip;
	[Widget] Gtk.Image	element_icon;
	[Widget] Gtk.Label	element_name;
	[Widget] Gtk.Label	element_description;
	[Widget] Gtk.Box	inputs;
	[Widget] Gtk.Box	outputs;

	public ElementTooltip (Element e)
	{
		Glade.XML gxml = new Glade.XML (null, "element-tooltip.glade", "tooltip", null);
		gxml.Autoconnect (this);
		
		element_icon.Pixbuf = e.Icon ();
		element_name.Markup =
			"<span weight=\"bold\" size=\"large\">" +
			e.Name () +
			"</span>";
		element_description.Markup =
			"<span size=\"small\">" +
			e.Description () +
			"</span>";
			
		string[,] inputs = e.InputDesc ();
		Gtk.Table input = new Gtk.Table ((uint) inputs.Length / 2, 2, false);
		for (uint i = 0; i < inputs.Length / 2; i++) {
			Gtk.Label variable = new Gtk.Label ();
			Gtk.Label desc = new Gtk.Label ();
			
			variable.Markup =
				"<span size=\"small\">" +
				inputs[i,0] +
				"</span>";
			desc.Markup =
				"<span size=\"small\">" +
				inputs[i,1] +
				"</span>";
			input.Attach (variable, 0, 1, i, i+1);
			input.Attach (desc, 1, 2, i, i+1);
		}
		this.inputs.PackStart (input, false, true, 0);

		string[,] outputs = e.OutputDesc ();
		Gtk.Table output = new Gtk.Table ((uint) outputs.Length / 2, 2, false);
		for (uint i = 0; i < outputs.Length / 2; i++) {
			Gtk.Label variable = new Gtk.Label ();
			Gtk.Label desc = new Gtk.Label ();
			
			variable.Markup =
				"<span size=\"small\">" +
				outputs[i,0] +
				"</span>";
			desc.Markup =
				"<span size=\"small\">" +
				inputs[i,1] +
				"</span>";
			output.Attach (variable, 0, 1, i, i+1);
			output.Attach (desc, 1, 2, i, i+1);
		}
		this.outputs.PackStart (output, false, true, 0);
	}
}