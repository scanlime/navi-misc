/*
 *   (C) 2002 Paul Wilkinson  wilko@users.sourceforge.net
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 * JaimConnection.java
 *
 * Created on 4 May 2002, 08:38
 */

package jaim;

import java.net.*;
import java.text.DateFormat;
import java.io.*;
import java.util.*;

/** The JaimConnection object is the primary interface into the Jaim library.
 *  Programs should instantiate a JaimConnection (in most cases the simple constructor should be used).
 *  Once JaimConnection has been instantiated, call {@link #connect} followed by {@link #logIn}.
 *
 *
 * @author paulw
 * @version $Revision: 1.20 $
 */
public class JaimConnection implements java.lang.Runnable {
    
    private Socket s;
    private InputStream sin;
    private OutputStream sout;
    private boolean connected;
    private boolean loggedIn;
    private boolean loginComplete;
    private boolean configValid;
    private String host;
    private int port;
    private int clientSequence;
    private int serverSequence;
    private ReceiverThread rt;
    private DeliveryThread dt;
    private Vector eventListeners;
    private HashMap watchedBuddies;
    private HashMap buddies;
    private HashMap groups;
    private String nickName;
    private long lastMessageSendTime;
    private boolean debug;
    private Thread myThread;
    private Vector messageQueue;
    private boolean exit;
    private long lastKeepAlive;
    
    // Number of send "points" - used to control send rate
    private int sendPoints=10;
    
    
    private static final int MAX_POINTS=10;
    private static final int BLOCK_POINTS=5;
    private static final int POINT_RECOVERY_TIME=2200;   // Recover one point every 2.2 seconds
    private static final int THRESHOLD_DELAY=5000;  // Delay when we are threshold of being blocked
    
    private static final int WAIT_TIME=61000;		// Wait 61 secs for a keep alive
  
    /** Creates new JaimConnection that connects to the default host and port.
     *  In most cases this constructor should be used.
     */
    public JaimConnection() {
        host="toc.oscar.aol.com";
        port=9898;
        startMe();
        
    }
    
    /** Creates a new Jaim Connection to the specified host/port.
     * There are currently no reasons to call this constructor, however AOL may change the TOC host and port in the future
     * @param host The hostname or IP address of the TOC server
     * @param port The port number to connect to on the host
     */
    public JaimConnection(String host,int port) {
        this.host=host;
        this.port=port;
        startMe();
    }
    
    /** start the message dispatcher thread
     */
    
    private void startMe() {
        connected=false;
        loggedIn=false;
        eventListeners=new Vector();
        loginComplete=false;
        lastMessageSendTime=0;
        watchedBuddies=new HashMap();
        buddies=new HashMap();
        groups=new HashMap();
        debug=false;
        exit=false;
        rt=null;
        configValid=false;
	lastKeepAlive=System.currentTimeMillis();
        TocResponseFactory.addResponseHandler(new BuddyUpdateTocResponse());
        TocResponseFactory.addResponseHandler(new ErrorTocResponse());
        TocResponseFactory.addResponseHandler(new EvilTocResponse());
        TocResponseFactory.addResponseHandler(new IMTocResponse());
        TocResponseFactory.addResponseHandler(new SignOnTocResponse());
        TocResponseFactory.addResponseHandler(new NickTocResponse());
        TocResponseFactory.addResponseHandler(new GotoTocResponse());
        TocResponseFactory.addResponseHandler(new ConfigTocResponse());
        messageQueue=new Vector();
        myThread = new Thread(this);
        myThread.setDaemon(true);
        myThread.start();
        dt=new DeliveryThread();
        dt.setDaemon(true);
        dt.start();
    }
    
    
    
    /** Enable/Disable debugging messages to stdout
     * @param debug true if debugging messages should be output
     */
    
    public void setDebug(boolean debug) {
        this.debug=debug;
    }
    
    
    /** Specify the intermessage delay time. <br>
     * The {@link #sendIM } method will ensure that at least this amount of time has elapsed between messages
     * @param msec The delay period in milliseconds
     * @deprecated This function is no longer used - send throttling is automatic
     */
    public void setInterMessageDelay(long msec) {
        
    }
    
    /** Get the intermessage delay time
     * @return The intermessage delay time in milliseconds
     * @deprecated This function is no longer used
     */
    public long getInterMessageDelay() {
        return(0);
    }
    
