/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class sets up the web update configuration.
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

public class webconfig implements ActionListener
{
	private admin myadmin;
	private TextField host;
	private TextField port;
	private TextField filename;
	private Button submit;
	
	/**
	 * This method constructs the webconfig object, which listends and acts
	 * @param my My administrator applet, for adding and removing parts.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public webconfig(admin my)
	{
		admin.help.setText("Please enter the hostname and port that the web updater is running on");
		host = new TextField("localhost",40);
		port = new TextField("8081",6);
		filename = new TextField("/Library/WebServer/Documents/chat/live.html");
		submit = new Button("Submit");
		myadmin = my;
		myadmin.ad(host);
		myadmin.ad(port);
		myadmin.ad(filename);
		myadmin.ad(submit);
		submit.addActionListener(this);
	}
	
	/**
	 * This method handles the click of the button, submitting the webconf info
	 * @param e An unused parameter for ActionEvent information
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		String temp;
		net.write(host.getText());
		net.write(port.getText());
		net.write(filename.getText());
		temp = net.read();
		if(temp.charAt(0) == '-')
		{
			admin.help.setText("You got the following error:\n" + temp);
			return;
		}
		myadmin.rm(submit);
		myadmin.rm(port);
		myadmin.rm(host);
		myadmin.rm(filename);
		new aimconfig(myadmin);
	}
}