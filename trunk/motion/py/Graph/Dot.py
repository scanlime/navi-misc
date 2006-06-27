"""Tools for printing graphs in a dot(1) format.

This module provides base classes for nodes and edges that need to be printed
to files readable by dot(1). The `dotString` function in each class returns a
string representation of the node or edge formatted for dot(1). Additionally,
the `__str__` function in each class is defined to use this `dotString`
function as well. The `DotPrint` class is an `Algorithm` that prints an entire
graph formatted for dot(1).
"""

# Copyright (C) 2006 W. Evan Sheehan
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

import string

class Printable:
    """Base class for all printable graph objects.

    Provides basic initialization for attributes, defines the interface, and
    sets the `__str__` function to return the value of `dotString`.
    """

    __slots__ = ["dotAttrs"]

    def __init__ (self, **kwargs):
        """Initialize the ``dotAttrs`` member variable to the list of keyword
        arguments.

        Arguments:
            - ``kwargs``    An arbitrary list of name=value pairs of attributes
              for an element in dot. See the dot man page for information on
              available attributes.
        """
        self.dotAttrs = kwargs

    def dotString (self):
        """Return a string representing this graph element in the dot format.
        
        All printable classes need to override this function.
        """
        raise Exception ("dotString not overridden in %s" % (self.__class__.__name__))

    def __str__ (self):
        """By default, the str() representation of these classes is their
        dot-formatted string.
        """
        return self.dotString ()


class Node (Printable):
    """Printable node base class."""

    def dotString (self):
        """Return a string representing a node in the dot file format."""
        id = str (hash (self))
        attrs = ['%s="%s"' % (key, value) for key, value in \
                self.dotAttrs.iteritems ()]

        return "%s [%s];" % (id, string.join (attrs, ','))


class Edge (Printable):
    """Printable edge base class."""

    __slots__ = ["u", "v", "dotAttrs"]

    def __init__ (self, u, v, **kwargs):
        """Initialize the edge with 2 nodes and a list of attributes.

        Arguments:
            - ``u``         The node from which the edge originates
            - ``v``         The node to which the edge goes
            - ``kwargs``    The attribute list
        """
        Printable.__init__ (self, **kwargs)
        self.u = u
        self.v = v

    def dotString (self):
        """Return a string representation of the edge formatted for dot."""
        uid, vid = (hash (self.u), hash (self.v))
        attrs = ['%s="%s"' % (key, value) for key, value in \
                self.dotAttrs.iteritems ()]
        return "%s -> %s [%s];" % (uid, vid, string.join (attrs, ','))

# vim: ts=4:sw=4:et

