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
    HOST_LINES=`cat $HOSTFILE`
    HOSTS=`perl -e '$_=join(",",<>);s/\s//g;print' $HOSTFILE`

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