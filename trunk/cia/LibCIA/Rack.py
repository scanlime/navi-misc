""" LibCIA.Rack

Like the shelve module, but more so :)

Rack is a higher level interface built on top of anydbm. Like shelve,
database values can be anything serializable by pickle. However, Rack
goes further and allows keys to be a subset of the objects serializable
by pickle, and introduces hierarchial namespaces within a database.

Since the pickle and cPickle modules can't guarantee that an object
always results in the same pickle (and frequently the pickle resulting
from one object does in fact change for no apparent reason) this module
includes a very simple pickler than handles a small set of python types.
Currently this includes strings, tuples, and integers. Lists are converted
to tuples. More types may be added in the future as long as the output
for any particular object never changes.
"""
#
# CIA open source notification system
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

import anydbm, cPickle, struct


class Rack(dict):
    """A dictionary object that, instead of representing in-memory data,
       represents objects stored in a database or other dictionary-like
       object that accepts strings for keys and values.

       Values can be any object serializable by cPickle. Keys and namespaces
       can be any object serializable by KeyPickler.

       Basic usage:

          >>> import Rack
          >>> r = Rack.open('/tmp/rack_test.db', flags='n')
          >>> r[12] = 'banana'
          >>> r['squid'] = 4
          >>> r[1,2,('boing', (None,))] = [1, 2, 4, 'Poing']
          >>> r.close()

          >>> r = Rack.open('/tmp/rack_test.db')
          >>> r['squid']
          4
          >>> r[1,2,('boing', (None,))]
          [1, 2, 4, 'Poing']

       Note that keys are a subset of pickleable objects while values
       can be any pickleable object:

          >>> r[6] = 5.2
          >>> r[5.2] = 6
          Traceback (most recent call last):
          ...
          TypeError: KeyPickler does not support <type 'float'>

       Namespaces:

          >>> eggs = r.namespace('eggs')
          >>> eggs[12]
          Traceback (most recent call last):
          ...
          KeyError: '12'
          >>> eggs[12] = ('spatula', 27)
          >>> r[12]
          'banana'
          >>> eggs[12]
          ('spatula', 27)

       Other mapping operators:

          >>> 6 in r
          True
          >>> 6 in eggs
          False
          >>> del r[6]
          >>> 6 in r
          False

       Rack supports keys(), values(), items(), and the iterator
       version of each on the root and any namespace:

          >>> r.items()
          [(12, 'banana'), ('squid', 4), ((1, 2, ('boing', (None,))), [1, 2, 4, 'Poing'])]
          >>> eggs.items()
          [(12, ('spatula', 27))]
       """
    def __init__(self, db, namespaces=(), pickleProtocol=-1, keyPickler=None):
        if not keyPickler:
            keyPickler = KeyPickler()
        self.db = db
        self.namespaces = namespaces
        self.pickleProtocol = pickleProtocol
        self.keyPickler = keyPickler

        # Since any one namespace will only have one set of sysNs keys
        # for the head and tail of its linked list, go ahead and pre-serialize these
        self._tailPointer = self._dumpKey(None, self.SYSNS_TAIL)
        self._headPointer = self._dumpKey(None, self.SYSNS_HEAD)

    def namespace(self, *ns):
        """Return a Rack object that refers to a namespace within this one.
           The namespace can be any object serializable by KeyPickler.
           If more than one argument is given, this burrows more than one
           level deep in the Rack's namespace hierarchy.
           """
        return self.__class__(self.db, self.namespaces + tuple(ns))

    def _dumpKey(self, key, sysNs=None):
        """Return a string representation of the given key.
           'sysNs' is an optional 'system namespace'. The value of a
           particular key is stored with sysNs=None, but other values
           of sysNs may be used to store metadata about the Rack
           out-of-band with the application's actual keys and values.
           By default this is the result of running KeyPickler on
           (namespaces, sysNs, key).
           """
        return self.keyPickler.dumps((self.namespaces, sysNs, key))

    # Values of sysNs currently in use:
    # Note that these can be any python value supported by KeyPickler,
    # but currently they are small integers so the resulting pickled
    # string is as small as possible.
    #
    # These sysNs values are used to maintain a singly linked list of all
    # keys inside a namespace. HEAD and TAIL are always added with
    # key==None, PREV and NEXT must exist for every key. The values
    # associated with these sysNs keys are dumpValue'ed representations of
    # normal keys, as would be passed to _dumpKey. They are -not- serialized
    # with keyPickler: they must be deserialized and reserialized anyway
    # to add namespaces and a sysNs, so we might as well make that fast.
    SYSNS_HEAD = 1
    SYSNS_TAIL = 2
    SYSNS_PREV = 3
    SYSNS_NEXT = 4

    def _dumpValue(self, value):
        """Return a string representation of the given value. By default
           this uses cPickle with the current protocol version set in
           our constructor.
           """
        return cPickle.dumps(value, self.pickleProtocol)

    def _loadValue(self, s):
        """Deserialize a value dumped by _dumpValue. By default this uses
           cPickle to unpickle the value.
           """
        return cPickle.loads(s)

    def close(self):
        self.db.close()

    def sync(self):
        self.db.sync()

    def turn(self):
        """Turn the rack!"""
        self.db.sync()

    def __setitem__(self, key, value):
        skey = self._dumpKey(key)
        if not self.db.has_key(skey):
            self._newKey(key)
        self.db[skey] = self._dumpValue(value)

    def __getitem__(self, key):
        try:
            return self._loadValue(self.db[self._dumpKey(key)])
        except KeyError:
            raise KeyError(repr(key))

    def has_key(self, key):
        return self.db.has_key(self._dumpKey(key))

    def __contains__(self, key):
        return self.db.has_key(self._dumpKey(key))

    def get(self, key, default=None):
        try:
            return self[key]
        except KeyError:
            return default

    def setdefault(self, key, default=None):
        try:
            return self[key]
        except KeyError:
            self[key] = default
            return default

    def __delitem__(self, key):
        try:
            del self.db[self._dumpKey(key)]
        except KeyError:
            raise KeyError(repr(key))
        self._delKey(key)

    def _newKey(self, key):
        """Append a new key to the linked list for this namespace"""
        dumpedKey = self._dumpValue(key)
        try:
            dumpedTail = self.db[self._tailPointer]
        except KeyError:
            # No tail, the list is empty. Set head and tail to this key
            self.db[self._headPointer] = dumpedKey
            self.db[self._tailPointer] = dumpedKey
            return

        # Add the new item at the tail
        tail = self._loadValue(dumpedTail)
        self.db[self._dumpKey(key, self.SYSNS_PREV)] = dumpedTail
        self.db[self._dumpKey(tail, self.SYSNS_NEXT)] = dumpedKey
        self.db[self._tailPointer] = dumpedKey

    def _delKey(self, key):
        """Delete a key from the linked list for this namespace"""
        nextPointer = self._dumpKey(key, self.SYSNS_NEXT)
        prevPointer = self._dumpKey(key, self.SYSNS_PREV)
        try:
            dumpedNext = self.db[nextPointer]
        except KeyError:
            dumpedNext = None
        try:
            dumpedPrev = self.db[prevPointer]
        except KeyError:
            dumpedPrev = None

        if dumpedNext and dumpedPrev:
            # We have both previous and next pointers. Link our
            # neighbours around this node and we're done.
            self.db[self._dumpKey(self._loadValue(dumpedPrev), self.SYSNS_NEXT)] = dumpedNext
            self.db[self._dumpKey(self._loadValue(dumpedNext), self.SYSNS_PREV)] = dumpedPrev

        elif dumpedNext and not dumpedPrev:
            # We have a next pointer but not a previous pointer-
            # this means we're the first node. Point the head
            # at the next node and delete the next node's previous pointer.
            self.db[self._headPointer] = dumpedNext
            del self.db[self._dumpKey(self._loadValue(dumpedNext), self.SYSNS_PREV)]

        elif dumpedPrev:
            # We have a previous pointer but not a next pointer- we're
            # the last node. Point the tail at our previous node and delete
            # the previous node's next pointer.
            self.db[self._tailPointer] = dumpedPrev
            del self.db[self._dumpKey(self._loadValue(dumpedPrev), self.SYSNS_NEXT)]

        else:
            # We're the last node in the namespace, delete head and tail pointers
            del self.db[self._headPointer]
            del self.db[self._tailPointer]

    def iterkeys(self):
        """A generator that iterates over this namespace's linked list of keys"""
        # Give up the moment we run into a node that doesn't exist
        try:
            n = self._loadValue(self.db[self._dumpKey(None, self.SYSNS_HEAD)])
            while n:
                yield n
                n = self._loadValue(self.db[self._dumpKey(n, self.SYSNS_NEXT)])
        except KeyError:
            return

    def itervalues(self):
        for key in self.iterkeys():
            yield self[key]

    def iteritems(self):
        for key in self.iterkeys():
            yield key, self[key]

    def keys(self):
        return list(self.keys())

    def values(self):
        return list(self.values())

    def items(self):
        return list(self.iteritems())


