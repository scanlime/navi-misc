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

This module includes a few simple data structures that sit on top of
dictionaries, databases, or Racks. This currently includes a ring buffer
and doubly-linked list. The linked list is used also used internally in
Rack to allow iteration over keys and sub-namespaces within any namespace.
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

from __future__ import generators
import cPickle, struct


class RackSerializer(object):
    """Encapsulates the key and value serialization used by all
       classes inheriting from BaseRack. By default, this uses cPickle
       for values and KeyPickler for keys.
       """
    def __init__(self, keyPickler=None, pickleProtocol=-1):
        if not keyPickler:
            keyPickler = KeyPickler()
        self.keyPickler = keyPickler
        self.pickleProtocol = pickleProtocol

    def dumpKey(self, obj):
        return self.keyPickler.dumps(obj)

    def loadKey(self, s):
        return cPickle.loads(s)

    def dumpValue(self, obj):
        return cPickle.dumps(obj, self.pickleProtocol)

    def loadValue(self, s):
        return cPickle.loads(s)


class BaseRack(object):
    """The base class of both Rack and InternalRack. This supports the key
       and value serialization necessary for both, but doesn't handle assigning
       namespaces or keeping iterable lists of namespaces and keys.
       """
    def __init__(self, db,
                 path = (),
                 serializer = None,
                 ):
        if serializer is None:
            serializer = RackSerializer()
        self.db = db
        self.path = path
        self.serializer = serializer
        self._internalNs = None

    def _dumpKey(self, key):
        """Return a string representation of the given key, in the current namespace"""
        return self.serializer.dumpKey((self.path, self._internalNs, key))

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
        self.db[skey] = self.serializer.dumpValue(value)

    def __getitem__(self, key):
        try:
            return self.serializer.loadValue(self.db[self._dumpKey(key)])
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

    def update(self, d):
        for k in d.iterkeys():
            self[k] = d[k]

    def _newKey(self, key):
        """A hook that's called when a new key is created in this Rack"""
        pass

    def _delKey(self, key):
        """A hook that's called when a key is removed from this Rack"""
        pass


class UnlistedRack(BaseRack):
    """A BaseRack subclass that doesn't support iteration over its contents"""
    pass


class InternalRack(BaseRack):
    """A BaseRack subclass used to store metadata in a namespace orthogonal
       to the normal user-accessable namespaces.
       """
    def __init__(self, externalRack, name):
        BaseRack.__init__(self,
                          externalRack.db,
                          externalRack.path,
                          externalRack.serializer)
        self._internalNs = name


