import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class interview extends java.applet.Applet implements ActionListener
{
	TextField username, password;
	TextArea question, answer;
	Button button1, button2, button3;

	public void init()
	{
		//set up the various fields from the information class
		username = new TextField("username",20);
		password = new TextField("password",20);
		question = new TextArea("",6,50,TextArea.SCROLLBARS_VERTICAL_ONLY);
		answer = new TextArea("",10,50,TextArea.SCROLLBARS_VERTICAL_ONLY);
		button1 = new Button("Sign In");
		button2 = new Button("Reject");
		button3 = new Button("Quit");
        
		//add the various objects to the applet
		add(username);
		add(password);
		add(button1);
		button1.addActionListener(this);
	}
	
	public void actionPerformed(ActionEvent e)
	{
		remove(button1);
		remove(button2);
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