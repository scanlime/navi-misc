import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

/**
 * This class is heart of the administration applet
 * @author Brandon Smith
 * @version 2.0
 */
public class admin extends java.applet.Applet
{
	/**
	 * This method initializes the applet and makes it all well and good
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void init()
	{
		//net.openConnection("localhost",8080);
		info.help = new TextArea("This is the information window, it will tell you everything you could want to know about what you need to do, and what stuff is.",6,70,TextArea.SCROLLBARS_VERTICAL_ONLY);
		//authenticate auth = new authenticate();
		add(info.help);
	}
}