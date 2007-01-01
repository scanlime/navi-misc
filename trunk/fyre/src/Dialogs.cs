/*
 * Dialogs.cs - Simple HIG-compliant dialogs for reporting errors and
 *	asking questions
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

namespace Fyre
{
	// An abstract dialog class. It sets up the labels with whatever text is going in
	// the dialog and makes the dialog window transient and modal for the main window.
	abstract class Dialog : Gtk.Dialog
	{
		[Glade.Widget] Gtk.HBox		toplevel;
		[Glade.Widget] Gtk.Label	label1, label2;

		public
		Dialog (Gtk.Window transient, string summary, string description)
		{
			Title = "";
			BorderWidth = 0;
			ActionArea.BorderWidth = 12;
			Resizable = false;
			HasSeparator = false;


			Glade.XML xml = new Glade.XML (null, "alert-dialog.glade", "toplevel", null);
			xml.Autoconnect (this);

			label1.Markup = "<span weight=\"bold\" size=\"larger\">" + summary + "</span>";
			label2.Text = description;

			VBox.PackStart (toplevel, true, true, 0);

			// Get the main window and set the dialog transient and modal for that window.
			TransientFor = transient;

			Modal = true;
		}
	}

	class ErrorDialog : Dialog
	{
		[Glade.Widget] Gtk.Image	image;

		public
		ErrorDialog (Gtk.Window transient, string summary, string description) : base (transient, summary, description)
		{
			image.SetFromStock (Gtk.Stock.DialogError, Gtk.IconSize.Dialog);
			AddButton (Gtk.Stock.Ok, Gtk.ResponseType.Ok);
		}
	}

	class WarningDialog : Dialog
	{
		[Glade.Widget] Gtk.Image	image;

		public
		WarningDialog (Gtk.Window transient, string summary, string description) : base (transient, summary, description)
		{
			image.SetFromStock (Gtk.Stock.DialogWarning, Gtk.IconSize.Dialog);
			AddButton (Gtk.Stock.Ok, Gtk.ResponseType.Ok);
		}
	}

	class ConfirmDialog : Dialog
	{
		[Glade.Widget] Gtk.Image	image;

		public
		ConfirmDialog (Gtk.Window transient, string summary, string description) : base (transient, summary, description)
		{
			image.SetFromStock (Gtk.Stock.DialogQuestion, Gtk.IconSize.Dialog);
			AddButton (Gtk.Stock.No, Gtk.ResponseType.No);
			AddButton (Gtk.Stock.Yes, Gtk.ResponseType.Yes);
		}
	}

	// This dialog is shown when the user attempts to quit and has unsaved changes.
	class ConfirmCloseDialog : Dialog
	{
		[Glade.Widget] Gtk.Image	image;

		public
		ConfirmCloseDialog (Gtk.Window transient, string summary, string description) : base (transient, summary, description)
		{
			image.SetFromStock (Gtk.Stock.DialogWarning, Gtk.IconSize.Dialog);

			AddButton ("Close Without Saving", Gtk.ResponseType.No);
			AddButton (Gtk.Stock.Cancel, Gtk.ResponseType.Cancel);
			AddButton (Gtk.Stock.Save, Gtk.ResponseType.Yes);
		}
	}
}
