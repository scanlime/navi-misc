/**
 * This class handles the java submission client.
 * @author Brandon Smith
 * @version 2.0
 */

import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class njavasub extends nbase
{
	public njavasub()//Socket gonnection, BufferedReader IN, OutputStreamWriter OUT)
	{
		//super(gonnection,IN,OUT);
	}
	
	public void reinit(Socket gonnection, BufferedReader IN, OutputStreamWriter OUT)
	{
		link = gonnection;
		in = IN;
		out = OUT;
	}
	
	public void run()
	{
		
	}
}
