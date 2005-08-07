#!/bin/bash

cvs -d :pserver:anonymous@cvs.sf.net:/cvsroot/xchat co xchat2
cd xchat2
svn cat http://svn.navi.cx/misc/trunk/xchat-gnome/buildsystem.diff | patch -p0
svn co http://svn.navi.cx/misc/trunk/xchat-gnome/plugins
pushd src
svn co http://svn.navi.cx/misc/trunk/xchat-gnome/fe-gnome/
popd
./autogen.sh
./configure --prefix=/usr --enable-maintainer-mode
make
cd src/fe-gnome
gconftool-2 --install-schema-file=apps_xchat.schemas
./xchat-gnome
