#!/bin/sh
#
# This script symlinks wasabi's freevo files into a freevo
# source tree, so they can run while still remaining under
# version control. It must be run from the wasabi directory.
#
# example:
# ./merge.sh /home/foo/freevo-1.5.0
#

FREEVO=$1
WASABI=`pwd`

ln -sf $WASABI/lircrc $FREEVO/
ln -sf $WASABI/local_conf.py $FREEVO/
ln -sf $WASABI/plugins/*.py $FREEVO/src/plugins/

cd $FREEVO
./freevo setup
perl -pi -e 's/800x600/640x480/' ~/.freevo/freevo.conf

