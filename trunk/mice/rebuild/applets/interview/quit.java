import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class quit implements ActionListener
{
	public interview head;
	
	public quit(interview nope)
	{
		head = nope;
	}
	
	public void actionPerformed(ActionEvent e)
	{
		net.write("quit");
		net.closeConnection();
		head.rmbuttons();
	}
}