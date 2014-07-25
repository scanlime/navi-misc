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
 * This class is the action listener for the accept buttons.
 * @author Brandon Smith
 * @version 2.0
 */
public class accepted implements ActionListener
{
	/** The text area that the question goes into. */
	public TextArea quest;
	
	/** The group that needs to know what was up*/
	public Checkbox[] group;
	
	/**
	 * This method sets the ID as its the constructor.
	 * @param questio The TextArea that has the question in it.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public accepted(TextArea questio, Checkbox[] switches)
	{
		quest = questio;
		group = switches;
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
		quest.setText("");
		int i, trues = 0;
		for(i=0;i<group.length;i++)
		{
			if(group[i] != null)
				if(group[i].getState())
					trues++;
		}
		net.write("accept");
		/*
		 This is where code will go to see who all its good for. 
		 The first number is the number of people who get the question.  It is also
		 the number of reads the server will perform to get that many approved ints.
		*/
		net.write(""+trues);
		for(i=0;i<group.length;i++)
		{ 
			if(group[i] != null)
				if(group[i].getState())
					net.write(""+i);
		}
		name = net.read();
		ques = net.read();
		quest.setText(name + " Asked: " + ques);
	}
}