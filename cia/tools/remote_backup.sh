#!/bin/sh
#
# Streams a mysqldump from the CIA server into a file locally.
# This avoids storing the backup on the CIA server itself, so we
# minimize the impact this has on our I/O quota.
#

CIA_HOST=cia@flapjack
BACKUPDIR=/navi/backups/cia

DUMP_FILE=`date "+cia-%F.dump.gz"`

ssh $CIA_HOST 'database=cia; user=root; . ~/.cia_db; nice -n 19 mysqldump -u $user --password=$passwd $database | gzip' > $BACKUPDIR/$DUMP_FILE


