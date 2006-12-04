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


class PathNode(bemused.CachedFileNode):
    """A FileNode representing a path in MPD's filesystem, either a
       file or directory.
       """
    _isLeaf = False
    
    def __init__(self, name, path=''):
        self.path = path
        bemused.CachedFileNode.__init__(self, name)

    def isLeaf(self):
        return self._isLeaf

    def getIdentity(self):
        return "MPD:" + self.path

    def _getChildren(self, player):
        children = []
        for info in player.mpd.lsinfo(self.path):
            type = info.type
            path = info[type]
            name = unicode(path.rsplit("/", 1)[-1], 'utf-8')
            child = PathNode(name, path)
            child._isLeaf = type == "file"
            children.append(child)
        return children

    def add(self, player):
        player.mpd.add(self.path)


class AlbumTracksNode(bemused.CachedFileNode):
    """A FileNode that represents a list of tracks from one album.
       The tracks are queried using the MPD 'find' command, and the
       results are sorted by track number.
       """
    def __init__(self, name, album):
        self.findQuery = ('album', album)
        bemused.CachedFileNode.__init__(self, name)

    def getIdentity(self):
        return "Album:" + repr(self.findQuery)

    def _getChildren(self, player):
        children = []
        for info in player.mpd.find(*self.findQuery):

            # Name each result using the metadata title, with Unicode support.
            # The client appears to do its own sorting, so prepend the track
            # number so it's used as the primary sort key.
            name = unicode(info.title, 'utf-8')
            if 'track' in info:
                name = "%02d. %s" % (int(info.track.split('/')[0]), name)

            child = PathNode(name, info.file)
            child._isLeaf = True
            children.append(child)
        return children


class AlbumListNode(bemused.CachedFileNode):
    """Represents a list of albums, optionally restricted by artist."""
    def __init__(self, name, artist=None):
        if artist:
            self.listQuery = ('album', 'artist', artist)
        else:
            self.listQuery = ('album',)
        bemused.CachedFileNode.__init__(self, name)

    def getIdentity(self):
        return "List:" + repr(self.listQuery)
        
    def _getChildren(self, player):
        return [AlbumTracksNode(unicode(info.album, 'utf-8'), info.album)
                for info in player.mpd.list(*self.listQuery)]


class ArtistListNode(bemused.CachedFileNode):
    """Represents a list of artists, each of which contains a list of albums"""
    def _getChildren(self, player):
        return [AlbumListNode(unicode(info.artist, 'utf-8'), info.artist)
                for info in player.mpd.list('artist')]


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
            return unicode("%s (%s, %s)" % (
                            metadata.title,
                            metadata.get('artist', 'None'),
                            metadata.get('album', 'None')),
                           'utf-8')
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

        root.append(ArtistListNode("Artists"))
        root.append(AlbumListNode("Albums"))
        root.append(PathNode("Filesystem"))
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


if __name__ == "__main__":
    bemused.simpleSPPServer("MPD", bemused.ProtocolHandler(MPDPlayer()).run)
