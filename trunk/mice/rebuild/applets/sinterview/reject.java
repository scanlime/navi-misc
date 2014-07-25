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
 * This class rejects questions when the reject button is pushed.
 * @author Brandon Smith
 * @version 2.0
 */
public class reject implements ActionListener
{
	/** This points to the applet that has the text fields in it. */
	public sint head;
	
	/**
	 * Mr. Constructor for... Rejected.  My spoon is too big.
	 * @param nope My Text Field is too big (cause this points to the info for it)
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public reject(sint nope)
	{
		head = nope;
	}
	
	/**
	 * Click...
	 * @param e The unused parameter that I never use, but accept anyway.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		String name, ques;
		net.write("reject");
		name = net.read();
		ques = net.read();
		head.question.setText(name + " Asks: " + ques);
	}
}