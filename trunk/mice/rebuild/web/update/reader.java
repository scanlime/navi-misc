/**
 * This class is all about reading a file in, and setting the headers and stuff.
 * @author Brandon Smith
 * @version 2.0
 */
 
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
		BufferedReader infile = new BufferedReader(new FileReader(filename));
		String line = infile.readLine();
		while(line.compareTo("<!-- End Header Block -->") != 0)
		{
			header = header + line + "\n";
			line = infile.readLine();
		}
		while(line.compareTo("<!-- End Question Block -->") != 0)
		{
			question = question + line + "\n";
			line = infile.readLine();
		}
		while(line.compareTo("!-- End Notice Block -->") != 0)
		{
			notice = notice + line + "\n";
			line = infile.readLine();
		}
		while(line != null)
		{
			footer = footer + line + "\n";
			line = infile.readLine();
		}
		infile.close();
	}
}