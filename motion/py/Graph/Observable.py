""" Graph.Observable

This module provides base classes for objects that are observable,
quantities that are observable, and expressions computed with
observable quantities. This is the basis for our rule system- the
trigger for a rule is represented as a composition of observable
quantities, and the rule's action is the observer callback.

The Event class is the basic observable object. It is effectively
a transparent callable proxy that can be observed. By default, all
observers are referenced weakly, so the observer's reference will
never be the only thing preventing an object from being garbage
collected. If your observer is important, make sure there is a
reference to it elsewhere.

For completeness, the Event class was originally written by
Micah Dowty for PyBZEngine, released under the LGPL.
"""

import weakref
import types

class Event:
    """An event that can be observed and triggered.

    This event can be called, and all observers will be called with the same
    arguments. Simple and effective :)

    The event can be constructed with a list of initial observers.  This makes
    it easy to transparently make a member function an event with a line like:
    	self.foo = Observable.Event (self.foo)
    """

    __slots__ = ['unhandledCallback', 'clients', 'callables', '__weakref__']

    def __init__ (self, *args):
        self.empty ()

        for arg in args:
            self.observe (arg)
        self._frozen = False
        self._frozenNotify = None
        self.unhandledCallback = None

    def observe (self, callback, *args, **kwargs):
	"""Call the supplied calllback with the same arguments when this event
	is called.

	Callbacks are referenced weakly, so their observation ends when the
	callback is not used anywhere else.

	This means that lines like the following won't work, since the callback
	immediately becomes unreferenced and is removed:
            foo.observe (lambda: self.boing (5))
            foo.observe (SuperClass ().frob)

	To solve these issues you must reference the callback at least one
	other place.  This could be in the local namespace of your script, in
	your class' dictionary, etc.  It is usually best to replace lambda
	expressions with class member functions.  The same is true of functions
	declared inside another function's body.
        """
        if kwargs.has_key ('index'):
            index = kwargs['index']
        else:
            index = -1

        if not self.clients.has_key (hash (callback)):
            if type (callback) is types.MethodType:
                # We tread bound method objects as a special case. Bound methods
                # are created when a class.method expression is evaluated, tying
                # the class and method together into a single callable object.
                # Unfortunately, these objects don't have the lifetime of the class
                # in question, they are usually discarded immediately. Here we
                # detect bound methods, and store proper weakrefs to the method
                # and the class.
                callable = _EventMethodObserver (self, callback)
            else:
                # The default wrapper for weak references. This class handles
                # deleting the client list entry properly when the callback
                # is deleted.
                callable = _EventObserver (self, callback)

            # Store the callable in the clients hash and in the callables list.
            # Insert it in the given position. We add the list length if our index
            # is negative, to be consistent with python indexing.
            self.clients[hash (callback)] = callable
            if index < 0:
                index += len (self.callables) + 1
            self.callables.insert (index, callable)
            self.args[hash (callback)] = (args, kwargs)

    def strongObserve (self, callback, *args, **kwargs):
        """Like observe (), but use a normal reference rather than a weak ref.

	This is necessary for using lambdas or other types of temporary
	functions as obsrvers.  The only way for these observers to be removed
	is explicitly calling unobserve().
        """
        if kwargs.has_key ('index'):
            index = kwargs['index']
        else:
            index = -1

        if not self.clients.has_key (hash (callback)):
            # We don't need to worry with a wrapper class since we don't need a weakref
            self.clients[hash (callback)] = callback
            if index < 0:
                index += len (self.callables) + 1
            self.callables.insert (index, callback)
            self.args[hash (callback)] = (args, kwargs)

    def observeOnce (self, callback, *args, **kwargs):
        """Install an observer that is automatically removed after it fires once.

        This uses a strong reference.
        """
        def oneshot (*args, **kwargs):
            self.unobserve (oneshot)
            return callback (*args, **kwargs)
        self.strongObserve (oneshot, *args, **kwargs)

    def empty (self):
        # Our client dict maps from callback hashes to callables.
        # This makes using weakref proxy objects easy without making unobserve() a pain.
        # Note that we must use the hash rather than the object itself, since the whole
        # point is to avoid requiring a reference to the callable.
        self.clients = {}

        # A list of callables, to preserve insertion order
        self.callables = []

        # A dictionary of arguments for each callback
        self.args = {}

    def replace (self, callback, *args, **kwargs):
        """Remove all existing observers for this event, and add the given one"""
        self.empty ()
        self.observe (callback, *args, **kwargs)

    def unobserve (self, callback):
        """Stop calling callback() when this event is triggered"""
        callable = self.clients[hash (callback)]
        del self.clients[hash (callback)]
        del self.args[hash (callback)]
        self.callables.remove (callable)

    def __call__ (self, *args, **kw):
        """Trigger this event by calling it.
	
	The parameters passed to the event will be broadcast to all of its
	observers.
	"""
        self._notify (*args, **kw)

    def _notify (self, *args, **kw):
        """Actually invoke the callables.
	
	This is always run from an "IO thread" as defined by Twisted. Usually
	this is the application's main thread.
	"""
        # If we're frozen, this notification get saved for later.
        # We only emit one final notification after unfreezing, so
        # only the most recent one gets stored.
        if self._frozen:
            self._frozenNotify = (args, kw)
            return

        if self.callables:
            # Iterate over a copy of the callables list, since the original
            # may be modified by our clients.

            callables = self.callables[:]
            for client in callables:
                # Retrieve the hash(callback) for this client. If we're
                # using a wrapper function this is in the callbackHash
                # attribute, otherwise we can take the hash directly.
                if hasattr (client, 'callbackHash'):
                    callbackHash = client.callbackHash
                else:
                    callbackHash = hash (client)

                # Add stored args for this client if we have any
                storedArgs, storedKwargs = self.args[callbackHash]
                mergedArgs = args + storedArgs
                mergedKwargs = dict (kw)
                mergedKwargs.update (storedKwargs)

                r = client (*mergedArgs, **mergedKwargs)

                # Allow the client to abort
                if r is not None:
                    return r

        else:
            # No handlers- can we call the unhandled event callback?
            if self.unhandledCallback:
                self.unhandledCallback (*args, **kw)

    def trace (self, fmt):
        """A debugging aid, prints a line of text whenever this event is triggered.

        fmt can be a printf-style format string that can include references
        to both positional and keyword arguments, for example:
             %(2)s       = The second argument, as a string
             %(boing)d   = keyword argument 'boing', as an integer

        fmt can also be a callable expression (created with lambda, for example)
        that will be called with the event's arguments and the result will be print'ed.

        A reference is returned to the trace's callback function. The trace is
        referenced strongly, so it will not disappear if you discard this reference,
        but the reference can be passed to unobserve () later to cancel the trace.
        """
        def traceCallback (*args, **kw):
            if type (fmt) == str:
                # Make a dictionary with both keyword args and normal
                # args, representing normal args by their place in the
                # argument list, starting with 1.
                index = 1
                for arg in args:
                    kw[str (index)] = arg
                    index += 1
                print fmt % kw
            else:
                print fmt(*args, **kw)
        self.strongObserve (traceCallback)
        return traceCallback

    def freeze (self):
        """Stop sending notifications for this event.
	
	If the event was invoked while frozen, thaw () will send only the most
	recent one.
        """
        self._frozen = True

    def thaw (self):
	"""Reverse the effects of freeze () and send at most one pending
	notification event.
        """
        self._frozen = False
        if self._frozenNotify is not None:
            args, kw = self._frozenNotify
            self._frozenNotify = None
            self._notify (*args, **kw)


