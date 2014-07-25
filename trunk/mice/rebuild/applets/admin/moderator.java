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
 * This class handles the moderation stuff, in that it sets its information in 
 * the server, by telling it how many moderators, what their usernames/passwords
 * are, and informing the moderator through all of it.
 * @author Brandon Smith
 * @version 2.0
 */

public class moderator implements ActionListener
{
	/** Keep this close to add and remove stuff */
	private admin myadmin;
	
	/** The number of moderators. */
	private int count = 0;
	
	/** The submit button */
	private Button submit;
	
	/** The joyful number of moderators text field */
	private TextField countin;
	
	/** The usernames for the moderators text fields */
	private TextField[] usernames;
	
	/** The passwords for the moderators text fields */
	private TextField[] passwords;
	
	/**
	 * This constructs the moderator object, and gets everything all set up
	 * @param my A reference to the applet for adding and removing purposes
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public moderator(admin my)
	{
		myadmin = my;
		admin.help.setText("Please enter the number of moderators, from 1 to 9");
		countin = new TextField("",4);
		myadmin.ad(countin);
		submit = new Button("Submit");
		submit.addActionListener(this);
		myadmin.ad(submit);
	}
	
	/**
	 * This handles the pressing of the button, such that when it is pressed, the
	 * applet does stuff.
	 * @param e The ActionEvent associated with this action, not used
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		int i;
		if(count == 0)
		{
			count = Integer.parseInt(countin.getText());
			if(count < 1 || count > 9) return;
			myadmin.rm(submit);
			myadmin.rm(countin);
			step1();
		}
		else
		{
			if(step2())
			{
				for(i=0;i<count;i++)
				{
					myadmin.rm(usernames[i]);
					myadmin.rm(passwords[i]);
				}
				myadmin.rm(submit);
				new interview(myadmin);
			}
		}
	}

	/**
	 * This method does the first step, which is to put the TextFields out there
	 * to collect the usernames and passwords from the administrator
	 * @author Brandon Smith
	 * @version 2.0
	 */
	private void step1()
	{
		int i;
		usernames = new TextField[count];
		passwords = new TextField[count];
		admin.help.setText("Please enter the usernames and passwords for each of the moderators.");
		for(i=0;i<count;i++)
		{
			usernames[i] = new TextField(("username " + (i+1)),30);
			passwords[i] = new TextField(("password " + (i+1)),30);
			myadmin.ad(usernames[i]);
			myadmin.ad(passwords[i]);
		}
		myadmin.ad(submit);
	}
	
	/**
	 * This method handles the data collection from the applet, so that it knows
	 * what the usernames and passwords are for the moderators.
	 * @author Brandon Smith
	 * @version 2.0
	 * @return True on successful sending of the data, False if the data is incomplete.
	 */
	private boolean step2()
	{
		int i;
		String temp;
		for(i=0;i<count;i++)
		{
			temp = usernames[i].getText();
			if(temp.startsWith("user")) return false;
			if(temp.compareTo("") == 0) return false;
			temp = passwords[i].getText();
			if(temp.startsWith("pass")) return false;
			if(temp.compareTo("") == 0) return false;
		}
		/* Now I do the net transfer stuff. */
		net.write(""+count);
		temp = net.read();
		for(i=0;i<count;i++)
		{
			net.write(key.encrypt(temp,usernames[i].getText()));
			net.write(key.encrypt(temp,passwords[i].getText()));
		}
		if(net.read().charAt(0) != '+')
			return false;
		return true;
	}
}