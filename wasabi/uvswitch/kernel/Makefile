WARN	:=
DEBUG   := -DDEBUG
INCLUDE := -isystem /lib/modules/`uname -r`/build/include -I../include
CFLAGS  := -O2 -DMODULE -D__KERNEL__ ${WARN} ${INCLUDE} ${DEBUG}
CC      := gcc
OBJS    := ${patsubst %.c, %.o, ${wildcard *.c}}

all: ${OBJS}

clean:
	rm -rf *.o

test: uvswitch.o
	sync
	/sbin/rmmod uvswitch || true
	/sbin/insmod uvswitch.o
