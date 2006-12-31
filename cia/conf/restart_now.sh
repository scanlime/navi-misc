#!/bin/sh
cd /home/cia

       PID=`cat cia/twistd.pid`
       echo `date` -- Killing $PID
       while kill -9 $PID; do sleep 0.5; done
       echo `date` -- Killed.

       echo `date` -- Restarting...
       (cd cia; twistd2.4 -l /var/log/cia/twistd.log -oy conf/official.monolithic.tac)
