import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class webconfig implements ActionListener
{
	private admin myadmin;
	private TextField host;
	private TextField port;
	private Button submit;
	
	public webconfig(admin my)
	{
		admin.help.setText("Please enter the hostname and port that the web updater is running on");
		host = new TextField("Web bot hostname",40);
		port = new TextField("port",6);
		submit = new Button("Submit");
		myadmin = my;
		myadmin.ad(host);
		myadmin.ad(port);
		myadmin.ad(submit);
		submit.addActionListener(this);
	}
	
	public void actionPerformed(ActionEvent e)
	{
	}
}