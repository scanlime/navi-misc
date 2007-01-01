/*
 * PipelineEditor.cs - pipeline editor. At this moment, this also
 *	stands in as the main program
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

using System.Collections;

namespace Fyre.Editor
{

	public class PipelineEditor
	{
		// Plugin stuff
		static IDictionaryEnumerator		plugin_enumerator;

		// Document
		Document				document;

		// Command manager
		CommandManager				command_manager;

		// High-level Widgets
		[Glade.Widget] Gtk.Window		toplevel;
		[Glade.Widget] Widgets.ElementList	element_list;
		[Glade.Widget] Widgets.NavigationImage	navigation_image;
		[Glade.Widget] Widgets.PipelineDrawing	pipeline_drawing;

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
		[Glade.Widget] Gtk.ImageMenuItem	menu_cut;
		[Glade.Widget] Gtk.ImageMenuItem	menu_copy;
		[Glade.Widget] Gtk.ImageMenuItem	menu_paste;
		[Glade.Widget] Gtk.ImageMenuItem	menu_delete;

		// Static counter for keeping track of the open windows
		static ArrayList			editors;

		// D-n-D target data
		Gtk.TargetEntry[]			targets;
		public Gtk.TargetEntry[]		DragTargets
		{
			get {
				if (targets == null) {
					targets = new Gtk.TargetEntry[1];
					targets[0] = new Gtk.TargetEntry (System.String.Format ("fyre element drag {0}", document.Number), Gtk.TargetFlags.App, 1);
				}
				return targets;
			}
		}

		Gtk.TargetList				target_list;
		public Gtk.TargetList			TargetList
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

			Glade.XML.CustomHandler = new Glade.XMLCustomWidgetHandler (GladeCustomHandler);

			Glade.XML gxml = new Glade.XML (null, "pipeline-editor.glade", "toplevel", null);
			gxml.Autoconnect (this);

			// Make sure the element list knows which window it can accept events from.
			element_list.window = this;

			// Create the document
			document = new Document ();

			// Hook up handlers to parts of the document so we can update our GUI appropriately
			document.Pipeline.Changed += new System.EventHandler (PipelineChanged);
			document.Layout.Selected  += new System.EventHandler (ElementSelected);
			document.FilenameChanged  += new System.EventHandler (TitleDataChanged);
			document.DocumentSaved    += new System.EventHandler (TitleDataChanged);

			// Give child widgets a handle to the document
			pipeline_drawing.Document = document;
			navigation_image.Document = document;

			// Set up our drawing canvas
			SetupDrawingCanvas ();

			// Iterate through each element in the elements list
			plugin_enumerator.Reset ();
			while (plugin_enumerator.MoveNext ())
				element_list.AddType ((System.Type) plugin_enumerator.Value);

			// Create the command manager for this editor, and add in the pipeline, layout, and
			// drawing into it
			command_manager = new CommandManager (pipeline_drawing, document);

			// Distribute the manager out to the pipeline drawing, as it uses it
			pipeline_drawing.command_manager = command_manager;

			// Set our title, etc.
			SetTitle ();
			PipelineChanged (null, null);

			// Show the window
			toplevel.Show ();

			// Give CanvasElement knowledge of our colors
			CanvasElement.SetColors (toplevel.Style);
		}

		void
		PipelineChanged (object o, System.EventArgs args)
		{
			UpdateToolbarSensitivity ();
			UpdateUndoRedo ();
		}

		void
		ElementSelected (object o, System.EventArgs args)
		{
			if (document.Layout.HasSelection ()) {
				menu_cut.Sensitive = true;
				menu_copy.Sensitive = true;
				menu_delete.Sensitive = true;
			} else {
				menu_cut.Sensitive = false;
				menu_copy.Sensitive = false;
				menu_delete.Sensitive = false;
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
				return new Widgets.ElementList (xml.GetWidget ("toplevel"));
			if (func_name == "CreateNavigationImage")
				return new Widgets.NavigationImage ();
			if (func_name == "CreatePipelineDrawing")
				return new Widgets.PipelineDrawing (xml);
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
			string		name = System.Text.Encoding.UTF8.GetString (args.SelectionData.Data);

			ElementFactory factory = ElementFactory.Instance;
			Element e = factory.Create (name);

			// FIXME - we should find out the frame's child offset rather than
			// hardcoding +2x2 here

			// FIXME - This is definitely not the most elegant way to do this, but it does
			// give the command the information it needs to undo an operation.
			Commands.Add adde = new Commands.Add (e, args.X - 2, args.Y -2);

			command_manager.Do (adde);
		}

		public Gtk.Window
		GetWindow ()
		{
			return toplevel;
		}

		void
		TitleDataChanged (object o, System.EventArgs e)
		{
			SetTitle ();
		}

		void
		SetTitle ()
		{
			string filename = System.IO.Path.GetFileName (document.Filename);
			if (document.Saved)
				toplevel.Title = filename;
			else
				toplevel.Title = "*" + filename;
		}

		void
		UpdateToolbarSensitivity ()
		{
			toolbar_save.Sensitive = !document.Saved;
			menu_save.Sensitive    = !document.Saved;
		}

		void
		UpdateUndoRedo ()
		{
			Gtk.Label undo_label = (Gtk.Label) menu_undo.Child;
			Gtk.Label redo_label = (Gtk.Label) menu_redo.Child;


			if (command_manager.undo_stack.Count == 0) {
				menu_undo.Sensitive = false;
				undo_label.TextWithMnemonic = "_Undo";
			} else {
				menu_undo.Sensitive = true;
				Command command = (Command) command_manager.undo_stack[command_manager.undo_stack.Count - 1];
				undo_label.TextWithMnemonic = "_Undo \"" + command.Name + "\"";
			}

			if (command_manager.redo_stack.Count == 0) {
				menu_redo.Sensitive = false;
				redo_label.TextWithMnemonic = "_Redo";
			} else {
				menu_redo.Sensitive = true;
				Command command = (Command) command_manager.redo_stack[command_manager.redo_stack.Count - 1];
				redo_label.TextWithMnemonic = "_Redo \"" + command.Name + "\"";
			}
		}

		public bool
		CloseWindow ()
		{
			if (document.Saved == false) {
				string filename = System.IO.Path.GetFileName (document.Filename);
				ConfirmCloseDialog confirm = new ConfirmCloseDialog (toplevel,
						System.String.Format ("Save changes to \"{0}\" before closing?", filename),
						"There are unsaved changes to the pipeline. Save before quitting?");

				int response = confirm.Run ();
				confirm.Destroy ();

				if (response == (int) Gtk.ResponseType.Cancel)
					return false;
				if (response == (int) Gtk.ResponseType.Yes)
					OnSave (null, null);
			}
			// Hide the window from view.  It'll later be destroyed by the DeleteEvent handler, or by the
			// quit menu option handler.
			toplevel.Hide();

			// The window was closed, so return false indicating it needs to be destroyed.
			return true;
		}

		// Event handlers - most of these come from the glade file
		// Window events
		public void
		OnDeleteEvent (object o, Gtk.DeleteEventArgs args)
		{
			bool close = CloseWindow();
			if (close) {
				editors.Remove (this);
				if (editors.Count == 0)
					Gtk.Application.Quit ();
				args.RetVal = false;
			} else {
				args.RetVal = true;
			}
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
				Gtk.Stock.Cancel, Gtk.ResponseType.Cancel,
				Gtk.Stock.Open,   Gtk.ResponseType.Accept,
			};
			Gtk.FileChooserDialog fs = new Gtk.FileChooserDialog ("Open...", null, Gtk.FileChooserAction.Open, responses);
			fs.DefaultResponse = Gtk.ResponseType.Accept;

			Gtk.ResponseType response = (Gtk.ResponseType) fs.Run ();

			if (response == Gtk.ResponseType.Accept) {
				string filename = fs.Filename;
				if (document.Pipeline.Empty) {
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
			if (!document.HasRealFilename) {
				object[] responses = {
					Gtk.Stock.Cancel, Gtk.ResponseType.Cancel,
					Gtk.Stock.Save,   Gtk.ResponseType.Accept,
				};
				Gtk.FileChooserDialog fs = new Gtk.FileChooserDialog ("Save As...", null, Gtk.FileChooserAction.Save, responses);
				fs.DefaultResponse = Gtk.ResponseType.Accept;
				fs.CurrentName = document.Filename;

				Gtk.ResponseType response = (Gtk.ResponseType) fs.Run ();

				if (response == Gtk.ResponseType.Accept) {
					string filename = fs.Filename;
					document.Save (filename);
					UpdateToolbarSensitivity ();
				}
				fs.Destroy ();
			} else {
				document.Save (document.Filename);
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
				Gtk.Stock.Cancel, Gtk.ResponseType.Cancel,
				Gtk.Stock.Save,   Gtk.ResponseType.Accept
			};
			Gtk.FileChooserDialog fs = new Gtk.FileChooserDialog ("Save As...", null, Gtk.FileChooserAction.Save, responses);
			fs.DefaultResponse = Gtk.ResponseType.Accept;
			if (!fs.SetFilename (document.Filename))
				fs.CurrentName = document.Filename;

			Gtk.ResponseType response = (Gtk.ResponseType) fs.Run ();
			fs.Hide ();

			if (response == Gtk.ResponseType.Accept) {
				string filename = fs.Filename;
				document.Save (filename);
				UpdateToolbarSensitivity ();
			}
			fs.Destroy ();
		}

		public void
		OnMenuFileClose (object o, System.EventArgs args)
		{
			bool close = CloseWindow();
			if (close) {
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
				bool close = win.CloseWindow ();
				if (close)
					removeList.Add (win);
				else
					break;
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
			command_manager.Undo ();
		}

		public void
		OnMenuEditRedo (object o, System.EventArgs args)
		{
			command_manager.Redo ();
		}

		public void
		OnMenuEditDelete (object o, System.EventArgs args)
		{
			// Figure out which element is selected
			System.Guid id = document.Layout.GetSelectedElement ();
			Element e = (Element) document.Pipeline.element_store[id.ToString ()];
			CanvasElement ce = document.Layout.Get (e);

			// Grab the X and Y coordinates of it
			int x = ce.Position.X;
			int y = ce.Position.Y;

			// Pass the element name to the new command, and execute it
			Commands.Delete deletee = new Commands.Delete (e, x, y);

			command_manager.Do (deletee);
			document.Layout.DeselectAll ();
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
				document.Load (filename);
			} catch (System.Exception e) {
				// Pop up an error dialog.
				// FIXME - it would be nice to automatically close this window if it was
				// created from OnOpen, but it's not really simple to do that right now.
				string file = System.IO.Path.GetFileName (filename);
				ErrorDialog ed = new ErrorDialog (toplevel,
						System.String.Format ("Error loading \"{0}\"", file),
						"Fyre was unable to load the pipeline. Either this is not a Fyre\npipeline file or it is corrupted.");
				ed.Run ();
				ed.Destroy ();
			}
		}
	}

}
