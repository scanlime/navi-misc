import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class interview extends java.applet.Applet implements ActionListener
{
	public TextField username, password, realname;
	public TextArea question, answer;
	public Button button1, button2, button3;

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
		button2 = new Button("Reject");
		button3 = new Button("Quit");
        
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
		add(button2);
		add(button3);
	}
}