/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class handles authentication stuff for the applet.  It is responsible
 * for setting up and taking down the pieces of itself that it uses to get the
 * Strings (username, password) and authenticate the administration client.
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
 
public class authenticate implements ActionListener
{
	/** The admin applet reference for messing with stuff. */
	private admin myadmin;

	/** The username text field */
	private TextField username;
	/** The password text field */
	private TextField password;
	
	/** The button for submission */
	private Button submit;
	
	/**
	 * This method creates a new authentication object.
	 * @param my The admin applet reference for adding and removing stuff to it.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public authenticate(admin my)
	{
		myadmin = my;
		username = new TextField("username",30);
		password = new TextField("password",30);
		submit = new Button("Submit");
		admin.help.setText("Please enter your username and password, then click submit to authenticate yourself to the server.");
		myadmin.ad(username);
		myadmin.ad(password);
		myadmin.ad(submit);
		submit.addActionListener(this);
	}
	
	/**
	 * This method is what handles the press of the button to submit info to the
	 * server.
	 * @param e The ActionEvent object associated with this event
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		String mykey,user,pass;
		if(!net.openConnection(admin.host,admin.port))
		{
			admin.help.setText("Failed to open connection, try again.");
			return;
		}
		net.write("admin");
		mykey = net.read();
		user = key.encrypt(mykey,username.getText());
		pass = key.encrypt(mykey,password.getText());
		net.write(user);
		net.write(pass);
		user = net.read();
		if(user.compareTo("+OK") != 0)
		{
			admin.help.setText("Failed Authentication, perhaps you should try again");
			username.setText("username");
			password.setText("password");
			return;
		}
		myadmin.rm(username);
		myadmin.rm(password);
		myadmin.rm(submit);
		/* This is where I find out about where I should go. */
		switch(Integer.parseInt(net.read()))
		{
		case 0:
			new moderator(myadmin);
			break;
		case 1:
			new interview(myadmin);
			break;
		case 2:
			new webconfig(myadmin);
			break;
		case 3:
			new aimconfig(myadmin);
			break;
		case 4:
			new notice(myadmin);
			break;
		}
	}
}