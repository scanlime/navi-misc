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
 * This class is heart of the moderation applet
 * @author Brandon Smith
 * @version 2.0
 */
public class moderate extends java.applet.Applet implements ActionListener
{
	/** The start button */
	public Button start;
	
	/**
	 * This method initializes the applet and makes it all well and good
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void init()
	{
		start = new Button("Start");
		start.addActionListener(this);
		add(start);
	}
	
	/**
	 * This is the first click of the program.  It gets everything started.
	 * @param e The AciontEvent associated with the click... ignored!
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		start.removeActionListener(this);
		remove(start);
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