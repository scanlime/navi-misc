/**
 *This object stores an individual question, and handles all the singularly questionable things.
 */

import java.util.*;

public class question
{
	//the next ID to be assigned
	private static int nextID = 1;

	//the name of the submitter
	private String name;
	
	//the question itself
	private String text;
	
	//the answer that is given
	private String answer;
	
	//the name of the person who answered it
	private String answeredby;
	
	//the time this question was asked
	private String time;
	
	//the ID for the question
	private int id;
	
	/**
	 * The Constructor for creating a new question.
	 * @param NAME The name of the person submitting the question.
	 * @param TEXT The text of the question.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public question(String NAME, String TEXT)
	{
		id = question.nextID++;
		name = NAME;
		text = TEXT;
		time = new Date().toString();
		System.out.println(this);
		//add this question to the log file
	}
	
	/**
	 * The method adds the answer to the question.
	 * @param who The name of the person who answered.
	 * @param ans The answer they gave.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public void addAnswer(String who, String ans)
	{
		answeredby = who;
		answer = ans;
		//add this answer to the log file
	}
	
	/**
	 * This method returns the question in a network friendly manner.
	 * @return A string representation of the question
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public String toNet()
	{
		if(answer == null)
			return name + "\r\n" + text;
		else
			return name + "\r\n" + text + "\r\n" + answer;
	}
	
	/**
	 * This method returns the question in a screen friendly manner.
	 * @return A string representation of the question.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public String toString()
	{
		return "ID: " + id + "\n\"" + name + "\" Asks: " + text + "\nAnswered with: " + answer + "\nAt: " + time;
	}
}