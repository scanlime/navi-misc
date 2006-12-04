#!/usr/bin/env python
#
# bemused-mpd.py - A daemon which bridges a Bemused client to an MPD server.
#
#   This was inspired by Dan Spencer's pbmpcd.py, but it is more
#   featureful, less broken, and it doesn't share any code with that
#   implementation.
#
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
import bemused
import mpdclient2


class PathNode(bemused.FileNode):
    """A FileNode representing a path in MPD's filesystem, either a
       file or directory.
       """
    _isLeaf = False
    
    def __init__(self, player, name, path=''):
        self.player = player
        self.path = path
        bemused.FileNode.__init__(self, name)

    def isLeaf(self):
        return self._isLeaf

    def getChild(self, name):
        if self.path and not self.path.endswith("/"):
            path = self.path + "/"
        else:
            path = self.path
        return PathNode(self.player, name, path + name)

    def getChildren(self):
        children = []
        for info in self.player.mpd.lsinfo(self.path):
            type = info.type
            path = info[type]

            # XXX: We really should convert the text encoding of the
            #      name here, but that would leave us with a hard
            #      problem to solve in getChild() where we need to
            #      convert a Bemused path back to an MPD path.
            name = path.rsplit("/", 1)[-1]

            child = PathNode(self.player, name, path)
            child._isLeaf = type == "file"
            children.append(child)
        return children

    def add(self, player):
        player.mpd.add(self.path)


class MPDPlaylist(bemused.Playlist):
    def __init__(self, player):
        self.player = player

    def clear(self):
        self.player.mpd.clear()

    def select(self, index):
        self.player.mpd.seek(index, 0)

    def getSelection(self):
        return int(self.player.status.get('song', 0))

    def _getTitleFromMetadata(self, metadata):
        if 'title' in metadata:
            return self.player.encodeText("%s (%s, %s)" % (
                metadata.title,
                metadata.get('artist', 'None'),
                metadata.get('album', 'None')))
        else:
            return 'None'

    def getLength(self):
        return int(self.player.status.playlistlength)

    def getTitles(self):
        return map(self._getTitleFromMetadata, self.player.mpd.playlistinfo())

    def getSelectionTitle(self):
        return self._getTitleFromMetadata(self.player.mpd.currentsong())


class MPDPlayer(bemused.MediaPlayer):
    def __init__(self):
        self.connect()
        self.playlist = MPDPlaylist(self)

        root = self.rootDirectory = bemused.StaticDirectory()

        #root.append(bemused.StaticDirectory("Browse Artists"))
        root.append(PathNode(self, "Browse Filesystem"))
        #root.append(bemused.StaticDirectory("Outputs"))
        
    def connect(self):
        self.mpd = mpdclient2.connect()
        bemused.logger.log("Connected to MPD server at %s:%s" % (self.mpd.talker.host,
                                                                 self.mpd.talker.port))

    def update(self):
        """Get the latest MPD status before processing every Bemused command,
           and use this as an opportunity to automatically reconnect if the
           MPD socket died.
           """
        while True:
            try:
                self.status = self.mpd.status()
                break
            except EOFError:
                self.connect()

    isPlaying = property(lambda self: self.status == 'play')
    trackLength = property(lambda self: int(self.status.get('time', '0:0').split(":")[1]))
    bitRate = property(lambda self: int(self.status.get('bitrate', 0)))
    sampleRate = property(lambda self: int(self.status.get('audio', '0:0:0').split(":")[0]))
    numchannels = property(lambda self: int(self.status.get('audio', '0:0:0').split(":")[2]))

    currentTime = property(lambda self: int(self.status.get('time', '0:0').split(":")[0]),
                           lambda self, x: self.mpd.seekid(int(self.status.songid), x))

    volume = property(lambda self: int(self.status.volume) / 100,
                      lambda self, x: self.mpd.setvol(int(x * 100 + 0.5)))

    shuffleMode = property(lambda self: int(self.status.random),
                           lambda self, x: self.mpd.random(x))

    repeatMode = property(lambda self: int(self.status.repeat),
                          lambda self, x: self.mpd.repeat(x))

    def play(self):
        self.mpd.play()

    def pause(self):
        self.mpd.pause(self.status.state != 'pause')

    def stop(self):
        self.mpd.stop()
            
    def encodeText(self, text):
        # Looks like MPD is giving us UTF-8, but S60 Bemused client
        # is using latin-1. Try to convert the encoding, but don't
        # worry about getting it perfect.
        return text.decode("utf-8", "replace").encode("latin-1", "replace")


if __name__ == "__main__":
    bemused.simpleSPPServer("MPD", bemused.ProtocolHandler(MPDPlayer()).run)
