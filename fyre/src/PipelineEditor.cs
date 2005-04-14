/*
 * PipelineEditor.cs - pipeline editor. At this moment, this also
 *	stands in as the main program
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

using System.Collections;

namespace Fyre
{

	public class PipelineEditor
	{
		// Plugin stuff
		static IDictionaryEnumerator		plugin_enumerator;

		// Document
		Pipeline				pipeline;

		// Indexes to store which new document this is
		int					new_index;
		static int				new_documents;

		// High-level Widgets
		[Glade.Widget] Gtk.Window		toplevel;
		[Glade.Widget] ElementList		element_list;
		[Glade.Widget] NavigationImage		navigation_image;
		[Glade.Widget] PipelineDrawing		pipeline_drawing;

		// Editor workspace (right)
		[Glade.Widget] Gtk.Frame		pipeline_frame;
		[Glade.Widget] Gtk.Scrollbar		drawing_hscroll;
		[Glade.Widget] Gtk.Scrollbar		drawing_vscroll;

		// Status bar
		[Glade.Widget] Gtk.Statusbar		statusbar;

		// Menu and toolbar widgets (for sensitivity)
		[Glade.Widget] Gtk.ToolButton		toolbar_save;
		[Glade.Widget] Gtk.ImageMenuItem	menu_save;
		[Glade.Widget] Gtk.ImageMenuItem	menu_undo;
		[Glade.Widget] Gtk.ImageMenuItem	menu_redo;

		// Static counter for keeping track of the open windows
		static ArrayList			editors;

		// D-n-D target data
		static Gtk.TargetEntry[]		targets;
		public static Gtk.TargetEntry[]		DragTargets
		{
			get {
				if (targets == null) {
					targets = new Gtk.TargetEntry[1];
					targets[0] = new Gtk.TargetEntry ("fyre element drag", Gtk.TargetFlags.App, 1);
				}
				return targets;
			}
		}

		static Gtk.TargetList			target_list;
		public static Gtk.TargetList		TargetList
		{
			get {
				if (target_list == null)
					target_list = new Gtk.TargetList (DragTargets);
				return target_list;
			}
		}

		public static void
		Main (string[] args)
		{
			// Setup the new GTK application
			Gtk.Application.Init ();

			// Set up plugins directory
			PluginManager plugin_manager = new PluginManager (Defines.PLUGINSDIR);
			plugin_manager.Load ();

			// The plugin manager has inserted all of the elements in plugins
			// into the element factory, so we only have to deal with this now.
			ElementFactory factory = ElementFactory.Instance;
			plugin_enumerator = factory.GetEnumerator ();

			// Setup the array of windows
			editors = new ArrayList ();

			// Create a new main window
			new PipelineEditor (args);

			// Finally, run the application
			Gtk.Application.Run ();
		}

		private
		PipelineEditor (string[] args)
		{
			editors.Add (this);
			new_documents += 1;
			new_index = new_documents;

			Glade.XML.SetCustomHandler (new Glade.XMLCustomWidgetHandler (GladeCustomHandler));

			Glade.XML gxml = new Glade.XML (null, "pipeline-editor.glade", "toplevel", null);
			gxml.Autoconnect (this);

			pipeline = new Pipeline ();
			pipeline.Changed += new System.EventHandler (PipelineChanged);

			// Set up our drawing canvas
			SetupDrawingCanvas ();

			// Iterate through each element in the elements list
			plugin_enumerator.Reset ();
			while (plugin_enumerator.MoveNext ())
				element_list.AddType ((System.Type) plugin_enumerator.Value);

			// Call update to set our title, etc.
			PipelineChanged (null, null);

			// Show the window
			toplevel.Show ();
		}

		void
		PipelineChanged (object o, System.EventArgs args)
		{
			SetTitle ();
			UpdateToolbarSensitivity ();
			UpdateUndoRedo ();
		}

		// Convenience function for getting a formatted filename string
		public string			Filename
		{
			get {
				if (pipeline.filename == null)
					return System.String.Format( "Untitiled{0}", new_index );
				else
					return pipeline.filename;
			}
		}

		// Glade custom widget handler
		static Gtk.Widget
		GladeCustomHandler (Glade.XML xml, string func_name, string name, string str1, string str2, int int1, int int2)
		{
			// It seems like the glade autoconnect stuff for custom widgets
			// isn't elegant yet for Glade#. Get around that by creating
			// the custom widgets here based on the strings that glade knows.
			if (func_name == "CreateElementList")
				return new ElementList (xml.GetWidget ("toplevel"));
			if (func_name == "CreateNavigationImage")
				return new NavigationImage ();
			if (func_name == "CreatePipelineDrawing")
				return new PipelineDrawing (xml);
			return null;
		}

		void
		SetupDrawingCanvas ()
		{
			pipeline_drawing.AddScrollbars (drawing_hscroll, drawing_vscroll);

			// Give the navigation image knowledge of the drawing, so it can
			// track canvas/window extents
			navigation_image.Drawing = pipeline_drawing;

			// Set up drag-and-drop for the frame. This looks better than setting it
			// up for the drawing area, and we delegate the event back into the
			// PipelineDrawing object when we drop.
			Gtk.Drag.DestSet (pipeline_frame, Gtk.DestDefaults.All, DragTargets, Gdk.DragAction.Copy);
			pipeline_frame.DragDataReceived += new Gtk.DragDataReceivedHandler (DrawingReceivedDrag);
		}

		void
		DrawingReceivedDrag (object o, Gtk.DragDataReceivedArgs args)
		{
			string name = System.Text.Encoding.UTF8.GetString (args.SelectionData.Data);

			ElementFactory factory = ElementFactory.Instance;
			Element e = factory.Create (name);

			Commands.AddElement adde = new Commands.AddElement (e);

			pipeline.Do (adde);
			e.Edit (toplevel);

			// FIXME - create drawing stuff
		}

		public Gtk.Window
		GetWindow ()
		{
			return toplevel;
		}

		void
		SetTitle ()
		{
			string filename = System.IO.Path.GetFileName (Filename);
			if (pipeline.saved)
				toplevel.Title = filename;
			else
				toplevel.Title = filename + "*";
		}

		void
		UpdateToolbarSensitivity ()
		{
			toolbar_save.Sensitive = !pipeline.saved;
			menu_save.Sensitive = !pipeline.saved;
		}

		void
		UpdateUndoRedo ()
		{
			if (pipeline.undo_stack.Count == 0) {
				menu_undo.Sensitive = false;
			} else {
				menu_undo.Sensitive = true;
				Gtk.Label label = (Gtk.Label) menu_undo.Child;

				PipelineCommand command = (PipelineCommand) pipeline.undo_stack[pipeline.undo_stack.Count - 1];
				label.TextWithMnemonic = "_Undo \"" + command.Name + "\"";
			}

			if (pipeline.redo_stack.Count == 0) {
				menu_redo.Sensitive = false;
			} else {
				menu_redo.Sensitive = true;
				Gtk.Label label = (Gtk.Label) menu_redo.Child;

				PipelineCommand command = (PipelineCommand) pipeline.redo_stack[pipeline.redo_stack.Count - 1];
				label.TextWithMnemonic = "_Redo \"" + command.Name + "\"";
			}
		}

		public bool
		CloseWindow ()
		{
			if (pipeline.saved == false) {
				string filename = System.IO.Path.GetFileName (Filename);
				ConfirmCloseDialog confirm = new ConfirmCloseDialog (toplevel,
						System.String.Format ("Save changes to \"{0}\" before closing?", filename),
						"There are unsaved changes to the pipeline. Save before quitting?");

				int response = confirm.Run ();
				confirm.Destroy ();

				if (response == (int) Gtk.ResponseType.Cancel)
					return true;
				if (response == (int) Gtk.ResponseType.Yes)
					OnSave (null, null);
			}
			// Hide the window from view.  It'll later be destroyed by the DeleteEvent handler, or by the
			// quit menu option handler.
			toplevel.Hide();

			// The window was closed, so return false indicating it needs to be destroyed.
			return false;
		}

		// Event handlers - most of these come from the glade file
		// Window events
		public void
		OnDeleteEvent (object o, Gtk.DeleteEventArgs args)
		{
			bool result = CloseWindow();
			if (!result) {
				editors.Remove (this);
				if (editors.Count == 0)
					Gtk.Application.Quit ();
				args.RetVal = false;
			} else
				args.RetVal = true;
		}

		// Shared events - menus/toolbars
		public void
		OnNew (object o, System.EventArgs args)
		{
			new PipelineEditor (null);
		}

		public void
		OnOpen (object o, System.EventArgs args)
		{
			object[] responses = {
				Gtk.Stock.Cancel, Gtk.ResponseType.Reject,
				Gtk.Stock.Open,   Gtk.ResponseType.Accept,
			};
			Gtk.FileChooserDialog fs = new Gtk.FileChooserDialog ("Open...", null, Gtk.FileChooserAction.Open, responses);
			fs.DefaultResponse = Gtk.ResponseType.Accept;

			Gtk.ResponseType response = (Gtk.ResponseType) fs.Run ();

			if (response == Gtk.ResponseType.Accept) {
				string filename = fs.Filename;
				if (pipeline.Empty) {
					Load (filename);
				} else {
					PipelineEditor p = new PipelineEditor (null);
					p.Load (filename);
				}
			}
			fs.Destroy ();
		}

		public void
		OnSave (object o, System.EventArgs args)
		{
			if (pipeline.filename == null) {
				object[] responses = {
					Gtk.Stock.Cancel, Gtk.ResponseType.Reject,
					Gtk.Stock.Save,   Gtk.ResponseType.Accept,
				};
				Gtk.FileChooserDialog fs = new Gtk.FileChooserDialog ("Save As...", null, Gtk.FileChooserAction.Save, responses);
				fs.DefaultResponse = Gtk.ResponseType.Accept;
				fs.CurrentName = Filename;

				Gtk.ResponseType response = (Gtk.ResponseType) fs.Run ();

				if (response == Gtk.ResponseType.Accept) {
					string filename = fs.Filename;
					pipeline.Save (filename);
					UpdateToolbarSensitivity ();
					fs.Destroy ();
				}
				fs.Destroy ();
			}
			else {
				pipeline.Save (pipeline.filename);
				UpdateToolbarSensitivity ();
			}
		}

		public void
		OnCut (object o, System.EventArgs args)
		{
			// FIXME - implement
		}

		public void
		OnCopy (object o, System.EventArgs args)
		{
			// FIXME - implement
		}

		public void
		OnPaste (object o, System.EventArgs args)
		{
			// FIXME - implement
		}

		// 'File' Menu events
		public void
		OnMenuFileSaveAs (object o, System.EventArgs args)
		{
			object[] responses = {
				Gtk.Stock.Cancel, Gtk.ResponseType.Reject,
				Gtk.Stock.Save,   Gtk.ResponseType.Accept
			};
			Gtk.FileChooserDialog fs = new Gtk.FileChooserDialog ("Save As...", null, Gtk.FileChooserAction.Save, responses);
			fs.DefaultResponse = Gtk.ResponseType.Accept;
			if (!fs.SetFilename (Filename))
				fs.CurrentName = Filename;

			Gtk.ResponseType response = (Gtk.ResponseType) fs.Run ();
			fs.Hide ();

			if (response == Gtk.ResponseType.Accept) {
				string filename = fs.Filename;
				pipeline.saved = false;
				pipeline.Save (filename);
				UpdateToolbarSensitivity ();
			}
			fs.Destroy ();
		}

		public void
		OnMenuFileClose (object o, System.EventArgs args)
		{
			bool result = CloseWindow();
			if (!result) {
				editors.Remove (this);
				if (editors.Count == 0)
					Gtk.Application.Quit ();
			}
		}

		public static void
		OnMenuFileQuit (object o, System.EventArgs args)
		{
			ArrayList removeList = new ArrayList ();
			IEnumerator e = editors.GetEnumerator ();
			e.Reset ();
			while (e.MoveNext ()) {
				PipelineEditor win = (PipelineEditor) e.Current;
				win.toplevel.Present ();
				bool result = win.CloseWindow ();
				if (result)
					break;
				else
					removeList.Add (win);
			}
			e = removeList.GetEnumerator ();
			e.Reset ();
			while (e.MoveNext ()) {
				PipelineEditor win = (PipelineEditor) e.Current;
				win.toplevel.Destroy ();
				editors.Remove (win);
			}
			if (editors.Count == 0)
				Gtk.Application.Quit ();
		}

		// 'Edit' Menu events
		public void
		OnMenuEditUndo (object o, System.EventArgs args)
		{
			pipeline.Undo ();
		}

		public void
		OnMenuEditRedo (object o, System.EventArgs args)
		{
			pipeline.Redo ();
		}

		public void
		OnMenuEditDelete (object o, System.EventArgs args)
		{
			// FIXME - implement
		}

		// 'View' Menu events
		public void
		OnMenuViewKeepClean (object o, System.EventArgs args)
		{
			// FIXME - implement
		}

		// 'Help' Menu events
		public void
		OnMenuHelpContents (object o, System.EventArgs args)
		{
			// FIXME - implement
		}

		public void
		Load (string filename)
		{
			try {
				pipeline.Load (filename);
			} catch (System.Exception e) {
				// FIXME - pop up an error dialog. if the pipeline is
				// empty, destroy the window.
			}
		}
	}

}