class Rack(BaseRack):
    """A dictionary-like object that, instead of representing in-memory data,
       represents objects stored in a database or other dictionary-like
       object that accepts strings for keys and values, such as an anydbm
       database. This module includes an open() function that opens a
       database file with anydbm and constructs a Rack around it.

       Values can be any object serializable by cPickle. Keys and namespaces
       can be any object serializable by KeyPickler. This class builds on
       BaseRack by allowing iteration over keys and namespaces.

       Basic usage:

          >>> db = {}
          >>> r = Rack(db)
          >>> r[12] = 'banana'
          >>> r['squid'] = 4
          >>> r[1,2,('boing', (None,))] = [1, 2, 4, 'Poing']

          >>> del r
          >>> r = Rack(db)

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

          >>> eggs = r.child('eggs')
          >>> eggs[12]
          Traceback (most recent call last):
          ...
          KeyError: '12'
          >>> eggs[12] = ('spatula', 27)
          >>> eggs.parent()[12]
          'banana'
          >>> eggs[12]
          ('spatula', 27)

          >>> list(r.catalog())
          ['eggs']
          >>> del eggs[12]
          >>> eggs
          {}
          >>> list(r.catalog())
          []

       Other mapping operators:

          >>> 6 in r
          True
          >>> del r[6]
          >>> 6 in r
          False

       Rack supports keys(), values(), items(), and the iterator
       version of each on the root and any namespace:

          >>> r.items()
          [(12, 'banana'), ('squid', 4), ((1, 2, ('boing', (None,))), [1, 2, 4, 'Poing'])]
       """
    def child(self, *ns):
        """Return a new Rack object that refers to a namespace within this one.
           The namespace can be any object serializable by KeyPickler.
           If more than one argument is given, this burrows more than one
           level deep in the Rack's namespace hierarchy.
           """
        return Rack(self.db, self.path + ns, self.serializer)

    def unlistedChild(self, *ns):
        """Like Child, but the child doesn't show up in this rack's list of
           namespaces and doesn't keep its own list of keys. It isn't iterable.
           """
        return UnlistedRack(self.db, self.path + ns, self.serializer)

    def parent(self):
        """Retrieve the parent namespace of this one"""
        if len(self.path) > 0:
            return Rack(self.db, self.path[:-1], self.serializer)
        else:
            return None

    def _getKeyList(self):
        """Return a LinkedList holding all keys in this namespace"""
        return LinkedList(InternalRack(self, 1))

    def _getSubNsList(self):
        """Return a LinkedList holding all namespaces directly under this one"""
        return LinkedList(InternalRack(self, 2))

    def _testKeySubNsAdd(self):
        """To be run before adding a new key or subnamespace,
           checks whether this is the first one and if so adds
           this to the parent's subnamespace list.
           """
        if len(self._getKeyList()) == 0 and len(self._getSubNsList()) == 0:
            parent = self.parent()
            if parent:
                parent._newChild(self.path[-1])

    def _testKeySubNsDel(self):
        """To be run after deleting a key or subnamespace,
           checks whether that was the last one and if so
           deletes this from the parent's subnamespace list.
           """
        if len(self._getKeyList()) == 0 and len(self._getSubNsList()) == 0:
            parent = self.parent()
            if parent:
                parent._delChild(self.path[-1])

    def _newKey(self, key):
        """Append a new key to the linked list for this namespace"""
        self._testKeySubNsAdd()
        self._getKeyList().append(key)

    def _delKey(self, key):
        """Delete a key from the linked list for this namespace"""
        try:
            self._getKeyList().remove(key)
        except KeyError:
            # This shouldn't happen, but what do we do if it does?
            pass
        self._testKeySubNsDel()

    def _newChild(self, child):
        """Add a new child namespace to this rack's subnamespace catalog"""
        self._testKeySubNsAdd()
        self._getSubNsList().append(child)

    def _delChild(self, child):
        """Delete a child from this rack's subnamespace catalog"""
        try:
            self._getSubNsList().remove(child)
        except KeyError:
            # This shouldn't happen, but what do we do if it does?
            pass
        self._testKeySubNsDel()

    def clear(self):
        # Since we don't want to change the linked list while iterating over it,
        # first we delete all keys, then we clear the linked list, and finally
        # check whether this subnamespace needs to be removed.
        for key in self:
            del self.db[self._dumpKey(key)]
        self._getKeyList().clear()
        self._testKeySubNsDel()

    def __iter__(self):
        """Iterate over all keys in this rack namespace"""
        return self._getKeyList().__iter__()

    def catalog(self):
        """Iterate over all child namespaces"""
        return self._getSubNsList().__iter__()

    def __repr__(self):
        return repr(dict(self.iteritems()))

    def iterkeys(self):
        return self.__iter__()

    def itervalues(self):
        for key in self.iterkeys():
            yield self[key]

    def iteritems(self):
        for key in self.iterkeys():
            yield key, self[key]

    def keys(self):
        return list(self.iterkeys())

    def values(self):
        return list(self.itervalues())

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


