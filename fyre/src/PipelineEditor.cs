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
		PluginManager plugin_manager;

		// High-level Widgets
		[Glade.Widget] Gtk.Window		toplevel;
		[Glade.Widget] ElementList		element_list;
		[Glade.Widget] NavigationImage		navigation_image;
		[Glade.Widget] PipelineDrawing		pipeline_drawing;

		// Editor workspace (right)
		[Glade.Widget] Gtk.Frame		pipeline_window;
		[Glade.Widget] Gtk.Scrollbar		drawing_hscroll;
		[Glade.Widget] Gtk.Scrollbar		drawing_vscroll;

		// Status bar
		[Glade.Widget] Gtk.Statusbar		statusbar;

		static Gtk.TargetEntry[]		targets;
		static Gtk.TargetList			target_list;
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
		public static Gtk.TargetList		TargetList
		{
			get {
				if (target_list == null) {
					target_list = new Gtk.TargetList (DragTargets);
				}
				return target_list;
			}
		}

		public static void Main (string[] args)
		{
			new PipelineEditor (args);
		}

		public PipelineEditor (string[] args)
		{
			Gtk.Application.Init();

			Glade.XML.SetCustomHandler (new Glade.XMLCustomWidgetHandler (GladeCustomHandler));

			Glade.XML gxml = new Glade.XML (null, "pipeline-editor.glade", "toplevel", null);
			gxml.Autoconnect (this);

			// Set up our drawing canvas
			SetupDrawingCanvas ();

			// Set up plugins directory
			plugin_manager = new PluginManager (Defines.PLUGINSDIR);
			foreach (System.Type t in plugin_manager.plugin_types)
				element_list.AddType (t);

			// Finally, run the application
			Gtk.Application.Run();
		}

		// Glade custom widget handler
		static Gtk.Widget GladeCustomHandler (Glade.XML xml, string func_name, string name, string str1, string str2, int int1, int int2)
		{
			if (func_name == "CreateElementList")
				return new ElementList (xml.GetWidget ("toplevel"));
			if (func_name == "CreateNavigationImage")
				return new NavigationImage ();
			if (func_name == "CreatePipelineDrawing")
				return new PipelineDrawing ();
			return null;
		}

		void SetupDrawingCanvas ()
		{
			pipeline_drawing.SetScrollAdjustments (drawing_hscroll.Adjustment, drawing_vscroll.Adjustment);

			// Set up drag-and-drop for the frame. This looks better than setting it
			// up for the drawing area, but it doesn't really affect structure
			Gtk.Drag.DestSet (pipeline_window, Gtk.DestDefaults.All, DragTargets, Gdk.DragAction.Copy);
		}

		// Event handlers - most of these come from the glade file
		// Window events
		public void OnDeleteEvent (object o, Gtk.DeleteEventArgs args)
		{
			Gtk.Application.Quit ();
			args.RetVal = true;
		}

		// Shared events - menus/toolbars
		public void OnNew (object o, System.EventArgs args)
		{
		}

		public void OnOpen (object o, System.EventArgs args)
		{
		}

		public void OnSave (object o, System.EventArgs args)
		{
		}

		public void OnCut (object o, System.EventArgs args)
		{
		}

		public void OnCopy (object o, System.EventArgs args)
		{
		}

		public void OnPaste (object o, System.EventArgs args)
		{
		}

		// 'File' Menu events
		public void OnMenuFileSaveAs (object o, System.EventArgs args)
		{
		}

		public void OnMenuFileQuit (object o, System.EventArgs args)
		{
			Gtk.Application.Quit ();
		}

		// 'Edit' Menu events
		public void OnMenuEditDelete (object o, System.EventArgs args)
		{
		}

		// 'View' Menu events
		public void OnMenuViewKeepClean (object o, System.EventArgs args)
		{
		}

		// 'Help' Menu events
		public void OnMenuHelpContents (object o, System.EventArgs args)
		{
		}
	}

}
