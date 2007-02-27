from django.conf import settings
import struct, os, datetime, re
import xml.sax, xml.dom.minidom


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

       XXX: Parent and sibling links are disabled, to keep the load down
            on Python's cyclic GC. I hate the DOM :p 
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
#            if childNodes:
#                last = childNodes[-1]
#                node.__dict__["previousSibling"] = last
#                last.__dict__["nextSibling"] = node
            childNodes.append(node)
#            node.__dict__["parentNode"] = top
#            node.__dict__["ownerDocument"] = stack[0]

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
#                if childNodes:
#                    last = childNodes[-1]
#                    node.__dict__["previousSibling"] = last
#                    last.__dict__["nextSibling"] = node
                childNodes.append(node)
#                node.__dict__["parentNode"] = top
#                node.__dict__["ownerDocument"] = stack[0]

            # Element with no attributes (fast path)
            elif op == 2:
                node = dom.Element(item[1:])

                # _append_child(top, node)
                childNodes = top.childNodes
#                if childNodes:
#                    last = childNodes[-1]
#                    node.__dict__["previousSibling"] = last
#                    last.__dict__["nextSibling"] = node
                childNodes.append(node)
#                node.__dict__["parentNode"] = top
#                node.__dict__["ownerDocument"] = stack[0]

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
#                d["ownerDocument"] = stack[0]

                # _set_attribute_node(attrs, attrKey)
                attrs._attrs[attrKey.name] = attrKey
#                attrKey.__dict__['ownerElement'] = attrs

                attrRemaining -= 1
                attrKey = None

                # Finished an element with attributes?
                if not attrRemaining:
                    # _append_child(top, attrs)
                    childNodes = top.childNodes
#                    if childNodes:
#                        last = childNodes[-1]
#                        node.__dict__["previousSibling"] = last
#                        last.__dict__["nextSibling"] = node
                    childNodes.append(attrs)
#                    attrs.__dict__["parentNode"] = top
#                    attrs.__dict__["ownerDocument"] = stack[0]

                    stack.append(attrs)
                    top = attrs
                    attrs = None

    return stack[0]


# All versions use the same header currently
HEADER_FORMAT= ">BIB"
HEADER_SIZE = struct.calcsize(HEADER_FORMAT)

# You may define new versions, but never change the existing dicts!
LATEST_VERSION = 1
DICTIONARIES = {
    1: (u'\x01name', u'\x03header', u'\x02file', u'\x01Received',
        u'\x02source', u'\x02project', u'\x02name', u'\x02generator',
        u'\x02body', u'\x02timestamp', u'\x02message', u'\x02commit',
        u'\x02author', u'\x02log', u'\x02files', u'\x02version',
        u'\x03file', u'action', u'From', u'Date', u'\x02mailHeaders',
        u'Message-Id', u'\x02module', u'modify', u'\x02revision',
        u'\x02url', u'\x02diffLines', u'\x01uri', u'\x02branch',
        u'\x04file', u'\x01add'),
    }

def bsax_dump(str, f):
    """Given an XML message, as a UTF-8 string, write the SAX-encoded
       message (with header) to the provided file object.

       Message archive files have no header. Each message is represented by:

         1. One NUL byte, indicating the beginning of the message. This
            is used as a flag character, since messages may not contain
            an internal NUL.

         2. The length of this message, including all headers, as a
            32-bit big endian integer.

         3. The message version, as a single byte. This identifies
            the dictionary used to encode the message, and it can indicate
            the presence of extra fields in the message header.

       There is no unique message identifier, since it isn't worth the
       effort to generate one atomically. The date code and file offset
       will become a unique identifier the moment the message is appended
       to disk.
       """
    encoder = SAXEncoder(DICTIONARIES[LATEST_VERSION])
    xml.sax.parseString(str, e)
    zmsg = encoder.getvalue()

    # Pack the message header
    header = struct.pack(HEADER_FORMAT, 0,
                         HEADER_SIZE + len(zmsg),
                         LATEST_VERSION)

    # Atomically write to the file
    os.write(f.fileno(), header + zmsg)

def bsax_load(f):
    """Read a single XML message, with header, from a provided file
       object. Returns a DOM document on success, or None on EOF.
       """
    header = f.read(HEADER_SIZE)
    if not header:
        return None
    
    zero, size, version = struct.unpack(HEADER_FORMAT, header)
    if zero != 0:
        raise ValueError("Badly formatted bsax header")

    msg_size = size - HEADER_SIZE
    zmsg = f.read(msg_size)
    if len(zmsg) < msg_size:
        return None

    return SAXDecoder(zmsg, DICTIONARIES[LATEST_VERSION])


def get_archive_for_day(day, create=False):
    """Given an ordinal day number, return the full path to that
       day's message archive. Optionally create parent directories
       as necessary.
       """
    date = datetime.date.fromordinal(day)
    p = settings.CIA_DATA_PATH
    for segment in ('db', 'archive',
                    "%04d" % date.year,
                    "%04d-%02d" % (date.year, date.month),
                    "%04d-%02d-%02d.bsax" % (date.year, date.month, date.day)):
        if create:
            try:
                os.mkdir(p)
            except OSError:
                pass
        p = os.path.join(p, segment)
    return p

def get_first_day():
    """Determine the ordinal number of the first day in the archive.
       Returns None if the archive is empty.
       """
    archive_dir = os.path.join(settings.CIA_DATA_PATH, 'db', 'archive')
    file_re = re.compile("^(\d{4})-(\d{2})-(\d{2}).bsax$")

    for root, dirs, files in os.walk(archive_dir):
        dirs.sort()
        files.sort()
        for file in files:
            match = file_re.match(file)
            if match:
                return datetime.date(year  = int(match.group(1)),
                                     month = int(match.group(2)),
                                     day   = int(match.group(3))).toordinal()

def iter_messages_after(day=None, offset=0):
    """Starting at the given day and offset, yield messages until we
       reach the most recent one. Yields (day, offset, dom) tuples.
       """
    if not day:
        day = get_first_day()
        if not day:
            return

    today = datetime.datetime.now().toordinal()
    while day <= today:
        try:
            f = open(get_archive_for_day(day), 'rb')
        except IOError:
            pass
        else:
            f.seek(offset)

            while True:
                offset = f.tell()
                dom = bsax_load(f)
                if not dom:
                    break
                yield (day, offset, dom)
                del dom

        day += 1
        offset = 0

def benchmark():
    import time
    c = 0
    start = time.time()
    for m in iter_messages_after():
        c += 1
        if c > 100000:
            break
    end = time.time()
    print "%.2f msg/sec" % (c / (end - start))


class MessageArchiver:
    """A MessageArchiver stores incoming messages in an archive
       collated by day. Messages are appended atomically.
       """
    def __init__(self):
        self._file = None
        self._date = None

    def _setDate(self, date):
        """Change days, opening a new log file if necessary."""
        if date == self._date:
            return
        self._date = date
        self._file = open(get_archive_for_day(date.toordinal(), create=True), 'ab')

    def push(self, msg):
        """Append a new message to the buffer, given a UTf-8 byte string.
           Returns None, as we can't efficiently return the message ID just yet.
           """
        self._setDate(datetime.datetime.now().date())
        bsax_dump(msg, self._file)

    def deliver(self, msg):
        """Deliver a Message instance to the archive. This is silly, because
           we have to convert the Message (a DOM) right back to UTF-8 text
           just so we can re-serialize it with SAX. This is a stopgap measure
           until the message filtering architecture is redesigned.
           """
        self.push(unicode(msg).encode('utf-8'))

### The End ###
