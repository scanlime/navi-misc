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
	private admin myadmin;

	/**
	 * This method creates a new authentication object.
	 * @param my The admin applet reference for adding and removing stuff to it.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public authenticate(admin my)
	{
		myadmin = my;
		info.username = new TextField("username",30);
		info.password = new TextField("password",30);
		info.submit = new Button("Submit");
		info.submit.addActionListener(this);
		myadmin.ad(info.username);
		myadmin.ad(info.password);
		myadmin.ad(info.submit);
		info.help.setText("Please enter your username and password, then click submit to authenticate yourself to the server.");
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
		myadmin.rm(info.username);
		myadmin.rm(info.password);
		myadmin.rm(info.submit);
	}
}