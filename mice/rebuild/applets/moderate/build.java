/**
 * This class handles the building of the applet for the rest runtime portion of
 * the runcycle of the program.
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

public class build
{
	/** This is the moderator for the addition and removal of components. */
	public moderate mymod;
	
	/** This is an array of Buttons for the purpose of having multiple approvals*/
	public Button[] approve;
	
	/** This is a Button for rejecting questions. */
	public Button reject;

	/** This is a button for quitting. */
	public Button quit;
	
	/** This button gets a question from the server. */
	public Button get;
	
	/** This TextArea is for the express purpose of displaying the questions. */
	public TextArea question;
	
	public build(moderate my)
	{
		mymod = my;
		int interviews, i;
		interviews = Integer.parseInt(net.read());
		approve = new Button[interviews];
		for(i=0;i<interviews;i++)
		{
			String foobar = net.read();
			approve[i] = new Button("Approve for " + foobar);
			approve[i].addActionListener(new accept(i,question));
		}
		question = new TextArea("",6,60,TextArea.SCROLLBARS_VERTICAL_ONLY);
		get = new Button("Get Question");
		reject = new Button("Reject");
		quit = new Button("Quit");

		mymod.ad(question);
		mymod.ad(get);
		for(i=0;i<interviews;i++)
			mymod.ad(approve[i]);
		mymod.ad(reject);
		mymod.ad(quit);
	}
}