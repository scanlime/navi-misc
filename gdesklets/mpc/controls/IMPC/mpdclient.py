"""
mpdclient.py / py-libmpdclient:  Python implementation of libmpdclient, much of
mpc functionality, and probably more to come.

To use:

<start mpd>

<run python>
>>> import mpdclient
>>> c = mpdclient.MpdController()
>>> c.load("yourplaylist")
>>> c.random()
>>> c.repeat()
>>> c.play()

.. And of course much more.
"""

from __future__ import generators
import os, socket, select, re

__release__ = "0.10.0"
__version__ = "$Revision: 1.22 $"[11:-2]
__author__ = "Nick Welch aka mackstann"
__email__ = "mack at incise dot org"
__revision__ = __cvsid__ = \
"$Id: mpdclient.py,v 1.22 2004/02/23 21:36:19 mackstann Exp $"

__license__ = """
py-libmpdclient, a Python client library for mpd (Music Player Daemon)
Copyright (C) 2003, 2004 Nick Welch <mack at incise.org>

This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 2.1 of the License, or (at your option) any
later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License along
with this library; if not, write to the Free Software Foundation, Inc., 59
Temple Place, Suite 330, Boston, MA  02111-1307  USA
"""

# some constants

MAX_BUFLEN = 50000
WELCOME_MSG = "OK MPD "

# completely ignore stored errors (not ACKs) from mpd
IGNORE_ERRORS = 0
# store them in an "error" attribute
STORE_ERRORS = 10
# store and raise them
RAISE_ERRORS = 20

##### our exception classes

# the basics

class MpdError(Exception):
  "Base error class"
  def __init__(self, msg, num="<none>"):
    self._msg = msg
    self._num = num

    Exception.__init__(self, "Errno %s: %s" % (self._num, self._msg))

class MpdNetError(MpdError):
  "Base error class for network-related errors"
  def __init__(self, msg, num, host, port=None):
    self._host = host
    self._port = port
    MpdError.__init__(self, msg % locals(), num)

# and the specifics

class MpdStoredError(MpdError):
  def __init__(self, msg):
    MpdError.__init__(self, msg, 50)

class MpdTimeoutError(MpdError):
  def __init__(self):
    MpdError.__init__(self, "connection timeout", 10)

class MpdSystemError(MpdError):
  def __init__(self):
    MpdError.__init__(self, "problems creating socket", 11)

class MpdUnknownHostError(MpdNetError):
  def __init__(self, host):
    MpdNetError.__init__(self, "host `%(host)s' not found" % host, 11, host)

class MpdConnectionPortError(MpdNetError):
  def __init__(self, host, port):
    MpdNetError.__init__(self,
      "problems connecting to `%(host)s' on port %(port)d", 12, host, port)

class MpdNotMpdError(MpdNetError):
  def __init__(self, host, port,
               msg="port %(port)d on host `%(host)s' is not mpd"):
    MpdNetError.__init__(self, msg, 13, host, port)

class MpdNoResponseError(MpdNetError):
  def __init__(self, host, port):
    MpdNetError.__init__(self,
      "problems getting a response from `%(host)s' on port %(port)d",
      14, host, port)

class MpdSendingError(MpdError):
  def __init__(self, command):
    MpdError.__init__(self, "problems giving command `%s'" % command, 15)
    self._command = command

class MpdConnectionClosedError(MpdError):
  def __init__(self):
    MpdError.__init__(self, "connection closed", 16)


##### various classes, formerly C structs

class Stats(object):
  "Represents statitistics grabbed from mpd via 'stats' command"

  artists   = -1
  albums    = -1
  songs     = -1
  uptime    = -1
  db_update = -1

  def dbHasChanged(self, since):
    if self.db_update == -1:
      return True

    if self.db_update > since:
      return True
    else:
      return False


