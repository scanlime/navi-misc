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
public class accepted implements ActionListener
{
	private int ID;
	
	public TextArea quest;
	
	/**
	 * This method sets the ID as its the constructor.
	 * @param ident The ID of the person that this will be approved for
	 * @param questio The TextArea that has the question in it.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public accepted(int ident, TextArea questio)
	{
		ID = ident;
		quest = questio;
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
		quest.setText("");
		net.write("accept");
		net.write(""+ID);
	}
}