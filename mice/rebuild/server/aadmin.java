/*
 *   (C) 2004 Brandon Smith smibrand@mscd.edu
 */

/**
 * This is the class that handles administration.  It keeps static information
 * on the state of the server, so if the administrator gets disconnected, life
 * will be fine with a reconnect.
 * @author Brandon Smith
 * @version 2.0
 */

public class aadmin
{
	/**
	 * This stores the level of configuration <br>
	 * 0 => Not configured at all <br>
	 * 1 => Moderators are set <br>
	 * 2 => Interviewees are set <br>
	 * 3 => Web based updates are set <br>
	 * 4 => AIM based updates are set <br>
	 */
	public static int confstate = 0;
	
	/** This holds the username for the admin */
	public static String user = "chulk";
	
	/** This holds the password for the admin */
	public static String pass = "pizze";
	
	/** This holds the moderator usernames */
	public static String[] muns;
	/** This holds the moderator passwords */
	public static String[] mpas;
	
	/** This holds the interview usernames */
	public static String[] iuns;
	/** This holds the interview passwords */
	public static String[] ipas;
}
