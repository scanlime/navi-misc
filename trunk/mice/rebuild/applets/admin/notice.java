import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

/**
 * This class handles the action event that involves notices
 * @author Brandon Smith
 * @version 2.0
 */
 
public class notice implements ActionListener
{
	private admin myadmin;

	private TextField text;
	
	private Button submit;

	public notice(admin my)
	{
		myadmin = my;
		text = new TextField("This is for notices",45);
		submit = new Button("Submit Notice");
		myadmin.ad(text);
		myadmin.ad(submit);
		submit.addActionListener(this);
	}
	
	public void actionPerformed(ActionEvent e)
	{
		text.setText("Submit Notice Clicked");
	}
	
	public void destroy()
	{
		myadmin.rm(text);
		myadmin.rm(submit);
	}
}