using System;
using Gtk;
using Glade;
using Gnome;

public class GladeApp
{
        public static void Main (string[] args)
        {
                new GladeApp (args);
        }

        public GladeApp (string[] args) 
        {
                Application.Init();

                Glade.XML gxml = new Glade.XML ("/home/david/projects/navi-misc/fyre/data/pipeline-editor.glade", "window1", null);
                gxml.Autoconnect (this);
                
                Gtk.HPaned paned = (Gtk.HPaned) gxml.GetWidget ("editor hpaned");
                Gnome.Canvas canvas = Gnome.Canvas.NewAa ();
                canvas.Show ();
                paned.Add2 (canvas);
                Application.Run();
        }

        /* Connect the Signals defined in Glade */
        public void OnWindowDeleteEvent (object o, DeleteEventArgs args) 
        {
                Application.Quit ();
                args.RetVal = true;
        }
}

