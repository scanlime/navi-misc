/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class holds the questions and notices that have been answered.
 * @author Brandon Smith
 * @version 2.0
 */

import java.util.*;

public class element
{
	/** The template string for questions from the template file. */
	public static reader strgs;

	/** A pointer to the next one for linked listing purposes. */
	public element next;
	
	/** The String that has the formatted question and answer */
	public String fork;

	/**
	 * The all powerful constructor. This one handles notices
	 * @param prev The previous element in the linked list.
	 * @param notice The notice text.
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public element(element prev, String notice)
	{
		String temp;
		next = prev;
		StringTokenizer it = new StringTokenizer(element.strgs.notice,"$",false);
		fork = it.nextToken();
		while(it.hasMoreTokens())
		{
			temp = it.nextToken();
			if(temp.startsWith("(NC)")) fork = fork + notice + temp.substring(4);
		}
	}

	/**
	 * The all powerful constructor.  This handles a question.
	 * @param prev The previous element in the linked list.
	 * @param qt The Question Text.
	 * @param qb The Question is By (who asked the question).
	 * @param at The Answer Text.
	 * @param ab The question is Answered By (who answered the question).
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public element(element prev, String qt, String qb, String at, String ab)
	{
		String temp;
		next = prev;
		StringTokenizer it = new StringTokenizer(element.strgs.question,"$",false);
		fork = it.nextToken();
		while(it.hasMoreTokens())
		{
			temp = it.nextToken();
			if(temp.startsWith("(QT)")) fork = fork + qt + temp.substring(4);
			if(temp.startsWith("(QB)")) fork = fork + qb + temp.substring(4);
			if(temp.startsWith("(AT)")) fork = fork + at + temp.substring(4);
			if(temp.startsWith("(AB)")) fork = fork + ab + temp.substring(4);
		}
	}
	
	/**
	 * A toString() method for easy output
	 * @author Brandon Smith
	 * @version 2.0
	 * @return A string representation of this question.
	 */
	public String toString()
	{
		return fork;
	}
}