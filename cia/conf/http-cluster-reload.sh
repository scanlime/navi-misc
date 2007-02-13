export LOGDIR=/var/log/cia
export REQUEST_HOST=cia.navi.cx
export REQUEST_PORT=80

for port in 3930 3931 3932 3933; do
    export PORT=$port
    pidfile=server-$PORT.pid

    echo Killing $port
    kill `cat $pidfile`
    sleep 5

    echo Starting $port
    twistd2.4 -oy conf/official.web.tac \
        -l $LOGDIR/server-$PORT.log --pidfile=$pidfile

    echo ...
    sleep 20
done

# The first web server is primarily for spiders: make it lower-priority
snice +10 `cat ~/cia/server-3930.pid`

