import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

/**
 * This class is the simplest java applet possible that can shoot a question
 * to the server.
 * @author Brandon Smith
 * @version 2.0
 */
public class question extends java.applet.Applet implements ActionListener
{
	/** The name field */
	public TextField name;
	/** The position field */
	public TextField position;
	/** The Text Area that the question gets scribbled into */
	public TextArea ques;
	/** The button that does the actual submission */
	public Button submit;
    
	/** the String that becomes the name string and position string */
	public String nstring;
	/** the String that holds the question as it is sent */
	public String qstring;
    
	//network information
	public Socket link;
	public BufferedReader in;
	public OutputStreamWriter out;
    
	/**
	 * This method initializes the applet and makes it all well and good
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void init()
	{
		//create the fields and buttons
		name = new TextField(lname,30);
		position = new TextField(lquest,30);
		ques = new TextArea("Question",4,30,TextArea.SCROLLBARS_VERTICAL_ONLY);
		submit = new Button("Submit");
		
		//register this as the action listener
		submit.addActionListener(this);
        
		//put stuff in the applet.
        add(name);
        add(position);
        add(ques);
        add(submit);
    }
    

    public void actionPerformed(ActionEvent e)
    {
        nstring = name.getText() + " (" + position.getText() + ")";
        qstring = ques.getText();
        
        StringTokenizer tokens = new StringTokenizer(qstring,"\n");

        if(qstring.compareTo("") == 0) return;

        if(!(nstring.compareTo(lname + " " + lquest) == 0 && qstring.compareTo("Question") == 0))
        {
            openConnection("studentactivities.mscd.edu",8080);
            read();
            //write the name string
            write(nstring);
            write(".");            
            //write the question
            qstring = tokens.nextToken();
            while(tokens.hasMoreTokens())
                qstring = qstring + " " + tokens.nextToken();
            write(qstring);
            write(".");
            read();
            closeConnection();
            lname = "Question submitted successfully";
            name.setText("Question submitted successfully");
            position.setText("Refresh your browser window");
            ques.setText("");
            submit.removeActionListener(this);
        }
    }
    
    /* These four methods handle network communication */
    
    /*
     * This method opens a connection and takes care of all the pre/post conditions for read and write.
     */
    public boolean openConnection(String host, int port)
    {
        //Initialize everything
	try
	{
	    link = new Socket(host,port);
	    in = new BufferedReader(new InputStreamReader(link.getInputStream()));
	    out = new OutputStreamWriter(link.getOutputStream());
	}
	catch(IOException exception) {
	    return false;
	}
        write("SUBM");
	return true;
    }
    
    //Close the connection
    public void closeConnection()
    {
	write("QUIT");
	try 
	{
	    link.close();
	}
	catch(IOException exception) 
	{
	}
    }
    
    //Read a single line from the socket
    public String read()
    {
	String toreturn = null;
	try
	{
	    toreturn = in.readLine();
	}
	catch(IOException exception)
	{
	    System.out.println("Error in recieving: " + exception);
	    closeConnection();
	    System.exit(1);
	}
	return toreturn;
    }
    
    //Write a single line to the socket
    public void write(String tosend)
    {
	try
	{
	    out.write(tosend + "\r\n");
	    out.flush();
	}
	catch(IOException exception)
	{
	    System.out.println("Error in sending:" + exception);
	    closeConnection();
	    System.exit(1);
	}
    }
}