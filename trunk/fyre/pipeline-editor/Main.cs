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

        /* Connect the Signals defined in Glade */
        public void OnDeleteEvent (object o, DeleteEventArgs args) 
        {
                Application.Quit ();
                args.RetVal = true;
        }
}

