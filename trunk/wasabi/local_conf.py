#
# Freevo configuration for Wasabi
#

CONFIG_VERSION = 5.13

# Decide whether we're actually running on wasabi by looking for the uvswitch
import os.path
have_wasabi_hardware = os.path.exists("/dev/usb/uvswitch0")

if have_wasabi_hardware:
    START_FULLSCREEN_X  = 1

    ROM_DRIVES = [
	('/mnt/cdrom', '/dev/cdrom', 'DVD'),
	]
    HIDE_UNUSABLE_DISCS = 1
    ROM_SPEED = 8

    ENABLE_SHUTDOWN_SYS = 1
    SHUTDOWN_SYS_CMD = 'sudo shutdown -h now'
    RESTART_SYS_CMD  = 'sudo shutdown -r now'

    # Show navigational and OSD information on the MI6K
    plugin.activate('mi6k_vfd')
    VFD_TITLE = "Wasabi"

    # Since we get information about the currently playing music
    # on the MI6K, we don't need the detachbar any more.
    plugin.remove('audio.detachbar')

    class InputHooks:
        def start(self):
            from gui import ProgressBox
            import time
            b = ProgressBox(text="Wheeee....", full=100)
            b.show()
            for i in xrange(100):
                time.sleep(0.01)
                b.tick()
            b.destroy()

    # Add the USB Video Switch plugin and map all our inputs
    plugin.activate('uvswitch.detector')
    plugin.activate('uvswitch.input', args=(1, 'Nintendo 64', 'n64'))
    plugin.activate('uvswitch.input', args=(2, 'Playstation 2', 'playstation'))
    plugin.activate('uvswitch.input', args=(3, 'Sega Dreamcast', 'dreamcast'))
    plugin.activate('uvswitch.input', args=(4, 'NES', 'nes'))
    plugin.activate('uvswitch.input', args=(5, 'Nintendo Gamecube', 'gamecube', InputHooks()))
    plugin.activate('uvswitch.input', args=(6, 'Sega Genesis', 'sega'))
    plugin.activate('uvswitch.input', args=(7, 'Atari', 'atari'))
    plugin.activate('uvswitch.input', args=(8, 'Video Input', 'video_input'))

# Our remote has a pretty high repeat rate, make these more sensitive
EVENTS['global']['VOL+'] = Event('MIXER_VOLUP', arg=2)
EVENTS['global']['VOL-'] = Event('MIXER_VOLDOWN', arg=2)

# Make freevo knowful of Navi
common_items = [
    ('Navi', '/navi'),
    ('Local Files', '/'),
    ]
VIDEO_ITEMS = [
    ('Movies', '/navi/media/video/movies'),
    ('Series', '/navi/media/video/series'),
    ('Anime', '/navi/media/video/anime'),
    ('Navi Video', '/navi/media/video'),
    ] + common_items
AUDIO_ITEMS = [
    ('Groups', '/navi/media/audio/groups'),
    ('Soundtracks', '/navi/media/audio/soundtracks'),
    ('Composers', '/navi/media/audio/composers'),
    ('Comedy', '/navi/media/audio/comedy'),
    ('Navi Audio', '/navi/media/audio'),
    ] + common_items
IMAGE_ITEMS = [
    ('Navi Images', '/navi/media/images'),
    ] + common_items

CACHE_IMAGES = 1

GAMES_ITEMS = [
    ('Super Nintendo', '/navi/media/games/snes',
     ('SNES', '/bin/sh', '/home/freevo/wasabi/snes9x.sh', '', None)),
    ('Nintendo Entertainment System', '/navi/media/games/nes',
     ('GENERIC', '/bin/sh', '/home/freevo/wasabi/fceu.sh', '', None)),
    ]

# Remove standard plugins we have no use for
plugin.remove('tv')

# Add standard plugins we want
plugin.activate('audio.mplayervis')

LIRCRC = 'lircrc'

# Always start up with the wasabi skin
SKIN_XML_FILE = 'wasabi-calm'

# Always write to a visualization buffer at /tmp/mpav,
# so we can get visualization on all media mplayer handles.
MPLAYER_ARGS_DEF = "%s -af export=/tmp/mpav " % MPLAYER_ARGS_DEF

### The End ###