    /** Set the  EventListener object.  This object will be notified of incoming TOC events
     * @param l The listener class to be notified
     * @deprecated replaced by {@link #addEventListener}
     */
    public void setEventListener(JaimEventListener l) throws TooManyListenersException {
        eventListeners.add(l);
    }
    
    /** Add an EventListener object.  This object will be notified of incoming TOC events
     * @param l The listener class to be notified
     */
    
    public void addEventListener(JaimEventListener l) {
        eventListeners.add(l);
    }
    
    /** Remove an EventListener object.  This object will no longer be notified of incoming TOC events
     *  @param l The listener class to be removed
     */
    
    public void removeEventListener(JaimEventListener l) {
        eventListeners.remove(l);
    }
    
    
    /** Initiate a connection to the TOC server
     * @throws IOException If an underlying network communication fails
     */
    public void connect() throws IOException {
        s=new Socket(host,port);
        s.setSoTimeout(500);
        sin=s.getInputStream();
        sout=s.getOutputStream();
        
        sout.write("FLAPON\r\n\r\n".getBytes());
        
        FLAPInputFrame inFrame = new FLAPInputFrame();
        
        int i=-1;
        
        
        while (!inFrame.completeFrameRead()) {
            i=sin.read();
            inFrame.addFrameData((byte)i);
        }
        
        try {
            FLAPFrame f = FLAPFrameFactory.createFLAPFrame(inFrame.getFrameData());
            FLAPSignonFrame sf = (FLAPSignonFrame)f;
            if (debug) {
                System.out.println("Starting sequence="+sf.getSequence());
                System.out.println("FLAP version = "+sf.getFLAPVersion());
            }
            clientSequence=sf.getSequence();
            serverSequence=sf.getSequence();
        }
        catch (FLAPFrameException e) {
            throw new IOException("FLAPFrameException:"+e.toString());
        }
        if (rt!=null) {
            rt.pleaseExit();
        }
        rt=new ReceiverThread(this);
        rt.setInputStream(sin);
        rt.setDaemon(true);
        rt.start();
        connected=true;
    }
    
    /** Disconnect from the TOC server
     * @throws IOException if a network transport error occurs
     */
    public void disconnect() throws IOException {
        exit=true;
        rt.pleaseExit();
        try {
            rt.join(700);
            myThread.join(700);
        }
        catch (InterruptedException e) {
        }
        
        if (connected) {
            if (loggedIn) {
                logOut();
            }
            s.close();
            connected=false;
        }
    }
    
    
    /** Check if the TOC login process has completed
     * @return true if the login process is complete
     */
    public boolean isLoginComplete() {
        return(loginComplete);
    }
    
    /** Log out from the TOC server
     */
    public void logOut() {
        loggedIn=false;
        loginComplete=false;
        configValid=false;
        
    }
    
    /** Get the formatted Nick Name for this connection.  If no formatted nick name has been registered with the TOC server, then the username provided to the logIn call is returned
     * @return The Nick Name associated with this connection
     */
    public String getNickName() {
        return(nickName);
    }
    
    /** login to the TOC server. {@link #connect() } method should be called first
     * @param username The username to log in with
     * @param password the password for the specified username
     * @param waitTime time in milliseconds for successful login before declaring an error
     * @throws IOException If a network error occurs
     * @throws JaimException If a login failure occurs or login fails to complete before waittime expires
     */
    public void logIn(String username,String password,int waitTime) throws JaimException, IOException {
        if (connected) {
            
            nickName=username;
            String nuser=Utils.normalise(username);
            FLAPSignonFrame sof = new FLAPSignonFrame();
            sof.setSequence(clientSequence++);
            sof.setFLAPVersion(1);
            sof.setTLVTag(1);
            sof.setUserName(nuser);
            sout.write(sof.getFrameData());
            TocSignonCommand soc=new TocSignonCommand(host,port,username,password);
            sendTocCommand(soc);
            for (int i=0;i<waitTime/100;i++)  // Wait a max of waitTime * 100ms
            {
                if (loginComplete||!connected)  // Have we logged in successfully
                {
                    break;              // If so then return
                }
                else {
                    try {
                        Thread.sleep(100);  //Sleep for a tenth of a second
                    }
                    catch (InterruptedException e) {
                    }
                }
            }
            if (loginComplete) {
                loggedIn=true;
            }
            else {
                throw new JaimTimeoutException("login failed-timeout waiting for valid response");
            }
            
        }
        
        else
            throw new JaimStateException("Not connected.");
    }
    
