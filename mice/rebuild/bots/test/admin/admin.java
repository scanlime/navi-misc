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
		System.out.println(keys);
		net.write(key.encrypt(keys,"chulk"));
		net.write(key.encrypt(keys,"pizze"));
		System.out.println(net.read());
		net.read();
		net.write("1");
		keys = net.read();
		System.out.println(keys);
		net.write(key.encrypt(keys,"me"));
		net.write(key.encrypt(keys,"mee"));
		System.out.println(net.read());

		net.write("1");
		keys = net.read();
		System.out.println(keys);
		net.write(key.encrypt(keys,"me"));
		net.write(key.encrypt(keys,"mee"));
		System.out.println(net.read());
		
		net.write("localhost");
		net.write("8081");
		net.read();
		
		net.write("localhost");
		net.write("8082");
		net.read();
		net.closeConnection();
	}
}