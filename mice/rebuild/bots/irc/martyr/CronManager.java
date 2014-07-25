package martyr;

import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;

/**
 * @since 0.3.2
 * */
public class CronManager
{
	private Timer timer;

	public CronManager()
	{
		timer = new Timer();
	}

	/**
	 * @param task
	 * @param time
	 */
	public void schedule(TimerTask task, Date time)
	{
		timer.schedule(task, time);
	}

	/**
	 * @param task
	 * @param firstTime
	 * @param period
	 */
	public void schedule(TimerTask task, Date firstTime, long period) 
	{
		timer.schedule(task, firstTime, period);
	}

	/**
	 * @param task
	 * @param delay
	 */
	public void schedule(TimerTask task, long delay) 
	{
		timer.schedule(task, delay);
	}

	/**
	 * @param task
	 * @param delay
	 * @param period
	 */
	public void schedule(TimerTask task, long delay, long period) 
	{
		timer.schedule(task, delay, period);
	}

	/**
	 * @param task
	 * @param firstTime
	 * @param period
	 */
	public void scheduleAtFixedRate(
		TimerTask task,
		Date firstTime,
		long period) 
	{
		timer.scheduleAtFixedRate(task, firstTime, period);
	}

	/**
	 * @param task
	 * @param delay
	 * @param period
	 */
	public void scheduleAtFixedRate(TimerTask task, long delay, long period) 
	{
		timer.scheduleAtFixedRate(task, delay, period);
	}

}
