#
# Script to run snes9x, autodetecting the first two
# attached gamecube controllers.
#

SNES9X_BIN=/usr/local/bin/osnes9x
DEVICES_FILE=/proc/bus/input/devices

# Get, in order, all handlers attached to gamecube controllers
HANDLERS=`(for i in 1 2 3 4; do grep -A 4 "Gamecube Controller $i" $DEVICES_FILE; done) | grep ^H | cut -d = -f 2`

# Extract only the joystick devices from that
JOYSTICKS=`echo $HANDLERS | sed 's/ /\n/g' | grep ^js`

# Separate that into joysticks 1 and 2, using /dev/null for any sticks we can't find
JOYDEV1=`echo $JOYSTICKS /dev/null /dev/null | cut -d ' ' -f 1`
JOYDEV2=`echo $JOYSTICKS /dev/null /dev/null | cut -d ' ' -f 2`

# Assume gamecube joysticks for now
JOYMAP1='0 1 2 3 5 6 7 4'
JOYMAP2='0 1 2 3 5 6 7 4'

# Yay, run snes9x
exec $SNES9X_BIN -joydev1 /dev/input/$JOYDEV1 -joydev2 /dev/input/$JOYDEV2 -joymap1 $JOYMAP1 -joymap2 $JOYMAP2 $*
