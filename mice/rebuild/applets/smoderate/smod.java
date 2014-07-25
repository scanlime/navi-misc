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

public class smod extends java.applet.Applet implements ActionListener
{
	/** This is for authentication purposes (username) */
	public TextField uname;
	/** This is for authentication purposes (password) */
	public TextField pword;
	/** This is for authentication purposes (clickski) */
	public Button auth;
	
	/** This is for the question as it appears. */
	public TextArea question;
	
	/** This is the get question button. */
	public Button bget;
	/** This is the reject question button. */
	public Button breject;
	/** An accept button. */
	public Button accept;
	/** Who is getting it accepted. */
	public Checkbox[] acptfor;
	
	public void init()
	{
		int i;
		uname = new TextField("username",20);
		pword = new TextField("password",20);
		pword.setEchoChar('*');
		auth = new Button("Log In");
		auth.addActionListener(this);
		
		add(uname);
		add(pword);
		add(auth);
		
		question = new TextArea("",6,60,TextArea.SCROLLBARS_VERTICAL_ONLY);
		add(question);
	
		bget = new Button("Get Question");
		breject = new Button("Reject");
		add(bget);
		add(breject);
		
		accept = new Button("Accept for checked");
		add(accept);
		
		acptfor = new Checkbox[10];
		
		for(i=0;i<10;i++)
		{
			acptfor[i] = new Checkbox("00000000000000000000000000000000000");
			add(acptfor[i]);
		}

	}
	
	public void actionPerformed(ActionEvent e)
	{
		int i, j;
		String mykey;
		/* Authenticate the client. */
		if(!net.openConnection("studentactivities.mscd.edu",8080)) return;
		net.write("jmod");
		mykey = net.read();
		net.write(key.encrypt(mykey,uname.getText()));
		net.write(key.encrypt(mykey,pword.getText()));
		mykey = net.read();
		if(mykey.compareTo("+OK") != 0) return;
		/* Configure the client. */
		auth.setLabel("quit");
		auth.removeActionListener(this);
		auth.addActionListener(new quit(question));
		bget.addActionListener(new get(question));
		breject.addActionListener(new reject(question));
		accept.addActionListener(new accepted(question,acptfor));
		j=Integer.parseInt(net.read());
		for(i=0;i<j;i++)
		{
			acptfor[i].setLabel(net.read());
		}
		for(;i<10;i++)
		{
			remove(acptfor[i]);
			acptfor[i] = null;
		}
	}
}
