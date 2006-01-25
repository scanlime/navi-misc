#!/bin/sh
#
# Streams a mysqldump from the CIA server into a file locally.
# This avoids storing the backup on the CIA server itself, so we
# minimize the impact this has on our I/O quota.
#

CIA_HOST=cia@flapjack.navi.cx
BACKUPDIR=~/download/cia-backups

FILE_PREFIX=`date "+cia-%F"`
DUMP_FILE=$FILE_PREFIX.dump.gz
TAR_FILE=$FILE_PREFIX.tar.gz

ssh $CIA_HOST 'database=cia; user=root; . ~/.cia_db; nice -n 19 mysqldump -u $user --password=$passwd $database | /usr/local/bin/io-throttle 0.8 | nice -n 19 gzip' > $BACKUPDIR/$DUMP_FILE
ssh $CIA_HOST 'cd ~/cia/data; nice -n 19 tar cf - db | /usr/local/bin/io-throttle 0.8 | nice -n 19 gzip' > $BACKUPDIR/$TAR_FILE                            

