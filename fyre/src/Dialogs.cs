/*
 * Dialogs.cs - Simple HIG-compliant dialogs for reporting errors and
 *	asking questions
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

namespace Fyre
{
	class Dialog : Gtk.Dialog
	{
		[Glade.Widget] Gtk.HBox		toplevel;
		[Glade.Widget] Gtk.Label	label1, label2;

		public
		Dialog (string summary, string description)
		{
			Title = "";
			BorderWidth = 6;
			Resizable = false;
			HasSeparator = false;


			Glade.XML xml = new Glade.XML (null, "alert-dialog.glade", "toplevel", null);
			xml.Autoconnect (this);

			label1.Markup = "<span weight=\"bold\" size=\"larger\">" + summary + "</span>";
			label2.Text = description;

			VBox.PackStart (toplevel, true, true, 0);

			// Get the main window and set the dialog transient and modal for that window.
			TransientFor = PipelineEditor.instance.GetWindow();
			Modal = true;
		}
	}

	class ErrorDialog : Dialog
	{
		[Glade.Widget] Gtk.Image	image;

		public
		ErrorDialog (string summary, string description) : base (summary, description)
		{
			image.SetFromStock (Gtk.Stock.DialogError, Gtk.IconSize.Dialog);
			AddButton (Gtk.Stock.Ok, Gtk.ResponseType.Ok);
		}
	}

	class WarningDialog : Dialog
	{
		[Glade.Widget] Gtk.Image	image;

		public
		WarningDialog (string summary, string description) : base (summary, description)
		{
			image.SetFromStock (Gtk.Stock.DialogWarning, Gtk.IconSize.Dialog);
			AddButton (Gtk.Stock.Ok, Gtk.ResponseType.Ok);
		}
	}

	class ConfirmDialog : Dialog
	{
		[Glade.Widget] Gtk.Image	image;

		public
		ConfirmDialog (string summary, string description) : base (summary, description)
		{
			image.SetFromStock (Gtk.Stock.DialogQuestion, Gtk.IconSize.Dialog);
		}
	}
}