def attachEvents (cls, *args):
    """A convenience function for setting up several transparent Event
    instances.
       
    Pass this your class instance and the names of all the functions you'd like
    turned into events.  If any of the names specified don't exist yet, they
    are still set to a new Event instance, but no callback will be associated
    with it yet.

    Note that Events are only suitable for use with functions without useful
    return values, however all arguments are passed through to observers.
    """
    for arg in args:
        if hasattr (cls, arg):
            setattr (cls, arg, Event (getattr (cls, arg)))
        else:
            setattr (cls, arg, Event ())

class _EventObserver:
    """A helper class for Event that wraps the callback in a weakref and
    automatically self-destructs when it's time.
    """

    __slots__ = ["event", "callbackHash", "ref", "__weakref__"]

    def __init__ (self, event, callback):
        self.event = event
        self.callbackHash = hash (callback)
        self.ref = weakref.ref (callback, self.unref)

    def __repr__ (self):
        return "<%s to %r>" % (self.__class__.__name__, self.ref)

    def __call__ (self, *args, **kw):
        self.ref ()(*args, **kw)

    def unref (self, ref):
        try:
            del self.event.clients[self.callbackHash]
            self.event.callables.remove (self)
        except KeyError:
            # Hmm, something else already deleted it
            pass

class _EventMethodObserver:
    """An alternative to _EventObserver that is used for bound methods.

    Since bound methods are usually one-time-use objects, this stores the
    unbound method and a weakref to the bound method's instance.
    """

    __slots__ = ["event", "callbackHash", "im_self_ref", "im_func", "__weakref__"]

    def __init__ (self, event, callback):
        self.event = event
        self.callbackHash = hash (callback)
        self.im_self_ref = weakref.ref (callback.im_self, self.unref)
        self.im_func = callback.im_func

    def __repr__ (self):
        return "<%s to %r::%r>" % (self.__class__.__name__,
                                   self.im_self_ref, self.im_func)

    def __call__ (self, *args, **kw):
        im_self = self.im_self_ref ()
        if im_self is not None:
            self.im_func (im_self, *args, **kw)
        else:
            raise weakref.ReferenceError

    def unref (self, ref):
        try:
            del self.event.clients[self.callbackHash]
            self.event.callables.remove (self)
        except KeyError:
            # Hmm, something else already deleted it
            pass

