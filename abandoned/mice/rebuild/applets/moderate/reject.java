/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class sends the reject message to the server
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

public class reject implements ActionListener
{
	private TextArea quest;
	/**
	 * This is a constructor that is empty, cause this class only does actions
	 * @param questio The question Text Area so I can clear it.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public reject(TextArea questio)
	{
		quest = questio;
	}
	
	/**
	 * This method is called with the reject button is clicked, and it sends the
	 * reject signal to the server.
	 * @param e The Action Event that is ignored
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		String name, ques;
		net.write("reject");
		name = net.read();
		ques = net.read();
		quest.setText(name + " Asked: " + ques);
	}
}