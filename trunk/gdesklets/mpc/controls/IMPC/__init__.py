from libdesklets.controls import Control

import os
import mpdclient
from IMPC import IMPC

class MPC(Control, IMPC):

    def __init__(self):
        self.__host = ''
        self.__port = 0

        self.__song = None
        self.__controller = None
        self.__connected = False
        Control.__init__(self)

    def __set_connect(self, val):
        self.__host = val[0]
        self.__port = val[1]
        try:
            self.__controller = mpdclient.MpdController(self.__host, self.__port)
            self.__connected = True
        except:
            self.__connected = False

    def __prev(self, val):
        self.__controller.prev()

    def __playpause(self, val):
        if self.__get_state() == 'play':
            self.__controller.pause()
        else:
            self.__controller.play()

    def __stop(self, val):
        self.__controller.stop()
        
    def __next(self, val):
        self.__controller.next()
        

    def __get_progress(self):
        try: return self.__controller.getSongPosition()[0]
        except: return 0

    def __get_length(self):
        try: return self.__controller.getSongPosition()[1]
        except: return 0

    def __get_percent(self):
        try: return self.__controller.getSongPosition()[2]
        except: return 0

    def __get_artist(self):
        self.__update_song()
        if self.__connected: return self.__song.artist
        else: return ''

    def __get_title(self):
        self.__update_song()
        if self.__connected: return self.__song.title
        else: return ''

    def __get_state(self):
        try:
            status = self.__controller.getStatus()
            return status.stateStr()
        except:
            self.__connected = False
            return 'fail'

    def __update_song(self):
        try:
            self.__song = self.__controller.getCurrentSong()
        except:
            self.__connected = False


    connect = property(fset=__set_connect, doc="open connection")
    
    prev = property(fset=__prev, doc="skip to previous song")
    playpause = property(fset=__playpause, doc="pause if playing, play otherwise")
    stop = property(fset=__stop, doc="stop")
    next = property(fset=__next, doc="skip to next song")
    
    progress = property(__get_progress, doc="get progress")
    length = property(__get_length, doc="get length")
    percent = property(__get_percent, doc="get played percent")
    artist = property(__get_artist, doc="get artist")
    title = property(__get_title, doc="get title")
    state = property(__get_state, doc="get state")


def get_class(): return MPC
