#!/bin/sh
#
# A simple script that runs mysqldump using parameters
# from ~/.cia_db, and stores a dated bzip2'ed dump file
# in a backups directory.
#

BACKUPDIR=~/backups

# Name this backup according to the date
DUMP_FILE=`date "+cia-%F.dump"`

# Read database parameters from the same file CIA uses
# The format is close enough we can run it like a shell
# script, as long as there's no whitespace around the '='
database=cia
user=root
. ~/.cia_db

# The password ends up on mysqldump's command line-
# this is bad and I don't like it, but it shouldn't
# be a huge security problem on our machine since a
# user can only see their own processes in /proc.
# If your machine is different, find a more secure
# way to do this. Some possibilities are modifying
# mysqldump to read the password from a file, or using
# 'expect' to send a password to mysqldump's prompt.
mysqldump -u $user --password=$passwd $database > $BACKUPDIR/$DUMP_FILE
bzip2 $BACKUPDIR/$DUMP_FILE

