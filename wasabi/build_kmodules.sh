#!/bin/sh
#
# Build and install all kernel modules from navi-misc that wasabi uses
#

INSTALLDIR=/lib/modules/`uname -r`/kernel/drivers/usb

for DEVICE in gchub mi6k uvswitch; do (
	cd devices/$DEVICE/kernel
	make || exit 1
	cp $DEVICE.ko $INSTALLDIR
); done

(
    cd ../rasterwand/kernel
    make || exit 1
    cp rwand.ko $INSTALLDIR
)

depmod -a

for mod in gchub mi6k uvswitch rwand; do
    modprobe $mod
done