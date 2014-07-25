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
 * This class is the simplest java applet possible that can shoot a question
 * to the server.
 * @author Brandon Smith
 * @version 2.0
 */
public class question extends java.applet.Applet implements ActionListener
{
	/** The name field */
	public TextField name;
	/** The position field */
	public TextField position;
	/** The Text Area that the question gets scribbled into */
	public TextArea ques;
	/** The button that does the actual submission */
	public Button submit;
    
	/** the String that becomes the name string and position string */
	public String nstring;
	/** the String that holds the question as it is sent */
	public String qstring;
    
	//network information
	public Socket link;
	public BufferedReader in;
	public OutputStreamWriter out;
    
	/**
	 * This method initializes the applet and makes it all well and good
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void init()
	{
		//create the fields and buttons
		name = new TextField("Name",30);
		position = new TextField("Standing/Major",30);
		ques = new TextArea("Question",4,30,TextArea.SCROLLBARS_VERTICAL_ONLY);
		submit = new Button("Submit");
		
		//register this as the action listener
		submit.addActionListener(this);
        
		//put stuff in the applet.
		add(name);
		add(position);
		add(ques);
		add(submit);
	}

	/**
	 * This method handles the click of the submit, and renders the applet useless
	 * @param e The action event that is effectively ignored
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		nstring = name.getText() + " (" + position.getText() + ")";
		qstring = ques.getText();
		
		StringTokenizer tokens = new StringTokenizer(qstring,"\n");

		if(qstring.compareTo("") == 0) return;

		name.setText("Question submitted");
		ques.setText("");
		position.setText("");

		if(!openConnection("studentactivities.mscd.edu",8080))
		{
			name.setText("connection failure");
			submit.removeActionListener(this);
			return;
		}
		write("jsub");
		write(nstring);
		qstring = tokens.nextToken();
		while(tokens.hasMoreTokens())
			qstring = qstring + " " + tokens.nextToken();
		write(qstring);
		closeConnection();
		submit.removeActionListener(this);
	}
    
	/* These four methods handle network communication */

	/**
	 * This method opens a connection and takes care of all the pre/post conditions for read and write.
	 * @param host The host to connect to.
	 * @param port The port on the host to connect to.
	 * @author Brandon Smith
	 * @version 2.0
	 * @return True on a successful connection, False
	 */
	public boolean openConnection(String host, int port)
	{
		//Initialize everything
		try
		{
	    link = new Socket(host,port);
	    in = new BufferedReader(new InputStreamReader(link.getInputStream()));
	    out = new OutputStreamWriter(link.getOutputStream());
		}
		catch(IOException exception) {
			return false;
		}
		return true;
	}
    
	/**
	 * This method closes the connection
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void closeConnection()
	{
		try 
		{
	    link.close();
		}
		catch(IOException exception){}
	}
    
	/**
	 * This method reads a string from the socket and returns it
	 * @author Brandon Smith
	 * @version 2.0
	 * @return A String with the network newline
	 */
	public String read()
	{
		String toreturn = null;
		try
		{
			toreturn = in.readLine();
		}
		catch(IOException exception)
		{
			System.out.println("Error in recieving: " + exception);
	    closeConnection();
	    System.exit(1);
		}
		return toreturn;
	}
    
	/**
	 * This method writes a string to the predescribed socket.
	 * @param tosend The string to send
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void write(String tosend)
	{
		try
		{
	    out.write(tosend + "\r\n");
	    out.flush();
		}
		catch(IOException exception)
		{
	    System.out.println("Error in sending:" + exception);
	    closeConnection();
	    System.exit(1);
		}
	}
}
