#WARN    := -W -Wall -Wstrict-prototypes -Wmissing-prototypes
WARN	:=
INCLUDE := -isystem /lib/modules/`uname -r`/build/include
CFLAGS  := -O2 -DMODULE -D__KERNEL__ ${WARN} ${INCLUDE}
CC      := gcc
OBJS    := ${patsubst %.c, %.o, ${wildcard *.c}}

all: ${OBJS}

.PHONY: clean

clean:
	rm -rf *.o

test: mi6k.o
	rmmod mi6k || true
	insmod mi6k.o
	echo -ne "\x15Hello World\r\nYay, this is line 2" > /dev/usb/widget
	rmmod mi6k || true
