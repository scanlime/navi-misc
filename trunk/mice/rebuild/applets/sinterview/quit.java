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
 * This class makes the thing quit.
 * @author Brandon Smith
 * @version 2.0
 */
public class quit implements ActionListener
{
	/** This holds the head so I can mess with text fields and stuff. */
	public sint head;
	
	/**
	 * This constructs the quittin stuff.
	 * @param nope The head for removing buttons and 
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public quit(sint nope)
	{
		head = nope;
	}
	
	/**
	 * This method handles quitting.
	 * @param e Its unused... ignore it!
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		net.write("quit");
		net.closeConnection();
	}
}