class LinkedList(object):
    """A doubly-linked list implemented on top of a dictionary-like object such
       as a Rack. Items in the list must be usable as keys in the underlying
       dictionary- for a standard dict this means they must be hashable, for
       a Rack this means they must be compatible with KeyPickler.

       This linked list supports efficient insertion, deletion, and forward
       or backward iteration. The methods supported by this object overlap in
       many ways with those supported by a standard Python list, but they
       are not a superset or a subset.

          >>> db = {}
          >>> l = LinkedList(db)

       Items can be inserted by appending or prepending only:

          >>> l.append(3)
          >>> l.prepend(2)
          >>> l.append(4)
          >>> l.prepend(1)
          >>> l.prepend(0)
          >>> l
          [0, 1, 2, 3, 4]
          >>> len(l)
          5

          >>> db
          {1: 0, 2: 4, 3: 5, (2, 1): 2, (1, 1): 0, (1, 3): 2, (1, 2): 1, (2, 0): 1, (1, 4): 3, (2, 2): 3, (2, 3): 4}

       Any item can be removed in constant time:

          >>> l.remove(4)
          >>> l
          [0, 1, 2, 3]
          >>> l.remove(0)
          >>> l
          [1, 2, 3]
          >>> l.remove(2)
          >>> l
          [1, 3]
          >>> l.append(10)
          >>> l
          [1, 3, 10]
          >>> len(l)
          3

       When the list is empty, it takes no space in the db:

          >>> l.clear()
          >>> l
          []
          >>> len(l)
          0

          >>> db
          {}
       """
    def __init__(self, db):
        self.db = db

        # We always use the same keys for the list's head, tail, and item count
        self._headKey = 1
        self._tailKey = 2
        self._countKey = 3

    def _getPrevKey(self, item):
        """Get a key used for the given item's previous item pointer"""
        return (1, item)

    def _getNextKey(self, item):
        """Get a key used for the given item's next item pointer"""
        return (2, item)

    def __repr__(self):
        return repr(list(self))

    def __iter__(self):
        """Efficiently iterate from the beginning of the list to the end"""
        try:
            i = self.db[self._headKey]
            while True:
                yield i
                i = self.db[self._getNextKey(i)]
        except KeyError:
            pass

    def reversed(self):
        """A reverse iterstor, iterate from the end of the list to the beginning"""
        try:
            i = self.db[self._tailKey]
            while True:
                yield i
                i = self.db[self._getPrevKey(i)]
        except KeyError:
            pass

    def __len__(self):
        try:
            return self.db[self._countKey]
        except KeyError:
            return 0

    def clear(self):
        """Delete the entire contents of this list"""
        try:
            i = self.db[self._headKey]
        except KeyError:
            # nothing to delete
            return

        while i:
            # Save the next key first
            try:
                next = self.db[self._getNextKey(i)]
            except KeyError:
                next = None
            self.remove(i)
            i = next

    def append(self, item):
        self.db[self._countKey] = len(self) + 1

        try:
            tailItem = self.db[self._tailKey]
        except KeyError:
            # No tail, the list is empty. Set head and tail to this key
            self.db[self._headKey] = item
            self.db[self._tailKey] = item
            return

        # Add the new item at the tail
        self.db[self._getPrevKey(item)] = tailItem
        self.db[self._getNextKey(tailItem)] = item
        self.db[self._tailKey] = item

    def prepend(self, item):
        self.db[self._countKey] = len(self) + 1

        try:
            headItem = self.db[self._headKey]
        except KeyError:
            # No head, the list is empty. Set head and tail to this key
            self.db[self._headKey] = item
            self.db[self._tailKey] = item
            return

        # Add the new item at the tail
        self.db[self._getNextKey(item)] = headItem
        self.db[self._getPrevKey(headItem)] = item
        self.db[self._headKey] = item

    def remove(self, item):
        nextKey = self._getNextKey(item)
        prevKey = self._getPrevKey(item)
        try:
            nextItem = self.db[nextKey]
        except KeyError:
            nextItem = None
        try:
            prevItem = self.db[prevKey]
        except KeyError:
            prevItem = None

        if prevItem and nextItem:
            # We have both previous and next pointers. Link our
            # neighbours around this node and we're done.
            self.db[self._getNextKey(prevItem)] = nextItem
            self.db[self._getPrevKey(nextItem)] = prevItem
            del self.db[prevKey]
            del self.db[nextKey]

        elif nextItem and not prevItem:
            # We have a next pointer but not a previous pointer-
            # this means we're the first node. Point the head
            # at the next node and delete the next node's previous pointer.
            self.db[self._headKey] = nextItem
            del self.db[self._getPrevKey(nextItem)]
            del self.db[nextKey]

        elif prevItem:
            # We have a previous pointer but not a next pointer- we're
            # the last node. Point the tail at our previous node and delete
            # the previous node's next pointer.
            self.db[self._tailKey] = prevItem
            del self.db[self._getNextKey(prevItem)]
            del self.db[prevKey]

        else:
            # We're the last node in the namespace, delete head and tail pointers
            del self.db[self._headKey]
            del self.db[self._tailKey]

        newCount = len(self) - 1
        if newCount:
            self.db[self._countKey] = newCount
        else:
            del self.db[self._countKey]


