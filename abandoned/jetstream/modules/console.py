#
# console.py - A popup console including a simple python IDE and other extras
#              like a video recorder. This is the backend for the GUI
#              defined in console.xwt
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

import PicoGUI, sys, re, string, os, struct
from code import InteractiveConsole
import engine, debug

def toggle():
    if hasattr(engine,'consoleInstance'):
        engine.consoleInstance.destroy()
    else:
        engine.consoleInstance = Console()

class Tee:
    def __init__(self, targets=[]):
        self.targets = targets
    def write(self, data):
        for i in self.targets:
            i.write(data)

class Console(InteractiveConsole):
    def __init__(self):
        self.app = engine.globals.app
        self.colorSchemeItems = []
        self.terminalTheme = None

        # Load our widget template, importing widgets from it
        self.template = self.app.newTemplate(open("data/console.wt").read())
        self.inst = self.template.instantiate([
            'PythonConsole',
            'PythonCommand',
            'PythonPrompt',
            'PythonRun',
            'PythonLoad',
            'PythonClear',
            'PythonSave',
            'PythonFile',
            'PythonEditor',
            'VideoRecorder_EnableHotkeys',
            'VideoRecorder_Record',
            'VideoRecorder_Filename',
            'VideoRecorder_Resolution',
            'VideoRecorder_Framerate',
            'VideoRecorder_Quality',
            'VideoRecorder_RTJpeg',
            'VideoRecorder_LZO',
            'ConsolePanel',
            'WireframeEnable',
            'BackgroundShade',
            'LaunchCameraPanel',
            'LaunchDetailPanel',
            'ShowFPS',
            'ConsoleTerminal',
            'TermColorSchemes',
            'FrustumLock',
            'DisableBackfaceCull',
            ])
        self.app.link(self.commandHandler, self.inst.PythonCommand, 'activate')
        self.app.link(self.vidRecordHandler, self.inst.VideoRecorder_Record, 'activate')
        self.app.link(self.vidHotkeyHandler, self.inst.VideoRecorder_EnableHotkeys, 'activate')
        self.app.link(self.pyLoadHandler, self.inst.PythonLoad, 'activate')
        self.app.link(self.pySaveHandler, self.inst.PythonSave, 'activate')
        self.app.link(self.pyRunHandler, self.inst.PythonRun, 'activate')
        self.app.link(self.pyClearHandler, self.inst.PythonClear, 'activate')
        self.app.link(self.closeHandler, self.inst.ConsolePanel, 'close')
        self.readbackVideoRecorder()

        # Debug options panel
        self.inst.WireframeEnable.on = engine.world.scene.wireframe
        self.inst.DisableBackfaceCull.on = engine.world.scene.disableBackfaceCull
        self.inst.ShowFPS.on = engine.globals.pgserver.getShowFPS()
        self.inst.FrustumLock.on = engine.globals.engine.frustumLock
        self.inst.BackgroundShade.size = 255
        self.inst.BackgroundShade.value = engine.world.scene.bgShade * 255
        self.app.link(self.wireframeHandler, self.inst.WireframeEnable, 'activate')
        self.app.link(self.fpsHandler, self.inst.ShowFPS, 'activate')
        self.app.link(self.cameraPanelHandler, self.inst.LaunchCameraPanel, 'activate')
        self.app.link(self.detailPanelHandler, self.inst.LaunchDetailPanel, 'activate')
        self.app.link(self.bgshadeHandler, self.inst.BackgroundShade, 'activate')
        self.app.link(self.frustumLockHandler, self.inst.FrustumLock, 'activate')
        self.app.link(self.backfaceHandler, self.inst.DisableBackfaceCull, 'activate')

        # Split stdout and stderr into our textbox
        self.savedStdout = sys.stdout
        self.savedStderr = sys.stderr
        sys.stdout = Tee([self.savedStdout, self.inst.PythonConsole])
        sys.stderr = Tee([self.savedStderr, self.inst.PythonConsole])

        locals = {
            "__name__":   "__console__",
            "__doc__":    None,
            "PicoGUI":    PicoGUI,
            "engine":     engine,
            "debug":      debug,
            }
        InteractiveConsole.__init__(self,locals)

        try:
            sys.ps1
        except AttributeError:
            sys.ps1 = ">>> "
        try:
            sys.ps2
        except AttributeError:
            sys.ps2 = "... "

        self.prompt = sys.ps1
        self.inst.PythonPrompt.text = self.prompt

        print "%s\nPython %s on %s\n(jetstream shell, try dir() for interesting objects)\n" %\
              (engine.globals.about,sys.version, sys.platform)

        self.initTerminal()

    def runLine(self,line):
        print self.prompt + line
        if self.push(line):
            self.prompt = sys.ps2
        else:
            self.prompt = sys.ps1
        self.inst.PythonPrompt.text = self.prompt

    def destroy(self):
        self.updateVideoRecorder()
        sys.stdout = self.savedStdout
        sys.stderr = self.savedStderr
        self.template.destroy()
        del engine.consoleInstance
        if engine.globals.frameHandlers.has_key('consoleTerminal'):
            del engine.globals.frameHandlers['consoleTerminal']
        for i in self.colorSchemeItems:
            self.app.delWidget(i)
        if self.terminalTheme:
            self.app.server.free(self.terminalTheme)

    def updateVideoRecorder(self):
        vid = engine.globals.videoRecorder
        res = re.match('([0-9]+)x([0-9]+)', self.inst.VideoRecorder_Resolution.text)
        vid.width = int(res.group(1))
        vid.height = int(res.group(2))
        vid.framerate = float(self.inst.VideoRecorder_Framerate.text)
        vid.filename = self.inst.VideoRecorder_Filename.text
        vid.quality = self.inst.VideoRecorder_Quality.value / 100.0
        vid.rtjpeg = self.inst.VideoRecorder_RTJpeg.on
        vid.lzo = self.inst.VideoRecorder_LZO.on

    def readbackVideoRecorder(self):
        vid = engine.globals.videoRecorder
        self.inst.VideoRecorder_Resolution.text = "%dx%d" % (vid.width,vid.height)
        self.inst.VideoRecorder_Framerate.text = str(vid.framerate)
        self.inst.VideoRecorder_Filename.text = vid.filename
        self.inst.VideoRecorder_Quality.value = int(vid.quality * 100)
        self.inst.VideoRecorder_RTJpeg.on = vid.rtjpeg
        self.inst.VideoRecorder_LZO.on = vid.lzo
        self.inst.VideoRecorder_Record.on = vid.isRunning()

    def commandHandler(self, ev, widget):
        self.runLine(widget.text[:])
        widget.text = ''

    def vidRecordHandler(self, ev, widget):
        if widget.on:
            self.updateVideoRecorder()
            engine.globals.videoRecorder.start()
        else:
            engine.globals.videoRecorder.stop()

    def vidHotkeyHandler(self, ev, widget):
        if widget.on:
            self.inst.VideoRecorder_Record.hotkey = 293   # F12
        else:
            self.inst.VideoRecorder_Record.hotkey = 0

    def pyClearHandler(self, ev, widget):
        self.inst.PythonEditor.write('')

    def pyLoadHandler(self, ev, widget):
        try:
            name = self.inst.PythonFile.text
            file = open(name,'r')
            self.inst.PythonEditor.text = file.read()
            file.close()
            print "Loaded %s" % name
        except:
            self.showtraceback()

    def pySaveHandler(self, ev, widget):
        try:
            name = self.inst.PythonFile.text
            file = open(name,'w')
            file.write(self.inst.PythonEditor.text);
            file.close()
            print "Saved %s" % name
        except:
            self.showtraceback()

    def pyRunHandler(self, ev, widget):
        for line in string.split(self.inst.PythonEditor.text,"\n"):
            self.push(line)

    def closeHandler(self, ev, widget):
        self.destroy()

    def wireframeHandler(self, ev, widget):
        engine.world.scene.wireframe = int(widget.on)

    def fpsHandler(self, ev, widget):
        engine.globals.pgserver.setShowFPS(int(widget.on))

    def backfaceHandler(self, ev, widget):
        engine.world.scene.disableBackfaceCull = int(widget.on)

    def frustumLockHandler(self, ev, widget):
        engine.globals.engine.frustumLock = int(widget.on)

    def cameraPanelHandler(self, ev, widget):
        debug.CameraControl()

    def detailPanelHandler(self, ev, widget):
        debug.DetailControl()

    def bgshadeHandler(self, ev, widget):
        engine.world.scene.bgShade = self.inst.BackgroundShade.value / 255.0

    def initTerminal(self):
        # Set up a shell in the terminal if we can
        try:
            # Load modules that aren't supported on all OSes.
            # If one of these doesn't load, we'll just disable the terminal.
            import pty, fcntl, termios
            self.fcntl = fcntl
            self.termios = termios

            # Fork off a shell
            (pid, fd) = pty.fork()
            if pid == 0:
                os.execlp("sh","sh")
            self.termFd = fd
            self.termPid = pid

            # Use nonblocking I/O
            self.fcntl.fcntl(self.termFd, self.fcntl.F_SETFL, os.O_NDELAY)

            # Set up event handlers for the terminal widget, and a main loop
            # handler to poll for data from our subprocess.
            engine.globals.frameHandlers['consoleTerminal'] = self.terminalFrameHandler
            self.termProcessRunning = True
            self.app.link(self.terminalHandler, self.inst.ConsoleTerminal, 'data')
            self.app.link(self.terminalResizeHandler, self.inst.ConsoleTerminal, 'resize')
        except:
            self.inst.ConsoleTerminal.write("The terminal isn't supported on this operating system.\n\r")

        # Load the terminal color schemes
        for scheme in [('Retro',   'data/term_retro.th'),
                       ('C-64',    'data/term_c64.th'),
                       ('Wet',     'data/term_wet.th'),
                       ('White',   'data/term_white.th'),
                       ('Default', None),
                       ]:
            item = self.inst.TermColorSchemes.addWidget('listitem','inside')
            item.text = scheme[0]
            item.file = scheme[1]
            self.colorSchemeItems.append(item)
            if not item.file:
                item.on = True
            self.app.link(self.terminalColorschemeHandler, item, 'activate')

    def terminalColorschemeHandler(self, ev, widget):
        if widget.on:
            if self.terminalTheme:
                self.app.server.free(self.terminalTheme)
                self.terminalTheme = None
            if widget.file:
                self.terminalTheme = self.app.server.mktheme(open(widget.file,"rb").read())

    def terminalHandler(self, ev, widget):
        os.write(self.termFd, ev.data)

    def terminalFrameHandler(self):
        if self.termProcessRunning:
            try:
                self.inst.ConsoleTerminal.write(os.read(self.termFd, 4096))
            except OSError:
                pass

    def terminalResizeHandler(self, ev, widget):
        if ev.x > 0 and ev.y > 0:
            self.fcntl.ioctl(self.termFd, self.termios.TIOCSWINSZ,
                             struct.pack('4H', ev.y, ev.x, 0, 0))
