/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class handles authentication stuff for the applet.  It is responsible
 * for setting up and taking down the pieces of itself that it uses to get the
 * Strings (username, password) and authenticate the moderation client.
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
	/** The moderation applet reference for messing with stuff. */
	private moderate mymod;

	/** The username text field */
	public TextField username;
	/** The password text field */
	public TextField password;
	
	/** The button for submission */
	private Button submit;
	
	/**
	 * This method creates a new authentication object.
	 * @param my The mod applet reference for adding and removing stuff to it.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public authenticate(moderate my)
	{
		mymod = my;
		username = new TextField("username",20);
		password = new TextField("password",20);
		submit = new Button("Submit");
		mymod.ad(username);
		mymod.ad(password);
		mymod.ad(submit);
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
		if(!net.openConnection("studentactivities.mscd.edu",8080)) return;
		net.write("jmod");
		mykey = net.read();
		user = key.encrypt(mykey,username.getText());
		pass = key.encrypt(mykey,password.getText());
		net.write(user);
		net.write(pass);
		user = net.read();
		if(user.compareTo("+OK") != 0)
		{
			username.setText("username");
			password.setText("password");
			return;
		}
		mymod.rm(username);
		mymod.rm(password);
		mymod.rm(submit);
		new build(mymod);
	}
}
