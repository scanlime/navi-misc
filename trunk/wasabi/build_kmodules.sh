#!/bin/sh
#
# Build and install all kernel modules from navi-misc that wasabi uses
#

INSTALLDIR=/lib/modules/`uname -r`/kernel/drivers

for SUBDIR in usb input; do (
	mkdir -f $INSTALLDIR/$SUBDIR
); done

for DEVICE in gchub mi6k uvswitch; do (
	cd devices/$DEVICE/kernel
	make || exit 1
	cp $DEVICE.ko $INSTALLDIR/usb
); done

(
    cd ../rasterwand/kernel
    make || exit 1
    cp rwand.ko $INSTALLDIR/usb
)

(
    cd ../inputpipe/uinput
    make || exit 1
    cp uinput.ko $INSTALLDIR/input
)

depmod -a

for mod in gchub mi6k uvswitch rwand uinput; do
    modprobe $mod
done
