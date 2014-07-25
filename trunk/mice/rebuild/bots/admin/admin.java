/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

/**
 * This class is a test class for quick user addition.
 * @author Brandon Smith
 * @version 2.0
 */
public class admin
{
	public static void main(String[] args)
	{
		String keys;
		net.openConnection("localhost",8080);
		net.write("admin");
		keys = net.read();
		net.write(key.encrypt(keys,"chulk"));
		net.write(key.encrypt(keys,"pizze"));
		net.read();
		net.read();
		net.write("1");
		keys = net.read();
		net.write(key.encrypt(keys,"me"));
		net.write(key.encrypt(keys,"mee"));
		net.read();

		net.write("3");
		keys = net.read();
		net.write(key.encrypt(keys,"ne"));
		net.write(key.encrypt(keys,"nee"));
		net.write(key.encrypt(keys,"oe"));
		net.write(key.encrypt(keys,"oee"));
		net.write(key.encrypt(keys,"pe"));
		net.write(key.encrypt(keys,"pee"));
		net.read();
		
		net.write("localhost");
		net.write("8081");
		net.read();
		
		net.write("localhost");
		net.write("8082");
		net.read();
		net.closeConnection();
		
		net.openConnection("localhost",8080);
		net.write("jsub\r\nBrandon1 (Senior/Philosophy)\r\nWhat is the meaning of a duck?");
		net.closeConnection();

		net.openConnection("localhost",8080);
		net.write("jsub\r\nBrandon2 (Senior/Philosophy)\r\nWhat is the meaning of a duck?");
		net.closeConnection();

		net.openConnection("localhost",8080);
		net.write("jsub\r\nBrandon3 (Senior/Philosophy)\r\nWhat is the meaning of a duck?");
		net.closeConnection();
		
		net.openConnection("localhost",8080);
		net.write("jsub\r\nBrandon4 (Senior/Philosophy)\r\nWhat is the meaning of a duck?");
		net.closeConnection();
		
		net.openConnection("localhost",8080);
		net.write("jsub\r\nBrandon5 (Senior/Philosophy)\r\nWhat is the meaning of a duck?");
		net.closeConnection();
		
		net.openConnection("localhost",8080);
		net.write("jmod");
		keys = net.read();
		net.write(key.encrypt(keys,"me"));
		net.write(key.encrypt(keys,"mee"));
		net.read();
		net.read();
		net.read();
		net.read();
		net.read();
		net.write("get");
		net.read();
		net.write("accept");
		net.write("0");
		net.write("quit");
		net.closeConnection();
	}
}