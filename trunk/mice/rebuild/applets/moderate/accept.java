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
	
	public accept(int ident)
	{
		ID = ident;
	}
	
	public void actionPerformed(ActionEvent e)
	{
		net.write("accept");
		net.write("" + ID);
	}
}