import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
 
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class interview extends java.applet.Applet implements ActionListener
{
	TextArea question, answer;
	Button button1, button2;

	public void init()
	{
		//set up the various fields from the information class
		question = new TextArea("",6,50,TextArea.SCROLLBARS_VERTICAL_ONLY);
		answer = new TextArea("",10,50,TextArea.SCROLLBARS_VERTICAL_ONLY);
		button1 = new Button("one");
		button2 = new Button("two");
        
		//add the various objects to the applet
		add(question);
		add(answer);
		add(button1);
		add(button2);
		button1.addActionListener(this);
	}
	
	public void actionPerformed(ActionEvent e)
	{
		remove(button1);
		remove(button2);
		button1.setLabel("Door Knob");
		add(button1);
	}
}