class Status(object):
  "Class representing the status of the player at a given point in time."

  volume         = -10
  repeat         = -1
  random         = -1
  playlistLength = -1
  playlist       = -1L
  state          = -1
  song           = 0
  elapsedTime    = 0
  totalTime      = 0

  def stateStr(self):
    "Returns string representation of state attribute."
    return playerStateSwapType(self.state)


class ReturnElement(object):
  r"""
  A class representing an element given back by mpd.  For example:

    % printf 'status\n' | nc localhost 6600
    OK MPD 0.9.0
    volume: 80
    repeat: 0
    <more stuff snipped>

  So, from this, we would get two ReturnElements.  The first would have a name
  attribute of "volume", and a value attribute of 80.  The second would would
  have a name and value of "repeat", and 0, respectively.
  """
  def __init__(self, name="", value=""):
    """
    __init__(name="", value="")

    The name and value attributes of the new instance will be given the values
    of their respective parameters passed to this constructor.
    """

    self.name = name
    self.value = value


class InfoEntity(object):
  """
  A base class for "entities" passed back from mpd.  An entity can be either a
  song, directory, or playlist file, each of which have classes subclassed from
  this one.
  """
  def __init__(self, path=""):
    """
    __init__(path="")

    The instance's path attribute will be given the value of the path parameter
    passed to this constructor.
    """
    if not isinstance(path, str):
      raise ValueError, "path must be string"

    self.path = path


class Directory(InfoEntity):
  """
  An InfoEntity subclass representing a directory somewhere inside of mpd's
  music directory.
  """
  pass


class PlaylistFile(InfoEntity):
  """
  An InfoEntity subclass representing a playlist file in mpd's playlist
  directory.
  """
  pass


class Song(InfoEntity):

  """
  An InfoEntity subclass representing a music file somewhere inside of mpd's
  music directory.  Each Song instance will have a path attribute, but other
  attributes may remain as empty strings, since not all files will have the
  corresponding metadata (id3 tags, etc) needed to obtain the information.
  """
  artist   = ""
  title    = ""
  album    = ""
  track    = ""


def playerStateSwapType(state):
  """
  The player's play state is stored as an integer, but has standard string
  descriptions as well.  This function will swap an integer to its
  corresponding string description, and vice versa.
  """
  if isinstance(state, int) or isinstance(state, str):
    if state in _playerStates:
      return _playerStates[state]
    else:
      raise ValueError, "state `%s' does not exist" % state
  else:
    raise ValueError, "state must be int or str"

def searchTableSwapType(state):
  """
  The various seach tables are identified by integers, but have standard
  string descriptions as well.  This function will swap an integer to its
  corresponding string description, and vice versa.
  """
  if isinstance(state, int) or isinstance(state, str):
    return _searchTables[state]
  else:
    raise ValueError, "state must be int or str"

##### some convenient mappings, formerly C #defines

_playerStates = {
  # states that the player may be in
  "unknown"  :  0,
  "stop"     :  1,
  "play"     :  2,
  "pause"    :  3,
   0 : "unknown",
   1 : "stop",
   2 : "play",
   3 : "pause"
}

_searchTables = {
  # types of things you can search for
  "artist"   : 0,
  "album"    : 1,
  "title"    : 2,
  "filename" : 3,
  0 : "artist",
  1 : "album",
  2 : "title",
  3 : "filename"
}



##### and some random utilitarian functions

def _sanitizeArg(arg):
  "Escapes backslashes and doublequotes and returns the escaped string."
  return arg.replace("\\", "\\\\").replace('"', '\\"')

def _versionOk(ver):
  """
  A version number should match the regexp "\d\.\d\.\d".  This is a pretty
  simple check, and really the only one of its kind in this module, so instead
  of importing re and using a regex, we can match it pretty easily using
  regular Python.
  """
  return re.match(r"\d+\.\d+\.\d+", ver)


