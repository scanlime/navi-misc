#!/bin/bash

# This script should be called as "ciabot %{}" from loginfo.
# Sample loginfo line:
# ALL $CVSROOT/CVSROOT/ciabot %{sVv}
# Put that in your CVSROOT/loginfo, then set the variables at the top of this
# script.  Then commit this file to your CVSROOT, and add it to
# CVSROOT/checkoutlist, and
#   echo | mail -s "JoinChannel #myproject" commits@picogui.org

projectname="YOUR_PROJECT_NAME_HERE"  # will announce to channel #<whatever you put here>
returnaddress="admin@YOUR_DOMAIN_HERE"

# You should turn stripnewlines on if you tend to write short blocks
# of text, and off if you tend to have any formatting.  If you stick
# full changelog entries in your commit messages, definitely turn it
# off.
stripnewlines=false
#stripnewlines=true

# If your logs don't include the name of the modified file, and you want to
# show it, turn this on
includefilename=false
#includefilename=true

# Deliver directly to the bot:
commitaddress="commits@picogui.org"
# Sourceforge projects might need to deliver via users.sf.net (slower)
#commitaddress="cia@users.sf.net"

#################### End of what you'll generally need to change

echo -n "Notifying #${projectname}..."

maxlines=6
message=`cat`
uname=`id -un`
lineno=`echo "$message" | grep -n "Log Message:" | awk -F: ' { print $1 } '`
message=`echo "$message" | sed "1,${lineno}d"`
newline=`echo`

if [ "$message" != "`echo \"$message\" | head -n $maxlines`" ]; then
# The following line appears to be the only way to insert a newline at
# that place.
    message="`echo \"$message\" | head -n $(($maxlines - 1))`
<...>"
fi

if $stripnewlines; then
    message=`echo -n "$message" | tr '\n\r' ' '`
fi

module=`echo $1 | cut -d/ -f1`

## disabled, as it might cause repeated messages
#if $includefilename; then
#	directory=`echo $1 | cut -d" " -f1`
#	filename=`echo $1 | cut -d" " -f2 | cut -d"," -f1`
#	module="$directory/$filename"
#fi

# the /tmp directory often has its sticky bit set, so do things
# in a private subdir so we can have shared state
tmpdir="/tmp/ciabot.cvs"
if [ ! -d $tmpdir ]; then
    mkdir $tmpdir;
    chmod 0777 $tmpdir
fi
tmpfile="$tmpdir/$RANDOM-$projectname"

cat <<EOF >$tmpfile
From: $returnaddress
To: $commitaddress
Content-Type: text/plain;
Subject: Announce $projectname

commit by $uname to $module: $message
EOF

lastlog="$tmpdir/lastlog-$projectname"
if [ -r $lastlog ]; then
    if ! diff $lastlog $tmpfile &>/dev/null; then
        # there are differences, so this is a different commit
        cat $tmpfile | /usr/sbin/sendmail -t
        # try to replace the lastlog if we're allowed
        if [ -w $lastlog ]; then
            mv $tmpfile $lastlog
            # allow the next person to overwrite
            chmod a+w $lastlog
        fi
    else
        # it's just cvs spamming us from another directory
        rm $tmpfile
    fi
else
    cat $tmpfile | /usr/sbin/sendmail -t
    mv $tmpfile $lastlog
    # To ensure that the next person to commit will be able to overwrite
    chmod a+w $lastlog
fi

echo "done."
