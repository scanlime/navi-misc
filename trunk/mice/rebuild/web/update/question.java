/**
 * This class holds the questions that have been answered.
 * @author Brandon Smith
 * @version 2.0
 */

import java.util.*;

public class question
{
	/** The template string from the template file. */
	public static String template;

	/** A pointer to the next one for linked listing purposes. */
	public question next;
	
	/** The String that has the formatted question and answer */
	public String fork;

	/**
	 * The all powerful constructor.
	 * @param prev The previous question in the linked list.
	 * @param qt The Question Text.
	 * @param qb The Question is By (who asked the question).
	 * @param at The Answer Text.
	 * @param ab The question is Answered By (who answered the question).
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public question(question prev, String qt, String qb, String at, String ab)
	{
		String temp;
		next = prev;
		StringTokenizer it = new StringTokenizer(question.template,"$",false);
		fork = it.nextToken();
		while(it.hasMoreTokens())
		{
			temp = it.nextToken();
		}
	}
}