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
		host = new TextField("Web bot hostname",40);
		port = new TextField("port",6);
		submit = new Button("Submit");
		myadmin = my;
		myadmin.ad(host);
		myadmin.ad(port);
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
		
		temp = net.read();
		if(temp.charAt(0) == '-')
		{
			admin.help.setText("check your host and port, the connection failed");
			return;
		}
		myadmin.rm(submit);
		myadmin.rm(port);
		myadmin.rm(host);
		new aimconfig(myadmin);
	}
}