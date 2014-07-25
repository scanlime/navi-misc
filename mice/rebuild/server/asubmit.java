/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This is the class that handles the abstraction for submitting a question, 
 * thus simplifying the network protocol for it.
 * @author Brandon Smith
 * @version 2.0
 */

public class asubmit
{
	/** Holds the question queue that is for unwashed questions. */
	public static qqueue putin;
	
	/** For unwashed questions that come before everything is inited. */
	public static qqueue store;
	
	/** 
	 * This method initializes the queue, and empties the temp into the dbase;
	 * @param root The question database this pulls from
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void init(qbase root)
	{
		putin = root.input;
		if(store != null)
		{
			question temp;
			temp = store.pop();
			while(temp != null)
			{
				putin.add(temp);
				temp = store.pop();
			}
		}
	}
	
	/**
	 * This method submits a question into the database, pure and simple.
	 * @param Name The name of the person submitting the quesiton.
	 * @param Question The question itself (no newlines permitted)
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public static void submit(String Name, String Question)
	{
		//this is where we have checks for auto-rejection
		if(Name == null || Question == null) return;
		//create a new question
		question in = new question(Name,Question);
		//add it to the input queue
		if(putin != null) putin.add(in);
		else
		{
			if(store == null) store = new qqueue();
			store.add(in);
		}
		//log(in);
	}
	
	/**
	 * This method handles the logging of the question
	 * @param it The question that got submitted
	 * @author Brandon Smith
	 * @version 2.1
	 */ /*
	private static void log(question it)
	{
		OutputStreamWriter out;
		try
		{
			out = new OutputStreamWriter(new FileOutputStream("logfile.txt",true));
			out.println("SUBMIT");
			out.println(it.toLog());
			out.close();
		}
		catch(FileNotFoundException e)
		{
			System.out.println("Logging error\n"+e);
		}
	}*/
}