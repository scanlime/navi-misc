import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

/**
 * This class handles the interview stuff (as far as setting up the number of
 * interviewees), getting their names and settings.
 * @author Brandon Smith
 * @version 2.0
 */
 
public class interview implements ActionListener
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
		// Kick them across the network to the server */
		return true;
	}
}