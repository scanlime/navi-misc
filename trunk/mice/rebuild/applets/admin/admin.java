import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

/**
 * This class is heart of the administration applet
 * @author Brandon Smith
 * @version 2.0
 */
public class admin extends java.applet.Applet
{
	/**
	 * This method initializes the applet and makes it all well and good
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void init()
	{
		//net.openConnection("localhost",8080);
		info.help = new TextArea("This is the information window, it will tell you everything you could want to know about what you need to do, and what stuff is.",6,60,TextArea.SCROLLBARS_VERTICAL_ONLY);
		//authenticate auth = new authenticate();
		add(info.help);
		info.auth = new authenticate(this);
	}
	
	/**
	 * This method removes a TextField from the applet
	 * @param foo The TextField that gets removed.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void rm(TextField foo)
	{
		remove(foo);
	}
	
	/**
	 * This method removes a TextArea from the applet
	 * @param foo The TextArea that gets removed.
	 * @author Brandon Smith
	 * @version 2.0
	 */	
	public void rm(TextArea foo)
	{
		remove(foo);
	}
	
	/**
	 * This method removes a Button from the applet
	 * @param foo The Button that gets removed.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void rm(Button foo)
	{
		remove(foo);
	}
	
	/**
	 * This method adds a TextField to the applet
	 * @param foo The TextField that gets added.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void ad(TextField foo)
	{
		add(foo);
	}
	
	/**
	 * This method adds a TextArea to the applet
	 * @param foo The TextArea that gets added.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void ad(TextArea foo)
	{
		add(foo);
	}
	
	/**
	 * This method adds a Button to the applet
	 * @param foo The Button that gets added.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void ad(Button foo)
	{
		add(foo);
	}
}