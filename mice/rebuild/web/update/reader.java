/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class is all about reading a file in, and setting the headers and stuff.
 * @author Brandon Smith
 * @version 2.0
 */

import java.io.*;

public class reader
{
	/** The header that has all the form stuff and whatnot. */
	public String header = "";
	/** The footer that finishes the form stuff off. */
	public String footer = "";
	/** The platonic form of an administrator/moderator notice. */
	public String notice = "";
	/** The platonic form of a question. */
	public String question="";
	
	/**
	 * This constructs the template information from the file
	 * @param filename The location of the template file.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public reader(String filename)
	{
		BufferedReader infile;
		String line;
		try
		{
			infile = new BufferedReader(new FileReader(filename));
			line = infile.readLine();
			while(line.compareTo("<!-- End Header Block -->") != 0)
			{
				header = header + line + "\n";
				System.out.println("H: " + line);
				line = infile.readLine();
			}
			line = infile.readLine();
			while(line.compareTo("<!-- End Question Block -->") != 0)
			{
				question = question + line + "\n";
				System.out.println("Q: " + line);
				line = infile.readLine();
			}
			line = infile.readLine();
			while(line.compareTo("<!-- End Notice Block -->") != 0)
			{
				notice = notice + line + "\n";
				System.out.println("N: " + line);
				line = infile.readLine();
			}
			line = infile.readLine();
			while(line != null)
			{
				footer = footer + line + "\n";
				System.out.println("F: " + line);
				line = infile.readLine();
			}
			System.out.println("preclose");
			infile.close();
		}
		catch(Exception foo)
		{ 
			System.out.println(foo);
		}
	}
}
