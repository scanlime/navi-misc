/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */
 
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
	}
	
	/**
	 * This method returns a nearly exact copy of the question
	 * @author Brandon Smith
	 * @version 2.1
	 */
	public question duplicate()
	{
		question dupe = new question(name,text);
		dupe.answer = answer;
		dupe.answeredby = answeredby;
		return dupe;
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
	 * @return A string that is ready for network sending.
	 */
	public String toNet()
	{
		if(answer == null)
			return name + "\r\n" + text;
		else
			return name + "\r\n" + text + "\r\n" + answeredby + "\r\n" + answer;
	}
	
	/**
	 * This method returns the question in a log friendly manner.
	 * @return A string representation of the question
	 * @author Brandon Smith
	 * @version 2.0
	 * @return A string that has just the facts for the log file.
	 */
	public String toLog()
	{
		if(answer == null)
			return name + "\n" + text;
		else
			return name + "\n" + text + "\n" + answeredby + "\n" + answer;
	}
	
	/**
	 * This method returns the question in a screen friendly manner.
	 * @return A string representation of the question.
	 * @author Brandon Smith
	 * @version 2.0
	 * @return A String that quickly and easily read by the human eye.
	 */
	public String toString()
	{
		return "ID: " + id + "\n\"" + name + "\" Asks: " + text + "\nAnswered with: " + answer + "\nAt: " + time;
	}
}