#!/bin/sh
#
# This is a silly script that monitors the machine's
# swap usage, restarting CIA if it gets above a preset
# threshold. This shouldn't be necessary of course, but
# CIA still has some memory leaks :-(
#

while true; do

    SWAP_USED=`cat /proc/swaps | tail -n1 | cut -f 3`

    echo `date` -- $SWAP_USED swap used

    if [ $(( $SWAP_USED > 100000 )) = 1 ]; then

       echo `date` -- Swap usage is above threshold

       PID=`cat cia/twistd.pid`
       echo `date` -- Killing $PID
       while kill $PID; do sleep 0.5; done
       echo `date` -- Killed.

       echo `date` -- Restarting...
       (cd cia; twistd -oy conf/flapjack_navi_cx.tac)

       sleep 10m

    fi

    sleep 5m

done