class MpdConnection(object):
  """
  MpdConnection([host="localhost"[, port=6600[, timeout=20]]])

  This is basically the C mpd_Connection struct, merged with all of the
  C functions that acted upon it, in a single class, with of course some
  changes to make it more Pythonic.  However, I tried to keep it fairly close
  to the C implementation.  Additional convenience stuff, and much more,
  is in MpdController.
  """

  DEBUG = False

  version        = []
  sock           = None
  buf            = ""
  doneProcessing = False
  commandList    = False
  returnElement  = None
  timeout        = 0
  host           = ""
  port           = 0
  timeout        = 0
  response       = ""
  error          = None
  errorHandling  = RAISE_ERRORS

  _recvlimit = lambda self: MAX_BUFLEN - len(self.buf)

  def __init__(self, host="localhost", port=6600, timeout=20):
    self.host = host
    self.port = port
    self.timeout = timeout

    self._getSocket()
    self._connect()
    self.getNextResponse()
    self._parseVersion()

  def clearError(self):
    """
    clearError() -> Exception instance

    Returns the stored error and clears it in both the MpdConnection instance
    and mpd itself.
    """
    self.executeCommand("clearerror")

    error = self.error
    self.error = None

    self.finishCommand()

    return error

  def sendCommandListEnd(self):
    "Turns command list mode off."

    if not self.commandList:
      raise MpdError("not in command list mode")

    self.executeCommand("command_list_end")
    self.finishCommand()

    self.commandList = False

  def sendCommandListBegin(self):
    """
    Command list mode is a way to send a bunch of commands in bulk at once.
    This method toggles it on.
    """

    if self.commandList:
      raise MpdError("already in command list mode")

    self.executeCommand("command_list_begin")
    self.commandList = True

  def sendShuffleCommand(self): self.executeCommand("shuffle")
  def sendClearCommand(self):   self.executeCommand("clear")
  def sendStopCommand(self):    self.executeCommand("stop")
  def sendPauseCommand(self):   self.executeCommand("pause")
  def sendNextCommand(self):    self.executeCommand("next")
  def sendUpdateCommand(self):  self.executeCommand("update")
  def sendPrevCommand(self):    self.executeCommand("previous")

  def sendSeekCommand(self, songNum, absSecs):
    if self.DEBUG: print "songNum is %d, moving to %d" % (songNum, absSecs)
    self.executeCommand('seek "%d" "%d"' % (songNum, absSecs))

  def sendVolumeCommand(self, newVolume):
    self.executeCommand('volume "%d"' % newVolume)

  def sendRandomCommand(self, mode):
    self.executeCommand('random "%d"' % mode)

  def sendRepeatCommand(self, mode):
    self.executeCommand('repeat "%d"' % mode)

  def sendSwapCommand(self, one, two):
    self.executeCommand('swap "%d" "%d"' % (one, two))

  def sendMoveCommand(self, fromNum, toNum):
    self.executeCommand('move "%d" "%d"' % (fromNum, toNum))

  def sendPlayCommand(self, songNum=None):
    if songNum is None:
      self.executeCommand('play')
    else:
      self.executeCommand('play "%d"' % songNum)

  def sendRmCommand(self, name):
    self.executeCommand('rm "%s"' % _sanitizeArg(name))

  def sendLoadCommand(self, name):
    self.executeCommand('load "%s"' % _sanitizeArg(name))

  def sendSaveCommand(self, name):
    self.executeCommand('save "%s"' % _sanitizeArg(name))

  def sendDeleteCommand(self, songNum):
    self.executeCommand('delete "%d"' % songNum)

  def sendAddCommand(self, filename):
    self.executeCommand('add "%s"' % _sanitizeArg(filename))

  def sendListCommand(self, table, arg):
    st = searchTableSwapType(table)

    if not st in ("artist", "album"):
      raise MpdError("unkown table for list: %s" % st)

    if arg:
      tosend = 'list %s "%s"' % (st, _sanitizeArg(arg))
    else:
      tosend = 'list %s' % st

    self.executeCommand(tosend)

  def sendFindCommand(self, table, search):
    self._searchOrFind("find", table, search)

  def sendSearchCommand(self, table, search):
    self._searchOrFind("search", table, search)

  def _searchOrFind(self, which, table, search):
    st = searchTableSwapType(table)

    if which != "search" and st == "filename":
      raise MpdError("unkown table for %s: %s" % (which, table))

    self.executeCommand('%s %s "%s"' % (which, st, _sanitizeArg(search)))

  def sendLsInfoCommand(self, directory=""):
    self.executeCommand('lsinfo "%s"' % _sanitizeArg(directory))

  def sendListallCommand(self, directory=""):
    self.executeCommand('listall "%s"' % _sanitizeArg(directory))

  def sendListallInfoCommand(self, directory=""):
    self.executeCommand('listallinfo "%s"' % _sanitizeArg(directory))

  def sendPlaylistInfoCommand(self, songNum=-1):
    if songNum == -1:
        self.executeCommand('playlistinfo')
    else:
        self.executeCommand('playlistinfo "%d"' % songNum)

  def getNextAlbum(self):
    self.getNextReturnElementNamed("Album")

  def getNextArtist(self):
    self.getNextReturnElementNamed("Artist")

  def getNextReturnElementNamed(self, name):
    if self.doneProcessing:
      return False

    self.getNextReturnElement()

    while self.returnElement:
      if self.returnElement.name == name:
        return self.returnElement.value

      self.getNextReturnElement()


  def iterInfoEntities(self):
    """
    Specific to the Python libmpdclient.  Returns an iterator over the info
    entities that you'd normally loop through by calling getNextInfoEntity()
    continuously.
    """

    while 1:
      entity = self.getNextInfoEntity()

      if entity:
        yield entity
      else:
        raise StopIteration

  def getNextInfoEntity(self):
    if self.doneProcessing:
      return False

    if not self.returnElement:
      self.getNextReturnElement()

    if not self.returnElement:
      return False

    if self.returnElement.name == "file":
      entity = Song(self.returnElement.value)

    elif self.returnElement.name == "directory":
      entity = Directory(self.returnElement.value)

    elif self.returnElement.name == "playlist":
      entity = PlaylistFile(self.returnElement.value)

    else:
      raise MpdError("problem parsing song info")

    self.getNextReturnElement()

    while self.returnElement:

      # get all of a Song's attributes

      name, value = self.returnElement.name, self.returnElement.value

      if name in ("file", "directory", "playlist"):
        return entity

      if isinstance(entity, Song) and len(value):
        for item in ("artist", "album", "title", "track"):
          if name == item.capitalize() and not getattr(entity, item):
            setattr(entity, item, value)
            break

      self.getNextReturnElement()

    return entity

  def _doSelect(self, mode="r"):
    assert mode in ("r", "w")
    r, w = [], []

    if mode == "r":
      r = [self.sock]
    else: #mode == "w"
      w = [self.sock]

    if select.select(r, w, [], self.timeout) == ([], [], []):
      raise MpdNoResponseError(self.host, self.port)

  def getStatus(self):
    "returns the current status in a Status instance"
    status = Status()

    self.executeCommand("status")

    self.getNextReturnElement()

    while self.returnElement:
      name, val = self.returnElement.name, self.returnElement.value

      if name in ("repeat", "random", "song"):
        setattr(status, name, int(val))

      if name == "volume":
        if val == -1:
          status.volume = False
        else:
          status.volume = int(val)

      elif name == "playlistlength":
        status.playlistLength = int(val)

      elif name == "playlist":
        status.playlist = long(val)

      elif name == "state":

        if val in ("play", "stop", "pause"):
          status.state = playerStateSwapType(val)
        else:
          status.state = playerStateSwapType("unknown")

      elif name == "time":
        status.elapsedTime, status.totalTime = map(int, val.split(":"))

      elif name == "error":

        if self.errorHandling >= STORE_ERRORS:
          self.error = MpdStoredError(val)

        if self.errorHandling is RAISE_ERRORS:
          raise self.error

      self.getNextReturnElement()

    tocheck = { "volume" : -9, "repeat" : 0, "random" : 0,
                "playlist" : 0, "playlistLength" : 0, "state": 0 }

    for attr, lowerbound in tocheck.items():
      if getattr(status, attr) < lowerbound:
        raise MpdError("%s not found" % attr)

    self.finishCommand()

    return status

  def getStats(self):
    "returns mpd stats"

    stats = Stats()

    self.executeCommand("stats")

    self.getNextReturnElement()

    while self.returnElement:
      name, val = self.returnElement.name, self.returnElement.value

      if name in ("artists", "albums", "songs", "uptime", "db_update"):
        setattr(stats, name, int(val))

      elif name == "error":

        if self.errorHandling >= STORE_ERRORS:
          self.error = MpdStoredError(val)

        if self.errorHandling is RAISE_ERRORS:
          raise self.error

      self.getNextReturnElement()

    for attr in ("artists", "albums", "songs", "uptime", "db_update"):
      if getattr(stats, attr) <= -1:
        raise MpdError("%s not found" % attr)

    self.finishCommand()

    return stats

  def finishCommand(self):
    if self.DEBUG: print "running finishCommand()"
    try:
      while not self.doneProcessing:
        self.getNextReturnElement()

    except MpdError, err:
      self.returnElement = None
      self.doneProcessing = True
      raise err



  def executeCommand(self, command):

    if not self.doneProcessing and not self.commandList:
      raise MpdError("not done processing current command")

    command = "%s\n" % command

    self._doSelect("w")

    try:
      if self.DEBUG: print "SENDING:", command,
      self.sock.send(command)
    except socket.error:
      raise MpdSendingError(command)

    if not self.commandList:
      self.doneProcessing = False

  def getNextReturnElement(self):
    self.returnElement = None

    if self.doneProcessing:
      raise MpdError("already done processing current command")

    self.getNextResponse()

    if self.response == "OK":
      self.doneProcessing = True
      return

    if self.response.startswith("ACK"):
      self.doneProcessing = True
      raise MpdError(self.response.split(" ", 1)[1])

    if not self.response.find(": "):
      self.doneProcessing = True
      raise MpdError("error parsing: %s" % self.response)

    name, value = self.response.split(": ", 1)

    self.returnElement = ReturnElement(name, value)

  def _parseVersion(self):
    if not self.response.startswith(WELCOME_MSG):
      raise MpdNotMpdError(self.host, self.port)

    ver = self.response.split()[2]

    if not _versionOk(ver):
      raise MpdNotMpdError(self.host, self.port,
            "error parsing version number `%s'" % ver)

    self.version = map(int, ver.split("."))

    self.doneProcessing = True

  def getNextResponse(self):
    while "\n" not in self.buf:
      self._doSelect("r")

      lastbuf = self.buf[:]

      try:
        self.buf = ''.join([self.buf, self.sock.recv(self._recvlimit())])
      except socket.error:
        raise MpdNoResponseError(self.host, self.port)

      if self.buf == lastbuf:
        raise MpdNoResponseError(self.host, self.port)

    else:
      self.response, self.buf = self.buf.split("\n", 1)

  def _getSocket(self):
    try:
      self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error:
      raise MpdSystemError()

  def _resolveHost(self):
    try:
      return socket.gethostbyname(self.host)
    except socket.error:
      raise MpdUnknownHostError(self.host)

  def _connect(self):
    self._getSocket()
    try:
      self.sock.connect((self._resolveHost(), self.port))
    except socket.error:
      raise MpdConnectionPortError(self.host, self.port)


