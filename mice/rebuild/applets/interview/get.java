import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class get implements ActionListener
{
	public interview head;
	
	public get(interview nope)
	{
		head = nope;
	}
	
	public void actionPerformed(ActionEvent e)
	{
		String name, ques;
		net.write("get");
		name = net.read();
		ques = net.read();
		head.question.setText(name + " Asks: " + ques);
	}
}