    private void sendTocCommand(TocCommand cmd) throws IOException {
        FLAPDataFrame fr=new FLAPDataFrame();
        fr.setSequence(nextSequence());
        if (debug) {
            System.out.println("Sending "+cmd.toString());
        }
        fr.addString(cmd.toString());
        sout.write(fr.getFrameData());
    }

    private int  nextSequence()
    {
        int seq=clientSequence++;
         if (clientSequence>65535)
  	     clientSequence=0;
 	return(seq);
    }

    private void sendKeepAlive() throws IOException {
       FLAPKeepAliveFrame fr=new FLAPKeepAliveFrame();
       fr.setSequence(nextSequence());
       if (debug) {
       	    System.out.println("Sending keepalive");
       }
       sout.write(fr.getFrameData());
    }   
    /** The run method for the dispatcher thread
     */
    
    public void run() {
        while (true) {
            
            if (messageQueue.size()>0) {
                realDispatch((FLAPFrame)messageQueue.remove(0));
            }
            else {
		if (System.currentTimeMillis()-lastKeepAlive>WAIT_TIME)
		{
		   if (debug)
		   {
			System.out.println("No keepalive received - sending");
		   }
		   try
		   {
		       sendKeepAlive();
		       lastKeepAlive=System.currentTimeMillis();
		   }
		   catch (IOException ioe)
		   {
			connectionLost();
		   }
                }
                
                try {
                    synchronized(this) {
                        this.wait(WAIT_TIME);
                    }
                }
                catch (InterruptedException e) {
                }
            }
        }
    }
    
    protected void Dispatch(FLAPFrame fr) {
        messageQueue.addElement(fr);
        synchronized(this) {
            this.notify();
        }
    }

	 
    private void realDispatch(FLAPFrame fr) {
        switch (fr.getFLAPFrameType()) {
            case FLAPFrame.FLAP_FRAME_ERROR:
                
                try {
                    disconnect();
                }
                catch (IOException e) {
                }
                break;
            case FLAPFrame.FLAP_FRAME_DATA:
                
                FLAPDataFrame df=(FLAPDataFrame)fr;
                TocResponse tr = TocResponseFactory.createResponse(df.getContent());
                HandleTocResponse(tr);
                break;
            case FLAPFrame.FLAP_FRAME_KEEP_ALIVE:
                if (debug) {
                    System.out.println("Received keep alive frame "+DateFormat.getTimeInstance().format(new Date()));
                }
		lastKeepAlive=System.currentTimeMillis();
	 	 try 
		{
		    sendKeepAlive();
		}
		catch (IOException e)
		{	
			connectionLost();
		}
                break;
            case FLAPFrame.FLAP_FRAME_SIGNOFF:
                connected=false;
                loggedIn=false;
                try {
                    s.close();
                }
                catch (IOException e) {
                }
                break;
            default:
                if (debug) {
                    System.out.println("Unknown type received: "+fr.getFLAPFrameType());
                }
                break;
        }
    }
    
    
    protected void HandleTocResponse(TocResponse tr) {
        if (debug) {
            System.out.println("Toc Response received:"+tr.toString());
        }
        if (tr instanceof SignOnTocResponse) {
            TocInitDoneCommand tid = new TocInitDoneCommand();
            TocAddBuddyCommand tab = new TocAddBuddyCommand();
            Iterator it=watchedBuddies.keySet().iterator();
            while (it.hasNext()) {
                tab.addBuddy((String)it.next());
            }
            try {
                sendTocCommand(tab);
                sendTocCommand(tid);
                deliverEvent(new LoginCompleteTocResponse());  // nform clients that login processing is now complete
                loginComplete=true;
            }
            catch (IOException e) {
            }
        }
        else if (tr instanceof ConfigTocResponse) {
            if (debug) {
                System.out.println("Received ConfigTocResponse");
            }
            
            ConfigTocResponse ctr=(ConfigTocResponse)tr;
            Enumeration e=ctr.enumerateGroups();
            while (e.hasMoreElements()) {
                Group g=(Group)e.nextElement();
                groups.put(g.getName(),g);
                Enumeration be=g.enumerateBuddies();
                while (be.hasMoreElements()) {
                    Buddy b=(Buddy)be.nextElement();
                    if (!buddies.containsKey(b.getName())) {
                        buddies.put(b.getName(),b);
                    }
                }
            }
            configValid=true;
        }
        
        deliverEvent(tr);
        
        
        
    }
    
