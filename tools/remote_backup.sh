#!/bin/sh
#
# Run backup.sh on the CIA server remotely, then save
# an offsite copy of the resulting dump file.
#

CIA_HOST=cia.navi.cx
REMOTE_BACKUPDIR=backups
LOCAL_BACKUPDIR=~/backups
REMOTE_BACKUPSCRIPT=cia/tools/backup.sh

# Name this backup according to the date
DUMP_FILE=`date "+cia-%F.dump.bz2"`

ssh $CIA_HOST "$REMOTE_BACKUPSCRIPT"
scp -q $CIA_HOST:$REMOTE_BACKUPDIR/$DUMP_FILE $LOCAL_BACKUPDIR

