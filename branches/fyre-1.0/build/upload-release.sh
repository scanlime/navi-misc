#!/bin/bash
#
# After running 'dist-win32.sh' to build the windows
# releases, this uploads them to our release server.
# It automatically chooses a different destination
# directory for svn snapshots.
#

OUTPUTDIR=output
RELEASE_BASE=flapjack:/var/www/localhost/htdocs/releases/fyre
SVN_RELEASE_BASE=$RELEASE_BASE/snapshots

for filename in `cd $OUTPUTDIR; ls`; do

    TARGET=$RELEASE_BASE
    if (echo $filename | grep -q svn); then
	TARGET=$SVN_RELEASE_BASE
    fi

    scp $OUTPUTDIR/$filename $TARGET/$filename

done

### The End ###
