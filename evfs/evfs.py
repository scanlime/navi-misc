import gamin, threading, atexit, os, select

# Data types
SOURCE   = "source"
CHILD    = "child"
METADATA = "metadata"
PROGRESS = "progress"

# Status codes
EXISTS   = "exists"
ADDED    = "added"
REMOVED  = "removed"
CHANGED  = "changed"

class Event:
    """Events are sent by nodes to indicate both the initial
       state of their content and to report changes.

       Every event represents the status of a particular
       piece of data- sources, children of a particular node, etc.
       Events other than those about sources themselves also
       indicate which source they came from.
       """
    def __init__(self, dataType, status, data, source=None):
        self.dataType = dataType
        self.data = data
        self.status = status
        self.source = source

    def __repr__(self):
        if self.source is None:
            source = ""
        else:
            source = " from %r" % self.source 
        return "<%s: %s %s %s%s>" % (
            self.__class__.__name__, self.dataType, self.data,
            self.status, source)

class EventSession:
    """EventSession represents a particular open() on a Node, a distinct stream
       of events with any number of listener functions.

       Opening a Node a second time and adding a new listener to an EventSession
       both provide a way to send Events to a new consumer, but there are
       important differences between the two methods. Adding listeners to an
       EventSession will tap into an existing stream of events, making it
       a very lightweight operation, but you don't receive 'exists' events
       unless they haven't already been sent. Open is a heavier operation since
       it must be sent all the way to the event source(s), but you are guaranteed
       to get a complete picture of the Node's current status.

       The session is automatically closed once its listener count reaches zero.
       """
    def __init__(self, *listeners):
        self.listeners = list(listeners)
        self.onClose = []
        self.closed = False

    def addListener(self, listener):
        assert not self.closed
        assert listener not in self.listeners
        self.listeners.append(listener)

    def removeListener(self, listener):
        assert not self.closed
        self.listeners.remove(listener)
        if not listeners:
            self.closed = True
            for handler in self.onClose(self):
                handler(self)

class Node(object):
    """This is an abstract base class for Nodes (analagous to
       files or directories) in the event-driven filesystem.

       Every Node has a name that identifies it uniquely within
       a particular tree level. Every node can be opened, creating
       a new EventSession instance which delivers Events relating
       the node's current status and any status changes.

       Concrete Node classes override the _open() method to
       provide a source of events to the new EventSession.
       """
    def __init__(self, name):
        self.name = name

    def __repr__(self):
        return "<%s %r>" % (self.__class__.__name__, self.name)

    def open(self, session):
        """Provided a new and previously unopened EventSession, this
           opens it and begins dispatching events to it.

           Note that the listeners already attached to the EventSession
           are the only ones guaranteed to see the complete state of the
           Node as it is now. Listeners attached after this may miss
           EXISTS events.

           For convenience, returns the EventSession. The caller should
           retain this reference for adding/removing listeners later.
           The session will close when the last listener is removed.
           """
        session.onClose.append(self.close)
        self._open(session)
        return session

    def close(self, session):
        """Explicitly disconnect this Node from the provided session.
           Normally this is not necessary, as the session will automatically
           be closed when all listeners are removed.
           """
        self._close(session)

    def _dispatch(self, session, event):
        """Subclasses call this to actually send an event to
           a particular EventSession. This exists here rather
           than in EventSession itself to allow Nodes to implement
           their own event filtering separated from the EventSession
           instance itself. This is important when an EventSession
           is shared between multiple nodes.
           """
        for listener in session.listeners:
            listener(event)

    def _open(self, session):
        """Subclasses override this to initiate a stream of events
           to a new EventSession instance.
           """
        pass

    def _close(self, session):
        """Subclasses override this to terminate the event stream
           to a particular EventSession instance.
           """
        pass

