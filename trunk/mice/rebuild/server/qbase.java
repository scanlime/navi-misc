/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This class is the question database, singularly known throughout the server.
 * @author Brandon Smith
 * @version 2.0
 */

public class qbase
{
	/** The question queue for freshly submitted questions. */
	public qqueue input;
	/** The question queue for rejected questions. */
	public qqueue rejected;
	/** An array of question queues for questions waiting to be answered. */
	public qqueue[] waiting;
	/** The question queue for answered questions. */
	public qqueue answered;
	
	/** An array of usernames for the people being interviewed. */
	public String[] interviewuser;
	/** An array of passwords for the people being interviewed. */
	public String[] interviewpass;
	
	/** An array of usernames for the moderator(s). */
	public String[] moduser;
	/** An array of passwords for the moderator(s). */
	public String[] modpass;
	
	/** Handles the updating of the clients. */
	public aupdate updater;
	
	/**
	 * Creates a new question database
	 * @param interviews The number of people being interviewed
	 * @param moderators The number of moderators that will be used
	 * @author Brandon Smith
	 * @version 2.0
	 */
	public qbase(int interviews, int moderators)
	{
		int i;
		input = new qqueue();
		rejected = new qqueue();
		waiting = new qqueue[interviews];
		for(i=0;i<interviews;i++)
			waiting[i] = new qqueue();
		answered = new qqueue();
	
		interviewuser = new String[interviews];
		interviewpass = new String[interviews];
		
		moduser = new String[moderators];
		modpass = new String[moderators];
		
		updater = new aupdate();
	}
	
	/**
	 * This method gives the question queue for the interviewee
	 * @param user the username of the interviewee
	 * @author Brandon Smith
	 * @version 2.0
	 * @return The interviewee's queue of waiting questions.
	 */
	public qqueue getIntQqueue(String user)
	{
		int i;
		for(i=0;i<interviewuser.length;i++)
		{
			if(user.compareTo(interviewuser[i]) == 0)
				return waiting[i];
		}
		return null;
	}
	
	/**
	 * This method authenticates an interviewee
	 * @param user The username that needs to be authenticated
	 * @param pass The password that is associated with the username
	 * @author Brandon Smith
	 * @version 2.0
	 * @return False on failed authentication, true on passed 
	 */
	public boolean authenticateInt(String user, String pass)
	{
		int i;
		for(i=0;i<interviewuser.length;i++)
		{
			if(user.compareTo(interviewuser[i]) == 0)
				if(pass.compareTo(interviewpass[i]) == 0)
					return true;
		}
		return false;
	}

	/**
	 * This method authenticates a moderator
	 * @param user The username that needs to be authenticated
	 * @param pass The password that is associated with the username
	 * @author Brandon Smith
	 * @version 2.0
	 * @return False on failed authentication, true on passed 
	 */
	public boolean authenticateMod(String user, String pass)
	{
		int i;
		for(i=0;i<moduser.length;i++)
		{
			if(user.compareTo(moduser[i]) == 0)
				if(pass.compareTo(modpass[i]) == 0)
					return true;
		}
		return false;
	}
}