class RingBuffer(object):
    """A FIFO buffer with a fixed maximum size. Objects can be pushed and popped,
       as you'd expect with a FIFO buffer. If an object is pushed into a full
       RingBuffer, the oldest object is discarded. It is also possible to return
       the first or last 'n' objects as lists, and iterate forwards or backwards.

       The RingBuffer uses a dictionary-like object as storage. This can be a
       normal dict, a Rack, or an anydbm-compatible database. The storage db
       includes keys for the numbers 0 through n-1, plus the following
       strings:

          head  : The key to place the next node at
          count : Total number of nodes in the buffer
          size  : Size of this buffer, can't be changed after it's created.
                  The size specified to the constructor is only
                  used when a new database has been created.

       Filling the FIFO with values:
       (Note that db will usually be an on-disk database rather than a dict)

         >>> db = {}
         >>> d = RingBuffer(db, 10)
         >>> list(d)
         []
         >>> d.push(None)
         >>> d.push('foo')
         >>> d.push('bar')
         >>> d.push((42, None))
         >>> list(d)
         [None, 'foo', 'bar', (42, None)]

       If you're curious what the db looks like..

         >>> db
         {'count': 4, 0: None, 'head': 4, 3: (42, None), 1: 'foo', 2: 'bar', 'size': 10}

       Numeric-style three argument slicing is supported:

         >>> d[1]
         'foo'
         >>> d[-1]
         (42, None)
         >>> list(d[-2:])
         ['bar', (42, None)]
         >>> list(d[::-1])
         [(42, None), 'bar', 'foo', None]

       Removing items from the FIFO:

         >>> list(d)
         [None, 'foo', 'bar', (42, None)]
         >>> list(d[:2])
         [None, 'foo']
         >>> d.pop(2)
         >>> list(d)
         ['bar', (42, None)]

       The FIFO discards old items when it overflows:

         >>> for i in xrange(1000):
         ...     d.push(i)
         >>> list(d)
         [990, 991, 992, 993, 994, 995, 996, 997, 998, 999]
         >>> list(d[-4:])
         [996, 997, 998, 999]

       Note that, being a ring buffer, the actual alignment of the
       keys used in the database relative to the beginning and
       end of the list will rotate:

         >>> db
         {'count': 10, 0: 996, 'head': 4, 3: 999, 4: 990, 5: 991, 6: 992, 7: 993, 8: 994, 9: 995, 2: 998, 1: 997, 'size': 10}

       """
    def __init__(self, db, size):
        self.db = db
        self.newSize = size
        self._loaded = False

    def _load(self, create):
        """Load important database keys, optionally creating them if necessary.
           This is called lazily rather than in __init__, so that ringbuffers
           that are never used don't take up any space.
           """
        if not self.db.has_key('size'):
            if create:
                # It's a new database, initialize the special keys
                self.db['head'] = 0
                self.db['count'] = 0
                self.db['size'] = self.newSize

        # Cache the special keys
        self.head = self.db['head']
        self.count = self.db['count']
        self.size = self.db['size']
        self._loaded = True

    def push(self, node):
        """Add the given node to the FIFO, overwriting
           the oldest entries if the buffer is full.
           """
        self._load(True)

        # Stow the new node at our head and increment it
        self.db[self.head] = node
        self.head = self.head + 1
        if self.head >= self.size:
            self.head -= self.size
        self.db['head'] = self.head

        # If we haven't just also pushed out an old item,
        # increment the count of items in our db.
        if self.count < self.size:
            self.count += 1
            self.db['count'] = self.count

    def pop(self, n):
        """Delete the 'n' oldest items from the RingBuffer. This does
           not return the items first, this should be done with the
           iterators and other functions below.
           """
        try:
            self._load(False)
        except KeyError:
            return

        # Delete the items we no longer need,
        # and most importantly decrease self.count
        key = (self.head - self.count) % self.size
        while n > 0 and self.count > 0:
            del self.db[key]
            key += 1
            if key == self.size:
                key = 0
            n -= 1
            self.count -= 1
        self.db['count'] = self.count

    def _iter(self, key, count, increment=1):
        """A general iterator for the RingBuffer that starts
           at the given key (after wrapping if necessary) and returns
           'count' items, incrementing the key by 'increment' after
           each value yielded.
           """
        key %= self.size
        while count > 0:
            yield self.db[key]
            key = (key + increment) % self.size
            count -= 1

    def __iter__(self):
        """Forward-iterate over every item in the ring buffer,
           oldest to newest
           """
        try:
            self._load(False)
        except KeyError:
            return iter([])

        return self._iter(self.head - self.count, self.count)

    def __len__(self):
        try:
            self._load(False)
        except KeyError:
            return 0
        return self.count

    def __getitem__(self, i):
        """Implements Numeric-style slicing for iterating forward
           or backward over any portion of the RingBuffer.
           """
        self._load(False)

        if type(i) == slice:
            # Normalize the slice a bit such that it doesn't
            # have any negative or None values
            start, stop, step = i.start, i.stop, i.step
            if start is None:
                start = 0
            elif start < 0:
                start += self.count
            if stop is None:
                stop = self.count
            elif stop < 0:
                stop += self.count
            if not step:
                step = 1

            # If we're iterating backwards, start at the end
            if step < 0:
                key = self.head - self.count + stop - 1
            else:
                key = self.head - self.count + start

            return self._iter(key, stop - start, step)
        else:
            if i < 0:
                i += self.count
            return self.db[(self.head - self.count + i) % self.size]

    def getLatest(self, limit=None):
        """Get a list of up to 'limit' of the most recently pushed items"""
        count = len(self)
        if limit is None or limit > count:
            limit = count
        if not limit:
            return []
        return list(self[-limit:])


def open(filename, flags='c', mode=0666, rootNamespace=(), serializer=None):
    """Create a Rack instance from an anydbm-compatible database file"""
    import anydbm
    return Rack(anydbm.open(filename, flags, mode), rootNamespace, serializer)


def _test():
    import doctest, Rack
    return doctest.testmod(Rack)

if __name__ == "__main__":
    _test()

### The End ###