    /** Deliver a TocResponse event to registered listeners
     *@param tr The TocResponse to be delivered
     */
    
    private void deliverEvent(TocResponse tr) {
        dt.deliverMessage(tr);
        
    }
    
    /** Send an instant message
     * @param recipient The nickname of the message recipient
     * @param msg The message to send
     * @throws IOException if a network error occurs
     */
    public void sendIM(String recipient,String msg) throws IOException {
        sendIM(recipient,msg,false);
    }
    
    /** Send an instant message
     * @param recipient The nickname of the message recipient
     * @param msg The message to send
     * @param auto true if this is an automatic response (eg. away message)
     * @throws IOException if a network error occurs
     */
    public void sendIM(String recipient,String msg,boolean auto) throws IOException {
        
        synchronized(this) {
            
            if (sendPoints < MAX_POINTS)                // If we have less than full points
            {
                long now=System.currentTimeMillis();
                long difference=now-lastMessageSendTime;
                sendPoints+=(int)(difference/POINT_RECOVERY_TIME);     // 1 point is regained every 2 seconds
                if (sendPoints >MAX_POINTS)
                    sendPoints=MAX_POINTS;
                
                if (sendPoints <BLOCK_POINTS)                          // If we are in danger of being limited
                {
                    try {
                        Thread.sleep(THRESHOLD_DELAY);             // Wait until we get one point back
                        sendPoints++;
                    }
                    catch (InterruptedException ie) {
                    }
                }
            }
        }
        TocIMCommand im=new TocIMCommand(recipient,msg,auto);
        
        sendTocCommand(im);
        sendPoints--;
        if (debug) {
            System.out.println("Points="+sendPoints);
        }
        
        lastMessageSendTime=System.currentTimeMillis();
    }
    
    
    /** Add a buddy to a group.  This information can be saved on the server by calling {@link #saveConfig}
     *  @param buddyName The normalised buddy name to add
     *  @param groupName The name of the group to add this buddy to
     *  @param pos the position in the group at which to add the buddy.
     *  @return The {@link Buddy} object that represents the specified buddy name.
     */
    
    public Buddy addBuddy(String buddyName, String groupName, int pos) {
        
        if (debug) {
            System.out.println("Adding "+buddyName+" to group "+groupName+" at position "+pos);
        }
        
        Buddy buddy;
        buddy=(Buddy)buddies.get(buddyName);
        if (buddy==null) {
            buddy=new Buddy(buddyName);
        }
        Group group=(Group)groups.get(groupName);
        if (group==null) {
            group=new Group(groupName);
            groups.put(groupName,group);
        }
        if (pos>group.getBuddyCount()||pos==-1) {
            group.addBuddy(buddy);
        }
        else {
            group.addBuddy(buddy,pos);
        }
        return(buddy);
    }
    
    /** Add a buddy to a group.  This information can be saved on the server by calling {@link #saveConfig}
     *  The buddy is added to the end of the group
     *  @param buddyName The normalised buddy name to add
     *  @param groupName The name of the group to add this buddy to
     *  @return The {@link Buddy} object that represents the specified buddy name.
     */
    
    public Buddy addBuddy(String buddyName, String groupName) {
        return(addBuddy(buddyName,groupName,-1));
    }
    
    /** Add a buddy to the watch list for this connection.
     * This method must be called after {@link #connect()}
     * It also appears that the login process will not complete unless at least one buddy is added to the watch list
     * @param buddy The nickname to add to the watch list
     * @throws JaimException if the method is called at the wrong time
     * @see JaimEventListener
     * @deprecated the {@link #watchBuddy} method should be used instead
     */
    public void addBuddy(String buddy) throws JaimException {
        watchBuddy(buddy);
        
    }
    
