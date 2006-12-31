export LOGDIR=/var/log/cia

#pound -f conf/official.pound.conf

export REQUEST_HOST=cia.navi.cx
export REQUEST_PORT=80

export PORT=3920
twistd2.4 -oy conf/official.rpc.tac \
    -l $LOGDIR/server-$PORT.log --pidfile=server-$PORT.pid

for port in 3930 3931 3932 3933 3934; do
    export PORT=$port
    twistd2.4 -oy conf/official.web.tac \
        -l $LOGDIR/server-$PORT.log --pidfile=server-$PORT.pid
done
