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

    # Link up our uvswitch plugin with PyUnicone to automatically
    # emulate video game controllers when a system is selected.
    import sys; sys.path.append("/home/freevo/wasabi/devices/unicone")
    from PyUnicone.Freevo import UniconeHooks
    from PyUnicone.Systems import GamecubeEmulator, GenesisEmulator

    # Add the USB Video Switch plugin and map all our inputs
    plugin.activate('uvswitch.detector')
    for args in (
        (1, 'Nintendo 64', 'n64'),
        (2, 'Playstation 2', 'playstation'),
        (3, 'Sega Dreamcast', 'dreamcast'),
        (4, 'NES', 'nes'),
        (5, 'Nintendo Gamecube', 'gamecube', UniconeHooks(GamecubeEmulator)),
        (6, 'Sega Genesis', 'sega', UniconeHooks(GenesisEmulator)),
        (7, 'Atari', 'atari'),
        (8, 'Video Input', 'video_input'),
        ):
        plugin.activate('uvswitch.input', args=args)

# Our remote has a pretty high repeat rate, make these more sensitive
EVENTS['global']['VOL+'] = Event('MIXER_VOLUP', arg=2)
EVENTS['global']['VOL-'] = Event('MIXER_VOLDOWN', arg=2)
EVENTS['video']['REW']   = Event('SEEK', arg=-1)
EVENTS['video']['FFWD']  = Event('SEEK', arg=1)
EVENTS['video']['LEFT']  = Event('SEEK', arg=-15)
EVENTS['video']['RIGHT'] = Event('SEEK', arg=15)
	
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
