/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

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
	public Button rejectb;

	/** This is a button for quitting. */
	public Button quitb;
	
	/** This button gets a question from the server. */
	public Button getb;
	
	/** This TextArea is for the express purpose of displaying the questions. */
	public TextArea question;
	
	/**
	 * This builds the applet such that it has all the important stuff there.
	 * @param my The moderator that takes care of stuff
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public build(moderate my)
	{
		mymod = my;
		int interviews, i;
		interviews = Integer.parseInt(net.read());
		approve = new Button[interviews];
		question = new TextArea("",6,60,TextArea.SCROLLBARS_VERTICAL_ONLY);
		for(i=0;i<interviews;i++)
		{
			String foobar = net.read();
			approve[i] = new Button("Approve for " + foobar);
			approve[i].addActionListener(new accepted(i,question));
		}
		getb = new Button("Get Question");
		getb.addActionListener(new get(question));
		rejectb = new Button("Reject");
		rejectb.addActionListener(new reject(question));
		quitb = new Button("Quit");
		quitb.addActionListener(new quit(question));

		mymod.ad(question);
		mymod.ad(getb);
		for(i=0;i<interviews;i++)
			mymod.ad(approve[i]);
		mymod.ad(rejectb);
		mymod.ad(quitb);
	}
}