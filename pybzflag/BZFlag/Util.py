""" BZFlag.Util

Small utilities that don't seem to fit anywhere else :)
"""
# 
# Python BZFlag Protocol Package
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
# 
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#  
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#  
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
# 

class Event:
    """An event that can be observed and triggered.
       This event can be called, and all observers will be
       called with the same arguments. Simple and effective :)
       The observers are called in an undefined order.

       The event can be constructed with a list of initial observers.
       This makes it easy to transparently make a member function
       an event with a line like:
         self.foo = Util.Event(self.foo)
       """
    def __init__(self, *args):
        self.clients = {}
        for arg in args:
            self.observe(arg)
        self.unhandledCallback = None

    def observe(self, callback):
        self.clients[callback] = 1

    def unobserve(self, callback):
        del self.clients[callback]

    def __call__(self, *args, **kw):
        if self.clients:
            for client in self.clients:
                client(*args, **kw)
        else:
            # No handlers- can we call the unhandled event callback?
            if self.unhandledCallback:
                self.unhandledCallback(*args, **kw)

    def trace(self, fmt):
        """A debugging aid- whenever this event is triggered,
           print a line using the supplied format string to
           represent the call parameters.
           """
        def traceCallback(*args, **kw):
            print fmt % args
        self.observe(traceCallback)


def initEvents(cls, *args):
    """A convenience function for setting up several transparent
       Event instances. Pass this your class instance and the names
       of all the functions you'd like turned into events.
       If any of the names specified don't exist yet, they are
       still set to a new Event instance, but no callback will
       be associated with it yet.
       """
    for arg in args:
        if hasattr(cls, arg):
            setattr(cls, arg, Event(getattr(cls, arg)))
        else:
            setattr(cls, arg, Event())

### The End ###
        
    
