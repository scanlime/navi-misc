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
 * This class is the main class for the java interview applet.
 * @author Brandon Smith
 * @version 2.0
 */
public class interview extends java.applet.Applet implements ActionListener
{
	/** Text Fields for the various jobs we have. */
	public TextField username, password, realname;
	/** Text Areas for the brunt of the work. */
	public TextArea question, answer;
	/** Buttons... I love buttons. */
	public Button button1, button2, button3, button4;

	/**
	 * This initializes the applet
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void init()
	{
		//set up the various fields from the information class
		realname = new TextField("Please put your name as you want it to appear next to your answers.",50);
		username = new TextField("username",20);
		password = new TextField("password",20);
		question = new TextArea("",6,50,TextArea.SCROLLBARS_VERTICAL_ONLY);
		answer = new TextArea("",10,50,TextArea.SCROLLBARS_VERTICAL_ONLY);
		button1 = new Button("Sign In");
		button2 = new Button("Get Question");
		button3 = new Button("Reject");
		button4 = new Button("Quit");
        
		//add the various objects to the applet
		add(realname);
		add(username);
		add(password);
		add(button1);
		button1.addActionListener(this);
	}
	
	/**
	 * This is the action handler for the initialization of the applet
	 * @param e The Action Event variable... its not used.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		String mykey;
		net.openConnection("studentactivities.mscd.edu",8080);
		net.write("jint");
		mykey = net.read();
		net.write(realname.getText());
		net.write(key.encrypt(mykey,username.getText()));
		net.write(key.encrypt(mykey,password.getText()));
		mykey = net.read();
		if(mykey.charAt(0) == '-')
		{
			realname.setText("Try again, authentication failed");
			return;
		}
		buttonconfig();
	}
	
	/**
	 * This sets up the buttons to be in their final form
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void buttonconfig()
	{
		remove(realname);
		remove(button1);
		remove(username);
		remove(password);
		button1.removeActionListener(this);
		button1.setLabel("Answer");
		add(question);
		add(answer);
		add(button1);
		button1.addActionListener(new submit(this));
		add(button2);
		button2.addActionListener(new get(this));
		add(button3);
		button3.addActionListener(new reject(this));
		add(button4);
		button4.addActionListener(new quit(this));
	}
	
	/**
	 * This removes all the stuff from the applet
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void rmbuttons()
	{
		remove(question);
		remove(answer);
		remove(button1);
		remove(button2);
		remove(button3);
		remove(button4);
	}
}
