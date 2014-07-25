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
 * This class handles the click of the submission button
 * @author Brandon Smith
 * @version 2.0
 */
public class submit implements ActionListener
{
	/** This holds the head information, so it can mess with text fields. */
	public interview head;

	/**
	 * This method creates a new submission handler.
	 * @param nope The textfield information thingy.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public submit(interview nope)
	{
		head = nope;
	}
	
	/**
	 * This method handles the click of the submit button
	 * @param e An unused parameter that I have to take...
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		String name, ques, answ;
		StringTokenizer a;
		if(head.answer.getText().compareTo("") == 0) return;
		net.write("submit");
		answ = head.answer.getText();
		a = new StringTokenizer(answ,"\n",false);
		answ = "";
		while(a.hasMoreTokens()) answ = answ + a.nextToken() + "<br>";
		net.write(answ);
		head.answer.setText("");
		name = net.read();
		ques = net.read();
		head.question.setText(name + " Asks: " + ques);
	}
}