    /** Add a buddy to the watch list for this connection.
     * This method must be called after {@link #connect()}
     * It also appears that the login process will not complete unless at least one buddy is added to the watch list
     * @param buddy The nickname to add to the watch list
     * @throws JaimException if the method is called at the wrong time
     * @see JaimEventListener
     */
    public void watchBuddy(String buddy) throws JaimException {
        if (loggedIn) {
            try {
                TocAddBuddyCommand tab = new TocAddBuddyCommand();
                tab.addBuddy(buddy);
                sendTocCommand(tab);
            }
            catch (IOException e) {
                throw new JaimException(e.toString());
            }
        }
        
        watchedBuddies.put(buddy,buddy);
        
    }
    
    /** Save group/buddy list configuration to the TOC server
     * @throws IOException if a network error occurs
     */
    
    public void saveConfig() throws IOException {
        TocSetConfigCommand tsc=new TocSetConfigCommand();
        Iterator it =groups.keySet().iterator();
        while (it.hasNext()) {
            Group g = (Group)groups.get(it.next());
            tsc.addGroup(g);
        }
        sendTocCommand(tsc);
        
    }
    
    /** Return the set of groups that have been stored in the TOC server
     *  The information returned from this method is only valid if {@link #isConfigValid} returns true
     *  @return A Collection of {@link Group} Objects
     */
    
    public Collection getGroups() {
        return(groups.values());
    }

     /**
     *  Return a group, given its name
     *  @return A {@link Group} Object corresponding to the string name
     */
    
    public Group getGroupBy(String name) {
        Group result = (Group) groups.get(name);
        return result;
    }

    
    /** Indicate whether configuration information has been received from the TOC server.
     *  If this method returns true then the information returned by {@link #getGroups} is valid
     *  @return true if configuration information has been received from the TOC server.
     */
    
    public boolean isConfigValid() {
        return(configValid);
    }
    
    /** Send a warning or "Evil" to another user.  You must be involved in a communication with a user before you can warn them
     * @param buddy The nickname of the buddy to warn
     * @param anonymous true if the warning should be sent anonymously
     * @throws IOException if a network error occurs
     */
    public void sendEvil(String buddy,boolean anonymous) throws IOException {
        TocEvilCommand ec=new TocEvilCommand(buddy,anonymous);
        sendTocCommand(ec);
    }
    
    
    /** Set the information for the logged in user
     * @param information The information for this user (May contain HTML)
     * @throws IOException if a network error occurs
     */
    public void setInfo(String information) throws IOException {
        TocSetInfoCommand sic=new TocSetInfoCommand(information);
        sendTocCommand(sic);
    }
    
    /** Get the information for the specified user
     * @param username The screenname for whom info is requested (May contain HTML)
     * @throws IOException if a network error occurs
     */
    public void getInfo(String username) throws IOException {
        TocGetInfoCommand gic=new TocGetInfoCommand(username);
        sendTocCommand(gic);
    }
    
    /** Get an Input stream associated with a URL returned by the "GOTO_URL" toc response
     *@param file The "file" returned by calling GotoTocResponse#getURL
     *@return An InputStream connected to the specified URL
     *@throws IOException if an IO error occurs
     *@throws MalformedURLException if there is an error building the URL
     */
    
    
    public InputStream getURL(String file) throws IOException, MalformedURLException {
        URL URL;
        
        URL=new URL("http",host,port,file);
        
        return(URL.openStream());
    }
    
    
    
    /** Set the information for the logged in user
     * @param awayMsg The away message for this user.  May contain HTML. To cancel "away" status set the awayMsg to ""
     * @throws IOException if a network error occurs
     */
    public void setAway(String awayMsg) throws IOException {
        TocSetAwayCommand sic=new TocSetAwayCommand(awayMsg);
        sendTocCommand(sic);
    }
    
    
    /** Adds the specified buddy to your permit list.
     * @param buddy The buddy to add to your block list.  If this is an empty string, mode is changed to "permit none"
     * @throws JaimException if a network error occurs
     */
    public void addPermit(String buddy) throws JaimException {
        if (loggedIn) {
            try {
                TocAddPermitCommand tap = new TocAddPermitCommand();
                tap.addPermit(buddy);
                sendTocCommand(tap);
            }
            catch (IOException e) {
                throw new JaimException(e.toString());
            }
        }
    }
    