class MpdController(MpdConnection):
  """
  MpdController([MpdConnection args]) -> Mpd Controller object

  MpdController is subclassed from MpdConnection, with some added methods for
  convenience.  This is the class you will want to use when writing a client.

  For each environment variable in $MPD_HOST and $MPD_PORT, if the
  corresponding parameter is not explicitly passed to the constructor of this
  class, the environment variable will be used, if set.  If not set, then the
  defaults will be used ("localhost" and 6600, respectively).
  """

  def __init__(self, host=False, port=False, *args):

    if host is False:
      host = os.environ.get("MPD_HOST", "localhost")

    if port is False:
      port = int(os.environ.get("MPD_PORT", "6600"))

    MpdConnection.__init__(self, host, port, *args)


  def status(self):
    return self.getStatus()

  def stats(self):
    return self.getStats()

  def shuffle(self):
    try: self.sendShuffleCommand()
    finally: self.finishCommand()

  def update(self):
    try: self.sendUpdateCommand()
    finally: self.finishCommand()

  def clear(self):
    try: self.sendClearCommand()
    finally: self.finishCommand()

  def pause(self):
    try: self.sendPauseCommand()
    finally: self.finishCommand()

  def next(self):
    try: self.sendNextCommand()
    finally: self.finishCommand()

  def play(self, songNum=None):
    try: self.sendPlayCommand(songNum)
    finally:
      try:
        self.finishCommand()
      except MpdError, err:
        if err._msg == "playlist is empty":
          pass
        else:
          raise

  def prev(self):
    try: self.sendPrevCommand()
    finally: self.finishCommand()

  def stop(self):
    try: self.sendStopCommand()
    finally: self.finishCommand()

  def swap(self, one, two):
    self._checkInts(one, two)
    try:
      self.sendSwapCommand(one, two)
    finally:
      self.finishCommand()

  def move(self, fromNum, toNum):
    self._checkInts(fromNum, toNum)
    try:
      self.sendMoveCommand(fromNum, toNum)
    finally:
      self.finishCommand()

  def _checkInts(self, *nums):
    "Check if *args are all non-negative integers, raise ValueError otherwise."
    for num in nums:
      if not isinstance(num, int):
        raise ValueError, "`%s' not an int" % num
      elif num < 0:
        raise ValueError, "`%s' not a positive integer" % num

  def seek(self, percent=None, seconds=None):
    """
    seek(percent=None, seconds=None)

    Seek to either <percent> or <seconds> position of song.
    """
    if percent != None:
      percent = int(percent)
      self._checkInts(percent)
      songlen = self.getSongPosition()[1]
      newSecs = int( (float(percent)/100) * songlen )

    elif seconds != None:
      seconds = int(seconds)
      self._checkInts(seconds)
      newSecs = seconds

    song = self.status().song

    try:
      self.sendSeekCommand(song, newSecs)
    finally:
      self.finishCommand()


  def volume(self, vol):
    """
    volume(num)

    Change the volume.  num should be a positive integer, and it is used as an
    absolute value (i.e. no incremental volume changes).
    """
    self._checkInts(vol)

    try:
      self.sendVolumeCommand(vol - self.status().volume)
    finally:
      self.finishCommand()

  def random(self):
    """
    random()

    Toggle random state.
    """
    try:
      self.sendRandomCommand(not self.status().random)
    finally:
      self.finishCommand()

  def repeat(self):
    """
    repeat()

    Toggle repeat state.
    """
    try:
      self.sendRepeatCommand(not self.status().repeat)
    finally:
      self.finishCommand()

  def add(self, filenames):
    """
    add(filenames) -> Added filename list

    Accepts a list of filenames and adds them to the playlist.  File paths must
    be relative to the mpd music directory.

    A list of successfully added filenames is returned; if any filenames are
    missing from this list, then those files were not found in the music
    directory and subsequently not added to the playlist.
    """

    for name in filenames:
      if not isinstance(name, str):
        raise ValueError, "all names must be strings"

    # get files in music dir
    try:
      self.sendListallCommand()
      songpaths = self._getAllAttrsOfType("path", Song)
    finally:
      self.finishCommand()

    # only keep the filenames that exist in the music dir

    tmp = []
    for songpath in songpaths:
      for filename in filenames:
        if songpath.startswith(filename):
          tmp.append(songpath)
          break

    filenames = tmp

    # add them all
    try:
      self.sendCommandListBegin()
      map(self.sendAddCommand, filenames)
    finally:
      self.sendCommandListEnd()

    return filenames


  def load(self, *names):
    """
    load(*names)

    names should be a list of playlist names to load from the playlist
    directory.
    """
    self.sendCommandListBegin()

    try:
      for name in names:
        if not isinstance(name, str):
          raise ValueError, "all names must be strings"

      map(self.sendLoadCommand, names)

    finally:
      self.sendCommandListEnd()


  def rm(self, name):
    """
    rm(name)

    Physically remove <name>.m3u from the playlist directory.
    """

    if not isinstance(name, str):
      raise ValueError, "name must be string"

    try:
      self.sendRmCommand(name)
    finally:
      self.finishCommmand()

  def save(self, name):
    """
    save(name)

    Saves current playlist to <name>.m3u in the playlist directory.
    """

    if not isinstance(name, str):
      raise ValueError, "name must be string"

    try:
      self.sendSaveCommand(name)
    finally:
      self.finishCommand()

  def delete(self, nums):
    """
    delete(nums)

    Each item in nums can be one of two things:

      If it is an integer, then the song with the number of that integer will
      be deleted from the playlist.

      If it is a list or tuple, and contains two ints, then it will be
      interpreted as a range of song numbers to remove, with the first integer
      in the pair representing the beginning of that range, and the second
      representing the end.  e.g. delete([0, 1, [4,10]]) will delete 0, 1, 4,
      5, 6, 7, 8, 9, 10.

      Any other argument will raise a ValueError.

    This is essentially the "del" mpc action, however del is a Python keyword,
    so it's called delete here.

    Return value is the number of songs deleted.
    """

    # make sure they're all valid

    todelete = []

    for thing in nums:
      if isinstance(thing, int):
        self._checkInts(thing)
        todelete.append(thing)

      elif isinstance(thing, list) or isinstance(thing, tuple):
        if len(thing) != 2:
          raise ValueError, "ranges must be pairs"

        self._checkInts(*thing)
        todelete.extend(range(thing[0], thing[1]+1))
      else:
        raise ValueError, "nums must be ints or pairs in lists/tuples"

    # send all of the delete commands

    todelete.sort()
    todelete.reverse()
    self.sendCommandListBegin()

    try:
      map(self.sendDeleteCommand, todelete)

    finally:
      self.sendCommandListEnd()

    return len(todelete)

  def playlist(self):
    """
    playlist() -> list of all songs (Song instances, not just file paths) in
                  current playlist
    """

    self.sendPlaylistInfoCommand(-1)

    try:
      listing = filter(lambda e: isinstance(e, Song), self.iterInfoEntities())

    finally:
      self.finishCommand()

    return listing

  def listall(self, *dirs):
    """
    listall(*dirs) -> list of all music files underneath each dir in *dirs

    Works like find(1) (and is recursive), but only returns a list of music
    files found, not directories.

    Passing no arguments will list all music files inside of mpd's music
    directory.
    """

    listing = []

    if not dirs:
      dirs = [""]

    for adir in dirs:

      if not isinstance(adir, str):
        raise ValueError, "all arguments must be strings"

      self.sendListallCommand(adir)

      try:
        listing.extend(self._getAllAttrsOfType("path", Song))

      finally:
        self.finishCommand()

    return listing

  def search(self, table, *words):
    """
    search(table, *words) -> list of paths of matching music files / directories

    table should be any of album|artist|title|filename, and words should be
    search phrases to search for in the given table.  Any music files or
    directories found matching the criteria are passed back in a list of their
    paths.

    examples:
      search("filename", "blah.mp3") -> list of all filenames containing
                                        "blah.mp3"
      search("artist", "acdc","mozart") -> list of all files with an artist
                                           of acdc or mozart
    """

    if not isinstance(table, str) or table not in _searchTables:
      raise ValueError, "table must be album|artist|title|filename"

    if not words:
      raise ValueError, "no search terms given"

    table = searchTableSwapType(table)

    found = []

    for word in words:

      if not isinstance(word, str):
        raise ValueError, "all words must be strings"

      try:
        self.sendSearchCommand(table, word)

        found.extend(self._getAllAttrsOfType("path", Directory, Song))

      finally:
        self.finishCommand()

    return found

  def ls(self, dirs=[""], onlyFiles=False, onlyDirs=False):
    """
    ls(dirs) -> list of directories and music files found in *dirs

    Basically works like find(1) works, except not recursively.  You pass the
    names of some directories (relative to the root of mpd's music dir), and
    whatever music files and/or directories are found, are returned in a list.

    If no arguments are passed, all directories and music files directly inside
    of mpd's music dir are returned.

    examples:
      ls() -> list of all music files / directories directly under mpd's music
              dir
      ls(["acdc"]) -> list of all files / directories directly under
                      <mpd_music_dir>/acdc
    """

    listing = []

    for adir in dirs:

      if not isinstance(adir, str):
        raise ValueError, "all arguments must be strings"

      try:
        self.sendLsInfoCommand(adir)

        if onlyDirs:
          listing.extend(self._getAllAttrsOfType("path", Directory))
        elif onlyFiles:
          listing.extend(self._getAllAttrsOfType("path", Song))
        else:
          listing.extend(self._getAllAttrsOfType("path", Song, Directory))

      finally:
        self.finishCommand()

    return listing

  def _getAllAttrsOfType(self, attr, *types):
    """
    _getAllAttrsOfType(attr, *types) -> attrs matching types

    After sending a command to mpd, you can use this to loop through all
    returned info entities, and get only the ones that are an instance of
    a type in *types, and only a specific attr from those items.

    examples: look around :)
    """

    if not isinstance(attr, str):
      raise ValueError, "attr must be string"

    all = []

    for entity in self.iterInfoEntities():

      for t in types:
        if isinstance(entity, t):
          all.append(getattr(entity, attr))
          break

    return all

  def getSongPosition(self):
    """
    getSongPosition() -> song position tuple

    Returns a 5-item tuple containing:
      0: int, Current song's position, in seconds
      1: int, Current song's total length, in seconds
      2: float, Position in song, ranges from 0.0 to 100.0

    example:
      >>> m.getSongPosition()
      (17, 42, 40.476190476190474)

      (Song is at 0:17 play time, song's length is 0:42, and it's 40.47% done)
    """
    status = self.status()

    if status.stateStr() == "stop":
      return False

    percent = 100.0*status.elapsedTime/status.totalTime

    return (status.elapsedTime, status.totalTime, percent)

  def getPlaylistPosition(self):
    """
    getPlaylistPosition() -> playlist position tuple

    Returns a 2-int tuple containing 1. current song number, and 2. the total
    number of songs in the playlist.
    """
    status = self.status()

    if status.stateStr() == "stop":
      return (0, status.playlistLength)

    return (status.song+1, status.playlistLength)

  def getPlaylistNames(self):
    """
    getPlaylistNames() -> list of playlist names

    Returns the names of all playlists, and in standard mpd fashion, omits the
    .m3u extension.
    """
    try:
      self.sendLsInfoCommand()

      playlistNames = filter(lambda e: isinstance(e, PlaylistFile),
                             self.iterInfoEntities())
    finally:
      self.finishCommand()

    return playlistNames

  def getCurrentSong(self):
    """
    getCurrentSong() -> Song instance

    Returns a Song class instance, of the song currently playing.
    """
    status = self.status()

    if status.stateStr() == "stop":
      return False

    try:
      self.sendPlaylistInfoCommand(status.song)

      song = self.getNextInfoEntity()

    finally:
      self.finishCommand()

    return song

