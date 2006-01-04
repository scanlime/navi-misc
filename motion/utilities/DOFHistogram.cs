/*
 * DOFHistogram.cs - displays a histogram of the data for a single DOF
 *
 * Copyright (C) 2005-2006 David Trowbridge
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

using System.Collections;

class DOFHistogram
{
	[Glade.Widget] Gtk.ComboBox		dof;
	[Glade.Widget] Gtk.VBox			vbox;
	[Glade.Widget] Gtk.Window		toplevel;
	//PlotSurface2D		plot;
	AMC.File				amc;
	Gtk.ListStore				dof_store;
	Gtk.CellRendererText			dof_renderer;

	public static void
	Main (string[] args)
	{
		Gtk.Application.Init ();
		new DOFHistogram ();
		Gtk.Application.Run ();
	}

	private
	DOFHistogram ()
	{
		Glade.XML gxml = new Glade.XML (null, "dof-histogram.glade", "toplevel", null);
		gxml.Autoconnect (this);

	//	plot = new PlotSurface2D ();
	//	vbox.PackEnd (plot);

		dof_store = new Gtk.ListStore (typeof (string));
		dof_renderer = new Gtk.CellRendererText ();
		dof_renderer.Font = "Bitstream Vera Sans 7";

		dof.Model = dof_store;
		dof.PackStart (dof_renderer, true);
		dof.AddAttribute (dof_renderer, "text", 0);

		toplevel.ShowAll ();
	}

	public void
	OnOpen (object o, System.EventArgs args)
	{
		object[] responses = {
			Gtk.Stock.Cancel, Gtk.ResponseType.Cancel,
			Gtk.Stock.Open,   Gtk.ResponseType.Accept,
		};
		Gtk.FileChooserDialog fs = new Gtk.FileChooserDialog ("Open AMC...", null, Gtk.FileChooserAction.Open, responses);

		// File filter - *.amc
		Gtk.FileFilter ff1 = new Gtk.FileFilter ();
		ff1.AddPattern ("*.amc");
		ff1.Name = "Acclaim Motion Capture (*.amc)";
		fs.AddFilter (ff1);

		// File filter - all files
		Gtk.FileFilter ff2 = new Gtk.FileFilter ();
		ff2.AddPattern ("*");
		ff2.Name = "All Files";
		fs.AddFilter (ff2);

		Gtk.ResponseType response = (Gtk.ResponseType) fs.Run ();
		fs.Hide ();

		if (response == Gtk.ResponseType.Accept) {
			AMC.File file = AMC.File.Load (fs.Filename);
			if (file == null) {
				// FIXME - pop up an error dialog
				System.Console.WriteLine ("Error loading {0}", fs.Filename);
			} else {
				amc = file;

				AMC.Frame f = (AMC.Frame) amc.frames[0];
				IDictionaryEnumerator e = f.GetEnumerator ();
				e.Reset ();
				while (e.MoveNext ()) {
					float[] s = (float[]) e.Value;
					for (int i = 0; i < s.Length; i++) {
						dof_store.AppendValues (System.String.Format ("{0}:{1}", e.Key, i));
					}
				}
			}
		}
	}

	public void
	OnQuit (object o, System.EventArgs args)
	{
		Gtk.Application.Quit ();
	}
}
