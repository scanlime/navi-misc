/*
 * PipelineEditor.cs - pipeline editor. At this moment, this also
 *	stands in as the main program
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

	public class PipelineEditor
	{
		// Plugin stuff
		PluginManager				plugin_manager;

		// Document
		Pipeline				pipeline;

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

		// Static copy, for singleton-like behavior
		public static PipelineEditor		instance;

		// D-n-D target data
		static Gtk.TargetEntry[]		targets;
		public static Gtk.TargetEntry[]		DragTargets
		{
			get {
				if (targets == null) {
					targets = new Gtk.TargetEntry[1];
					targets[0] = new Gtk.TargetEntry ("fyre element drag", Gtk.TargetFlags.App, 0);
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
			new PipelineEditor (args);
		}

		private
		PipelineEditor (string[] args)
		{
			instance = this;

			Gtk.Application.Init();

			Glade.XML.SetCustomHandler (new Glade.XMLCustomWidgetHandler (GladeCustomHandler));

			Glade.XML gxml = new Glade.XML (null, "pipeline-editor.glade", "toplevel", null);
			gxml.Autoconnect (this);

			pipeline = new Pipeline ();

			// Set up our drawing canvas
			SetupDrawingCanvas ();

			// Set up plugins directory
			plugin_manager = new PluginManager (Defines.PLUGINSDIR);
			foreach (System.Type t in plugin_manager.plugin_types)
				element_list.AddType (t);

			// Set the window title
			SetTitle ();

			// Show the window
			toplevel.Show ();

			// Finally, run the application
			Gtk.Application.Run();
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
			pipeline_drawing.SetScrollAdjustments (drawing_hscroll.Adjustment, drawing_vscroll.Adjustment);

			// Give the navigation image knowledge of the drawing, so it can
			// track canvas/window extents
			navigation_image.drawing = pipeline_drawing;

			// Set up drag-and-drop for the frame. This looks better than setting it
			// up for the drawing area, and we delegate the event back into the
			// PipelineDrawing object when we drop.
			Gtk.Drag.DestSet (pipeline_frame, Gtk.DestDefaults.All, DragTargets, Gdk.DragAction.Copy);
			pipeline_frame.DragDataReceived += new Gtk.DragDataReceivedHandler (DrawingReceivedDrag);
		}

		void
		DrawingReceivedDrag (object o, Gtk.DragDataReceivedArgs args)
		{
			System.Console.WriteLine ("drop: {0}", args.SelectionData.Text);
		}

		public Gtk.Window
		GetWindow ()
		{
			return toplevel;
		}

		void
		SetTitle ()
		{
			string filename;
			if (pipeline.filename == null)
				filename = "Untitled";
			else
				filename = pipeline.filename;

			if (pipeline.saved)
				toplevel.Title = filename;
			else
				toplevel.Title = filename + "*";
		}

		// Event handlers - most of these come from the glade file
		// Window events
		public void
		OnDeleteEvent (object o, Gtk.DeleteEventArgs args)
		{
			ErrorDialog e = new ErrorDialog ("Whoops!", "Someone left a stinker on the lawn");
			e.Run ();

			if (true) { //pipeline.saved == false) {
				// FIXME - ask the user for confirmation
				ConfirmCloseDialog confirm = new ConfirmCloseDialog("Save pipeline before closing?",
						"There are unsaved changes to the pipeline. Save before quitting?");
				confirm.Run();
				confirm.Destroy();
			}

			Gtk.Application.Quit ();
			args.RetVal = true;
		}

		// Shared events - menus/toolbars
		public void
		OnNew (object o, System.EventArgs args)
		{
			// FIXME - implement
		}

		public void
		OnOpen (object o, System.EventArgs args)
		{
			// FIXME - implement
		}

		public void
		OnSave (object o, System.EventArgs args)
		{
			// FIXME - implement
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
			// FIXME - implement
		}

		public void
		OnMenuFileQuit (object o, System.EventArgs args)
		{
			if (pipeline.saved == false) {
				// FIXME - ask the user for confirmation
			}
			Gtk.Application.Quit ();
		}

		// 'Edit' Menu events
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
	}

}
