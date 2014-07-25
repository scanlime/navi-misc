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
 * This class is the action listener for the quit button.
 * @author Brandon Smith
 * @version 2.0
 */
public class get implements ActionListener
{
	private TextArea question;
	
	/**
	 * This method sets the ID as its the constructor.
	 * @param quest The TextArea that has the question in it.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public get(TextArea quest)
	{
			question = quest;
	}
	
	/**
	 * This method handles the click of the button.  It informs the server which
	 * person the question is approved for.
	 * @param e An ununsed event information object
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		String name, ques;
		net.write("get");
		name = net.read();
		ques = net.read();
		question.setText(name + " Asked: " + ques);
	}
}
