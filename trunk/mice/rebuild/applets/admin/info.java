/**
 * This class stores the references to the various parts of the applet.
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

public class info
{
	/** This is the help text area.  It informs the user what they need to do. */
	public static TextArea help;
	
	/** This is the button for submitting stuff. */
	public static Button submit;
	
	/** This Field is for the username when authenticating the client. */
	public static TextField username;
	/** This Field is for the password when authenticating the client. */
	public static TextField password;
	
	/** This holds the authentication object. */
	public static authenticate auth;
}