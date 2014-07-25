#
# engine.py - Python glue to assist with interfacing to JetEngine.so
#
# Copyright (C) 2002-2004 Micah Dowty and David Trowbridge
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
#

import PicoGUI, getopt, sys, re, imp, code
from JetEngine import *


class EngineGlobals:
    """Initialization for all global JetEngine components"""

    version = "pre-0.01 SVN"

    about = """jetstream (http://jetstream.babylonia.flatirons.org)
Version %s
""" % (version)

    help = """
%s
usage: jetstream [-fhirw] [-m WxH[xD]] [--fullscreen] [--help] [--record]
                 [--wireframe] [--mode=WxH[xD]] [--video-file file.nuv]
                 [--video-lzo 1/0] [--video-rtjpeg 1/0] [--video-size WxH]
                 [--video-framerate fps] [--video-quality 0..1]
                 [-e file.py] [--exec file.py] [--interact]

  -h, --help        : This help message

Video options:

  -f, --fullscreen  : Fullscreen mode
  -m, --mode        : Set the video mode resolution and optionally the color depth

Initial settings:

  -r, --record      : Begin recording a video immediately
  -w, --wireframe   : Begin in wireframe mode
  -e, --exec        : Execute the given python script instead of the game
  -i, --interact    : Drop into an interactive shell instead of running the game

Initial video recorder settings:

  --video-file      : Set the default file to record video to
  --video-lzo       : Enable or disable LZO compression for the video
  --video-rtjpeg    : Enable or disable RTJpeg compression for the video
  --video-size      : Set the width and height of the recorded video
  --video-framerate : Set the target frame rate for the video recorder
  --video-quality   : Set the quality factor from 0 to 1 of the RTJpeg codec
""" % (about)

    def commandLineHandler(self):
        try:
            opts, args = getopt.getopt(sys.argv[1:], "fhm:rwe:i",
                                       ["fullscreen", "help", "mode=", "record", "wireframe",
                                        "video-file=","video-lzo=","video-rtjpeg=","video-size=",
                                        "video-framerate=","video-quality=","exec=","interact"])
        except getopt.GetoptError:
            print self.help
            sys.exit(2)

        startRecording = False
        self.defaultWireframe = False

        for o, a in opts:

            if o in ("-h", "--help"):
                print self.help
                sys.exit()

            if o in ("-f", "--fullscreen"):
                self.pgserver.setParam("video-sdlgl", "fullscreen", "1")
            if o in ("-m", "--mode"):
                self.pgserver.setParam("pgserver", "mode", a)

            if o in ("-r", "--record"):
                startRecording = True
            if o in ("-e", "--exec"):
                if not hasattr(sys,'in_jetstream_exec'):
                    sys.in_jetstream_exec = True
                    imp.load_source("__main__",a,open(a,"r"))
                    sys.exit(0)
            if o in ("-i", "--interact"):
                code.interact(self.about)
                sys.exit(0)
            if o in ("-w", "--wireframe"):
                self.defaultWireframe = True

            if o == "--video-file":
                self.videoRecorder.filename = a
            if o == "--video-lzo":
                self.videoRecorder.lzo = int(a)
            if o == "--video-rtjpeg":
                self.videoRecorder.rtjpeg = int(a)
            if o == "--video-framerate":
                self.videoRecorder.framerate = float(a)
            if o == "--video-size":
                res = re.match('([0-9]+)x([0-9]+)', a)
                self.videoRecorder.width = int(res.group(1))
                self.videoRecorder.height = int(res.group(2))
            if o == "--video-quality":
                self.videoRecorder.quality = float(a)

        if startRecording:
            self.videoRecorder.start()


    def __init__(self):
        self.engine = Engine()
        self.videoRecorder = VideoRecorder()
        self.pgserver = EmbeddedPGserver()
        self.commandLineHandler()
        self.pgserver.init()

        # The PicoGUI.Server class in cli_python can use any object with
        # read and write methods as a connection. The EmbeddedPGserver class in C++
        # implements these by directly calling the request_exec function in pgserver.
        # This is an InvisibleApp since we don't want the Application to manage a main
        # window- those will be created and managed later with WTs.
        # We still need this app class to handle the connection and event handlers.
        self.app = PicoGUI.InvisibleApp('',PicoGUI.Server(None,None,self.pgserver,1))

        self.frameHandlers = {
            'engineIteration': self.engine.iteration,
            'eventPoll': self.app.eventPoll,
            }

    def run(self):
        while self.engine.running:
            for i in self.frameHandlers.keys():
                self.frameHandlers[i]()

    def shutdown(self):
        self.pgserver.shutdown()


class World:
    def open(self, file="world.cow", sceneName="MainScene", readOnly=True):
        # Open a world from an existing JetCOW
        self.cow = JetCOW(file,readOnly)
        sceneId = self.cow.getHeader().getAttrInt(sceneName)
        self.scene = self.cow.checkoutScene(sceneId)
        self.createShortcuts()

    def create(self, file="world.cow", sceneName="MainScene"):
        # Create a new world in a new JetCOW
        self.cow = JetCOW(file, False, True)
        self.scene = self.cow.newScene()
        self.cow.getHeader().setAttrInt("MainScene", self.scene.id)
        self.createShortcuts()

    def createShortcuts(self):
        self.scene.wireframe = globals.defaultWireframe
        self.camera = self.scene.getCamera()
