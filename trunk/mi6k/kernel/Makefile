WARN	:=
DEBUG   := -DDEBUG
INCLUDE := -isystem /lib/modules/`uname -r`/build/include -I../include
INSTALLDIR := /lib/modules/`uname -r`/kernel/drivers/usb/
CFLAGS  := -O2 -DMODULE -D__KERNEL__ ${WARN} ${INCLUDE} ${DEBUG}
CC      := gcc
OBJS    := ${patsubst %.c, %.o, ${wildcard *.c}}
MODS    := ${patsubst %.c, %, ${wildcard *.c}}

all: ${OBJS}

clean:
	rm -rf *.o

install: ${OBJS}
	mkdir -p ${INSTALLDIR} || true
	cp -f ${OBJS} ${INSTALLDIR}
	/sbin/depmod -a

test: install
	sync
	/sbin/rmmod ${MODS} || true
	/sbin/modprobe ${MODS}
