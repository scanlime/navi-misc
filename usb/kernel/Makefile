#WARN    := -W -Wall -Wstrict-prototypes -Wmissing-prototypes
WARN    :=
INCLUDE := -isystem /lib/modules/`uname -r`/build/include
CFLAGS  := -O2 -DMODULE -D__KERNEL__ ${WARN} ${INCLUDE}
CC      := gcc
OBJS    := ${patsubst %.c, %.o, ${wildcard *.c}}

all: ${OBJS}

.PHONY: clean

clean:
	rm -rf *.o

test: usbwidget.o
	rmmod usbwidget || true
	insmod usbwidget.o
	echo -ne "\n\nHello World" > /dev/usb/widget
	sleep 1
	rmmod usbwidget || true
	dmesg