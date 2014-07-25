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
 * This class handles the action event that involves notices
 * @author Brandon Smith
 * @version 2.0
 */
 
public class notice implements ActionListener
{
	private admin myadmin;
	private TextField text;
	private Button submit;

	/**
	 * This method constructs the notice form and such.
	 * @param my The applet object so I can add and remove components.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public notice(admin my)
	{
		myadmin = my;
		text = new TextField("This is for notices",45);
		submit = new Button("Submit Notice");
		myadmin.help.setText("Enter the notice, and click submit to send it out.");
		myadmin.ad(text);
		myadmin.ad(submit);
		submit.addActionListener(this);
	}
	
	/**
	 * This method handles the click, and sends the notice to the server.
	 * @param e An unused piece of information
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		if(text.getText().charAt(0) == '*')
		{
			destroy();
			net.write("quit");
			net.closeConnection();
		}
		else
		{
			net.write("notice");
			net.write(text.getText());
			text.setText("Submit Notice Clicked");
		}
	}
	
	/**
	 * This method destroys whats in here
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void destroy()
	{
		myadmin.rm(text);
		myadmin.rm(submit);
	}
}