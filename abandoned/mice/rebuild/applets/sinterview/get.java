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
 * This class handles getting questions, and setting the stuff up with that.
 * @author Brandon Smith
 * @version 2.0
 */
public class get implements ActionListener
{
	/** The applet that gets messed with by this listener. */
	public sint head;
	
	/**
	 * This method sets stuff up for it to get questions.
	 * @param nope The applet that this listens for and messes with.
	 * @author Brandon smith
	 * @version 2.0
	 */
	public get(sint nope)
	{
		head = nope;
	}
	
	/**
	 * This means the 'get' button got clicked, so I get a question.
	 * @param e only one button talks to me, so this is all I need.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		String name, ques;
		net.write("get");
		name = net.read();
		ques = net.read();
		head.question.setText(name + " Asks: " + ques);
	}
}