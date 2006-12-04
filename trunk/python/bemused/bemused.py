#
# bemused.py - A Python library for creating Bemused servers
# Copyright (c) 2006 Micah Dowty <micah@navi.cx>
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

from __future__ import division
import bluetooth, syslog
import sys, os, traceback
import struct, time


class DefaultLogger:
    """Default logging object: send log data to syslog and to stderr"""
    syslogName = None
    _initialized = False

    def init(self):
        if not self.syslogName:
            self.syslogName = os.path.basename(sys.argv[0])
        
        syslog.openlog(self.syslogName, syslog.LOG_PID)
        self._initialized = True

    def log(self, message):
        if not self._initialized:
            self.init()

        sys.stderr.write("%s\n" % message)
        for line in message.split('\n'):
            syslog.syslog(line)

logger = DefaultLogger()


def simpleSPPServer(name, clientHandler):
    """Main loop for a simple single-connection Serial Port Profile server.
       This advertises an SPP service, and waits for clients to connect.
       When a client connects, clientHandler(socket) is invoked.
       Exceptions are logged, and cause the connection to terminate.
       """
    socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    socket.bind(('', bluetooth.PORT_ANY))
    socket.listen(1)

    bluetooth.advertise_service(socket, name,
                                service_classes = [bluetooth.SERIAL_PORT_CLASS],
                                profiles = [bluetooth.SERIAL_PORT_PROFILE])

    while True:
        logger.log("Listening for clients on RFCOMM channel %d" % socket.getsockname()[1])
        clientSocket, clientInfo = socket.accept()
        logger.log("Client %s connected" % clientInfo[0])
        try:
            clientHandler(clientSocket)
        except KeyboardInterrupt:
            pass
        except Exception, e:
            logger.log("*** Unhandled exception\n%s" % traceback.format_exc().strip())
        clientSocket.close()


class FileNode:
    """Base class for files/directories in the Bemused client's virtual filesystem"""

    def __init__(self, name=''):
        self.name = name

    def isLeaf(self):
        """Returns true if this is a file rather than a directory."""
        return False

    def getChild(self, name):
        """Look up a child node, by name."""
        return None

    def getChildren(self):
        """Returns all valid child FileNodes below this directory"""
        return []

    def getSize(self):
        """Return the number of bytes available for download"""
        return 0

    def sendContent(self, writer):
        """Write this file's content by calling writer() repeatedly
           with arbitrary-sized data strings. This must write the same
           number of bytes promised via getSize().
           """
        pass

    def play(self, player):
        """Add this file/directory to the end of the playlist, and start
           playing it. By default, this adds ourselves to the playlist
           using add() and uses the public playlist interface to begin
           playing the first new track.

           If this is a special file, you might want to override 'play'
           to do something completely unrelated.
           """
        newIndex = player.playlist.getLength()
        self.add(player)
        player.playlist.select(newIndex)

    def add(self, player):
        """Add this file or directory to the end of the current playlist."""
        pass


class StaticDirectory(FileNode):
    """A directory FileNode which contains an unchanging list of children"""
    def __init__(self, name=''):
        FileNode.__init__(self, name)
        self._childList = []
        self._childMap = {}

    def getChild(self, name):
        return self._childMap[name]

    def getChildren(self):
        return self._childList

    def append(self, child):
        self._childMap[child.name] = child
        self._childList.append(child)


class Playlist:
    """Abstract base class for the current playlist"""

    def clear(self):
        """Remove all tracks from the playlist."""
        pass

    def select(self, index):
        """Make current the track at 'index'"""
        pass

    def getSelection(self):
        """Returns the currently selected index"""
        return 0
        
    def selectRel(self, relIndex):
        """Select a new track at the given index relative to the current index"""
        self.select(self.getSelection() + relIndex)

    def getLength(self):
        """Return the playlist length"""
        return 0

    def getTitles(self):
        """Returns a list of strings representing all entries in the playlist"""
        return []

    def getSelectionTitle(self):
        """Returns the title of the currently selected track"""
        titles = self.getTitles()
        index = self.getSelection()
        try:
            return titles[index]
        except IndexError:
            return ""


