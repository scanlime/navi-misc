# Bash shell functions to assist in working with clusters
# Usage:
#  . cluster_utils.sh
#
# The commands "cset <hosts file>", "crun <command>", and
# "cstart <command>" become available.
#

# Set the current cluster- argument is a file of hostnames
cset () {
    HOSTFILE=`pwd`/$1
    HOST_LINES=""
    for host in `cat $HOSTFILE`; do
        echo -n "testing $host..."
        ping -c 1 $host >&/dev/null
        if [ "$?" -eq "0" ]; then
            echo "ok!"
	    HOST_LINES="$HOST_LINES $host"
        else
	    echo "down"
        fi
    done
    HOSTS=`echo $HOST_LINES | tr ' ' ','`

    export HOSTS
    export HOST_LINES
    export HOSTFILE
}

crun () {
    for host in $HOST_LINES; do
	ssh $host "$@"
    done
}

cstart () {
    for host in $HOST_LINES; do
	ssh $host "$@" &
    done
}
