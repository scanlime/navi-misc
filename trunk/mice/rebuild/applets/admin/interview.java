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
 * This class handles all the interview information collection from the admin
 * @author Brandon Smith
 * @version 2.0
 */

public class interview implements ActionListener
{
	/** Keep this close to add and remove stuff */
	private admin myadmin;
	
	/** The number of interviews. */
	private int count = 0;
	
	/** The submit button */
	private Button submit;
	
	/** The joyful number of interviews text field */
	private TextField countin;
	
	/** The usernames for the interviews text fields */
	private TextField[] usernames;
	
	/** The passwords for the interviews text fields */
	private TextField[] passwords;
	
	/** The checkbox group array for them */
	private CheckboxGroup[] cbgs;
	
	/** The Java checkboxes. */
	private Checkbox[] java;
	
	/** The AIM checkboxes. */
	private Checkbox[] aim;
	
	/**
	 * This constructs the interview object, and gets everything all set up
	 * @param my A reference to the applet for adding and removing purposes
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public interview(admin my)
	{
		myadmin = my;
		admin.help.setText("Please enter the number of interviews, from 1 to 9");
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
					myadmin.rm(java[i]);
					myadmin.rm(aim[i]);
				}
				myadmin.rm(submit);
				new webconfig(myadmin);
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
		
		cbgs = new CheckboxGroup[count];
		java = new Checkbox[count];
		aim = new Checkbox[count];
		
		admin.help.setText("Please enter the usernames and passwords for each of the interviews, and select the type of client they will use.  If iChat is selected, the password field will be ignored.");
		for(i=0;i<count;i++)
		{
			usernames[i] = new TextField(("username " + (i+1)),20);
			passwords[i] = new TextField(("password " + (i+1)),20);
			cbgs[i] = new CheckboxGroup();
			java[i] = new Checkbox("Java",cbgs[i],true);
			aim[i] = new Checkbox("iChat",cbgs[i],false);
			
			myadmin.ad(usernames[i]);
			myadmin.ad(passwords[i]);
			myadmin.ad(java[i]);
			myadmin.ad(aim[i]);
		}
		myadmin.ad(submit);
	}
	
	/**
	 * This method handles the data collection from the applet, so that it knows
	 * what the usernames and passwords are for the interviews.
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

		net.write(""+count);
		temp = net.read();
		for(i=0;i<count;i++)
		{
			if(cbgs[i].getSelectedCheckbox().getLabel().charAt(1) == 'C') //aim
				net.write(key.encrypt(temp,"AIM0" + usernames[i].getText()));
			//else if(cbgs[i].getSelectedCheckbox().getLabel().charAt(1) == 'I') //irc
			//	net.write(key.encrypt(temp,"IRC0" + usernames[i].getText()));
			else
				net.write(key.encrypt(temp,usernames[i].getText()));
			net.write(key.encrypt(temp,passwords[i].getText()));
		}
		if(net.read().charAt(0) != '+')
			return false;
		return true;
	}
}