#!/bin/sh
#
# CIA bot client script for Subversion repositories, delivering via email
# -- Micah Dowty <micah@picogui.org>
#
# See http://cia.navi.cx
# for more information on what the CIA bot is and how it works.
#
# To use the CIA bot in your Subversion repository...
#
# 1. Customize the parameters below, specifically the ones under
#    the "Project information" section
#
# 2. This script should be called from your repository's post-commit
#    hook with the repository and revision as arguments. For example,
#    you could copy this script into your repository's "hooks" directory
#    and add something like the following to the "post-commit" script,
#    also in the repository's "hooks" directory:
#
#      REPOS="$1"
#      REV="$2"
#      $REPOS/hooks/ciabot_svn.sh "$REPOS" "$REV"&
#
############# There are some parameters for this script that you can customize:

# Project information
project_name="YOUR_PROJECT_NAME_HERE"
return_address="YOUR_EMAIL_ADDRESS@HERE"

# System
sendmail_command="/usr/sbin/sendmail -t"

############# Below this line you shouldn't have to change anything

# Script arguments
REPOS="$1"
REV="$2"

# The email address CIA lives at
cia_address="cia@cia.navi.cx"

author=`svnlook author -r "$REV" "$REPOS" | sed 's/\&/\&amp;/g;s/</\&lt;/g;s/>/\&gt;/g'`
log=`svnlook log -r "$REV" "$REPOS" | sed 's/\&/\&amp;/g;s/</\&lt;/g;s/>/\&gt;/g'`
diff_lines=`svnlook diff -r "$REV" "$REPOS" | wc -l`
for file in `svnlook changed -r "$REV" "$REPOS" | cut -c 3- | sed 's/\&/\&amp;/g;s/</\&lt;/g;s/>/\&gt;/g'`; do
    files="$files<file>$file</file>"
done

# Send an email with the final XML message
(cat <<EOF
From: $return_address
To: $cia_address
Subject: DeliverXML

<message>
    <generator>
        <name>Subversion CIA Bot client shell script</name>
        <version>1.0</version>
    </generator>
    <source>
        <project>$project_name</project>
    </source>
    <body>
        <commit>
            <revision>$REV</revision>
            <author>$author</author>
            <files>$files</files>
            <log>$log</log>
            <diffLines>$diff_lines</diffLines>
        </commit>
    </body>
</message>
EOF
) | $sendmail_command

### The End ###