    /** Adds the specified buddy to your block list.
     * @param buddy The buddy to add to your block list.  If this is an empty string, mode is changed to "deny none"
     * @throws JaimException if a network error occurs
     */
    public void addBlock(String buddy) throws JaimException {
        if (loggedIn) {
            try {
                TocAddDenyCommand tad = new TocAddDenyCommand();
                tad.addDeny(buddy);
                sendTocCommand(tad);
            }
            catch (IOException e) {
                throw new JaimException(e.toString());
            }
        }
    }
    
    /** Called by receiver thread to indicate that the connection has been terminated by an IOException
     */
    
    private void connectionLost() {
        deliverEvent(new ConnectionLostTocResponse());
        logOut();
        connected=false;
    }
    
    
    
    /** Set the idle time for this user
     * @param idleSecs The number of seconds the user has been idle for.  Set to 0 to indicate current activity.  The server will increment the idle time if non-zero
     * @throws IOException if a network error occurs
     */
    public void setIdle(int idleSecs) throws IOException {
        TocSetIdleCommand sic=new TocSetIdleCommand(idleSecs);
        sendTocCommand(sic);
    }
    
    
    /** Delete a buddy from the buddy watch list.  The buddy should have been added with {@link #addBuddy } first.
     * The buddy list can only be modified after {@link #connect } is called.
     * @param buddy The buddy name to be deleted\
     * @deprecated use {@link #unwatchBuddy } instead
     */
    public void deleteBuddy(String buddy) {
        unwatchBuddy(buddy);
    }
    
    /** Delete a buddy from the buddy watch list.  The buddy should have been added with {@link #addBuddy } first.
     * The buddy list can only be modified after {@link #connect } is called.
     * @param buddy The buddy name to be deleted
     */
    public void unwatchBuddy(String buddy) {
        watchedBuddies.remove(buddy);
    }
    
    private class ReceiverThread extends Thread {
        private InputStream sin;
        private boolean exit;
        private JaimConnection parent;
        
        private ReceiverThread(JaimConnection parent) {
            this.parent=parent;
            exit=false;
        }
        
        private void setInputStream(InputStream in) {
            sin=in;
        }
        
        public void run() {
            if (debug) {
                System.out.println("Receiver starting");
            }
            FLAPInputFrame inframe=new FLAPInputFrame();
            try {
                while (!exit) {
                    try {
                        int i;
                        while ( !inframe.completeFrameRead()) {
                            i=sin.read();
                            inframe.addFrameData((byte)i);
                        }
                        try {
                            FLAPFrame fr=FLAPFrameFactory.createFLAPFrame(inframe.getFrameData());
                            parent.Dispatch(fr);
                        }
                        catch (FLAPFrameException ffe) {
                            if (debug) {
                                ffe.printStackTrace();
                            }
                        }
                        if (inframe.completeFrameRead()) {
                            inframe.resetInputFrame();
                        }
                    }
                    catch (InterruptedIOException iie) {
                        // We expect these because we are performing reads with a timeout
                    }
                }
            }
            catch (IOException e) {
                connectionLost();  // Indicate that we have lost our connection
                if (debug) {
                    e.printStackTrace();
                }
            }
        }
        
        private void pleaseExit() {
            exit=true;
        }
        
    }
    private class DeliveryThread extends Thread {
        private Vector messages;
        private boolean exit;
        private DeliveryThread() {
            messages=new Vector();
            exit=false;
        }
        
        private void deliverMessage(TocResponse tr) {
            synchronized(this) {
                messages.add(tr);
                this.notify();
            }
        }
        
        public void run() {
            if (debug) {
                System.out.println("Delivery Thread starting");
            }
            while (!exit) {
                if (messages.size()>0) {
                    TocResponse tr=(TocResponse)messages.remove(0);
                    doDelivery(tr);
                }
                else {
                    synchronized(this) {
                        try
                        {
                            this.wait();
                        }
                        catch (InterruptedException e)
                        {
                        }
                    }
                }
                
            }
        }
        
        private void doDelivery(TocResponse tr) {
            for (int i=0;i<eventListeners.size();i++) {
                JaimEventListener el=(JaimEventListener)eventListeners.elementAt(i);
                el.receiveEvent(new JaimEvent(this,tr));
            }
        }
        
        private void pleaseExit() {
            exit=true;
        }
        
        
    }
    
    
}
