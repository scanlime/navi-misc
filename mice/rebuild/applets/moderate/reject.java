/**
 * This class sends the reject message to the server
 * @author Brandon Smith
 * @version 2.0
 */

import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class reject implements ActionListener
{
	/**
	 * This is a constructor that is empty, cause this class only does actions
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public reject(){}
	
	/**
	 * This method is called with the reject button is clicked, and it sends the
	 * reject signal to the server.
	 * @param e The Action Event that is ignored
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void actionPerformed(ActionEvent e)
	{
		net.write("reject");
	}
}