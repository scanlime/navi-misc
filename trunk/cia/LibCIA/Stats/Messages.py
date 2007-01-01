""" LibCIA.Stats.Messages

This package provides a file format that stores a bounded amount
of recent message data, as an indexed ring buffer of compressed
SAX events.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2007 Micah Dowty <micah@navi.cx>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

import struct, os
import xml.sax, xml.dom.minidom

class CircularFile:
    """This is a file-like object wrapper that turns a fixed-size
       normal file into a file that wraps around at the end, creating
       a ring buffer. This can optionally work within a subset of
       the entire file by providing a smaller size and a nonzero origin.
       """
    def __init__(self, original, size, origin=0):
        self.original = original
        self.origin = origin
        self.size = size
        self.origin = origin

    def seek(self, offset):
        self.original.seek(self.origin + (offset % self.size))

    def tell(self):
        return self.original.tell() - self.origin

    def write(self, data, offset=None):
        """Write data to the circular file, wrapping around if we hit the end.
           """
        if offset is not None:
            self.seek(offset)
        initial = self.tell()
        dataLen = len(data)

        while dataLen > 0:
            final = initial + dataLen

            if final < self.size:
                # This write will fit without wrapping
                self.original.write(data)
                return

            elif final == self.size:
                # This write just barely fits, but the file pointer needs
                # to come back to the beginning afterwards.
                self.original.write(data)
                self.seek(0)
                return

            else:
                # This write will straddle the end of the file. Write
                # what we can now, and catch the rest later. This iterative
                # method handles writes that are larger than the entire
                # buffer without any special cases.
                chunkLen = self.size - initial
                chunk = data[:chunkLen]
                data = data[chunkLen:]
                dataLen -= chunkLen

                self.original.write(chunk)
                del chunk
                self.seek(0)
                initial = 0

    def read(self, dataLen, offset=None):
        """Read from the circular file, wrapping around if we hit the end"""
        blocks = []
        self.readBlocks(dataLen, blocks, offset)
        return ''.join(blocks)

    def readBlocks(self, dataLen, blocks, offset=None):
        """Like read(), but instead of returning a single string this
           adds each individual piece to the 'blocks' list. This is intended
           to improve efficiency when doing several reads into the
           same buffer.

           If an initial offset is given, we seek to that rather than
           reading the current offset in the file.
           """
        if offset is not None:
            self.seek(offset)
        offset = self.tell()

        while dataLen > 0:
            final = offset + dataLen

            if final < self.size:
                # This read will fit without wrapping
                blocks.append(self.original.read(dataLen))
                break

            elif final == self.size:
                # This read just barely fits, but the file pointer needs
                # to come back to the beginning afterwards.
                blocks.append(self.original.read(dataLen))
                self.seek(0)
                break

            else:
                # This read will straddle the end of the file
                chunkLen = self.size - offset
                dataLen -= chunkLen

                blocks.append(self.original.read(chunkLen))
                self.seek(0)
                offset = 0

    def readSlice(self, firstOffset, stoppingOffset):
        """Read all data starting at the firstOffset and stopping just before
           we reach the stoppingOffset. This is the same style of half-open range
           that python list slicing uses. stoppingOffset may be less than
           firstOffset, in which case we wrap around. If stoppingOffset equals
           firstOffset, this returns nothing.
           """
        return self.read((stoppingOffset - firstOffset) % self.size, offset=firstOffset)


class SAXEncoder(xml.sax.handler.ContentHandler):
    """Encodes a SAX event stream as a unicode text
       stream, with string memoization.

       Every event begins with a single unicode
       character that has an opcode packed into the low
       2 bits, and a paramter occupying all upper bits.

       Opcodes:

         0. The parameter specifies the number of endElement events to emit

         1. The parameter is a string length L. The following L bytes define
            a new string, which is also added to the memo.

         2. Emit a memoized string. The parameter is the memo ID of that string.
            Memo IDs start at zero, but the memo is generally pre-populated with
            a database-wide dictionary.

         3. Emit whitespace. The parameter is a number of blank spaces to insert.

       Strings, either included directly or copied from the memo, may appear in
       multiple forms:

         - In the default context, a string can be interpreted as either character
           data or as an element. Character data strings are prefixed with 0x01,
           while element data strings are prefixed with 0x02 plus their attribute
           count. After an element with attributes, the stream alternates between
           attribute name and attribute value contexts.

         - In attribute name/value contexts, no string prefix is used.

       This method of encoding actually makes an element's arity part of its name,
       as far as our compression is concerned.
       """
    def __init__(self, dictionary=()):
        self.dictionary = dictionary

    def startDocument(self):
        self.output = []
        self.memo = {}
        self.state = (None,)
        for item in self.dictionary:
            self.memoize(item)

    def memoize(self, string):
        self.memo[string] = len(self.memo)

    def startElement(self, name, attrs):
        self.flush()

        attrNames = attrs.getNames()
        self.encode_element(name, len(attrNames))

        for attr in attrNames:
            self.encode_string(attr)
            self.encode_string(attrs[attr])

    def endElement(self, name):
        if self.state[0] == 'END':
            self.state = ('END', self.state[1] + 1)
        else:
            self.flush()
            self.state = ('END', 1)

    def characters(self, content):
        if self.state[0] == 'CHAR':
            self.state = ('CHAR', self.state[1] + content)
        else:
            self.flush()
            self.state = ('CHAR', content)

    def flush(self):
        if self.state[0] == 'CHAR':
            self.encode_chardata(self.state[1])
        elif self.state[0] == 'END':
            self.encode_end(self.state[1])
        self.state = (None,)

    def encode_chardata(self, data):
        # Split the character data into leading whitespace,
        # a normal string, and trailing whitespace

        dlen = len(data)
        s = data.lstrip()
        slen = len(s)

        if slen != dlen:
            self.encode_whitespace(dlen - slen)
            data = s
            dlen = slen

        dlen = len(data)
        s = data.rstrip()
        slen = len(s)

        if s:
            self.encode_string(unichr(1) + s)

        if slen != dlen:
            self.encode_whitespace(dlen - slen)

    def encode_element(self, name, params):
        self.encode_string(unichr(params + 2) + name)

    def encode_string(self, data):
        if data in self.memo:
            self.encode_memoized(self.memo[data])
            return
        self.memoize(data)

        self.output.append(unichr((len(data) << 2) | 1))
        self.output.append(data)

    def encode_end(self, count):
        self.output.append(unichr(count << 2))

    def encode_memoized(self, index):
        self.output.append(unichr((index << 2) | 2))

    def encode_whitespace(self, count):
        self.output.append(unichr((count << 2) | 3))

    def getvalue(self):
        return u''.join(self.output).encode('utf-8')

def SAXDecoder(encoded, dictionary):
    """Decode the stream produced by SAXEncoder, returning a minidom tree.
       This function is a bit hard to follow and it relies on minidom
       internals- unfortunately it's a big speed bottleneck, so
       any extra performance is worth a little obfuscation here.
       """
    dom = xml.dom.minidom
    memo = list(dictionary)
    stack = [dom.Document()]
    attrs = None
    top = stack[-1]
    m = unicode(encoded, 'utf8')
    mlen = len(m)
    i = 0

    while i < mlen:
        op = ord(m[i])
        type = op & 3
        param = op >> 2

        # Decode a memoized string
        if type == 2:
            item = memo[param]
            i += 1

        # Decode and memoize a literal string
        elif type == 1:
            i += 1
            item = m[i:i+param]
            i += param
            memo.append(item)

        # End elements
        elif type == 0:
            del stack[-param:]
            top = stack[-1]
            i += 1
            continue

        # Whitespace
        elif type == 3:
            node = dom.Text()
            node.data = " " * param
            
            # _append_child(top, node)
            childNodes = top.childNodes
            if childNodes:
                last = childNodes[-1]
                node.__dict__["previousSibling"] = last
                last.__dict__["nextSibling"] = node
            childNodes.append(node)
            node.__dict__["parentNode"] = top
            node.__dict__["ownerDocument"] = stack[0]

            i += 1
            continue

        # Processing this string in the default context
        if attrs is None:
            op = ord(item[0])

            # Character data
            if op == 1:
                node = dom.Text()
                node.data = item[1:]

                # _append_child(top, node)
                childNodes = top.childNodes
                if childNodes:
                    last = childNodes[-1]
                    node.__dict__["previousSibling"] = last
                    last.__dict__["nextSibling"] = node
                childNodes.append(node)
                node.__dict__["parentNode"] = top
                node.__dict__["ownerDocument"] = stack[0]

            # Element with no attributes (fast path)
            elif op == 2:
                node = dom.Element(item[1:])

                # _append_child(top, node)
                childNodes = top.childNodes
                if childNodes:
                    last = childNodes[-1]
                    node.__dict__["previousSibling"] = last
                    last.__dict__["nextSibling"] = node
                childNodes.append(node)
                node.__dict__["parentNode"] = top
                node.__dict__["ownerDocument"] = stack[0]

                stack.append(node)
                top = node

            # Element with attributes, general
            else:
                attrRemaining = op - 2
                attrKey = None
                attrs = dom.Element(item[1:])

        # Attribute context
        else:

            # Attribute key
            if attrKey is None:
                attrKey = dom.Attr(item)

            # Attribute value
            else:
                d = attrKey.__dict__
                d["value"] = d["nodeValue"] = item
                d["ownerDocument"] = stack[0]

                # _set_attribute_node(attrs, attrKey)
                attrs._attrs[attrKey.name] = attrKey
                attrKey.__dict__['ownerElement'] = attrs

                attrRemaining -= 1
                attrKey = None

                # Finished an element with attributes?
                if not attrRemaining:
                    # _append_child(top, attrs)
                    childNodes = top.childNodes
                    if childNodes:
                        last = childNodes[-1]
                        node.__dict__["previousSibling"] = last
                        last.__dict__["nextSibling"] = node
                    childNodes.append(attrs)
                    attrs.__dict__["parentNode"] = top
                    attrs.__dict__["ownerDocument"] = stack[0]

                    stack.append(attrs)
                    top = attrs
                    attrs = None

    return stack[0]


class MessageBuffer:
    """This object represents on-disk storage for a stats target's
       recent messages. It is based on two ring buffers: one with
       message IDs, and one with message content.

       Message IDs are implemented as seek offsets into the content
       buffer. The IDs increase monotonically, are unique within
       this particular MessageBuffer, and are validated on access.

       Message content is encoded using a simple method of representing
       SAX event streams as Unicode strings. This gives about the same
       performance as gzip on small messages, but worse performance on
       large messages. The huge advantage of this SAX format, though,
       is that message buffers as a whole compress much better. This
       is important for backups.

       Format:
         - Fixed-size header
             - Magic number
             - Dictionary size
             - Index size
             - Content size
             - Tail pointer
         - Dictionary, NUL-separated UTF-8
         - Index ringbuffer
         - Content ringbuffer
       """
    magic = "CIA Message Buffer v1\r\n\x00"
    headerFmt = ">24sIIII"
    headerSize = struct.calcsize(headerFmt)

    #
    # Defaults only.
    # When opening existing databases, these are read from the header.
    #
    indexSize = 4096
    contentSize = 256 * 1024
    
    def __init__(self, path, filename="_msg", file=None):
        self.dirPath = path
        self.file = file
        self.filePath = os.path.join(path, filename)
        self._initialized = False

    def close(self):
        if self._initialized:
            self.file.close()
            self._initialized = False

    def _initRingBuffers(self):
        # Place our two ring buffers in the file. This should be called
        # as soon as the sizes in the header are all known.
        self.indexRing = CircularFile(self.file, self.indexSize,
                                      self.headerSize + self.dictionarySize)
        self.contentRing = CircularFile(self.file, self.contentSize,
                                        self.headerSize + self.dictionarySize +
                                        self.indexSize)

    def _init(self, msg=None):
        """Either read header data from an existing database, or set
           up the header and dictionary on a new database.

           If the database doesn't exist and 'msg' is not None, this
           message will be used to help build the dictionary.

           If the 'msg' is None, this will never set up a new database.
           If the database is not already initialized, it will return
           False and change nothing.
           """
        if not self.file:
            self.file = os.fdopen(os.open(self.filePath, os.O_RDWR | os.O_CREAT, 0666), 'w+')
        header = self.file.read(self.headerSize)

        if header:
            # Unpack the header
            try:
                (magic, self.dictionarySize, self.indexSize,
                 self.contentSize, self.tail) = struct.unpack(self.headerFmt, header)
            except struct.error:
                magic = None
            if magic != self.magic:
                raise ValueError("File header is incorrect")

            # Unpack the original dictionary
            self.dictionary = unicode(self.file.read(self.dictionarySize), 'utf8').split('\x00')
            self._initRingBuffers()

        elif msg is None:
            return False

        else:
            self.dictionary = self._createDictionary(msg)
            
            # Measure the encoded dictionary and set up critical sizes
            encodedDict = u'\x00'.join(self.dictionary).encode('utf8')
            self.dictionarySize = len(encodedDict)
            self._initRingBuffers()

            # Zero out the whole index, point to the beginning of it
            self.tail = 0
            self.indexRing.write('\x00' * self.indexSize, 0)

            # Initialize the dictionary
            self.file.seek(self.headerSize)
            self.file.write(encodedDict)

            self._commitHeader()

        self._initialized = True
        return True

    def _commitHeader(self):
        self.file.seek(0)
        self.file.write(struct.pack(
            self.headerFmt, self.magic, self.dictionarySize,
            self.indexSize, self.contentSize, self.tail))
        self.file.flush()

    def _createDictionary(self, msg):
        """Decide on a dictionary, using the first message as a hint"""
        #
        # Start out with a default dictionary that contains some
        # general-purpose strings, so that even if msg is some
        # pathological case we still have a somewhat usable
        # dictionary.
        #
        encoder = SAXEncoder((
            u'\x01name', u'\x03header', u'\x02file', u'\x01Received',
            u'\x02source', u'\x02project', u'\x02name', u'\x02generator',
            u'\x02body', u'\x02timestamp', u'\x02message', u'\x02commit',
            u'\x02author', u'\x02log', u'\x02files', u'\x02version',
            u'\x03file', u'action', u'From', u'Date', u'\x02mailHeaders',
            u'Message-Id', u'\x02module', u'modify', u'\x02revision',
            u'\x02url', u'\x02diffLines', u'\x01uri', u'\x02branch',
            u'\x04file', u'\x01add',
            ))

        # Feed in the first message, to populate the memo with strings
        # that may be common in this particular database.
        xml.sax.parseString(msg, encoder)

        #
        # Put shorter symbols first, since they're
        # more likely to be the common ones. Completely
        # discard anything really long, since it's likely
        # to be just a log message or something equally
        # useless.
        #
        dict = [ key for key in encoder.memo if len(key) < 64 ]
        dict.sort(lambda a,b: cmp(len(a), len(b)))

        # Put an upper limit on the number of dictionary symbols,
        # to avoid really pathological messages using up lots of
        # disk space in the file header.
        return dict[:128]

    def push(self, msg):
        """Append a new message to the buffer, returning its ID"""
        if not self._initialized:
            # Create the parent directory if necessary
            if not os.path.isdir(self.dirPath):
                os.makedirs(self.dirPath)
            self._init(msg)

        # Parse the message into a compressed SAX event stream
        encoder = SAXEncoder(self.dictionary)
        xml.sax.parseString(msg, encoder)
        zmsg = encoder.getvalue()

        # Read the tail offset. This is our new message ID
        msgId = struct.unpack(">I", self.indexRing.read(4, self.tail))[0]

        # Every message is preceeded with a marker that identifies its
        # full message ID (to identify a location uniquely across wrap-arounds).
        # The zeroes on either side are for framing- messages never include
        # zeroes, so this ensures that we're actually reading a marker.    
        content = struct.pack(">BIIB", 0, msgId, len(zmsg), 0) + zmsg
        self.contentRing.write(content, msgId)

        # Update the index with the next message ID
        self.tail += 4
        nextId = msgId + len(content)
        self.indexRing.write(struct.pack(">I", nextId), self.tail)
        self._commitHeader()
        return msgId

    def getMessageById(self, msgId):
        """Retrieve a particular message, by ID. Returns None if the
           message doesn't exist or has been overwritten.
           """
        if not self._initialized:
            # This shouldn't ever create the file
            if not os.path.isfile(self.filePath):
                return
            if not self._init():
                return

        try:
            zero1, checkId, size, zero2 = struct.unpack(
                ">BIIB", self.contentRing.read(10, msgId)) 
        except struct.error:
            return
        if checkId == msgId and zero1 == 0 and zero2 == 0:
            return SAXDecoder(self.contentRing.read(size), self.dictionary)

    def getLatest(self, limit=None):
        """Yield up to 'limit' of the most recent messages, in
           the same order they were added. Yields (id, msg) tuples.
           """
        if not self._initialized:
            # This shouldn't ever create the file
            if not os.path.isfile(self.filePath):
                return
            if not self._init():
                return
        if limit is None:
            limit = self.indexSize // 4

        tail = self.tail
        head = max(0, tail - min(limit * 4, self.indexSize))
        index = self.indexRing.read(tail - head, head)
        for i in xrange(0, len(index), 4):
            msgId = struct.unpack(">I", index[i:i+4])[0]
            msg = self.getMessageById(msgId)
            if msg is not None:
                yield (msgId, msg)

### The End ###
