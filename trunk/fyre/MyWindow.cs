using System;
using Gtk;

public class MyWindow : Window {
	
	public MyWindow () : base ("MyWindow")
	{
		this.SetDefaultSize (400, 300);
		this.DeleteEvent += new DeleteEventHandler (OnMyWindowDelete);
		this.ShowAll ();
	}
	
	void OnMyWindowDelete (object o, DeleteEventArgs args)
	{
		Application.Quit ();
	}
}