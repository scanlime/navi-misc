/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class handles capturing and sending AIM Bot configuration.
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

public class aimconfig implements ActionListener
{
	private admin myadmin;
	private TextField host;
	private TextField port;
	private Button submit;
	
	/**
	 * This method constructs the AIM configuration portion of the applet
	 * @param my The Applet that has the capacity to add and remove stuff.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public aimconfig(admin my)
	{
		admin.help.setText("Please enter the hostname and port that the web updater is running on");
		host = new TextField("localhost",40);
		port = new TextField("8082",6);
		submit = new Button("Submit");
		myadmin = my;
		myadmin.ad(host);
		myadmin.ad(port);
		myadmin.ad(submit);
		submit.addActionListener(this);
	}
	
	/**
	 * This method handles the clicks and takes care of it
	 * @param e The unused bits of information from the click
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		String temp;
		net.write(host.getText());
		net.write(port.getText());
		
		temp = net.read();
		if(temp.charAt(0) == '-')
		{
			admin.help.setText("check your host and port, the connection failed");
			return;
		}
		
		myadmin.rm(submit);
		myadmin.rm(port);
		myadmin.rm(host);
		new notice(myadmin);
	}
}