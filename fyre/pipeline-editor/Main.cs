/*
 * Main.cs - the main program
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

public class PipelineEditor
{
        public static void Main (string[] args)
        {
                new PipelineEditor (args);
        }

        public PipelineEditor (string[] args) 
        {
                Application.Init();

                Glade.XML gxml = new Glade.XML (null, "pipeline-editor.glade", "window1", null);
                gxml.Autoconnect (this);
                Application.Run();
        }

        /* Event handlers - most of these come from the glade file */
        
        /* Window events */
        public void OnDeleteEvent (object o, DeleteEventArgs args) 
        {
                Application.Quit ();
                args.RetVal = true;
        }

		/* 'File' Menu events */
		public void OnMenuFileNew (object o, EventArgs args)
		{
		}
		
		public void OnMenuFileOpen (object o, EventArgs args)
		{
		}
		
		public void OnMenuFileSave (object o, EventArgs args)
		{
		}
		
		public void OnMenuFileSaveAs (object o, EventArgs args)
		{
		}
		
		public void OnMenuFileQuit (object o, EventArgs args)
		{
				Application.Quit ();
		}
		
		/* 'Edit' Menu events */
		public void OnMenuEditCut (object o, EventArgs args)
		{
		}
		
		public void OnMenuEditCopy (object o, EventArgs args)
		{
		}
		
		public void OnMenuEditPaste (object o, EventArgs args)
		{
		}
		
		public void OnMenuEditDelete (object o, EventArgs args)
		{
		}
		
		public void OnMenuHelpAbout (object o, EventArgs args)
		{
		}
}
