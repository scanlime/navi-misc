import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class aimconfig implements ActionListener
{
	private admin myadmin;
	private TextField host;
	private TextField port;
	private Button submit;
	
	public aimconfig(admin my)
	{
		admin.help.setText("Please enter the hostname and port that the web updater is running on");
		host = new TextField("AIM bot hostname",40);
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
		String temp;
		net.write(host.getText());
		net.write(port.getText());
		
		temp = net.read();
		if(temp.charAt(0) == '-')
		{
			admin.help.setText("check your host and port, the connection failed");
			return;
		}
		
		myadmin.rm(submit);
		myadmin.rm(port);
		myadmin.rm(host);
		new notice(myadmin);
	}
}