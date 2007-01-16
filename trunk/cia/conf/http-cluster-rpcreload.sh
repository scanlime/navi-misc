export LOGDIR=/var/log/cia
export REQUEST_HOST=cia.navi.cx
export REQUEST_PORT=80

export PORT=3920
pidfile=server-$PORT.pid

echo Killing $PORT
kill `cat $pidfile`
sleep 1

echo Starting $PORT
twistd2.4 -oy conf/official.rpc.tac \
    -l $LOGDIR/server-$PORT.log --pidfile=$pidfile
