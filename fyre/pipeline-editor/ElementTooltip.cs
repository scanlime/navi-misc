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
	}
}