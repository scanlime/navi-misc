""" uvswitch Freevo plugin

This file contains Freevo plugins for integrating it with
the uvswitch (USB Video Switch) peripheral. The uvswitch.detector
plugin will look for changes in the input states, and the
uvswitch.input plugin will map one input channel to a menu item.

Your local_conf.py will need to load the uvswitch.detector
plugin once, then a uvswitch.input for each input you want to
show on the menu. Each input needs a channel number, name, and
optional icon respectively.

For example, if you have a VCR on input 1 and a Super Nintento
on input 2, your local_conf.py might read:

  plugin.activate('uvswitch.detector')
  plugin.activate('uvswitch.input', args=(1, 'VCR', 'vcr'))
  plugin.activate('uvswitch.input', args=(2, 'Super Nintendo', 'snes'))

Copyright (C) 2004 Micah Dowty <micah@navi.cx>
"""

import plugin
import rc
import item
import config
from event import *


class VideoSwitch:
    """Abstraction for the USB Video Switch driver"""
    def __init__(self, device=None, reset=True):
        if device is None:
            device = "/dev/usb/uvswitch0"
        self.device = device
        if reset:
            self.reset()

    def reset(self):
        # Turn on the computer's channel, turn off all video channels
        self.setChannel(0, 1)

    def getActive(self):
        """Return a list of channel numbers identifying inputs
           with active video sources attached.
           """
        f = open(self.device)
        active = map(int, f.readline().strip().split())
        f.close()
        return active

    def setChannel(self, video, bypass=None, audio=None):
        """Set the video channel, optionally also setting
           the bypass switch and audio channel. If no audio
           channel is given, the kernel module will assume it
           is the same as the video channel. A bypass switch
           value of 1 will send the computer's video to the TV,
           whereas a value of 0 will send the input channel
           to the TV directly.
           """
        args = [video]
        if bypass is not None:
            args.append(bypass)
        if audio is not None:
            args.append(audio)
        s = " ".join(map(str, map(int, args)))
        f = open(self.device, "w")
        f.write(s + "\n")
        f.close()

    def setAudioBalance(self, b):
        """Set the balance between video channel sound and
           any sound we might be playing. b=0 gives our PCM
           channel full volume and the line in nothing, b=1
           sets the line in to full volume and PCM is muted.
           """
        mixer = plugin.getbyname("MIXER")
        mixer.setPcmVolume(int(config.MAX_VOLUME * (1-b) + 0.5))
        mixer.setLineinVolume(int(config.MAX_VOLUME * b + 0.5))

    def getAudioBalance(self):
        mixer = plugin.getbyname("MIXER")
        pcm = mixer.getPcmVolume()
        line =mixer.getLineinVolume()
        return float(line) / (pcm + line)

    def stepAudioBalance(self, s):
        """Modify the audio balance by the given amount, then
           show the current value on the OSD.
           """
        b = self.getAudioBalance()
        b += s
        if b < 0:
            b = 0
        if b > 1:
            b = 1
        self.setAudioBalance(b)
        msg = "Input Audio: %d%%" % int(b * 100 + 0.5)
        rc.post_event(Event(OSD_MESSAGE, arg=msg))


_globalVideoSwitch = None

def getVideoSwitch(*args, **kwargs):
    """Get the VideoSwitch singleton, creating it with the
       given arguments if necessary.
       """
    global _globalVideoSwitch
    if not _globalVideoSwitch:
        _globalVideoSwitch = VideoSwitch(*args, **kwargs)
    return _globalVideoSwitch


class detector(plugin.DaemonPlugin):
    def __init__(self, device=None):
        try:
            self.switch = getVideoSwitch(device)
            self.previousActive = []
            self.poll_interval = 100
            plugin.DaemonPlugin.__init__(self)
        except IOError:
            self.reason = "Can't open the uvswitch, disabling it"

    def poll(self, menuw=None, arg=None):
        active = self.switch.getActive()
        if active != self.previousActive:
            self.previousActive = active
            rc.post_event(plugin.event('UVSWITCH', active))


class input(plugin.MainMenuPlugin):
    def __init__(self, channel, name, skin_type=None):
        plugin.MainMenuPlugin.__init__(self)
        self.active = False

        self.channel = channel
        self.name = name
        self.skin_type = skin_type

    def eventhandler(self, event=None, menuw=None, arg=None):
        if plugin.isevent(event) == 'UVSWITCH':
            active = event.arg and self.channel in event.arg
            if active and not self.active:
                # Our input has just been enabled, add a menu item
                self.addItem(menuw, VideoInputItem(self))
            elif not active and self.active:
                # Out input has been disabled
                self.removeItems(menuw)
            self.active = active

    def addItem(self, menuw, i, makeCurrent=True):
        """Add the given item to freevo's main menu. If it's
           the currently active menu, this also refreshes it.
           """
        i.owner = self
        menuw.menustack[0].choices.append(i)
        if makeCurrent:
            menuw.menustack[0].selected = i
        if len(menuw.menustack) == 1:
            menuw.refresh(reload=1)

    def removeItems(self, menuw):
        """Remove all items added with addItem"""
        choices = menuw.menustack[0].choices
        i = 0
        while i < len(choices):
            if hasattr(choices[i], 'owner') and choices[i].owner is self:
                if menuw.menustack[0].selected is choices[i]:
                    # Move the selection to the next item if possible, otherwise
                    # move it to the previous item.
                    if i+1 < len(choices):
                        menuw.menustack[0].selected = choices[i+1]
                    else:
                        menuw.menustack[0].selected = choices[i-1]
                del choices[i]
            else:
                i += 1
        if len(menuw.menustack) == 1:
            menuw.refresh(reload=1)


class VideoInputItem(item.Item):
    """An Item representing one input on the uvswitch. When this input
       is made active, it becomes the current rc.app event handler.
       """
    def __init__(self, inputPlugin):
        item.Item.__init__(self, skin_type=inputPlugin.skin_type)
        self.switch = getVideoSwitch()
        self.channel = inputPlugin.channel
        self.name = inputPlugin.name
        self.menuw = None

    def actions(self):
        return [ (self.select, "Select video input") ]

    def select(self, arg=None, menuw=None):
        """Make this channel active on the uvswitch, and make us
           the current event handler.
           """
        self.switch.setChannel(self.channel, False)
        self.switch.setAudioBalance(0.5)
        menuw.hide()
        self.menuw = menuw
        rc.app(self)

    def eventhandler(self, event, menuw=None):
        if event == "MENU_BACK_ONE_MENU":
            self.unselect()
        elif event == "MENU_UP":
            self.switch.stepAudioBalance(0.02)
        elif event == "MENU_DOWN":
            self.switch.stepAudioBalance(-0.02)

    def unselect(self):
        rc.app(None)
        self.menuw.show()
        self.switch.setAudioBalance(0)
        self.switch.reset()

### The End ###
