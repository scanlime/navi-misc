import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

/**
 * This class is the action listener for the accept buttons.
 * @author Brandon Smith
 * @version 2.0
 */
public class accept implements ActionListener
{
	private int ID;
	
	private TextArea question;
	
	/**
	 * This method sets the ID as its the constructor.
	 * @param ident The ID of the person that this will be approved for
	 * @param quest The TextArea that has the question in it.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public accept(int ident, TextArea quest)
	{
		ID = ident;
		question = quest;
	}
	
	/**
	 * This method handles the click of the button.  It informs the server which
	 * person the question is approved for.
	 * @param e An ununsed event information object
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		net.write("accept");
		net.write("" + ID);
		question.setText("");
	}
}