class MediaPlayer(object):
    """Abstract base class for media players. This should be subclassed
       to implement interfaces to individual pieces of player software.
       """
    playlist = None
    rootDirectory = None

    isPlaying = True
    shuffleMode = False
    repeatMode = False
    volume = 1.0
    trackLength = 0
    currentTime = 0
    bitRate = 0
    sampleRate = 0
    numChannels = 0

    def getFileByName(self, filename):
        """Return a file or directory in our filesystem, given its path"""
        current = self.rootDirectory
        for segment in filename.split("\\"):
            if segment:
                current = current.getChild(segment)
        return current

    def update(self):
        """If subclasses need to poll the media player for status,
           they can do so here. This is called before every command
           from the client is processed.
           """
        pass

    def shutdown(self):
        """Optional: Shut down the computer"""
        pass

    def fadeOut(self, steps=32, duration=2.0):
        """Fade out the current track, and stop it.  The default
           implementation implements this on top of setVolume,
           getVolume, and stop. It performs a linear fade, with
           adjustable timing.
           """
        fullVolume = self.volume
        for i in range(steps-1, -1, -1):
            self.volume = fullVolume * i / steps
            time.sleep(duration / steps)
        self.stop()
        self.volume = fullVolume

    def play(self):
        """Start playing the current track"""
        pass

    def pause(self):
        """Pause the current track"""
        pass

    def stop(self):
        """Stop playing the current track"""
        pass

    def stopAtEnd(self):
        """Stop after reaching the end of the current track, if supported.
           The default implementation stops immediately.
           """
        self.stop()

    def seek(self, seconds):
        """Seek forward or backward in the current track, relative to the current position.
           The default implementation calculates an absolute time using self.currentTime.
           """
        self.currentTime = self.currentTime + seconds


TREE_ROOT         = 0x00
TREE_CHILD        = 0x10
TREE_ONLY_CHILD   = 0x20
TREE_SIBLING      = 0x30
TREE_LAST_SIBLING = 0x40
TREE_END          = 0xF0

TYPE_DRIVE        = 0x00
TYPE_DIRECTORY    = 0x01
TYPE_FILE         = 0x02
TYPE_UNEXPANDED   = 0x03
    

