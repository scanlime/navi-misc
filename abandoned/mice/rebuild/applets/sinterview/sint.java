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
 * This class is the main class for the simple java interview applet.
 * @author Brandon Smith
 * @version 2.0
 */
public class sint extends java.applet.Applet implements ActionListener
{
	/** This is for the real name */
	public TextField rname;
	/** This is for the username */
	public TextField uname;
	/** This is for the password */
	public TextField pword;
	/** This is for authenticating, and later, for quitting. */
	public Button auth;
	
	/** Text Area for the question. */
	public TextArea question;
	/** Text Area for the answer. */
	public TextArea answer;
	/** get a question button */
	public Button bget;
	/** submit an answer button */
	public Button bsubmit;
	/** reject a question button */
	public Button breject;
	
	/**
	 * This method initializes the applet, so that it is ready to do stuff.
	 * @author Brandon Smith
	 * @version 2.1
	 */
	public void init()
	{
		rname = new TextField("Name for output purposes.",49);
		uname = new TextField("username",19);
		pword = new TextField("password",19);
		pword.setEchoChar('*');
		
		question = new TextArea("",6,50,TextArea.SCROLLBARS_VERTICAL_ONLY);
		answer = new TextArea("",10,50,TextArea.SCROLLBARS_VERTICAL_ONLY);

		auth = new Button("Sign In");
		auth.addActionListener(this);
		
		bsubmit = new Button("Answer");
		bget = new Button("Get Question");
		breject = new Button("Reject");
		
		add(uname);
		add(pword);
		add(auth);
		add(rname);
		
		add(question);
		add(answer);
		add(bget);
		add(bsubmit);
		add(breject);
	}

	/**
	 * This method handles the first button click, initializing the connection.
	 * @param e This is an unused event description object.
	 * @author Brandon Smith
	 * @version 2.1
	 */
	public void actionPerformed(ActionEvent e)
	{
		String mykey;
		net.openConnection("studentactivities.mscd.edu",8080);
		net.write("jint");
		mykey = net.read();
		net.write(rname.getText());
		net.write(key.encrypt(mykey,uname.getText()));
		net.write(key.encrypt(mykey,pword.getText()));
		mykey = net.read();
		if(mykey.charAt(0) == '-')
		{
			rname.setText("Try again, authentication failed");
			return;
		}
		auth.removeActionListener(this);
		bsubmit.addActionListener(new submit(this));
		bget.addActionListener(new get(this));
		breject.addActionListener(new reject(this));
		auth.addActionListener(new quit(this));
		auth.setLabel("Quit");
	}
}
