/*
 * ErrorDialog.cs - Simple HIG-compliant dialog for reporting errors
 *	to the user
 *
 * Copyright (C) 2005 David Trowbridge
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

namespace Fyre
{
	class ErrorDialog : Gtk.Dialog
	{
		[Glade.Widget] Gtk.HBox		toplevel;
		[Glade.Widget] Gtk.Label	label;

		public
		ErrorDialog (string summary, string description)
		{
			Title = "";
			BorderWidth = 6;
			Resizable = false;
			HasSeparator = false;


			Glade.XML xml = new Glade.XML (null, "error-dialog.glade", "toplevel", null);
			xml.Autoconnect (this);

			label.Markup = "<span weight=\"bold\" size=\"larger\">" + summary + "</span>\n\n" + description;

			VBox.PackStart (toplevel, true, true, 0);

			AddButton (Gtk.Stock.Ok, Gtk.ResponseType.Ok);
		}
	}

	class WarningDialog : ErrorDialog
	{
		[Glade.Widget] Gtk.Image	image;

		public
		WarningDialog (string summary, string description) : base (summary, description)
		{
			image.SetFromStock (Gtk.Stock.DialogWarning, Gtk.IconSize.Dialog);
		}
	}
}
