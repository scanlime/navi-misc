/**
 * Handles the port and stuff.  It listens and spawns the correct threads as per
 * who connects.
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class updater
{
	public static BufferedReader in;
	public static OutputStreamWriter out;
	public static ServerSocket server;
	public static Socket connection;
	public static String mykey, password;
        
	public static void main(String[] args)
	{
		startServer(8081);
	}

	public static void startServer(int port)
	{
            try
            {
		server = new ServerSocket(port);
		connection = server.accept();
		in = new BufferedReader(new InputStreamReader(connection.getInputStream()));
		out = new OutputStreamWriter(connection.getOutputStream());
            }
            catch(IOException exception)
            {
                System.out.println(exception);
                System.exit(0);
            }
            mykey = key.keygen();
            write(mykey);
            password = key.decrypt(mykey,read());
            if(password.compareTo("aplefriterducksauce772821") != 0){
                write("-ERR");
                closeConnection();
                System.exit(0);
            }
            write("+OK");
            write(ServerRoot);
            ServerRoot = ServerRoot + read();
            //do some checking on the directory stuff
            write("+OK");
            precursor = precursor + "<h2><font color=\"#000066\" face=\"Arial, Helvetica, sans-serif\">Interviewing: " + read() + "</font></h2>\n";
            //precursor = "<html>\n<body>\n<applet code=question.class width=300 height=170>\n</applet>\n<h1>Interviewing: " + read() + "</h1>\n";
            tail = "\n</body>\n</html>";
            ServerRoot = ServerRoot + "chatlog.html"; //convert it into a file name
            updatePage();
            String exchange = read();
            while(exchange.charAt(0) != 'Q') {
                if(exchange.charAt(0) == 'E') {
                    String n="",q="",a="",t;
                    write("+OK");
                    t = read();
                    while(t.compareTo(".") != 0) {
                        n = n + t;
                        t = read();
                    }
                    t = read();
                    while(t.compareTo(".") != 0) {
                        q = q + t + " ";
                        t = read();
                    }
                    t = read();
                    while(t.compareTo(".") != 0) {
                        a = a + t;
                        t = read();
                    }
                    writePage(n,q,a);
                    write("+OK");
                }
                if(exchange.charAt(0) == 'U') {
                    updatePage();
                    write("+OK");
                }
                if(exchange.charAt(0) == 'N') {
                    addNotice(read());
                    write("+OK");
                }
                exchange = read();
            }
	}

        //this just writes what is in the files to the page
        public static void updatePage()
        {
            try { 
                PrintWriter outFile = new PrintWriter(new BufferedWriter(new FileWriter(ServerRoot)));
                outFile.println(precursor);
                outFile.println(tail);
                outFile.close();
            }
            catch(IOException exception) {
                write("-ERR couldn't open page for writing. " + exception.toString());
                System.exit(1);
            }
            write("+OK");
        }

        //updates the page in memory.
        public static void writePage(String Name, String Question, String Answer)
        {
            tail = "\n" + 
                "<dt><font face=\"Arial, Helvetica, sans-serif\"><b>" + Question + "</b> - asked by " + Name + 
                "\n<dd><font face=\"Arial, Helvetica, sans-serif\">" + Answer + "</font><br></dd></dt><br>" + tail;
            //tail = "\n<dt><b>" + Name + " asked: " + Question + "</b><dd>\nAnswer: " + Answer + "<br></dd></dt>" + tail;
        }

        //adds a notice to the text to be output
        public static void addNotice(String notice)
        {
            tail = "<font color=\"#000066\" face=\"Arial, Helvetica, sans-serif\"><b>---- Notice from the Moderator: " + notice + "</b><br></font><br>\n" + tail;
        }

        /**********************************************************************************/
        //just the typical network single line read/write stuff, to make it all happy and good

	public static String read()
	{
		String toreturn = null;
		try {
			toreturn = in.readLine();
		}
		catch(IOException exception) {
			System.out.println("Error in recieving: " + exception);
			closeConnection();
		}
                System.out.println("READ: " + toreturn);
		return toreturn;
	}

	public static void write(String tosend)
	{
                System.out.println("WRTE: " + tosend);
		try	{
			out.write(tosend + "\r\n");
			out.flush();
		}
		catch(IOException exception){
			System.out.println("Error in sending:" + exception);
			closeConnection();
		}	
	}
	
	public static void closeConnection()
	{
		try{
			connection.close();
		}
		catch(IOException exception){
			System.out.println(exception);
		}
	}
}	
