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
	 */
	public moderator(admin my)
	{
		myadmin = my;
		admin.help.setText("Please enter the number of moderators.");
		countin = new TextField("",4);
		myadmin.ad(countin);
		submit = new Button("Submit");
		submit.addActionListener(this);
		myadmin.ad(submit);
	}
	
	public void actionPerformed(ActionEvent e)
	{
		if(count == 0)
		{
			count = Integer.parseInt(countin.getText());
			myadmin.rm(submit);
			myadmin.rm(countin);
			step1();
		}
		
	}
	
	private void step1()
	{
		int i;
		usernames = new TextField[count];
		passwords = new TextField[count];
		for(i=0;i<count;i++)
		{
			usernames[i] = new TextField(("username " + (i+1)),30);
			passwords[i] = new TextField(("password " + (i+1)),30);
			myadmin.ad(usernames[i]);
			myadmin.ad(passwords[i]);
		}
		myadmin.ad(submit);
	}
	
	private void step2()
	{
	}
}