class ProtocolHandler:
    """This class implements the Bemused protocol.  The state of the
       media player is represented by a supplied MediaPlayer instance.
       """
    version = (1, 73)
    trace = bool(os.environ.get("BEMUSED_TRACE"))

    def __init__(self, player):
        self.player = player

    def reply(self, data):
        """Blocking write of arbitrary data to the client.
           This always succeeds. If we can't write the entire
           string, this throws IOError.
           """
        if self.trace:
            logger.log("Reply: %r" % data)
        while data:
            sent = self.client.send(data)
            if sent <= 0:
                raise IOError("Error writing reply")
            data = data[sent:]

    def replyStruct(self, fmt, *args):
        self.reply(struct.pack(fmt, *args))

    def receive(self, size):
        """Unconditionally receive 'size' bytes from the client.
           If we can't do so, throw the IOError exception.
           """
        data = ''
        while len(data) < size:
            received = self.client.recv(size - len(data))
            if not received:
                raise IOError("Error reading from client")
            data += received
        if self.trace:
            logger.log("Receive: %r" % data)
        return data

    def receiveStruct(self, fmt):
        """Receive a fixed-size struct using the supplied format"""
        return struct.unpack(fmt, self.receive(struct.calcsize(fmt)))

    def receiveByte(self):
        """Receive a single byte, as an 8-bit unsigned integer"""
        return ord(self.receive(1))

    def receiveFilename(self):
        """Receive a filename: a counted string prefixed by a 2-byte length"""
        return self.receive(self.receiveStruct(">H")[0])

    def run(self, client):
        """Until the connection dies, keep processing commands from the client."""
        self.client = client
        while True:
            cmd = self.receive(4)
            handler = getattr(self, "cmd_%s" % cmd, None)
            if not handler:
                raise ValueError("Unsupported command %r" % cmd)
            self.player.update()
            handler()

    def sendTree(self, root, expand=False, relation=TREE_ROOT):
        """Send a tree of file nodes. If 'expand' is true, this recurses into
           subdirectories to send the entire tree. If not, it sends only a
           single layer.
           """
        assert not root.isLeaf()
        self.sendTreeNode(relation | TYPE_DIRECTORY, root.name)

        children = root.getChildren()
        numChildren = len(children)
        for index in xrange(numChildren):

            # Determine how this next node is related to the previous one we sent
            if numChildren == 1:
                relation = TREE_ONLY_CHILD
            elif index == 0:
                relation = TREE_CHILD
            elif index == numChildren-1:
                relation = TREE_LAST_SIBLING
            else:
                relation = TREE_SIBLING

            # Determine the file type
            child = children[index]
            if child.isLeaf():
                self.sendTreeNode(TYPE_FILE | relation, child.name)
            elif expand:
                self.sendTree(child, expand, relation)
            else:
                self.sendTreeNode(TYPE_UNEXPANDED | relation, child.name)

    def sendTreeNode(self, flags, name):
        self.reply(chr(flags) + name + chr(0))

    def cmd_CHCK(self):
        self.reply("Y")

    def cmd_DINF(self):
        self.replyStruct(">7sIII", "DINFACK",
                         int(self.player.bitRate),
                         int(self.player.sampleRate),
                         int(self.player.numChannels))

    def cmd_DLST(self):
        self.reply("LISTACK")
        self.sendTree(self.player.getFileByName(self.receiveFilename()))
        self.sendTreeNode(TREE_END, '')

    def cmd_DOWN(self):
        file = self.player.getFileByName(self.receiveFilename())
        file.sendContent(self.reply)

    def cmd_FADE(self):
        self.player.fadeOut()

    def cmd_FFWD(self):
        self.player.seek(5)

    def cmd_FINF(self):
        file = self.player.getFileByName(self.receiveFilename())
        self.replyStruct(">7sI", "FINFACK", file.getSize())

    def cmd_GVOL(self):
        print "VOL: %s" % self.player.volume
        v = max(0, min(255, int(self.player.volume * 255.0 + 0.5)))
        self.replyStruct(">7sB", "GVOLACK", v)

    def cmd_INFO(self):
        self.replyStruct(">7sBIIBB", "INFOACK",
                         int(not self.player.isPlaying),
                         int(self.player.trackLength),
                         int(self.player.currentTime),
                         int(self.player.shuffleMode),
                         int(self.player.repeatMode))
        self.reply(self.player.playlist.getSelectionTitle())

    def cmd_INF2(self):
        self.cmd_INFO()
        self.reply(chr(0))

    def cmd_LADD(self):
        file = self.player.getFileByName(self.receiveFilename())
        file.add(self.player)

    def cmd_LIST(self):
        self.reply("LISTACK")
        self.sendTree(self.player.getFileByName(""), expand=True)
        self.sendTreeNode(TREE_END, '')

    def cmd_NEXT(self):
        self.player.playlist.selectRel(1)

    def cmd_PAUS(self):
        self.player.pause()

    def cmd_PLAY(self):
        file = self.player.getFileByName(self.receiveFilename())
        file.play(self.player)

    def cmd_PLEN(self):
        self.replyStruct(">H", self.player.playlist.getLength())

    def cmd_PLST(self):
        self.replyStruct(">7sH", "PLSTACK", self.player.playlist.getSelection())
        self.reply("\n".join(self.player.playlist.getTitles()) +
                   "\n\0")

    def cmd_PREV(self):
        self.player.playlist.selectRel(-1)

    def cmd_REPT(self):
        self.player.repeatMode = bool(self.receiveByte())

    def cmd_RMAL(self):
        self.player.playlist.clear()

    def cmd_RWND(self):
        self.player.seek(-5)

    def cmd_SHFL(self):
        self.player.shuffleMode = bool(self.receiveByte())

    def cmd_SEEK(self):
        self.player.currentTime = self.receiveStruct(">I")[0]

    def cmd_SHUT(self):
        self.player.shutdown()

    def cmd_STEN(self):
        self.player.stopAtEnd()

    def cmd_SLCT(self):
        self.player.playlist.select(self.receiveStruct(">H")[0])

    def cmd_STOP(self):
        self.player.stop()
         
    def cmd_STRT(self):
        self.player.play()

    def cmd_VOLM(self):
        self.player.volume = self.receiveByte() / 255.0

    def cmd_VERS(self):
        self.replyStruct(">7sBB", "VERSACK", *self.version)