class SourceNode(Node):
    """This is a Node subclass for use by the original source of Events,
       rather than a Node which acts as a filter of some sort.

       The primary function of this node is to send events with
       dataType=SOURCE, and to tag all events with their
       original source.
       """
    def open(self, session):
        session.onClose.append(self.close)
        self._dispatch(session, Event(SOURCE, ADDED, self))
        self._open(session)
        return session

    def close(self, session):
        self._close(session)
        # This only matters if the session was closed manually, so it
        # still has some listeners attached.
        self._dispatch(session, Event(SOURCE, REMOVED, self))

    def _dispatch(self, session, event):
        event.source = self
        Node._dispatch(self, session, event)

class UnionNode(Node):
    """A UnionNode acts as a proxy for merging several other Nodes
       into a single Node. Source nodes can be added/removed at any
       time.
       """
    def __init__(self, name, *sources):
        Node.__init__(self, name)
        self.sources = list(sources)
        self.sessions = []

    def open(self, session):
        session.onClose.append(self.close)
        self.sessions.append(session)
        for source in self.sources:
            source.open(session)

    def close(self, session):
        self.sessions.remove(session)
        # No need to pass this on to each source, they
        # have their own onClose callbacks.
        # FIXME: what do do on a manual close?

    def addSource(self, source):
        self.sources.append(source)
        for session in self.sessions:
            source.open(session)

    def removeSource(self, source):
        self.sources.remove(source)
        for session in self.sessions:
            source.close(session)

class TempNode(SourceNode):
    """This Node acts as an in-memory filesystem. Children, metadata,
       and such can be added or removed at any time, and are stored
       nonpersistently.
       """
    def __init__(self, name):
        Node.__init__(self, name)
        self.sessions = []
        # A list of (dataType, data) tuples
        self.content = []

    def add(self, dataType, data):
        self.content.append((dataType, data))
        for session in self.sessions:
            self._dispatch(session, Event(dataType, ADDED, data))

    def remove(self, dataType, data):
        self.content.remove((dataType, data))
        for session in self.sessions:
            self._dispatch(session, Event(dataType, REMOVED, data))

    def _open(self, session):
        self.sessions.append(session)
        for dataType, data in self.content:
            self._dispatch(session, Event(dataType, EXISTS, data))

    def _close(self, session):
        self.sessions.remove(session)

class GaminThread(threading.Thread):
    """This is a singleton thread that runs the gamin main loop"""
    def __init__(self):
        threading.Thread.__init__(self, name=self.__class__.__name__)

        self.mon = gamin.WatchMonitor()

        self.running = True
        self.pipe = os.pipe()
        atexit.register(self.stop)
        self.start()

    def stop(self):
        """Signal the thread to stop immediately, using a run flag and pipe"""
        self.running = False
        os.write(self.pipe[1], "x")

    def run(self):
        fd = self.mon.get_fd()
        while self.running:
            select.select([fd, self.pipe[0]], [], [])
            if self.mon.event_pending():
                self.mon.handle_events()
        self.mon.disconnect()

_gaminThread = None

def getGaminMonitor():
    global _gaminThread
    if not _gaminThread:
        _gaminThread = GaminThread()
    return _gaminThread.mon

class GaminNode(SourceNode):
    """This Node monitors a directory on disk using Gamin"""
    def __init__(self, path, name=None):
        if name is None:
            name = os.path.basename(path)
        Node.__init__(self, name)
        self.path = path

    def _open(self, session):
        getGaminMonitor().watch_directory(self.path, self._callback, session)
    
    def _close(self, session):
        # FIXME
        pass

    eventMapping = {
        gamin.GAMExists: EXISTS,
        gamin.GAMCreated: ADDED,
        gamin.GAMDeleted: REMOVED,
        gamin.GAMChanged: CHANGED,
        }

    def _callback(self, name, status, session):
        # Don't listen to the events generated on this directory itself
        if name.startswith(os.sep):
            return

        # Everything else maps directly to one of our events
        evfsStatus = self.eventMapping.get(status)
        if evfsStatus is not None:
            child = self.__class__(os.path.join(self.path, name), name)
            self._dispatch(session, Event(CHILD, evfsStatus, child))