class KeyPickler(object):
    r"""A very simple pickler that handles a very restricted set of data types, but
        in return guarantees that a particular object will always generate the same
        pickled output, not true of the standard picklers. This doesn't support unpickling,
        since the standard cPickle or pickle modules are compatible with the resulting
        pickles.

        This pickler is quite simple, and currently only supports data structures composed
        of tuples, strings, None, and integers. (long or otherwise) It will pickle lists as tuples.
        It may be extended to support other types as long as the serialized representation
        of all supported objects stays constant. One nice side-effect of this pickler is
        that it usually produces pickles that are shorter than those generated by the standard
        picklers.

        A reading of python 2.3's pickletools module will probably help with the understanding
        of this class quite a bit.

        A simple example:

           >>> kpickler = KeyPickler()

           >>> p = kpickler.dumps('boing')
           >>> p
           'U\x05boing.'
           >>> cPickle.loads(p)
           'boing'

        An example showing off all our features:

           >>> p = kpickler.dumps(('boing', ['x', 'squeegie', None], 1, -1, 500, 12345678900000))
           >>> p
           '(U\x05boing(U\x01xU\x08squeegieNtK\x01I-1\nM\xf4\x01I12345678900000\nt.'
           >>> cPickle.loads(p)
           ('boing', ('x', 'squeegie', None), 1, -1, 500, 12345678900000L)

        Just for reference, cPickle's output on the above test:

           >>> p = cPickle.dumps(('boing', ['x', 'squeegie', None], 1, -1, 500, 12345678900000), 1)
           >>> p
           '(U\x05boingq\x01]q\x02(U\x01xU\x08squeegieq\x03NeK\x01J\xff\xff\xff\xffM\xf4\x01L12345678900000L\nt.'

        Notice that cPickle uses a longer BININT4 instead of INT for small negative numbers,
        and it fills the memo with unused values. Ok, maybe it's not that obvious unless you've
        spent too long looking at pickles :)
        Also don't be surprised if the above doctest fails, since the output of cPickle isn't
        guaranteed to be constant.
        """
    def dumps(self, obj):
        """Return a string holding a pickled representation of 'obj'.
           Lists will be coerced to tuples, unsupported types will raise
           a TypeError.
           """
        # Most of the work is done in _serialize, but we have to tack on
        # the stop instruction after it all.
        return self.serialize(obj) + '.'

    def serialize(self, obj):
        """Look up the data type of 'obj' and call the appropriate serializer"""
        try:
            f = getattr(self, 'serialize_' + type(obj).__name__)
        except AttributeError:
            raise TypeError("KeyPickler does not support %r" % type(obj))
        return f(obj)

    def serialize_tuple(self, obj):
        """Output a markobject, the serialized representation of all elements,
           then the 't' opcode, to turn all items between that and the markobject
           into a tuple.
           """
        return '(' + ''.join([self.serialize(i) for i in obj]) + 't'

    def serialize_list(self, obj):
        """We serialize lists as tuples. It would be easy to add real list support
           by simply using the 'l' opcode instead of 't', but for the purpose of making
           database keys it makes more sense to coerce lists into tuples.
           """
        return self.serialize_tuple(obj)

    def serialize_str(self, obj):
        """Serialize a string as either a BINSTRING ('T') or SHORT_BINSTRING ('U').
           This is deterministic, as any string that will fit in a SHORT_BINSTRING
           is always output as one.
           """
        if len(obj) < 0x100:
            return 'U' + struct.pack('<B', len(obj)) + obj
        return 'T' + struct.pack('<I', len(obj)) + obj

    def serialize_int(self, obj):
        """Serialize an integer as either a BININT1 ('K'), BININT2 ('M'), or INT ('I').
           BININT4 is skipped since it isn't always shorter than INT, particularly
           for small negative numbers, and introduces a bit more complexity with
           having to handle signed binary numbers.
           """
        if obj >= 0 and obj < 0x100:
            return 'K' + struct.pack('<B', obj)
        if obj >= 0 and obj < 0x10000:
            return 'M' + struct.pack('<H', obj)
        return 'I' + str(obj) + '\n'

    def serialize_long(self, obj):
        """Our integer serializer can handle longs just fine too"""
        return self.serialize_int(obj)

    def serialize_NoneType(self, obj):
        return 'N'


def open(filename, flags='c', mode=0666, rootNamespace=(), pickleProtocol=-1):
    """Create a RackDict object from an anydbm-compatible database file"""
    return Rack(anydbm.open(filename, flags, mode), rootNamespace, pickleProtocol)

def _test():
    import doctest, Rack
    return doctest.testmod(Rack)

if __name__ == "__main__":
    _test()

### The End ###
