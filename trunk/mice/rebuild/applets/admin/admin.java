/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

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
public class admin extends java.applet.Applet implements ActionListener
{
	/** The start button */
	public Button start;
	
	/** The hostbox */
	public TextField hostbox;
	
	/** The portbox */
	public TextField portbox;
	
	/** The Help TextArea */
	public static TextArea help;
	
	/** The host to connect to */
	public static String host;
	
	/** The port to connect to on the host */
	public static int port;
	
	/**
	 * This method initializes the applet and makes it all well and good
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void init()
	{
		admin.help = new TextArea("This is the information window, it will tell you everything you could want to know about what you need to do, and what stuff is.  Please press start to continue.",6,60,TextArea.SCROLLBARS_VERTICAL_ONLY);
		add(admin.help);
		
		hostbox = new TextField("studentactivities.mscd.edu",40);
		portbox = new TextField("8080",6);
		add(hostbox);
		add(portbox);
		start = new Button("Start");
		start.addActionListener(this);
		add(start);
	}
	
	/**
	 * This method handles the first click, and keeps things going
	 * @param e The unused parameter
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		start.removeActionListener(this);
		remove(start);
		admin.host = hostbox.getText();
		admin.port = Integer.parseInt(portbox.getText());
		remove(hostbox);
		remove(portbox);
		new authenticate(this);
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
	 * This method removes a Checkbox from the applet
	 * @param foo The Checkbox that gets removed.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void rm(Checkbox foo)
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
	
	/**
	 * This method adds a Checkbox to the applet
	 * @param foo The Checkbox that gets added.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void ad(Checkbox foo)
	{
		add(foo);
	}
}
