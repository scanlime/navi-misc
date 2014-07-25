/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class handles the sending of a quit message to the server when the quit
 * button is clicked.
 * @author Brandon Smith
 * @version 2.0
 */

import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class quit implements ActionListener
{
	private TextArea quest;
	/**
	 * This constructs the quit object handler
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public quit(TextArea questio)
	{
		quest = questio;
	}
	
	/**
	 * This just sends the quit message when called
	 * @param e The unused Action Event
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		net.write("quit");
		net.closeConnection();
		quest.setText("");
	}
}